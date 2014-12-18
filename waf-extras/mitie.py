#! /usr/bin/env python
# encoding: utf-8

"""
Module that is used to find where, if at all, the MITIE library is present

Usage:
When using this tool, the wscript will look like:
  def options(opt):
    opt.load('compiler_c compiler_cxx mitie')

  def configure(conf):
    conf.load('compiler_c compiler_cxx mitie')

  def build(bld):
    bld(source='main.cpp', target='app', use='MITIE')
"""

# Name to call this library in use variables
use_name = 'MITIE'

# List of shared library names to check for
libs = ['mitie']

# List of header files to check for
header_names = ['mitie/ner_trainer.h',
                'dlib/svm.h']

# List of static library names to check for
stlibs = libs

# List of paths to search for installation root, in order of preference
# (may be overridden by the '--cld2-root' and '--install-deps-prefix'
# command line options)
roots = [
            '~/usr/local',
            '~/Software',
            '/bb/bigstorn/news/nlpgo2/deps',
        ]

# List of paths to search for shared libraries, in order of preference
libpaths = [root + '/lib' for root in roots]

# List of paths to search for static libraries, in order of preference
stlibpaths = libpaths

# List of include paths
includes = [root + '/include' for root in roots]


def options(opt):
  opt.add_option('--mitie-path',
                  type='string',
                  default='',
                  dest='mitie_path',
                  help='''path to the root of the mitie installation''')


from waflib.Configure import conf
@conf
def install_mitie(ctx):
    ctx.start_msg('Searching for git')
    ctx.find_program('git', target='GIT')
    ctx.end_msg(str(ctx.env.GIT), color='GREEN')

    ctx.msg('Cloning MITIE repository', '...', color='YELLOW')
    # Download folder
    dl_node = ctx.root.make_node(ctx.options.install_deps_prefix +
                                 '/src/MITIE')
    command = (ctx.env.GIT + ['clone',
                              'https://github.com/mit-nlp/MITIE.git',
                              '--depth=1',
                              dl_node.abspath()])
    from subprocess import call
    call(command)

    ctx.msg('Making MITIE libs', '...', color='YELLOW')
    print('cd ' + dl_node.abspath() + '/mitielib')
    from os import chdir
    chdir(dl_node.abspath() + '/mitielib')
    call(['make'])

    ctx.msg('Installing MITIE', '...', color='YELLOW')
    # Libs folder
    lib_node = ctx.root.make_node(ctx.options.install_deps_prefix + '/lib')
    lib_node.mkdir()
    command = ['mv', 'libmitie.a', 'libmitie.so', lib_node.abspath()]
    print(' '.join(command))
    call(command)

    # Includes folder
    inc_node = ctx.root.make_node(ctx.options.install_deps_prefix + '/include')
    inc_node.mkdir()
    commands = [['cp', '-r', 'include/mitie.h', 'include/mitie', inc_node.abspath()],
                ['cp', '-r', '../dlib/dlib', inc_node.abspath()]]
    for command in commands:
        print(' '.join(command))
        call(command)


def configure(ctx):
    if not ctx.env['CXX'] or not ctx.env['CC']:
        ctx.fatal('c/c++ compilers not loaded')

    global includes, libpaths, stlibpaths
    root = ctx.options.mitie_path
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
            ctx.install_mitie()
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

