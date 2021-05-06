set(AUTOSRC
"test.cpp"
)
set(INC
"../")

set(LIBS 
    jpc_tracer
)

jpctr_add_gbenchmark(jpctracer_benchmarks "${AUTOSRC}" "${INC}" "${LIBS}" )