

from distutils.core import setup, Extension

vfm = Extension('vfm', sources = ['pyapi.c'], 
                include_dirs = ['.', '../../include/vfmapi', '../../include'],
                libraries = ['vfmapi'],
                library_dirs = ['../'])
setup (name = 'VFM_PYAPI',
              version = '1.0',
              description = 'This is a wrapper to call the vfm cli.',
              ext_modules = [vfm])

