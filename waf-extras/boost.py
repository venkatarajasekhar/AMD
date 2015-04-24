#!/usr/bin/env python
# encoding: utf-8
#

"""
Module that is used to find where, if at all, the BOOST library is present.

Usage:
When using this tool, the wscript will look like:
  def options(opt):
    opt.load('compiler_c compiler_cxx boost')

  def configure(conf):
    ctx.env.BOOST_PKGS = ['unit_test_framework', 'serialization']
    conf.load('compiler_c compiler_cxx boost')

  def build(bld):
    bld(source='main.cpp', target='app', use='BOOST')
"""

###############################################################################
#                                TOOL PARAMETERS
###############################################################################

# Name to call this library in use variables
USE_NAME = 'BOOST'

# List of paths to search for shared libraries, in order of preference
LIBPATHS = ['/lib', '/usr/lib', '/usr/local/lib']

# List of paths to search for static libraries, in order of preference
STLIBPATHS = LIBPATHS

# List of include paths
INCLUDES = ['/include', '/usr/include', '/usr/local/include']

# Header names -- these are the only headers that are checked for during config
HEADER_NAMES = ['boost/version.hpp']

# Minimum Boost version we require
MIN_VERSION = [1, 55]

###############################################################################
#                             COMMAND-LINE OPTIONS
###############################################################################


def options(opt):
  grp = opt.get_option_group('Options for 3rd-party libraries')
  grp.add_option('--boost-path',
                 type='string',
                 default='',
                 dest='boost_path',
                 help='path to the root directory of the Boost installation')


###############################################################################
#                        CONFIGURATION HELPER FUNCTIONS
###############################################################################

from os               import chdir
from os               import devnull
from subprocess       import call
from waflib.Configure import conf
from waflib.Logs      import pprint
from waflib.Utils     import unversioned_sys_platform

@conf
def check_boost_version(ctx):
    '''Returns True if the detected Boost version is >= MIN_VERSION'''

    FRAGMENT = '''
    #include <iostream>
    #include <boost/version.hpp>
    int main() {
        std::cout << BOOST_LIB_VERSION << std::endl;
    }
    '''
    ctx.start_msg("Checking BOOST version")
    version_string = ctx.check_cxx(use        = [USE_NAME] + ctx.env.BOOST_USE_LIBS,
                                   fragment   = FRAGMENT,
                                   execute    = True,
                                   define_ret = True)
    version_string = version_string.replace('_', '.').strip()
    major = int(version_string.split('.')[0])
    minor = int(version_string.split('.')[1])

    if MIN_VERSION[0] <= major and MIN_VERSION[1] <= minor:
        ctx.end_msg('ok [' + version_string + ']', color='GREEN')
        return True
    else:
        ctx.end_msg('not ok [found: ' + version_string + ', need: ' +
                    '.'.join([str(num) for num in MIN_VERSION]) + ']',
                    color='YELLOW')
        return False


@conf
def boost_is_compiled(self, lib):
    '''Returns True if `lib` is a compiled library'''

    compiled_libs = [
        'atomic',
        'chrono',
        'context',
        'date_time',
        'exception',
        'filesystem',
        'geometry',
        'graph',
        'graph_parallel',
        'iostreams',
        'locale',
        'log',
        'math',
        'mpi',
        'mpl',
        'program_options',
        'python',
        'random',
        'regex',
        'serialization',
        'smart_ptr',
        'system',
        'test',
        'thread',
        'timer',
        'wave',
    ]
    return lib in compiled_libs



@conf
def boost_libs_from_pkgs(ctx, pkgs):
    LIBS = []
    for pkg in filter(ctx.boost_is_compiled, pkgs):
        if pkg == 'test':
            LIBS += ['boost_unit_test_framework']
        else:
            LIBS += ['boost_' + pkg]
        if pkg == 'locale':
            LIBS += ['boost_chrono', 'boost_thread', 'boost_system']
        if pkg == 'log':
            LIBS += ['boost_log_setup', 'boost_regex', 'boost_filesystem',
                     'boost_date_time', 'boost_chrono', 'boost_thread',
                     'boost_system']
    return list(set(LIBS))


###############################################################################
#                          DOWNLOAD AND INSTALLATION
###############################################################################

