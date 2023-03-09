# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\MASTER_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\MASTER_autogen.dir\\ParseCache.txt"
  "MASTER_autogen"
  )
endif()
