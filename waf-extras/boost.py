#!/usr/bin/env python
# encoding: utf-8
#
# partially based on boost.py written by Gernot Vormayr
# written by Ruediger Sonderfeld <ruediger@c-plusplus.de>, 2008
# modified by Bjoern Michaelsen, 2008
# modified by Luca Fossati, 2008
# rewritten for waf 1.5.1, Thomas Nagy, 2008
# rewritten for waf 1.6.2, Sylvain Rouquette, 2011

'''

This is an extra tool, not bundled with the default waf binary.
To add the boost tool to the waf file:
$ ./waf-light --tools=compat15,boost
  or, if you have waf >= 1.6.2
$ ./waf update --files=boost

When using this tool, the wscript will look like:

  def options(opt):
    opt.load('compiler_cxx boost')

  def configure(conf):
    conf.load('compiler_cxx boost')
    conf.check_boost(lib='system filesystem')

  def build(bld):
    bld(source='main.cpp', target='app', use='BOOST')

Options are generated, in order to specify the location of boost
includes/libraries.  The `check_boost` configuration function allows to specify
the used boost libraries.  It can also provide default arguments to the
--boost-mt command-line arguments.  Everything will be packaged together in a
BOOST component that you can use.

When using MSVC, a lot of compilation flags need to match your BOOST build
configuration:
 - you may have to add /EHsc to your CXXFLAGS or define boost::throw_exception
   if BOOST_NO_EXCEPTIONS is defined.  Errors: C4530
 - boost libraries will try to be smart and use the (pretty but often not
   useful) auto-linking feature of MSVC So before calling `conf.check_boost`
   you might want to disabling by adding conf.env.DEFINES_BOOST +=
   ['BOOST_ALL_NO_LIB'] Errors:
 - boost might also be compiled with /MT, which links the runtime statically.
   If you have problems with redefined symbols, self.env['DEFINES_%s' % var] +=
   ['BOOST_ALL_NO_LIB'] self.env['CXXFLAGS_%s' % var] += ['/MD', '/EHsc']
   Passing `--boost-linkage_autodetect` might help ensuring having a correct
   linkage in some basic cases.

'''

import sys
import re
from waflib import Utils, Logs, Errors
from waflib.Configure import conf
from waflib.TaskGen import feature, after_method

BOOST_LIBS = ['/bb/bigstorn/news/nlpgo2/deps/lib',
              '/usr/lib/x86_64-linux-gnu',
              '/usr/lib/i386-linux-gnu',
              '/usr/lib',
              '/usr/local/lib',
              '/opt/local/lib',
              '/sw/lib',
              '/lib']
BOOST_INCLUDES = ['/bb/bigstorn/news/nlpgo2/deps/include',
                  '/usr/include',
                  '/usr/local/include',
                  '/opt/local/include',
                  '/sw/include']
BOOST_VERSION_FILE = 'boost/version.hpp'
BOOST_VERSION_CODE = '''
#include <iostream>
#include <boost/version.hpp>
int main() { std::cout << BOOST_LIB_VERSION << std::endl; }
'''

BOOST_ERROR_CODE = '''
#include <boost/system/error_code.hpp>
int main() { boost::system::error_code c; }
'''

BOOST_THREAD_CODE = '''
#include <boost/thread.hpp>
int main() { boost::thread t; }
'''

# toolsets from {boost_dir}/tools/build/v2/tools/common.jam
PLATFORM = Utils.unversioned_sys_platform()
detect_intel = lambda env: (PLATFORM == 'win32') and 'iw' or 'il'
detect_clang = lambda env: (PLATFORM == 'darwin') and 'clang-darwin' or 'clang'
detect_mingw = lambda env: (re.search('MinGW', env.CXX[0])) and 'mgw' or 'gcc'
BOOST_TOOLSETS = {
  'borland':  'bcb',
  'clang':  detect_clang,
  'como':  'como',
  'cw':    'cw',
  'darwin':   'xgcc',
  'edg':    'edg',
  'g++':    detect_mingw,
  'gcc':    detect_mingw,
  'icpc':  detect_intel,
  'intel':  detect_intel,
  'kcc':    'kcc',
  'kylix':  'bck',
  'mipspro':  'mp',
  'mingw':  'mgw',
  'msvc':  'vc',
  'qcc':    'qcc',
  'sun':    'sw',
  'sunc++':   'sw',
  'tru64cxx': 'tru',
  'vacpp':  'xlc'
}


