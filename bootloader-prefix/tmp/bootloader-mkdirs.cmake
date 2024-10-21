# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/flint/esp/v5.3.1/esp-idf/components/bootloader/subproject"
  "C:/Repos/Skateboard_speedometer/bootloader"
  "C:/Repos/Skateboard_speedometer/bootloader-prefix"
  "C:/Repos/Skateboard_speedometer/bootloader-prefix/tmp"
  "C:/Repos/Skateboard_speedometer/bootloader-prefix/src/bootloader-stamp"
  "C:/Repos/Skateboard_speedometer/bootloader-prefix/src"
  "C:/Repos/Skateboard_speedometer/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Repos/Skateboard_speedometer/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Repos/Skateboard_speedometer/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
