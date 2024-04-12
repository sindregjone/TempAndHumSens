# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/sindregjone/ESP-IDF/esp-idf-v5.1.2/components/bootloader/subproject"
  "/Users/sindregjone/ESPworkspace/TempAndHumSens/build/bootloader"
  "/Users/sindregjone/ESPworkspace/TempAndHumSens/build/bootloader-prefix"
  "/Users/sindregjone/ESPworkspace/TempAndHumSens/build/bootloader-prefix/tmp"
  "/Users/sindregjone/ESPworkspace/TempAndHumSens/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/sindregjone/ESPworkspace/TempAndHumSens/build/bootloader-prefix/src"
  "/Users/sindregjone/ESPworkspace/TempAndHumSens/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/sindregjone/ESPworkspace/TempAndHumSens/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/sindregjone/ESPworkspace/TempAndHumSens/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
