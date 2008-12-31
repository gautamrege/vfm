

from distutils.core import setup, Extension

bxm = Extension('bxm', sources = ['pyapi.c'], 
                include_dirs = ['.', '../../include/bxmapi', '../../include'],
                libraries = ['bxmapi'],
                library_dirs = ['../'])
setup (name = 'BXM_PYAPI',
              version = '1.0',
              description = 'This is a wrapper to call the bxm cli.',
              ext_modules = [bxm])

