add_executable(main main.cpp
               lockstep.cpp
               petriTranslation.cpp
               set_test.cpp
               lockstep_test.cpp
               graph_examples.cpp)

target_link_libraries(main PRIVATE Sylvan::Sylvan)

