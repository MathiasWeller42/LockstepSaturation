#include <filesystem>
#include <list>
#include <string>
#include <deque>
#include <iostream>
#include <deque>
#include <algorithm>
#include <chrono>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "lockstep.h"
#include "petriTranslation.h"
#include "utilities.h"
#include "../test/graph_examples.h"

void simpleTestGraph(Graph graph) {
  auto start1 = std::chrono::high_resolution_clock::now();
  std::list<sylvan::Bdd> sccList1 = lockstepSaturation(graph);
  auto stop1 = std::chrono::high_resolution_clock::now();
  auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(stop1 - start1);
  std::cout << "Time elapsed (saturation): " << duration1.count() << " milliseconds" << std::endl;
  std::cout << std::endl << "Found " << sccList1.size() << " SCCs" << std::endl;

  auto start2 = std::chrono::high_resolution_clock::now();
  std::list<sylvan::Bdd> sccList2 = lockstepRelationUnion(graph);
  auto stop2 = std::chrono::high_resolution_clock::now();
  auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(stop2 - start2);
  std::cout << "Time elapsed (relation union): " << duration2.count() << " milliseconds" << std::endl;
  std::cout << std::endl << "Found " << sccList2.size() << " SCCs" << std::endl;

  if(!sccListCorrectness(sccList1, sccList2)) {
    std::cout << "scc lists did not contain the same bdds" << std::endl;
  }
  /*std::cout << "Printing SCCs" << std::endl;
  for(sylvan::Bdd scc : sccList) {
    //printBdd(scc);
    printBddAsString(graph.cube.size(), scc);
    std::list<std::string> stringList = __printBddAsString("", scc);
    for(std::string s : stringList) {
      std::cout << s << std::endl;
    }
  }*/
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

void testRelationSorting() {
  Relation rel1 = {};
  rel1.top = 25;
  Relation rel2 = {};
  rel2.top = 30;
  Relation rel3 = {};
  rel3.top = 38;
  Relation rel4 = {};
  rel4.top = 10;
  Relation rel5 = {};
  rel5.top = 42;

  std::deque<Relation> relations = {rel1, rel2, rel3, rel4, rel5};
  std::sort(relations.begin(),relations.end());
  std::reverse(relations.begin(), relations.end());
  for(Relation rel : relations) {
    std::cout << rel.top << std::endl;
  }

}



void testCubeOperations() {
    sylvan::BddSet cube1 = sylvan::BddSet();
    cube1.add(0);
    cube1.add(2);
    cube1.add(100);
    cube1.add(116);
    std::vector<uint32_t> cubeVec1 = cube1.toVector();
    std::cout << "Printing cube as vector:" << std::endl;
    for (uint32_t i : cubeVec1) {

      std::cout << std::to_string(i) << ' ';
    }
    std::cout << std::endl;

    sylvan::BddSet cube2 = sylvan::BddSet();
    cube2.add(0);
    cube2.add(2);
    cube2.add(101);
    cube2.add(117);
    //You can use remove and add on a cube with another cube, performing union and difference
    cube1.remove(cube2);
    std::vector<uint32_t> cubeVec2 = cube1.toVector();
    std::cout << "Printing cube1 union cube2 as vector:" << std::endl;
    for (uint32_t i : cubeVec2) {
      std::cout << std::to_string(i) << ' ';
    }
    std::cout << std::endl;


    Graph graph = fourNodesOneRelation();

    sylvan::Bdd nodes = graph.nodes;
    printBddAsString(8, nodes);
    //nodes.UnionCube(cube2);
    printBddAsString(8, nodes);


}