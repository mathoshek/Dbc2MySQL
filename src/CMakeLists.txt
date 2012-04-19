file(GLOB Dbc2MySQL_sources *.cpp *.h)

include_directories(
  ${MYSQL_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/dep/tinyxml
)

add_executable(Dbc2MySQL ${Dbc2MySQL_sources})

if( WIN32 )
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

target_link_libraries(Dbc2MySQL tinyxml ${MYSQL_LIBRARY})

install(TARGETS Dbc2MySQL DESTINATION ${CMAKE_INSTALL_PREFIX})

file(GLOB Dbc2MySQL_structure_files *.xml)
install(FILES ${Dbc2MySQL_structure_files} DESTINATION ${CMAKE_INSTALL_PREFIX})
#install(FILES 
message(STATUS "${MYSQL_LIBRARIES}")