@conf
def download_boost(ctx):
    # First, check if the source has already been downloaded
    download_path = ctx.env.DEPS_DOWNLOADS + '/boost_1_57_0.tar.gz'
    ctx.start_msg('Searching for BOOST download')
    download_node = ctx.root.find_node(download_path)
    if not download_node:
        # If it hasn't been downloaded already, download it
        ctx.end_msg('not found', color='YELLOW')
        ctx.find_dep_program('curl')
        ctx.msg('Downloading BOOST', '...', color='YELLOW')
        command = ctx.env.CURL + ['--progress-bar', '--create-dirs', '-Lf',
                                  '-o', download_path,
                                  'http://downloads.sourceforge.net/project/' +
                                  'boost/boost/1.57.0/boost_1_57_0.tar.gz']
        call(command)
    else:
        ctx.end_msg('ok')
    # By this point, we should have the .tar.gz
    # Begin by creating the directory where the sources will be extracted
    source_dir_node = ctx.root.make_node(ctx.env.DEPS_SRC)
    source_dir_node.mkdir()
    # Then untar the download into it
    ctx.find_dep_program('tar')
    command = ctx.env.TAR + ['-xf', download_path, '-C', ctx.env.DEPS_SRC]
    msg = 'Extracting BOOST sources'
    ctx.exec_with_msg(command, msg)


@conf
def install_boost(ctx):
    pprint('YELLOW', 'Attempting BOOST auto-installation ...')

    # First, search for the source files
    ctx.start_msg('Searching for BOOST sources')
    source_path = ctx.env.DEPS_SRC + '/boost_1_57_0'
    source_node = ctx.root.find_node(source_path)
    # If sources not found, try to download them
    if not source_node:
        ctx.end_msg('not found', color='YELLOW')
        ctx.download_boost()
    else:
        ctx.end_msg('ok')

    # At this point, we should have the sources
    chdir(source_path)

    # Bootstrap
    msg = 'Bootstrapping BOOST.Build'
    command = ['./bootstrap.sh']
    ctx.exec_with_msg(command, msg)

    # Build
    msg = 'Building and installing BOOST'
    command = ['./b2', '--prefix=' + ctx.env.DEPS_PREFIX ] + \
    ['--with-test'] + \
    ['--with-' + lib for lib in ctx.env.BOOST_PKGS
                     if ctx.boost_is_compiled(lib)] + \
    ['install']
    ctx.exec_with_msg(command, msg)

    # On MacOSX, the install_name is not set correctly by the boost build
    # Fix that by invoking the install_name_tool
    if unversioned_sys_platform() == 'darwin':
        chdir(ctx.env.DEPS_LIB)
        msg = 'Fixing dynamic library install name'
        command = 'for i in libboost*.dylib; do' + \
                  '  install_name_tool -id "@rpath/$i" $i;' + \
                  '  for j in libboost*.dylib; do' + \
                  '    install_name_tool -change $j "@rpath/$j" $i;' + \
                  '  done;' + \
                  'done;'
        ctx.exec_with_msg(command, msg, shell=True)

    pprint('YELLOW', '... BOOST auto-installation complete.')


###############################################################################
#                                   CONFIGURE
###############################################################################

def configure(ctx):
    # The BOOST_PKGS env variable tells us which Boost packages to search for
    if not ctx.env.BOOST_PKGS:
        return

    # Determine which boost compiled libs to configure
    LIBS = ctx.boost_libs_from_pkgs(ctx.env.BOOST_PKGS)

    # If using Boost.Python, configure the Pythondev package as well
    USE_LIBS = None
    if 'python' in ctx.env['BOOST_PKGS']:
        ctx.load('pythondev', tooldir = '.')
        ctx.env.append_unique('BOOST_USE_LIBS', 'PYTHONDEV')

    # Check for the presence of Boost
    # This will define the *_BOOST use variables
    ctx.check_dep(
        header_names       = HEADER_NAMES,
        includes           = INCLUDES,
        install_method     = ctx.install_boost,
        install_path       = ctx.options.boost_path,
        libpaths           = LIBPATHS,
        libs               = LIBS,
        stlibpaths         = STLIBPATHS,
        stlibs             = LIBS,
        use_name           = USE_NAME,
        use_libs           = ctx.env.BOOST_USE_LIBS
    )

    # Check the Boost version
    ctx.check_dep_requirement(
        header_names   = HEADER_NAMES,
        install_method = ctx.install_boost,
        libs           = LIBS,
        requirement    = ctx.check_boost_version,
        stlibs         = LIBS,
        use_name       = USE_NAME,
        use_libs       = ctx.env.BOOST_USE_LIBS
    )

    # If building Boost.Log, we need the following define
    if 'log' in ctx.env.BOOST_PKGS:
        ctx.env.append_unique('DEFINES_BOOST', 'BOOST_LOG_DYN_LINK')
