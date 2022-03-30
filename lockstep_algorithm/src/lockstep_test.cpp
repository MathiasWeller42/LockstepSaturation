#include <filesystem>
#include <list>
#include <string>
#include <deque>
#include <iostream>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "lockstep.h"
#include "graph_examples.h"
#include "petriTranslation.h"

void simpleTest(){
  Graph graph = fourNodesOneRelation();
  sylvan::Bdd nodes = graph.nodes;
  std::cout << "Printing nodeset" << std::endl;
  printBdd(nodes);
  std::list<sylvan::Bdd> sccList = lockstepSaturation(graph);
  std::cout << "Printing SCCs" << std::endl;
  for(sylvan::Bdd scc : sccList) {
    printBdd(scc);
  }
}

void simpleTestGraph(Graph graph) {
  sylvan::Bdd nodes = graph.nodes;
  std::list<sylvan::Bdd> sccList = lockstepSaturation(graph);
  std::cout << "Printing SCCs" << std::endl;
  for(sylvan::Bdd scc : sccList) {
    printBddAsString(graph.cube.size(), scc);
  }
}
