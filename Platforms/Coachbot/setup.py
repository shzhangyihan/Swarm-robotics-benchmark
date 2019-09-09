from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

swarmnet_dir = "../../SwarmNet/src/"
swarmos_dir = "../../SwarmOS/src/"
driver_dir = "../../Driver/Coachbot/"
user_program_dir = "./"
compile_flags = ["-std=c++11", "-Wall", "-g", "-I" + driver_dir, "-I" + user_program_dir]

pyx_source_files = ["swarmos_driver_api.pyx"]
swarmnet_source_files = [swarmnet_dir + "channel.cpp",
                         swarmnet_dir + "packet.cpp",
                         swarmnet_dir + "publisher.cpp",
                         swarmnet_dir + "subscriber.cpp",
                         swarmnet_dir + "swarmnet.cpp",
                         swarmnet_dir + "util.cpp"]
swarmos_source_files = [swarmos_dir + "swarmos.cpp"]
driver_source_files = [driver_dir + "coach_driver.cpp", driver_dir + "python_driver.cpp"]
#driver_source_files = []
source_files = pyx_source_files + swarmnet_source_files + swarmos_source_files + driver_source_files

setup(ext_modules = [
                    Extension("swarmos_driver", 
                              source_files,  
                              language = "c++",
                              extra_compile_args = compile_flags)],
      cmdclass = {'build_ext': build_ext})
