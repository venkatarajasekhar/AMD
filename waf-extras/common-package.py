#!/usr/bin/env python
# encoding utf-8
#
# Author: Shefaet Rahman

def options(ctx):
    pass


from waflib import Logs
def configure(ctx):
    # Add internal dependencies to the list of paths to be built
    for dep in int_deps:
        ctx.env.append_unique('BLD_PATHS', dep)

    # Add 'USE_*' variables for external dependencies
    for dep in ext_deps:
        # Boost libraries are given as 'BOOST.*'
        # Put all the needed Boost packages in BOOST_PKGS
        if dep.upper().startswith('BOOST.'):
            ctx.env.append_unique('BOOST_PKGS', dep.split('.')[1].lower());
            ctx.env['USE_BOOST'] = True
        else:
            ctx.env['USE_' + dep.upper()] = True

    # BOOST unit test framework needed for tests
    if ctx.env.TESTS:
        ctx.env.append_unique('BOOST_PKGS', 'test');
        ctx.env['USE_BOOST'] = True



def build(ctx):
    # Get the list of all the source files
    sources = ctx.path.ant_glob(incl=['**/*.cc','**/*.cpp','**/*.c'],
                                excl=['**/*.t.cpp', '**/.#*.cpp'])

    # int_deps (of the form 'nlpgo/util') and ext_deps ('CLD2') are defined
    # in the calling wscripts for each package
    use_libs  = int_deps
    use_libs += [dep.upper() for dep in ext_deps
                             if not dep.upper().startswith('BOOST')]
    if ctx.env.USE_BOOST: use_libs += ['BOOST']

    # If building the sources in 'nlpgo/core' the resulting library will be
    # libnlpgo_core.{so, a}
    package_path = ctx.path.path_from(ctx.path.ctx.launch_node())

    includes = [top]
    if ctx.env.STATIC:
        ctx.stlib(source          = sources,
                  includes        = includes,
                  target          = package_path.replace('/', '_'),
                  name            = package_path,
                  use             = use_libs,
                  cxxflags        = ['-fPIC'],
                  export_includes =  '.')
    else:
        ctx.shlib(source          = sources,
                  includes        = includes,
                  target          = package_path.replace('/', '_'),
                  name            = package_path,
                  use             = use_libs,
                  export_includes =  '.')

    if ctx.env.TESTS:
        test_sources = ctx.path.ant_glob(incl=['*.t.cpp'])
        for test_source in test_sources:
            from os.path import basename
            target = basename(test_source.abspath()).replace('.t.cpp','_test')
            ctx.program(source   = basename(test_source.abspath()),
                        includes = includes,
                        target   = target,
                        use      = package_path + ' ' + ' '.join(use_libs),
                        defines  = ['BOOST_TEST_DYN_LINK'])
