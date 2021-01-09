#macro(remove_warnings target)
#  if(MSVC)
#    #target_compile_options(${target} PRIVATE
#    #   /W4)
#    target_compile_definitions(${target} PRIVATE _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
#  endif()
#endmacro()
#
##set(FETCHCONTENT_QUIET ON)
#
#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#
#include(FetchContent)
#
#FetchContent_Declare(
#  googletest
#  GIT_REPOSITORY https://github.com/google/googletest.git
#  GIT_TAG        release-1.10.0
#)
#FetchContent_MakeAvailable(googletest)
#remove_warnings(gtest_main)
#remove_warnings(gtest)
#remove_warnings(gmock)
#remove_warnings(gmock_main)
#jpctr_set_solution_folder(gtest_main)
#jpctr_set_solution_folder(gtest)
#jpctr_set_solution_folder(gmock)
#jpctr_set_solution_folder(gmock_main)

add_subdirectory(googletest)