def options(opt):
  opt.add_option('--boost-root',
                 type='string',
                 default='',
                 dest='boost_root',
                 help='Root directory for the boost installation')
  opt.add_option('--boost-includes',
                 type='string',
                 default='',
                 dest='boost_includes',
                 help='''\
path to the boost includes root (~boost root) e.g. /path/to/boost_1_57_0''')
  opt.add_option('--boost-libs',
                 type='string',
                 default='',
                 dest='boost_libs',
                 help='''\
path to the directory where the boost libs are e.g. \
/path/to/boost_1_57_0/stage/lib''')
  opt.add_option('--boost-mt', action='store_true',
                 default=False, dest='boost_mt',
                 help='select multi-threaded libraries')
  opt.add_option('--boost-abi',
                 type='string',
                 default='',
                 dest='boost_abi',
           help='''\
select libraries with tags (gd for debug, static is automatically added),
           see doc Boost, Getting Started, chapter 6.1''')
  opt.add_option('--boost-linkage_autodetect',
                 action="store_true",
                 dest='boost_linkage_autodetect',
                 help='''\
auto-detect boost linkage options (don't get used to it / might break other \
stuff)''')
  opt.add_option('--boost-toolset', type='string',
           default='', dest='boost_toolset',
           help='force a toolset e.g. msvc, vc90, \
            gcc, mingw, mgw45 (default: auto)')
  py_version = '%d%d' % (sys.version_info[0], sys.version_info[1])
  opt.add_option('--boost-python', type='string',
           default=py_version, dest='boost_python',
           help='select the lib python with this version \
            (default: %s)' % py_version)


@conf
def __boost_get_version_file(self, d):
  if not d:
    return None
  dnode = self.root.find_dir(d)
  if dnode:
    return dnode.find_node(BOOST_VERSION_FILE)
  return None


@conf
def boost_get_version(self, d):
  """silently retrieve the boost version number"""
  node = self.__boost_get_version_file(d)
  if node:
    try:
      txt = node.read()
    except (OSError, IOError):
      Logs.error("Could not read the file %r" % node.abspath())
    else:
      re_but = re.compile('^#define\\s+BOOST_LIB_VERSION\\s+"(.*)"', re.M)
      m = re_but.search(txt)
      if m:
        return m.group(1)
  return self.check_cxx(fragment=BOOST_VERSION_CODE, includes=[d], execute=True, define_ret=True)


@conf
def boost_get_includes(self, *k, **kw):
  includes = k and k[0] or kw.get('includes', None)
  if includes and self.__boost_get_version_file(includes):
    return includes
  for d in self.environ.get('INCLUDE', '').split(';') + BOOST_INCLUDES:
    if self.__boost_get_version_file(d):
      return d
  if includes:
    self.end_msg('not found', color='YELLOW')
    self.fatal('\nBoost headers could not be found in \'' + includes + '\''
               '\nYou may install Boost using the --install-deps flag\n')
  else:
    self.end_msg('not found', color='YELLOW')
    self.fatal('\nBoost headers could not be found in the default paths:\n  ' +
               '\n  '.join(BOOST_INCLUDES) +
               '\n\nYou may provide an include path using --boost-includes ' +
               'or --boost-root,\nor install Boost using --install-deps\n')


@conf
def boost_get_toolset(self, cc):
  toolset = cc
  if not cc:
    build_platform = Utils.unversioned_sys_platform()
    if build_platform in BOOST_TOOLSETS:
      cc = build_platform
    else:
      cc = self.env.CXX_NAME
  if cc in BOOST_TOOLSETS:
    toolset = BOOST_TOOLSETS[cc]
  return isinstance(toolset, str) and toolset or toolset(self.env)


