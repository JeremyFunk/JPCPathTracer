#inspired by https://github.com/mcneel/cycles/blob/master_rhino-7.0_20200203_tmp/src/cmake/macros.cmake

function(jpctr_set_solution_folder target)
  #get USE_FOLDERS
  set(should_use_folder )
  get_property(should_use_folder GLOBAL PROPERTY USE_FOLDERS)
  message("Use Folder: ${should_use_folder}")

  
  if(${should_use_folder})
    message("Use Folders by target")
    get_filename_component(folderdir ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
    string(REPLACE ${CMAKE_SOURCE_DIR} "" folderdir "${folderdir}")
    set_target_properties(${target} PROPERTIES FOLDER "${folderdir}")
  endif()
endfunction()

macro(jpctr_set_source_group source)
  #get USE_FOLDERS
  set(should_use_folder )
  get_property(should_use_folder GLOBAL PROPERTY USE_FOLDERS)
  message("Use Folder: ${should_use_folder}")
  
  
  if(${should_use_folder})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${source})
  endif()
endmacro()

#scope = PUBLIC | PRIVATE | INTERFACE
macro(__jpctr_set_include_dirs target includes scope)
  foreach(include ${includes})
    message("Include Dir ${include}")
    target_include_directories(${target} ${scope}
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${include}>
    $<INSTALL_INTERFACE:${include}>
  )
  endforeach()
endmacro()

macro(jpctr_link_dependencies target library_deps scope)
   # On Windows certain libraries have two sets of binaries: one for debug builds and one for
  # release builds. The root of this requirement goes into ABI, I believe, but that's outside
  # of a scope of this comment.
  #
  # CMake have a native way of dealing with this, which is specifying what build type the
  # libraries are provided for:
  #
  #   target_link_libraries(tagret optimized|debug|general <libraries>)
  #
  # The build type is to be provided as a separate argument to the function.
  #
  # CMake's variables for libraries will contain build type in such cases. For example:
  #
  #   set(FOO_LIBRARIES optimized libfoo.lib debug libfoo_d.lib)
  #
  # Complications starts with a single argument for library_deps: all the elements are being
  # put to a list: "${FOO_LIBRARIES}" will become "optimized;libfoo.lib;debug;libfoo_d.lib".
  # This makes it impossible to pass it as-is to target_link_libraries sine it will treat
  # this argument as a list of libraries to be linked against, causing missing libraries
  # for optimized.lib.
  #
  # What this code does it traverses library_deps and extracts information about whether
  # library is to provided as general, debug or optimized. This is a little state machine which
  # keeps track of whiuch build type library is to provided for:
  #
  # - If "debug" or "optimized" word is found, the next element in the list is expected to be
  #   a library which will be passed to target_link_libraries() under corresponding build type.
  #
  # - If there is no "debug" or "optimized" used library is specified for all build types.
  #
  # NOTE: If separated libraries for debug and release ar eneeded every library is the list are
  # to be prefixed explicitly.
  #
  #  Use: "optimized libfoo optimized libbar debug libfoo_d debug libbar_d"
  #  NOT: "optimized libfoo libbar debug libfoo_d libbar_d"
  #
  # TODO(sergey): This is the same as Blender's side CMake. Find a way to avoid duplication
  # somehow in a way which allows to have Cycles standalone.
  if(NOT "${library_deps}" STREQUAL "")
    set(next_library_mode "")
    foreach(library ${library_deps})
      message("Library: ${library}")
      string(TOLOWER "${library}" library_lower)
      if(("${library_lower}" STREQUAL "optimized") OR
         ("${library_lower}" STREQUAL "debug"))
        set(next_library_mode "${library_lower}")
      else()
        if("${next_library_mode}" STREQUAL "optimized")
          target_link_libraries(${target} ${scope} optimized ${library})
        elseif("${next_library_mode}" STREQUAL "debug")
          target_link_libraries(${target} ${scope} debug ${library})
        else()
          target_link_libraries(${target} ${scope} ${library})
        endif()
        set(next_library_mode "")
      endif()
    endforeach()
  endif()
endmacro()

macro(__jpctr_target_setup target sources includes library_deps scope)
  jpctr_link_dependencies(${target} "${library_deps}" "${scope}")

  jpctr_set_solution_folder(${target})
  jpctr_set_source_group("${sources}")
  #source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sources})
  
  __jpctr_set_include_dirs(${target} "${includes}" PUBLIC)

  # TODO: Add tests and install targets if needed.
  set_property(TARGET ${target} PROPERTY VS_GLOBAL_ROOTNAMESPACE "${target}")

  if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
   target_compile_options(${target} PRIVATE /Zi)

   # Tell linker to include symbol data
    set_target_properties(${target} PROPERTIES 
        LINK_FLAGS "/INCREMENTAL:NO /DEBUG /OPT:REF /OPT:ICF"
    )
    target_compile_options(${target} PUBLIC "-openmp:experimental")

    # Set file name & location
    set_target_properties(${target} PROPERTIES 
        COMPILE_PDB_NAME ${target} 
        COMPILE_PDB_OUTPUT_DIR ${CMAKE_BINARY_DIR}
    )
  endif()

endmacro()

macro(jpctr_add_library target sources includes library_deps lib_typ)
  add_library(${target} ${lib_typ} ${sources})
  __jpctr_target_setup(${target} "${sources}" "${includes}" "${library_deps}" "")
  
endmacro()

macro(jpctr_add_header_only_lib target includes library_deps)
  add_library(${target} INTERFACE)
  __jpctr_set_include_dirs(${target} "${includes}" INTERFACE)
  jpctr_link_dependencies(${target} "${library_deps}" INTERFACE)
  
endmacro()

macro(jpctr_add_executable target sources includes library_deps )
  add_executable(${target} ${sources})
  __jpctr_target_setup(${target} "${sources}" "${includes}" "${library_deps}" "")
  
endmacro()

macro(jpctr_add_gtest target sources includes library_deps)
  add_executable(${target} ${sources})
  add_test("${target}" ${target})
  set(library_deps_edit "${library_deps}"  gmock_main gmock gtest_main gtest)
  #message("gtest library_deps_edit ${gtest_libs}")
  __jpctr_target_setup(${target} "${sources}" "${includes}" "${library_deps_edit}" PRIVATE)
  #foreach(gtest_lib ${gtest_libs})
  #  target_link_libraries(${target} PRIVATE ${gtest_lib})
  #endforeach()
  
endmacro()

macro(jpctr_add_gmock_lib target sources includes library_deps)
  add_library(${target} STATIC ${sources})
  set(library_deps_edit "${library_deps}" gmock gtest)
  message("GMOCK libs: ${library_deps_edit}")
  __jpctr_target_setup(${target} "${sources}" "${includes}" "${library_deps_edit}" PUBLIC)
  
endmacro()