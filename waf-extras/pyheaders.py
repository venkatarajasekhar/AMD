#! /usr/bin/env pyheaders
# encoding: utf-8

"""
Module that is used to find where, if at all, PYHEADERS lib is present

Usage:
When using this tool, the wscript will look like:
  def options(opt):
    opt.load('compiler_c compiler_cxx pyheaders')

  def configure(conf):
    conf.load('compiler_c compiler_cxx pyheaders')

  def build(bld):
    bld(source='main.cpp', target='app', use='PYHEADERS')
"""

import sys
import re
import platform
from waflib import Utils, Logs, Errors
from waflib.Configure import conf

def options(opt):
  '''
  Additional options that can be specified with using PYHEADERS libraries
  '''
  tpg = opt.get_option_group('Options for 3rd-party libraries')
  tpg.add_option('--pyheaders-root',
                  type='string',
                  default='/usr',
                  dest='pyheaders_root',
                  help='''path to the root of the pyheaders installation''')
  tpg.add_option('--pyheaders-version',
                  type='string',
                  default='2.7',
                  dest='pyheaders_version',
                  help='''version of Python that one wants to use''')

@conf
def check_pyheaders(self, *k, **kw):
  """
  Find and initialize PYHEADERS library variables for use

  The flags that are set include:

  INCLUDES_PYHEADERS

  """
  (major,minor,patchlevel) = platform.python_version_tuple()
  self.load('common-tool')
  self.check_includes(
                       use_name='PYHEADERS',
                       header_names=['pyconfig.h'],
                       includes=[
                                  self.options.pyheaders_root + '/include',
                                  '/usr/include',
                                  '/usr/include/python%s.%s'%(major,minor),
                                ]
                      )
  self.check_libs(
                  use_name='PYHEADERS',
                  libs=['python%s.%s'%(major,minor)],
                  libpaths=[
                            self.options.pyheaders_root + '/lib',
                                  '/usr/lib',
                                  '/usr/lib64',
                                  '/usr/lib/python%s.%s'%(major,minor),
                                  '/usr/lib64/python%s.%s'%(major,minor),
                           ]
                      )

def configure(ctx):
  ctx.check_pyheaders()