@conf
def __boost_get_libs_path(self, *k, **kw):
  ''' return the lib path and all the files in it '''
  if 'files' in kw:
    return self.root.find_dir('.'), Utils.to_list(kw['files'])
  libs = k and k[0] or kw.get('libs', None)
  if libs:
    path = self.root.find_dir(libs)
    if not path:
      self.end_msg('libs directory does not exist %s' % libs)
      self.fatal('The configuration failed')
    files = path.ant_glob('*boost_*')
  if not libs or not files:
    for d in self.environ.get('LIB', '').split(';') + BOOST_LIBS:
      if not d:
        continue
      path = self.root.find_dir(d)
      if path:
        files = path.ant_glob('*boost_*')
        if files:
          break
      path = self.root.find_dir(d + '64')
      if path:
        files = path.ant_glob('*boost_*')
        if files:
          break
  if not path:
      if libs:
        self.end_msg('not found', color='YELLOW')
        self.fatal('\nBoost libraries could not be found in \'' + libs + '\''
                   '\nYou may install Boost using the --install-deps flag\n')
      else:
        self.end_msg('not found', color='YELLOW')
        self.fatal('\nBoost libraries could not be found in ' +
                   'any of the default paths:\n  ' +
                   '\n  '.join(BOOST_LIBS) +
                   '\n\nYou may provide a search path using --boost-libs ' +
                   'or --boost-root,\nor install Boost using --install-deps\n')

  self.to_log('Found the boost path in %r with the libraries:' % path)
  for x in files:
    self.to_log('    %r' % x)
  return path, files

@conf
def boost_get_libs(self, *k, **kw):
  '''
  return the lib path and the required libs
  according to the parameters
  '''
  path, files = self.__boost_get_libs_path(**kw)
  toolset = self.boost_get_toolset(kw.get('toolset', ''))
  toolset_pat = '(-%s[0-9]{0,3})' % toolset
  version = '(-%s)' % self.env.BOOST_VERSION

  def find_lib(re_lib, files):
    for file in files:
      if re_lib.search(file.name):
        self.to_log('Found boost lib %s' % file)
        return file
    return None

  def format_lib_name(name):
    if name.startswith('lib') and self.env.CC_NAME != 'msvc':
      name = name[3:]
    return name[:name.rfind('.')]

  def match_libs(lib_names, is_static):
    libs = []
    lib_names = Utils.to_list(lib_names)
    if not lib_names:
      return libs
    t = []
    if kw.get('mt', False):
      t.append('(-mt)')
    if kw.get('abi', None):
      t.append('(%s%s)' % (is_static and '-s' or '-', kw['abi']))
    elif is_static:
      t.append('(-s)')
    tags_pat = t and ''.join(t) or ''
    for lib in lib_names:
      if lib == 'python':
        tags = '%s?(-py%s)?' % (tags_pat, kw['python'])
      else:
        tags = tags_pat
      # Trying libraries, from most strict match to least one
      for pattern in ['boost_%s%s%s%s' % (lib, toolset_pat, tags, version),
              'boost_%s%s%s' % (lib, tags, version),
              # Give up trying to find the right version
              'boost_%s%s%s' % (lib, toolset_pat, tags),
              'boost_%s%s' % (lib, tags),
              'boost_%s' % lib]:
        self.to_log('Trying pattern %s' % pattern)
        file = find_lib(re.compile(pattern), files)
        if file:
          libs.append(format_lib_name(file.name))
          break
      else:
        self.end_msg('lib \'%s\' not found' % (lib))
        self.fatal('\nThe required Boost library \'' + lib +
                   '\' could not be found in \n' + path.abspath() +
                   '\n\nYou may provide an alternate search path using ' +
                   '--boost-libs ' + 'or --boost-root,\n' +
                   'or install the required library using --install-deps\n')
    return libs

  return  path.abspath(), \
      match_libs(kw.get('lib', None), False), \
      match_libs(kw.get('stlib', None), True)

