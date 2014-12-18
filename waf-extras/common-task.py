#!/usr/bin/env python
# encoding utf-8
#

from waflib import Task, Logs, Utils, Errors
import re


def options(ctx):
  pass


def configure(ctx):
  # Turn on all the dependencies for this pacakge
  if Logs.verbose:
    if int_deps:
        Logs.pprint('YELLOW', 'INTERNAL-deps:' + ' '.join(int_deps).upper())
    if ext_deps:
        Logs.pprint('YELLOW', 'EXTERNAL-deps:' + ' '.join(ext_deps).upper())

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
                               excl=['**/*.t.cpp'])
             if not x.is_bld()]

  use_string = ' '.join(int_deps) + ' ' +  ' '.join(ext_deps).upper()
  if ctx.env.BOOST_PKGS: use_string += ' ' + 'BOOST'

  ctx.program(
    source=sources,
    includes=top,
    target=task_name,
    use=use_string,
    export_includes='.'
  )
