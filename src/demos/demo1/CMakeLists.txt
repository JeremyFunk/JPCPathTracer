set(AUTOSRC
"main.cpp"
)




set(INC 
"../")

set(LIBS 
    jpctracer
)


jpctr_add_executable(demo1 "${AUTOSRC}" "${INC}" "${LIBS}")
