file(GLOB tinyxml_sources *.cpp *h)
add_library(tinyxml STATIC ${tinyxml_sources})

if( WIN32 )
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()