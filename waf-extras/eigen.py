#!/usr/bin/env python
# encoding: utf-8
#

"""
Module that is used to find where, if at all, the EIGEN library is present.

Usage:
When using this tool, the wscript will look like:
  def options(opt):
    opt.load('compiler_c compiler_cxx eigen')

  def configure(conf):
    conf.load('compiler_c compiler_cxx eigen')

  def build(bld):
    bld(source='main.cpp', target='app', use='EIGEN')
"""

###############################################################################
#                                TOOL PARAMETERS
###############################################################################

# Name to call this library in use variables
USE_NAME = 'EIGEN'

# List of shared library names to check for
LIBS = []

# List of static library names to check for
STLIBS = LIBS

# List of header files to check for
HEADER_NAMES = ['Eigen/Core']

# List of paths to search for shared libraries, in order of preference
LIBPATHS = []

# List of paths to search for static libraries, in order of preference
STLIBPATHS = LIBS

# List of include paths
INCLUDES = ['/include', '/usr/include', '/usr/local/include']


###############################################################################
#                             COMMAND-LINE OPTIONS
###############################################################################

def options(opt):
  grp = opt.get_option_group('Options for 3rd-party libraries')
  grp.add_option('--eigen-path',
                  type='string',
                  default='',
                  dest='eigen_path',
                  help='''path to the root of the EIGEN installation''')


###############################################################################
#                      DOWNLOAD AND INSTALLATION PROCEDURE
###############################################################################

from os               import chdir
from os               import devnull
from subprocess       import call
from waflib.Configure import conf
from waflib.Logs      import pprint
from waflib.Utils     import unversioned_sys_platform

@conf
def download_eigen(ctx):
    # First, check if the source has already been downloaded
    download_path = ctx.env.DEPS_DOWNLOADS + '/3.2.4.tar.gz'
    ctx.start_msg('Searching for ' + USE_NAME + ' download')
    download_node = ctx.root.find_node(download_path)
    if not download_node:
        # If it hasn't been downloaded already, download it
        ctx.end_msg('not found', color='YELLOW')
        ctx.find_dep_program('curl')
        ctx.msg('Downloading ' + USE_NAME, '...', color='YELLOW')
        command = ctx.env.CURL + ['--progress-bar', '--create-dirs', '-Lf',
                                  '-o', download_path,
                'http://bitbucket.org/eigen/eigen/get/3.2.4.tar.gz']
        call(command, bufsize=0)
    else:
        ctx.end_msg('ok')
    # By this point, we should have the .tgz
    # Begin by creating the directory where the sources will be extracted
    source_dir_node = ctx.root.make_node(ctx.env.DEPS_SRC)
    source_dir_node.mkdir()
    # Then untar the download into it
    ctx.find_dep_program('tar')
    command = ctx.env.TAR + ['-xf', download_path, '-C', ctx.env.DEPS_SRC]
    msg = 'Extracting ' + USE_NAME + ' sources'
    ctx.exec_with_msg(command, msg)


@conf
def install_eigen(ctx):
    pprint('YELLOW', 'Attempting ' + USE_NAME + ' auto-installation ...')

    # First, search for the source files
    source_path = ctx.env.DEPS_SRC + '/eigen-eigen-10219c95fe65'
    ctx.start_msg('Searching for ' + USE_NAME + ' sources')
    source_node = ctx.root.find_node(source_path)
    # If sources not found, try to download them
    if not source_node:
        ctx.end_msg('not found', color='YELLOW')
        ctx.download_eigen()
    else:
        ctx.end_msg('ok')

    pprint('YELLOW', '... ' + USE_NAME + ' auto-installation complete.')


###############################################################################
#                               CONFIGURE COMMAND
###############################################################################

def configure(ctx):
    ctx.check_dep(
        header_names       = HEADER_NAMES,
        includes           = INCLUDES,
        install_method     = ctx.install_eigen,
        install_path       = ctx.options.eigen_path,
        libpaths           = LIBPATHS,
        libs               = LIBS,
        stlibpaths         = STLIBPATHS,
        stlibs             = STLIBS,
        use_name           = USE_NAME,
        use_libs           = None,
        installed_includes = ctx.env.DEPS_SRC + '/eigen-eigen-10219c95fe65',
        installed_libpaths = None
    )
