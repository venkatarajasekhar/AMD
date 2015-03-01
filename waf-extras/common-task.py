#!/usr/bin/env python
# encoding utf-8
#

def options(ctx):
  pass


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

    linkflags = ''
    from waflib.Utils import unversioned_sys_platform
    if unversioned_sys_platform() == 'darwin':
        linkflags = '-all_load' # Needed for serialization macros to work

    includes = [top, '.']
    rpath = [node.abspath()
             for node in ctx.path.find_node(top)
                                 .get_bld()
                                 .ant_glob('nlpgo/', maxdepth=1, dir=True)]
    ctx.program(
        source          = sources,
        includes        = includes,
        target          = task_name,
        use             = use_libs,
        cxxflags        = ctx.env.GLOBAL_CXXFLAGS,
        linkflags       = linkflags,
        rpath           = rpath,
        export_includes = '.'
    )
