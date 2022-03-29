add_executable(test test_lockstep.cpp
               ../src/lockstep.cpp
               ../src/petriTranslation.cpp
               ../src/set_test.cpp
               ../src/lockstep_test.cpp
               ../src/graph_examples.cpp)

target_link_libraries(test PRIVATE Sylvan::Sylvan)