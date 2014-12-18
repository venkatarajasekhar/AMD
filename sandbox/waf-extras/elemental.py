#! /usr/bin/env python
# encoding: utf-8

"""
Module that is used to find where, if at all, the ELEMENTAL library is present

Usage:
When using this tool, the wscript will look like:
  def options(opt):
    opt.load('compiler_c compiler_cxx elemental')

  def configure(conf):
    conf.load('compiler_c compiler_cxx elemental')

  def build(bld):
    bld(source='main.cpp', target='app', use='ELEMENTAL')
"""

# Name to call this library in use variables
use_name = 'ELEMENTAL'

# List of shared library names to check for
libs = [
         'elemental',
         'pmrr',
       ]

# List of header files to check for
header_names = [
                    'elemental/core.hpp',
               ]

# List of static library names to check for
stlibs = libs

# List of paths to search for installation root, in order of preference
# (may be overridden by the '--cld2-root' and '--install-deps-prefix'
# command line options)
roots = [
            '~/usr/local',
            '~/Software',
        ]

# List of paths to search for shared libraries, in order of preference
libpaths = [root + '/lib' for root in roots]

# List of paths to search for static libraries, in order of preference
stlibpaths = libpaths

# List of include paths
includes = [root + '/include' for root in roots]


def options(opt):
  opt.add_option('--elemental-path',
                  type='string',
                  default='',
                  dest='elemental_path',
                  help='''path to the root of the elemental installation''')


from waflib.Configure import conf
@conf
def install_elemental(ctx):
    ctx.start_msg('Searching for CMake')
    ctx.find_program('cmake', target='CMAKE')
    ctx.end_msg(str(ctx.env.CMAKE), color='GREEN')

    from subprocess import call
    from os import chdir
    install_path = ctx.options.install_deps_prefix + '/src/elemental_0_83'
    install_node = ctx.root.find_node(install_path)
    if not install_node:
        tmp_path = '/tmp/elemental-0.83.tgz'
        tmp_node = ctx.root.find_node(tmp_path)
        if not tmp_node:
            ctx.start_msg('Searching for wget')
            ctx.find_program('wget', target='WGET')
            ctx.end_msg('ok ' + str(ctx.env.WGET), color='GREEN')
            ctx.start_msg('Searching for curl')
            ctx.find_program('curl', target='CURL')
            ctx.end_msg('ok ' + str(ctx.env.CURL), color='GREEN')
            ctx.msg('Downloading Elemental 0.83', '...', color='YELLOW')
            tmp_node = ctx.root.make_node(tmp_path)
            tmp_node.parent.mkdir()
            command = ctx.env.WGET + ['-L', '-o', tmp_node.abspath(),
               'http://libelemental.org/pub/releases/elemental-0.83.tgz']
            call(command)
        ctx.msg('Extracting Elemental libraries', '...', color='YELLOW')
        install_node = ctx.root.make_node(install_path)
        install_node.parent.mkdir()
        chdir(install_node.parent.abspath())
        call(['tar', 'xf', tmp_path])

    chdir(install_node.abspath())
    ctx.msg('Installing Elemental', '...', color='YELLOW')
    build_node = ctx.root.make_node(install_node.abspath() + '/build')
    build_node.parent.mkdir()
    chdir(build_node)
    command = ['cmake', \
        '-DCMAKE_INSTALL_PREFIX=' + ctx.options.install_deps_prefix]
    call(command)
    command = 'make'
    call(command)
    command = 'make install'
    call(command)

    chdir(ctx.srcnode.abspath())

def configure(ctx):
    if not ctx.env['CXX'] or not ctx.env['CC']:
        ctx.fatal('c/c++ compilers not loaded')

    global includes, libpaths, stlibpaths
    root = ctx.options.elemental_path
    if root:
        includes = [root + '/include']
        libpaths = [root + '/lib']
        stlibpaths = libpaths
    else:
        install_deps_prefix = ctx.options.install_deps_prefix
        includes = [install_deps_prefix + '/include'] + includes
        libpaths = [install_deps_prefix + '/lib'] + libpaths
        stlibpaths = libpaths

    ctx.load('common-tool', tooldir='waf-extras')

    if not ctx.check_includes(use_name=use_name,
                               header_names=header_names,
                               includes=includes):
        if ctx.options.install_deps:
            print('\n*** Could not find ' + use_name + ' includes' +
                  '\n*** Attempting to install ' + use_name + '...\n')
            ctx.install_elemental()
            print('\n*** Done installing ' + use_name + '\n')
            if not ctx.check_includes(use_name=use_name,
                                       header_names=header_names,
                                       includes=includes):
                ctx.fatal('Could not find ' + use_name + ' includes in ' +
                          str(includes) + ' after install ')
        else:
            ctx.fatal('\nCould not find ' + use_name + ' includes in ' +
                      'any of the default include paths:\n' +
                      '    ' + '\n    '.join(includes) +
                      '\n\nYou may provide an installation path using ' +
                      '--' + use_name.lower() + '-path,\nor download and ' +
                      'install the library using --install-deps\n')

    if not ctx.check_libs(use_name=use_name,
                           libs=libs,
                           libpaths=libpaths) and \
       not ctx.check_stlibs(use_name=use_name,
                             stlibs=stlibs,
                             stlibpaths=stlibpaths):
        ctx.fatal('\nCould not find ' + use_name + ' libraries  in ' +
                  'any of the default paths:\n' +
                  '    ' + '\n    '.join(set(libpaths + stlibpaths)) +
                  '\n\nYou may provide a CLD2 installation path using ' +
                  '--'+ use_name.lower() + '-path,\nor download and ' +
                  'install the library using --install-deps\n')

