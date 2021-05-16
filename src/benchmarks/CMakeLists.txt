set(AUTOSRC
"main_benchmark.cpp"
"math/Vector.cpp"
)
set(INC
"../")

set(LIBS 
    jpc_tracer
)

jpctr_add_gbenchmark(jpctracer_benchmarks "${AUTOSRC}" "${INC}" "${LIBS}" )