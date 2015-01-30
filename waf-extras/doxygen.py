#! /usr/bin/env python
# encoding: UTF-8

"""

Doxygen support

Variables passed to bld():
* doxyfile -- the Doxyfile to use

When using this tool, the wscript will look like:

    def options(opt):
        opt.load('doxygen')

    def configure(conf):
        conf.load('doxygen')
        # check conf.env.DOXYGEN, if it is mandatory

    def build(bld):
        if bld.env.DOXYGEN:
            bld(features="doxygen", doxyfile='Doxyfile', ...)
"""

def options(ctx):
    dog = ctx.add_option_group('Options for Doxygen')
    dog.add_option('--doxyfile',
                   type='string',
                   default='docs/doxygen/Doxyfile',
                   dest='doxyfile',
                   help='''path to Doxygen configuration file''')
    dog.add_option('--doxygen-output',
                   type='string',
                   default='docs/doxygen',
                   dest='doxygen_output',
                   help='''path to Doxygen output directory''')


def configure(conf):
    conf.find_program('doxygen', var='DOXYGEN', mandatory=False)


def doxygen(ctx):
    '''generates Doxygen documentation'''
    if ctx.env.DOXYGEN:
        ctx.exec_command(ctx.env.DOXYGEN + [ctx.options.doxyfile])
        from os import chdir
        chdir(ctx.options.doxygen_output + '/latex')
        ctx.exec_command('make')


from waflib import Context
Context.g_module.__dict__['doxygen'] = doxygen

from waflib.Build import BuildContext
class doxygen(BuildContext):
    cmd = 'doxygen'
    fun = 'doxygen'