@conf
def is_compiled(self, lib):
  '''
  Simply checks against a pre-defined list of libraries that are compiled
  or are not compiled.
  '''
  compiled_lib_map = { 'accumulators'            : False,
                       'algorithm'               : False,
                       'align'                   : False,
                       'any'                     : False,
                       'array'                   : False,
                       'asio'                    : False,
                       'assert'                  : False,
                       'assign'                  : False,
                       'atomic'                  : True,
                       'bimap'                   : False,
                       'bind'                    : False,
                       'call_traits'             : False,
                       'chrono'                  : True,
                       'circular_buffer'         : False,
                       'compatibility'           : False,
                       'compressed_pair'         : False,
                       'concept_check'           : False,
                       'config'                  : False,
                       'container'               : False,
                       'context'                 : True,
                       'conversion'              : False,
                       'core'                    : False,
                       'coroutine'               : True,
                       'crc'                     : False,
                       'date_time'               : True,
                       'dynamic_bitset'          : False,
                       'exception'               : True,
                       'enable_if'               : False,
                       'filesystem'              : True,
                       'flyweight'               : False,
                       'foreach'                 : False,
                       'format'                  : False,
                       'function'                : False,
                       'function_types'          : False,
                       'functional'              : False,
                       'fusion'                  : False,
                       'gil'                     : False,
                       'geometry'                : True,
                       'graph'                   : True,
                       'graph_parallel'          : True,
                       'heap'                    : False,
                       'icl'                     : False,
                       'identity_type'           : False,
                       'integer'                 : False,
                       'interprocess'            : False,
                       'interval'                : False,
                       'intrusive'               : False,
                       'in_place_factory'        : False,
                       'typed_in_place_factory'  : False,
                       'iostreams'               : True,
                       'iterator'                : False,
                       'lambda'                  : False,
                       'lexical_cast'            : False,
                       'local_function'          : False,
                       'locale'                  : True,
                       'lockfree'                : False,
                       'log'                     : True,
                       'math'                    : True,
                       'minmax'                  : False,
                       'move'                    : False,
                       'mpi'                     : True,
                       'mpl'                     : True,
                       'multi_array'             : False,
                       'multi_index'             : False,
                       'multiprecision'          : False,
                       'odeint'                  : False,
                       'operators'               : False,
                       'optional'                : False,
                       'parameter'               : False,
                       'phoenix'                 : False,
                       'predef'                  : False,
                       'polygon'                 : False,
                       'pool'                    : False,
                       'preprocessor'            : False,
                       'program_options'         : True,
                       'property_map'            : False,
                       'property_tree'           : False,
                       'proto'                   : False,
                       'python'                  : True,
                       'random'                  : True,
                       'range'                   : False,
                       'ratio'                   : False,
                       'rational'                : False,
                       'ref'                     : False,
                       'regex'                   : True,
                       'result_of'               : False,
                       'scope_exit'              : False,
                       'serialization'           : True,
                       'signals2'                : False,
                       'smart_ptr'               : True,
                       'statechart'              : False,
                       'static_assert'           : False,
                       'spirit'                  : False,
                       'string_algo'             : False,
                       'swap'                    : False,
                       'system'                  : True,
                       'test'                    : True,
                       'throw_exception'         : False,
                       'thread'                  : True,
                       'timer'                   : True,
                       'tokenizer'               : False,
                       'tribool'                 : False,
                       'tti'                     : False,
                       'tuple'                   : False,
                       'type_erasure'            : False,
                       'type_index'              : False,
                       'type_traits'             : False,
                       'typeof'                  : False,
                       'uBLAS'                   : False,
                       'units'                   : False,
                       'unordered'               : False,
                       'utility'                 : False,
                       'value_initialized'       : False,
                       'uuid'                    : False,
                       'variant'                 : False,
                       'wave'                    : True,
                       'xpressive'               : False }
  if not lib in compiled_lib_map:
    self.end_msg(lib + " is not a valid Boost library");
    self.fatal('The configuration failed')
  return compiled_lib_map[lib]

@conf
def get_lib_link_name(self, lib_name):
  # Add others to this list as things progress
  lib_to_link_name_map = {
    'test' : 'unit_test_framework',
  }

  if not lib_to_link_name_map.get(lib_name): return lib_name
  else: return lib_to_link_name_map.get(lib_name)


