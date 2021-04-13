add_executable(BsdfFactoryExperiment "main.cpp")
target_link_libraries(BsdfFactoryExperiment jpc_tracer)
set_target_properties(BsdfFactoryExperiment PROPERTIES COMPILE_FLAGS "-save-temps -Ofast") 