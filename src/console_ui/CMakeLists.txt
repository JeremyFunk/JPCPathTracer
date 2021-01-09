set(AUTOSRC
"main.cpp"
)




set(INC 
"../")

set(LIBS 
)


jpctr_add_executable(console_ui "${AUTOSRC}" "${INC}" "${LIBS}")

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(console_ui OpenMP::OpenMP_CXX)
endif()