@conf
def check_boost(self, *k, **kw):
  """
  Initialize boost libraries to be used.

  Keywords: you can pass the same parameters as with the command line (without
  "--boost-").  Note that the command line has the priority, and should
  preferably be used.
  """
  if not self.env['CXX']:
    self.fatal('load a c++ compiler first, conf.load("compiler_cxx")')

  if not self.options.boost_includes and \
     self.options.boost_root:
    self.options.boost_includes = self.options.boost_root + '/include'

  if not self.options.boost_libs and \
     self.options.boost_root:
    self.options.boost_libs = self.options.boost_root + '/lib'

  params = {
    'lib': k and k[0] or kw.get('lib', None),
    'stlib': kw.get('stlib', None)
  }
  for key, value in self.options.__dict__.items():
    if not key.startswith('boost_'):
      continue
    key = key[len('boost_'):]
    params[key] = value and value or kw.get(key, '')

  var = kw.get('uselib_store', 'BOOST')

  self.start_msg('Checking Boost includes')
  self.env['INCLUDES_%s' % var] = inc = self.boost_get_includes(**params)
  self.env.BOOST_VERSION = self.boost_get_version(inc)
  self.end_msg('ok [' + inc + '] ' +
               '(ver. ' + self.env.BOOST_VERSION.replace('_', '.') + ')')
  if Logs.verbose:
    Logs.pprint('CYAN', ' path : %s' % self.env['INCLUDES_%s' % var])

  if not params['lib'] and not params['stlib']:
    return
  if 'static' in kw or 'static' in params:
    Logs.warn('boost: static parameter is deprecated, use stlib instead.')
  self.start_msg('Checking Boost libs')

  # We need to only check for those librarires that are compiled. Here is
  # a good spot to spot these and cull them from the checks
  if params.get('lib'):
    libs_to_check = ''
    for lib in params['lib'].split(' '):
      if self.is_compiled(lib):
        libs_to_check += ' ' + self.get_lib_link_name(lib)
    params['lib'] = libs_to_check.strip()

  if params.get('stlib'):
    stlibs_to_check = ''
    for stlib in params['stlib'].split(' '):
      if self.is_compiled(stlib):
        stlibs_to_check += ' ' + self.get_lib_link_name(stlib)
    params['stlib'] = stlibs_to_check.strip()

  path, libs, stlibs = self.boost_get_libs(**params)
  self.env['LIBPATH_%s' % var] = [path]
  self.env['STLIBPATH_%s' % var] = [path]
  self.env['RPATH_%s' % var] = [path]
  self.env['LIB_%s' % var] = libs
  self.env['STLIB_%s' % var] = stlibs
  self.end_msg('ok [' + path + ']')
  if Logs.verbose:
    Logs.pprint('CYAN', ' path : %s' % path)
    Logs.pprint('CYAN', ' shared libs : %s' % libs)
    Logs.pprint('CYAN', ' static libs : %s' % stlibs)


  def try_link():
    if (params['lib'] and 'system' in params['lib']) or \
      params['stlib'] and 'system' in params['stlib']:
      self.check_cxx(fragment=BOOST_ERROR_CODE, use=var, execute=False)
    if (params['lib'] and 'thread' in params['lib']) or \
      params['stlib'] and 'thread' in params['stlib']:
      self.check_cxx(fragment=BOOST_THREAD_CODE, use=var, execute=False)

  if params.get('linkage_autodetect', False):
    self.start_msg("Attempting to detect Boost linkage flags")
    toolset = self.boost_get_toolset(kw.get('toolset', ''))
    if toolset in ('vc',):
      # disable auto-linking feature, causing error LNK1181
      # because the code wants to be linked against
      self.env['DEFINES_%s' % var] += ['BOOST_ALL_NO_LIB']

      # if no dlls are present, we guess the .lib files are not stubs
      has_dlls = False
      for x in Utils.listdir(path):
        if x.endswith(self.env.cxxshlib_PATTERN % ''):
          has_dlls = True
          break
      if not has_dlls:
        self.env['STLIBPATH_%s' % var] = [path]
        self.env['STLIB_%s' % var] = libs
        del self.env['LIB_%s' % var]
        del self.env['LIBPATH_%s' % var]
        del self.env['RPATH_%s' % var]

      # we attempt to play with some known-to-work CXXFLAGS combinations
      for cxxflags in (['/MD', '/EHsc'], []):
        self.env.stash()
        self.env["CXXFLAGS_%s" % var] += cxxflags
        try:
          try_link()
          self.end_msg("ok: winning cxxflags combination: %s" % \
                        (self.env["CXXFLAGS_%s" % var]))
          exc = None
          break
        except Errors.ConfigurationError as e:
          self.env.revert()
          exc = e

      if exc is not None:
        self.end_msg("Could not auto-detect boost linking flags combination, you may report it to boost.py author", ex=exc)
        self.fatal('The configuration failed')
    else:
      self.end_msg("Boost linkage flags auto-detection not implemented (needed ?) for this toolchain")
      self.fatal('The configuration failed')
  else:
    self.start_msg('Checking for Boost linkage')
    try:
      try_link()
    except Errors.ConfigurationError as e:
      self.end_msg("Could not link against boost libraries using supplied options")
      self.fatal('The configuration failed')
    self.end_msg('ok')


