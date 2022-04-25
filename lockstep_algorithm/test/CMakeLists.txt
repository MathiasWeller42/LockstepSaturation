add_executable(test test_lockstep.cpp
               test_runlockstep.cpp
               test_sccListCorrectness.cpp
               graph_examples.cpp
               ../src/graph_creation.cpp
               ../src/lockstep.cpp
               ../src/petriTranslation.cpp
               ../src/print.cpp
               ../src/benchmark.cpp)

target_link_libraries(test PRIVATE Sylvan::Sylvan)