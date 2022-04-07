add_executable(main main.cpp
               lockstep.cpp
               print.cpp
               petriTranslation.cpp
               lockstep_test.cpp
               bdd_utilities.cpp
               graph_creation.cpp
               ../test/graph_examples.cpp)

target_link_libraries(main PRIVATE Sylvan::Sylvan)

