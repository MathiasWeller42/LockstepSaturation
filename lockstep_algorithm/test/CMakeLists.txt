add_executable(test test_lockstep.cpp
               test_runlockstep.cpp
               test_sccListCorrectness.cpp
               graph_examples.cpp
               ../src/lockstep.cpp
               ../src/petriTranslation.cpp
               ../src/lockstep_test.cpp)

target_link_libraries(test PRIVATE Sylvan::Sylvan)