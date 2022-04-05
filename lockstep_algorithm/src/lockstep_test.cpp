#include <filesystem>
#include <list>
#include <string>
#include <deque>
#include <iostream>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "lockstep.h"
#include "petriTranslation.h"
#include "../test/graph_examples.h"

void simpleTestGraph(Graph graph) {
  sylvan::Bdd nodes = graph.nodes;
  std::list<sylvan::Bdd> sccList = lockstepSaturation(graph);
  std::cout << "Printing SCCs" << std::endl;
  for(sylvan::Bdd scc : sccList) {
    //printBdd(scc);
    printBddAsString(graph.cube.size(), scc);
  }
}
