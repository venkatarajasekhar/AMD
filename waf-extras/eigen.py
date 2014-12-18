#! /usr/bin/env python
# encoding: utf-8

"""
Module that is used to find where, if at all, EIGEN lib is present

Usage:
When using this tool, the wscript will look like:
  def options(opt):
    opt.load('compiler_c compiler_cxx eigen')

  def configure(conf):
    conf.load('compiler_c compiler_cxx eigen')

  def build(bld):
    bld(source='main.cpp', target='app', use='EIGEN')
"""

import sys
import re
from waflib import Utils, Logs, Errors
from waflib.Configure import conf

def options(opt):
  '''
  Additional options that can be specified with using EIGEN libraries
  '''
  opt.add_option('--eigen-root',
                  type='string',
                  default='/bb/bigstorn/news/nlpgo2/deps/include',
                  dest='eigen_root',
                  help='''path to the root of the eigen installation''')
  opt.add_option('--eigen-version',
                  type='string',
                  default='3.0.0',
                  dest='eigen_version',
                  help='''version of Eigen that one wants to use''')

@conf
def check_eigen(self, *k, **kw):
  """
  Find and initialize EIGEN library variables for use

  The flags that are set include:

  INCLUDES_EIGEN
  EIGEN_VERSION

  """

  # 0. We have to ensure that we have a C++ compiler
  if not self.env['CXX'] or not self.env['CC']:
    self.fatal('load c/c++ compilers, conf.load("compiler_c compiler_cxx")')

  # Now add everything to the EIGEN environment variables:
  self.env['INCLUDES_EIGEN']  = self.options.eigen_root
  eigen_version_file = 'Eigen/src/Core/util/Macros.h'

  # Check if the version file is present
  self.start_msg('Checking EIGEN version')
  try:
    self.check_cxx (header_name=eigen_version_file, use='EIGEN')
  except self.errors.ConfigurationError:
    self.to_log('Could not find EIGEN version file')
    self.fatal('Configuration error in finding Eigen')

  # If present, make sure that we have a version that is correct
  eigen_version_file_abs_path = \
              self.options.eigen_root + '/' + eigen_version_file
  with open(eigen_version_file_abs_path, 'r') as f: contents = f.read()
  found_world_string, found_major_string, found_minor_string = \
      re.findall(r'EIGEN_\w+_VERSION\s+([0-9]+)', contents, re.MULTILINE);

  expected_world_string, expected_major_string, expected_minor_string = \
                                re.split(r'\.', self.options.eigen_version)

  if int(found_world_string) < int(expected_world_string) or \
     int(found_major_string) < int(expected_major_string) or \
     int(found_minor_string) < int(expected_minor_string):
    self.to_log('Required EIGEN version (or greater) not found')
    self.fatal('Configuration error in finding Eigen')

  self.end_msg('%s.%s.%s' % \
               (found_world_string, found_major_string, found_minor_string))

def configure(ctx):
  ctx.check_eigen()
