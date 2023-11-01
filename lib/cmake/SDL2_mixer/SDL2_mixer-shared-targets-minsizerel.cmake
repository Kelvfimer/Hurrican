#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "SDL2_mixer::SDL2_mixer" for configuration "MinSizeRel"
set_property(TARGET SDL2_mixer::SDL2_mixer APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(SDL2_mixer::SDL2_mixer PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_MINSIZEREL "SDL2::SDL2"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/lib/libSDL2_mixer-2.0.so.0.600.2"
  IMPORTED_SONAME_MINSIZEREL "libSDL2_mixer-2.0.so.0"
  )

list(APPEND _cmake_import_check_targets SDL2_mixer::SDL2_mixer )
list(APPEND _cmake_import_check_files_for_SDL2_mixer::SDL2_mixer "${_IMPORT_PREFIX}/lib/libSDL2_mixer-2.0.so.0.600.2" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
