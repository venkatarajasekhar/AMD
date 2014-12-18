#! /usr/bin/env python
# encoding: utf-8

"""
Module that is used to find includes and libs

Usage:
  def configure(conf):
    conf.load('compiler_c compiler_cxx common-tool')

    use_name = TEST

    header_names = [...]
    includes = [...]
    conf.check_includes(use_name=use_name, header_name=header_name, includes=includes)

    libs = [...]
    libpaths = [...]
    conf.check_libs(use_name=use_name, libs=libs, libpaths=libpaths)

    stlibs = [...]
    stlibpaths = [...]
    conf.check_stlibs(use_name=use_name, stlibs=stlibs, stlibpaths=stlibpaths)
"""

from waflib.Configure import conf
from waflib.Errors import ConfigurationError

@conf
def check_includes(self, use_name, header_names, includes):
  # 0. We have to ensure that we have a C++ compiler
  if not self.env['CXX'] or not self.env['CC']:
    self.fatal('load c/c++ compilers, conf.load("compiler_c compiler_cxx")')


  # Check for header files
  self.start_msg('Checking ' + use_name + ' includes')
  for incpath in includes:
    try:
      self.check_cxx(header_name=' '.join(header_names),
                     includes=incpath,
                     uselib_store=use_name)
    except ConfigurationError:
      continue
    break

  if self.env['INCLUDES_' + use_name]:
    self.end_msg('ok [' + str(self.env['INCLUDES_' + use_name][0]) + ']');
    return True
  else:
    self.end_msg('not found', color='YELLOW')
    return False


@conf
def check_libs(self, use_name, libs, libpaths):
    self.start_msg('Checking ' + use_name + ' shared libs')
    for libpath in libpaths:
        try:
            self.check_cxx(lib=' '.join(libs),
                           libpath=libpath,
                           rpath=libpath,
                           uselib_store=use_name,
                           use=use_name)
        except ConfigurationError:
            continue
        break

    if self.env['LIBPATH_' + use_name]:
        self.end_msg('ok [' + str(self.env['LIBPATH_' + use_name][0]) + ']')
        return True
    else:
        self.end_msg('not found', color='YELLOW')
        return False


@conf
def check_stlibs(self, use_name, stlibs, stlibpaths):
    self.start_msg('Checking ' + use_name + ' static libs')
    for stlibpath in stlibpaths:
        try:
            self.check_cxx(stlib=' '.join(stlibs),
                           stlibpath=stlibpath,
                           rpath=stlibpath,
                           uselib_store=use_name,
                           use=use_name)
        except ConfigurationError:
            continue
        break

    if self.env['STLIBPATH_' + use_name]:
        self.end_msg('ok [' + str(self.env['STLIBPATH_' + use_name][0]) + ']')
        return True
    else:
        self.end_msg('not found', color='YELLOW')
        return False
