
from distutils.core import setup
from distutils.extension import Extension
import os.path
import sys

moos = '../../../moos-ivp'

core_include_dirs = [
'/usr/include/boost',
'.', 
moos+'/MOOS/MOOSCore/Core/libMOOS/include',
moos+'/MOOS/MOOSCore/Core/libMOOS/App/include',
moos+'/MOOS/MOOSCore/Core/libMOOS/Comms/include',
moos+'/MOOS/MOOSCore/Core/libMOOS/Utils/include',
moos+'/MOOS/MOOSCore/Core/libMOOS/Thirdparty/PocoBits/include',
moos+'/include'
]
core_libs = ['boost_python',]
core_slibs = [moos+'/MOOS/MOOSCore/lib/libMOOS.a',
               moos+'/lib/libmbutil.a']
core_macros = [ ('UNIX',None),
                ('PLATFORM_LINUX',None),
                ('ASYNCHRONOUS_CLIENT',None)]
core_files = ['MOOSCore.cpp']

logutils_include_dirs = [
'/usr/include/boost',
'.',
moos+'/include'
]
logutils_libs = ['boost_python']
logutils_slibs = [moos+'/lib/liblogutils.a',
               moos+'/lib/libmbutil.a']
logutils_files = ['logutils.cpp']

setup(name='moos-ivp',
        packages = ['moos'],
        ext_modules=[
            Extension("moos.core", core_files,
                libraries=core_libs,
                include_dirs=core_include_dirs,
                define_macros=core_macros,
                extra_link_args=core_slibs,
                depends=[]),
            Extension("moos.logutils", logutils_files,
                libraries=logutils_libs,
                include_dirs=logutils_include_dirs,
                define_macros=core_macros,
                extra_link_args=logutils_slibs,
                depends=[])
            ])
