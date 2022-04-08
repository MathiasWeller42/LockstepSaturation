
#include <iostream>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "lockstep.h"
#include "petriTranslation.h"
#include "lockstep_test.h"
#include "print.h"
#include "../test/graph_examples.h"

int main() {

  // Init LACE
  lace_start(1, 1000000);

  const size_t memory_bytes = 1024u * 1024u * 1024u;

  // Init Sylvan
  sylvan::sylvan_set_limits(memory_bytes, // Set memory limit
                            6,            // Set (exponent) of cache ratio
                            0);           // Initialise unique node table to full size
  sylvan::sylvan_set_granularity(1);
  sylvan::sylvan_init_package();
  sylvan::sylvan_init_bdd();

  std::cout << "Hello World!" << std::endl;

  Graph bigGraph = PNMLtoGraph("ShieldIIPt/PT/shield_t_iip_001_a.pnml");
  timeSaturationIterative(bigGraph);
  //testRelNext();

  std::cout << "Goodbye :) - hilsen hyggelige helterotte hoppe-loppe hepatitis herpes Herbert Hans Handanovic " << std::endl;

  sylvan::sylvan_quit();
  lace_stop();
  return 0;
}