# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\Syncordify_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Syncordify_autogen.dir\\ParseCache.txt"
  "Syncordify_autogen"
  )
endif()