@feature('cxx')
@after_method('apply_link')
def install_boost(self):
  if install_boost.done or not Utils.is_win32 or not self.bld.cmd.startswith('install'):
    return
  install_boost.done = True
  inst_to = getattr(self, 'install_path', '${BINDIR}')
  for lib in self.env.LIB_BOOST:
    try:
      file = self.bld.find_file(self.env.cxxshlib_PATTERN % lib, self.env.LIBPATH_BOOST)
      self.bld.install_files(inst_to, self.bld.root.find_node(file))
    except:
      continue
install_boost.done = False


@conf
def get_boost(ctx):
    from subprocess import call
    from os import chdir
    install_path = ctx.options.install_deps_prefix + '/src/boost_1_57_0'
    install_node = ctx.root.find_node(install_path)
    if not install_node:
        tmp_path = '/tmp/boost_1_57_0.tar.gz'
        tmp_node = ctx.root.find_node(tmp_path)
        if not tmp_node:
            ctx.start_msg('Searching for curl')
            ctx.find_program('curl', target='CURL')
            ctx.end_msg('ok ' + str(ctx.env.CURL), color='GREEN')
            ctx.msg('Downloading Boost release 1.57', '...', color='YELLOW')
            tmp_node = ctx.root.make_node(tmp_path)
            tmp_node.parent.mkdir()
            command = ctx.env.CURL + ['-L', '-o', tmp_node.abspath(),
                                     'http://sourceforge.net/projects/boost/' +
                                      'files/boost/1.57.0/boost_1_57_0.tar.gz']
            call(command)
        ctx.msg('Extracting Boost libraries', '...', color='YELLOW')
        install_node = ctx.root.make_node(install_path)
        install_node.parent.mkdir()
        chdir(install_node.parent.abspath())
        call(['tar', 'xf', tmp_path])

    chdir(install_node.abspath())
    b2_node = install_node.find_node('b2')
    if not b2_node:
        ctx.msg('Bootstrapping Boost.Build', '...', color='Yellow')
        call('./bootstrap.sh')

    ctx.msg('Installing Boost', '...', color='YELLOW')
    # We always have '--with-test' to suppress building all the libraries
    command = ['./b2', '--prefix=' + ctx.options.install_deps_prefix] + \
    ['--with-test'] + \
    ['--with-' + lib for lib in ctx.env.BOOST_PKGS if ctx.is_compiled(lib)] + \
      ['install']
    call(command)

    chdir(ctx.srcnode.abspath())


def configure(ctx):
    global BOOST_LIBS, BOOST_INCLUDES
    BOOST_LIBS = [ctx.options.install_deps_prefix + '/lib'] + BOOST_LIBS
    BOOST_INCLUDES = [ctx.options.install_deps_prefix + '/include'] + BOOST_INCLUDES
    if not ctx.env.BOOST_PKGS:
        return
    try:
        ctx.check_boost(lib=' '.join(ctx.env.BOOST_PKGS))
    except ctx.errors.ConfigurationError as e:
        if ctx.options.install_deps:
            print('\n*** Could not find at least one of the following ' +
                  'Boost libraries:')
            print('***   - ' + '\n***   - '.join(ctx.env.BOOST_PKGS))
            print('*** Attempting to install Boost in ' +
                  ctx.options.install_deps_prefix + '...\n')
            ctx.get_boost()
            print('\n*** Done installing Boost\n')
            try:
                ctx.check_boost(lib=' '.join(ctx.env.BOOST_PKGS))
            except ctx.errors.ConfigurationError as e:
                ctx.fatal(e + 'after install')
        else:
            ctx.fatal(e)
    ctx.define('USE_BOOST', 1)
    ctx.define('BOOST_VERSION', ctx.env.BOOST_VERSION)

