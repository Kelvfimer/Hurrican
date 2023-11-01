#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "SDL2::SDL2" for configuration "MinSizeRel"
set_property(TARGET SDL2::SDL2 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(SDL2::SDL2 PROPERTIES
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/lib/libSDL2-2.0.so.0.2800.4"
  IMPORTED_SONAME_MINSIZEREL "libSDL2-2.0.so.0"
  )

list(APPEND _cmake_import_check_targets SDL2::SDL2 )
list(APPEND _cmake_import_check_files_for_SDL2::SDL2 "${_IMPORT_PREFIX}/lib/libSDL2-2.0.so.0.2800.4" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
