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
#include "utilities.h"
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

void testCubes() {
  std::list<std::pair<int, int>> r1 =
    {{0,3}, {1,2}};
  const std::list<std::list<std::pair<int, int>>> relationList = {r1};
  Graph graph = makeGraph(16, relationList);

  sylvan::Bdd nodeSet = graph.nodes;

  std::list<std::string> placeList0And1 = {"0000","0001","1000","1001"};
  sylvan::Bdd nodes0And1 = makeNodes(placeList0And1);

  std::list<std::pair<std::string, std::string>> r2 =
    {{"00","11"}, {"01","10"}};
  sylvan::Bdd relation = makeRelation(r2, graph.cube).relationBdd;

  std::cout << "Printing nodeset:" << std::endl;
  printBddAsString(4,nodeSet);
  std::cout << "Printing nodes 0 and 1:" << std::endl;
  printBddAsString(4,nodes0And1);
  std::cout << "Printing relation:" << std::endl;
  printSingleRelationAsString(relation);

  sylvan::BddSet cubeAll = graph.cube;
  sylvan::BddSet cubeRel = sylvan::BddSet();
  cubeRel.add(0);
  cubeRel.add(2);

  sylvan::Bdd resultAll = nodes0And1.RelNext(relation, cubeAll);
  sylvan::Bdd resultRel = nodes0And1.RelNext(relation, cubeRel);

  std::cout << "Printing result on nodes 0 and 1 with cubeAll:" << std::endl;
  printBddAsString(4,resultAll);
  std::cout << "Printing result on nodes 0 and 1 with cubeRel:" << std::endl;
  printBddAsString(4,resultRel);
}


