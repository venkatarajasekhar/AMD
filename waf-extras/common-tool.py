#! /usr/bin/env python
# encoding: utf-8
#

from waflib.Configure import conf
from waflib.Errors    import ConfigurationError

@conf
def find_dep_program(ctx, program, var=None):
    if not var:
        var = program.upper()
    if ctx.env[var]:
        return
    try:
        ctx.find_program(program, var=var)
    except:
        ctx.fatal("\nThe program '" + program + "' is required " +
                  ", but could not be located.\nPlease install '" + program + "'" +
                  " and/or provide its location using the " + var +
                  " environment variable.\n")


@conf
def exec_with_msg(ctx, command, start_msg, end_msg="done", shell=False):
    from os import devnull
    from subprocess import call
    ctx.start_msg(start_msg)
    verbosity = ctx.options.verbose
    if verbosity == 0:
        call(command, stdout=open(devnull, 'wb'), stderr=open(devnull, 'wb'),
             shell=shell)
        ctx.end_msg(end_msg)
    else:
        ctx.end_msg('...')
        call(command, shell=shell)


@conf
def fatal_missing_dep(
    ctx,
    files,
    kind,
    use_name,
    paths=None,
    suggest_install=True
):
    msg = '\nCould not find valid ' + use_name + ' ' + kind + 's'
    if files:
        msg += ':\n\n' + '  - ' + \
               '\n  - '.join(files) + '\n\n'
    if paths:
        msg += ' in any of the following paths:\n\n' + '  - ' + \
               '\n  - '.join(paths)
    else:
        msg += '.\n'
    msg += '\n\nYou may provide an installation path using the ' + \
           '`--' + use_name.lower() + '-path` configure flag'
    if suggest_install:
        msg += ',\nor download and install the library using the ' + \
               '`--install-deps` flag.\n'
    else:
        msg += '.\n'
    ctx.fatal(msg)


@conf
def find_all_files(ctx, files, path, patterns=['%s']):
    patterns = list(set(patterns))
    ctx.to_log('\nSearching for ' + str(patterns) +  ' % ' + str(files) +
               ' in ' + path)
    for f in files:
        found = False
        for pattern in patterns:
            filename = pattern % f
            node = ctx.root.find_node(path + '/' + filename)
            if node:
                found = True
                break
        if not found:
            ctx.to_log(f + ' not found')
            raise ConfigurationError
    ctx.to_log('Found all files')


@conf
def check_includes(ctx, use_name, use_libs, header_names, includes):
    ctx.start_msg('Checking for ' + use_name + ' headers')

    # Checking for a valid include path involves first checking to make sure
    # that the files exist in the given path, then checking that a code
    # fragment that includes the required headers compiles given that path.
    for incpath in includes:
        try:
            ctx.find_all_files(header_names, incpath)
            ctx.check_cxx(header_name  = ' '.join(header_names),
                          use          = use_libs,
                          includes     = incpath,
                          uselib_store = use_name)

            for use_lib in use_libs:
                ctx.env.append_unique('INCLUDES_' + use_name,
                                      ctx.env['INCLUDES_' + use_lib])
            ctx.end_msg('ok [' + incpath + ']', color='GREEN')
            return True
        except ConfigurationError:
            continue

    # If that didn't work, try the default compiler path (+ whatever use_libs
    # brings in)
    try:
        ctx.to_log('Checking in default compiler path')
        ctx.check_cxx(header_name  = ' '.join(header_names),
                      use          = use_libs,
                      uselib_store = use_name)
        for use_lib in use_libs:
            ctx.env.append_unique('INCLUDES_' + use_name,
                                  ctx.env['INCLUDES_' + use_lib])
        ctx.end_msg('ok', color='GREEN')
        return True
    except ConfigurationError:
        pass

    ctx.end_msg('not found', color='YELLOW')
    return False


@conf
def check_libs(ctx, use_name, use_libs, libs, libpaths, shared):
    if shared:
        ctx.start_msg('Checking for ' + use_name + ' shared libs')
    else:
        ctx.start_msg('Checking for ' + use_name + ' static libs')

    # Checking for a valid lib path involves first checking to make sure
    # that the files exist in the given path, then checking that a code
    # fragment that links the required libraries compiles given that path.
    for libpath in libpaths:
        try:
	    ctx.find_all_files(libs, libpath,
                               patterns=[ctx.env.cxxshlib_PATTERN,
                                         ctx.env.cxxstlib_PATTERN,
                                         'lib%s.a', 'lib%s.so'])
            if shared:
                ctx.check_cxx(lib          = ' '. join(libs),
                              libpath      = libpath,
                              rpath        = libpath,
                              use          = use_libs,
                              uselib_store = use_name)
            else:
                ctx.check_cxx(stlib        = ' '. join(libs),
                              stlibpath    = libpath,
                              rpath        = libpath,
                              use          = use_libs,
                              uselib_store = use_name)

            for use_lib in use_libs:
                ctx.env.append_unique('LIB_' + use_name,
                                      ctx.env['LIB_' + use_lib])
                ctx.env.append_unique('STLIB_' + use_name,
                                      ctx.env['STLIB_' + use_lib])
                ctx.env.append_unique('LIBPATH_' + use_name,
                                      ctx.env['LIBPATH_' + use_lib])
                ctx.env.append_unique('STLIBPATH_' + use_name,
                                      ctx.env['STLIBPATH_' + use_lib])
                ctx.env.append_unique('RPATH_' + use_name,
                                      ctx.env['RPATH_' + use_lib])
            ctx.end_msg('ok [' + libpath + ']', color='GREEN')
            return True
        except ConfigurationError:
            continue

    # If that didn't work, try the default compiler path (+ whatever use_libs
    # brings in)
    try:
        ctx.to_log('Checking in default compiler path')
        if shared:
            ctx.check_cxx(lib          = ' '.join(libs),
                          use          = use_libs,
                          uselib_store = use_name)
        else:
            ctx.check_cxx(stlib        = ' '.join(libs),
                          use          = use_name,
                          uselib_store = use_name)

        ctx.end_msg('ok', color='GREEN')
        return True
    except ConfigurationError:
        pass

    ctx.end_msg('not found', color='YELLOW')
    return False


