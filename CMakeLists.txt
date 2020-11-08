# This is a community contribution. The official way is to use pmake.
#
# This isn't meant to be a good CMakeList, but it tries to maximize its odds of
# taying relevant without someone to maintain it.

cmake_minimum_required(VERSION 3.0)
project(kgt LANGUAGES C)

# The final executable:
add_executable(kgt_exe ./src/main.c)
set_target_properties(kgt_exe PROPERTIES OUTPUT_NAME "kgt")
target_link_libraries(kgt_exe PRIVATE kgt)

# The common library:
file(GLOB kgt_common_files
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)
list(REMOVE_ITEM kgt_common_files
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
)
add_library(common STATIC ${kgt_common_files})
add_library(kgt::common ALIAS common)
set_property(TARGET common PROPERTY C_STANDARD 11)
list(APPEND libraries common)
set_target_properties(common PROPERTIES LINKER_LANGUAGE C)

# The modules libraries:
file(GLOB subdirs ${CMAKE_CURRENT_SOURCE_DIR}/src/*)
foreach(subdir ${subdirs})
  if(IS_DIRECTORY ${subdir})
    get_filename_component(library ${subdir} NAME)
    file(GLOB files ${subdir}/*.h ${subdir}/*.c)

    add_library(${library} STATIC ${files})
    add_library(kgt::${library} ALIAS ${library})
    set_property(TARGET ${library} PROPERTY C_STANDARD 11)
    target_link_libraries(${library} PRIVATE m)
    target_link_libraries(${library} PUBLIC common)
    set_target_properties(${library} PROPERTIES LINKER_LANGUAGE C)

    string(REPLACE "-" "_" FORM ${library})
    target_compile_definitions(${library} PRIVATE "LX_HEADER=\"lexer.h\"")
    target_compile_definitions(${library} PRIVATE "FORM=${FORM}")

    list(APPEND libraries ${library})
  endif()
endforeach()

# Some depencencies in between the modules above:
target_link_libraries(rrll PUBLIC rrd)
target_link_libraries(rrtext PUBLIC rrd)

# The main library:
add_library(kgt STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/ast.h)
add_library(kgt::kgt ALIAS kgt)
set_property(TARGET kgt PROPERTY C_STANDARD 11)
set_target_properties(kgt PROPERTIES LINKER_LANGUAGE C)
target_include_directories(kgt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)
target_link_libraries(kgt PUBLIC ${libraries})

export(
  TARGETS
    kgt
    ${libraries}
  NAMESPACE kgt::
  FILE kgt.cmake
)

if(MSVC)
  # Force to always compile with W4
  if(CMAKE_C_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
  else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUC)
  # Update if necessary
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-long-long -pedantic")
endif()
