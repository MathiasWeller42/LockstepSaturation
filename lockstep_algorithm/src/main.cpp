// Your First C++ Program
#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include <iostream>

int main() {

    // Init LACE
    lace_start(1, 1000000);

    const size_t memory_bytes = 128u * 1024u * 1024u;

    // Init Sylvan
    sylvan::sylvan_set_limits(memory_bytes,      // Set memory limit
                              6,                 // Set (exponent) of cache ratio
                              0);                // Initialise unique node table to full size
    sylvan::sylvan_set_granularity(1);
    sylvan::sylvan_init_package();
    sylvan::sylvan_init_bdd();


    std::cout << "Hello World!";


    sylvan::sylvan_quit();
    lace_stop();
    return 0;
}

sylvan::Bdd testFunction() {
    sylvan::Bdd t = sylvan::Bdd::bddOne();
    sylvan::Bdd f = sylvan::Bdd::bddZero();
    return t.And(f);
}



