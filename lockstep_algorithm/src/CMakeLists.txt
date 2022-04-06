add_executable(main main.cpp
               lockstep.cpp
               petriTranslation.cpp
               set_test.cpp
               lockstep_test.cpp
               ../test/graph_examples.cpp)

target_link_libraries(main PRIVATE Sylvan::Sylvan)

