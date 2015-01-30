#!/usr/bin/env python
# encoding utf-8

import sys

# Define the name and version of the project for "dist" command
APPNAME='AMD'
VERSION='1.0.0'

# Root of project treep and directory where built files are output
top='.'
out='build'

# Directory with  waf tools to configure external libraries and list of tools
tooldir = 'waf-extras'
tools = [
            'boost',
            'pyheaders',
            'common-tool',
        ]

# Dict to store a mapping of command line flags for building packages to the
# paths from the root where the wscripts to build those packages can be found
# e.g. opt_paths['bld_util'] == 'AMD/util'
from collections import defaultdict
opt_paths = defaultdict(list)

def options(ctx):
    # Check Python version
    if (sys.hexversion < 0x2060000 or 0x3000000 <= sys.hexversion ):
        ctx.fatal('Python 2.6 or Python 2.7 is required')

    # Options for C and C++ compilers
    ctx.load('compiler_c')
    ctx.load('compiler_cxx')

    # Options for third party librarys
    third_party_group = ctx.add_option_group('Options for 3rd-party libraries')

    # Options for various waf tools for various third party libraries
    ctx.load(tools, tooldir=tooldir)

    # General options related to building AMD
    general_group = ctx.add_option_group('General options for AMD')
    general_group.add_option('--bld-all', action='store_true',
                             help='build all of AMD, ' +
                                  'plus contrib and examples')
    general_group.add_option('--tests', action='store_true',
                             help='build all tests in AMD')
    general_group.add_option('--install-deps', action='store_true',
                             help='download and install required dependencies')
    general_group.add_option('--install-deps-prefix', action='store',
                             type='string',
                             default=ctx.path.abspath() + '/deps',
                             help='prefix to download and install ' +
                                  'any required dependencies')

    def add_bld_flag_group(name, desc, excl='', path='', in_bld_default=True):
        '''Add command line build flags for the the package group `name`.
        This function will add an option group for a directory, with options to
        build all the components (subdirectories with wscripts) within it.
        If ` path ` is specified, it is the relative path to group directory
        from the project root --- otherwise this path is assumed to be the
        top-level-directory called `name`. If any subdirectories must be
        excluded, they are specified using `excl`. A description of the group
        specified using ` desc ` .
        '''
        if not path: path = name
        grp_node = ctx.path.find_dir(path)
        if not grp_node: return
        grp_group = ctx.add_option_group('Options for ' + desc)
        grp_opt = 'bld-' + name
        grp_var = grp_opt.replace('-', '_')
        for pkg_node in grp_node.ant_glob('**', dir=True, excl=excl):
            if pkg_node.find_node('wscript'):
                pkg_path = pkg_node.path_from(ctx.path)
                pkg_opt = grp_opt + '-' + \
                          pkg_node.path_from(grp_node).replace('/', '-')
                pkg_var = pkg_opt.replace('-', '_')
                opt_paths[pkg_var] = pkg_path
                opt_paths[grp_var].append(pkg_path)
                opt_paths['bld_all'].append(pkg_path)
                if in_bld_default:
                    opt_paths['bld_default'].append(pkg_path)
                grp_group.add_option('--' + pkg_opt, action='store_true',
                                     help='build ' + pkg_path)
            grp_group.add_option('--' + grp_opt, action='store_true',
                                 help='build all the ' + desc)

    add_bld_flag_group('AMD',    'AMD packages', excl='contrib')
    add_bld_flag_group('examples', 'AMD examples')
    add_bld_flag_group('python','AMD python bindings',in_bld_default=False)
    ctx.load('doxygen', tooldir='waf-extras')

def configure(ctx):
    from waflib.Logs import pprint

    pprint('CYAN', '\nC/C++ COMPILERS\n---------------')
    ctx.load('compiler_c')
    ctx.load('compiler_cxx')

    if 'clang++' in ctx.env.CXX[0]:
        ctx.env.append_unique('CXXFLAGS', ['-Wno-c++11-extensions',
                                           '-fcolor-diagnostics',])

    # Add the paths to packages that were requested on the command line to the
    # 'BLD_PATHS' environment variable. If no bld flags were explicitly
    # requested, assume we're building everything (same as --bld-all)
    for key, value in ctx.options.__dict__.items():
        if value is True:
            ctx.env.append_unique('BLD_PATHS', opt_paths[key])
    if not ctx.env['BLD_PATHS']:
        ctx.env.append_unique('BLD_PATHS', opt_paths['bld_default'])

    # Store if tests were requested on the command line
    ctx.env.TESTS = ctx.options.tests

    # Call the configure method in the wscript for all packages in BLD_PATHS
    pprint('CYAN', '\nAMD PACKAGES\n----------------')
    for pkg_path in ctx.env['BLD_PATHS']:
        ctx.msg('Build ' + pkg_path, 'yes', color='GREEN')
        ctx.recurse(pkg_path)

    # If a third party library was required by any of the configured packages,
    # it should have set a 'USE_...' environment variable. Iterate over these
    # and call the relevant waf tool to configure the third party dependency.
    pprint('CYAN', '\nTHIRD-PARTY LIBRARIES\n---------------------')
    deps = [k[4:].lower() for k in ctx.env.keys() if k.startswith('USE_')]
    deps += ['doxygen']
    ctx.load(deps, tooldir=tooldir)
    print ''

    # Write out a config.h for anyone who needs to access the defines
    # This header is written in the build directory, not the source dir.
    ctx.write_config_header('config.h')


def build(ctx):
  # Call the build method in the wscript for all packages in BLD_PATHS
  for pkg_path in ctx.env['BLD_PATHS']:
      ctx.recurse(pkg_path)


from waflib import TaskGen
@TaskGen.feature('cxx')
@TaskGen.after_method('apply_link', 'propagate_uselib_vars')
def set_flags(self):
    if sys.platform == 'linux2':
        self.link_task.env.STLIB_MARKER = [self.link_task.env.STLIB_MARKER, \
                                           '-Wl,--start-group']
        self.link_task.env.SHLIB_MARKER = ['-Wl,--end-group', \
                                           self.link_task.env.SHLIB_MARKER]