@conf
def check_dep(
    ctx,
    header_names,
    includes,
    install_method,
    install_path,
    libpaths,
    libs,
    stlibpaths,
    stlibs,
    use_name,
    use_libs = None,
    installed_includes = None,
    installed_libpaths = None
):
    if use_libs:
        use_libs = [use_name] + use_libs
    else:
        use_libs = use_name

    # If the install paths are specified, only search there
    # Otherwise, search the deps folder then other paths
    if installed_includes:
        includes = [installed_includes]
    elif install_path:
        includes = [install_path + '/include']
    else:
        includes = [ctx.env.DEPS_INCLUDE] + includes

    if installed_libpaths:
        libpaths   = [installed_libpaths]
        stlibpaths = [installed_libpaths]
    elif install_path:
        libpaths   = [install_path + '/lib']
        stlibpaths = [install_path + '/lib']
    else:
        libpaths   = [ctx.env.DEPS_LIB] + libpaths
        stlibpaths = [ctx.env.DEPS_LIB] + stlibpaths

    # First, check for headers
    found_includes = ctx.check_includes(use_name     = use_name,
                                        use_libs     = use_libs,
                                        header_names = header_names,
                                        includes     = includes)

    # If includes were not found, try to install
    ctx.env['ATTEMPTED_' + use_name + '_INSTALL'] = False
    if not found_includes:
        if ctx.env.INSTALL_DEPS and install_method:
            ctx.env['ATTEMPTED' + use_name + '_INSTALL'] = True
            install_method()
        else:
            ctx.fatal_missing_dep(use_name        = use_name,
                                  kind            = 'header',
                                  paths           = includes,
                                  files           = header_names,
                                  suggest_install = True)

        # Check for headers again
        found_includes = ctx.check_includes(use_name     = use_name,
                                            use_libs     = use_libs,
                                            header_names = header_names,
                                            includes     = includes)

        # If headers still not found, fail with a msg
        if not found_includes:
            ctx.fatal_missing_dep(use_name        = use_name,
                                  kind            = 'header',
                                  paths           = includes,
                                  files           = header_names,
                                  suggest_install = False)

    # Now check for libraries
    # First, check for shared libraries
    found_libs = ctx.check_libs(use_name = use_name,
                                use_libs = use_libs,
                                libs     = libs,
                                libpaths = libpaths,
                                shared   = True)
    # If not found, check for static libs
    if not found_libs:
        found_stlibs = ctx.check_libs(use_name   = use_name,
                                      use_libs   = use_libs,
                                      libs       = stlibs,
                                      libpaths   = stlibpaths,
                                      shared     = False)

    # If libs or stlibs were not found attempt install (if not already tried)
    if not found_libs and not found_stlibs:
        if ctx.env.INSTALL_DEPS and install_method and not \
           ctx.env['ATTEMPTED_' + use_name + '_INSTALL']:
            ctx.env['ATTEMPTED_' + use_name + '_INSTALL']= True
            install_method()
        else:
            ctx.fatal_missing_dep(use_name        = use_name,
                                  kind            = 'lib',
                                  paths     = list(set(libpaths + stlibpaths)),
                                  files           = libs,
                                  suggest_install = True)

        # Check for shared libs again
        found_libs = ctx.check_libs(use_name = use_name,
                                    use_libs = use_libs,
                                    libs     = libs,
                                    libpaths = libpaths,
                                    shared   = True)
        # If not found, check for static libs
        if not found_libs:
            found_stlibs = ctx.check_libs(use_name = use_name,
                                          use_libs = use_libs,
                                          libs     = stlibs,
                                          libpaths = stlibpaths,
                                          shared   = False)

        # If libs still not found, fail with a msg
        if not found_libs and not found_stlibs:
            ctx.fatal_missing_dep(use_name        = use_name,
                                  kind            = 'lib',
                                  paths     = list(set(libpaths + stlibpaths)),
                                  files           = libs,
                                  suggest_install = False)


@conf
def check_dep_requirement(
    ctx,
    header_names,
    install_method,
    libs,
    requirement,
    stlibs,
    use_name,
    use_libs = None
):

    # If the requirement is met,  nothing to do
    if requirement():
        return

    # If the user doesn't want us to install deps or already tried, fail
    if not ctx.env.INSTALL_DEPS or not install_method:
        ctx.fatal_missing_dep(use_name = use_name, kind = 'installation',
           files = None,
           suggest_install = not ctx.env['ATTEMPTED_' + use_name + '_INSTALL'])

    # Otherwise try to install
    install_method()

    # Recheck the depenendency
    ctx.check_dep(
        header_names       = header_names,
        includes           = None,
        install_method     = None,
        install_path       = ctx.env.DEPS_PREFIX,
        installed_includes = None,
        installed_libpaths = None,
        libpaths           = None,
        libs               = libs,
        stlibpaths         = None,
        stlibs             = stlibs,
        use_name           = use_name,
        use_libs           = use_libs
    )

    # Try requirement again: if failed, then fatal exit
    if not requirement():
        ctx.fatal_missing_dep(use_name        = use_name,
                              kind            = 'installation',
                              suggest_install = False)
