set(AUTOSRC
"main.cpp"
)




set(INC 
"../")

set(LIBS 
    jpctracer
)


jpctr_add_executable(mix_materials "${AUTOSRC}" "${INC}" "${LIBS}")
