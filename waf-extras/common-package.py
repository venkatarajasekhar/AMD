#!/usr/bin/env python
# encoding utf-8
#

# Set the top dir and the build dir

use_libs = ' '.join(int_deps) + ' ' + ' '.join(ext_deps).upper()

from waflib import Task, Logs, Utils, Errors
import re


def options(ctx):
  pass


def configure(ctx):
  # Turn on all the dependencies for this pacakge
  if Logs.verbose:
    Logs.pprint('YELLOW', 'Internal deps:' + ' '.join(int_deps).upper())
    if ctx.env.TESTS:
      Logs.pprint('YELLOW', 'BOOST-deps:TEST')

  # BOOST needed for tests
  if ctx.env.TESTS:
    ctx.env["USE_BOOST"] = True
    ctx.env.append_unique('BOOST_PKGS', 'test');

  for dep in int_deps:
    ctx.env.append_unique('BLD_PATHS', dep)

  for dep in ext_deps:
    if dep.upper().startswith('BOOST.'):
        ctx.env["USE_BOOST"] = True
        ctx.env.append_unique('BOOST_PKGS', dep.split('.')[1].lower());
    else:
        ctx.env["USE_" + dep.upper()] = True


def build(ctx):
  # 1. Get the list of all the source files
  sources = [x for x in
             ctx.path.ant_glob(incl=['**/*.cc','**/*.cpp','**/*.c'],
                               excl=['**/*.t.cpp', '**/.#*.cpp'])
             if not x.is_bld()]

  package_path = ctx.path.path_from(ctx.path.ctx.launch_node())
  paths = []
  paths.append(top)
  if 'shared' == ctx.env.LIBRARY_TYPE:
    ctx.shlib(source=sources,
              includes=paths,
              target=package_path.replace('/', ''),
              name=package_path,
              use=use_libs + ' BOOST',
              export_includes='.')
  else:
    ctx.stlib(source=sources,
              includes=paths,
              target=package_path.replace('/', ''),
              name=package_path,
              use=use_libs + ' BOOST',
              cxxflags='-fPIC',
              export_includes='.')

  if ctx.env.TESTS:
    # 2. Get the list of all the test files
    tests = [x for x in ctx.path.ant_glob(incl=['*.t.cpp'])
             if not x.is_bld()]
    if tests:
      for test in tests:
        test_file_name = test.abspath()[test.abspath().rfind('/')+1:]
        test_target_name = re.sub('.t.cpp','', test_file_name)
        ctx.program(source='%s'%(test_file_name),
                    includes=paths,
                    target='%s'%(test_target_name),
                    use=package_path + ' ' + use_libs + ' BOOST',
                    defines=['BOOST_TEST_DYN_LINK'])
