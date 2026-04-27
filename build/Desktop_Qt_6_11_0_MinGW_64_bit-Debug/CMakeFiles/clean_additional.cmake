# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\CodingCalculator_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\CodingCalculator_autogen.dir\\ParseCache.txt"
  "CodingCalculator_autogen"
  )
endif()
