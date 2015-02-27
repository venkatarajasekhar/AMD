#!/usr/bin/env python
# encoding: utf-8
#
# Author: Shefaet Rahman

"""
Module that is used to find where, if at all, the PYTHONDEV library is present.

Usage:
When using this tool, the wscript will look like:
  def options(opt):
    opt.load('compiler_c compiler_cxx pythondev')

  def configure(conf):
    conf.load('compiler_c compiler_cxx pythondev')

  def build(bld):
    bld(source='main.cpp', target='app', use='PYTHONDEV')
"""

###############################################################################
#                             COMMAND-LINE OPTIONS
###############################################################################

def options(opt):
  grp = opt.get_option_group('Options for 3rd-party libraries')
  grp.add_option('--python-path',
                  type='string',
                  default='',
                  dest='python_path',
                  help='''path to the root of the PYTHONDEV installation''')
  grp.add_option('--python-version',
                  type='string',
                  default='2.7',
                  dest='python_version',
                  help='''version of Python that one wants to use''')


###############################################################################
#                               CONFIGURE COMMAND
###############################################################################

def configure(ctx):
    ctx.load('common-tool', tooldir='waf-extras')

    use_name = 'PYTHONDEV'
    (major, minor) = ctx.options.python_version.split('.')
    roots = ['', '/usr', '/usr/local']
    # If a path has been specified, limit search there
    if ctx.options.python_path:
        roots = [ctx.options.python_path]
    # Enumerate possible paths
    includes = [root + '/include' for root in roots] + \
               [root + '/include/python' + major+'.'+minor for root in roots]
    libpaths = [root + '/lib' for root in roots] + \
               [root + '/lib64' for root in roots] + \
               [root + '/lib/python' + major+'.'+minor for root in roots] + \
               [root + '/lib64/python' + major+'.'+minor for root in roots]
    stlibpaths = libpaths
    header_names = ['Python.h', 'pyconfig.h']
    libs = ['python' + major+'.'+minor]
    stlibs = libs

    ctx.check_dep(
        header_names       = header_names,
        includes           = includes,
        install_method     = None,
        install_path       = None,
        installed_includes = None,
        installed_libpaths = None,
        libpaths           = libpaths,
        libs               = libs,
        stlibpaths         = stlibpaths,
        stlibs             = stlibs,
        use_name           = use_name
    )
