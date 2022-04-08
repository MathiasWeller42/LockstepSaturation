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

#include "lockstep_test.h"
#include "lockstep.h"
#include "petriTranslation.h"
#include "bdd_utilities.h"
#include "graph_creation.h"
#include "print.h"
#include "../test/graph_examples.h"

void timeSaturation(Graph &graph) {
  graph = graphPreprocessing(graph);

  auto start1 = std::chrono::high_resolution_clock::now();
  std::list<sylvan::Bdd> sccList1 = lockstepSaturation(graph);
  auto stop1 = std::chrono::high_resolution_clock::now();
  auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(stop1 - start1);
  std::cout << "Time elapsed (saturation): " << duration1.count() << " milliseconds" << std::endl;
  std::cout << "Found " << sccList1.size() << " SCCs" << std::endl << std::endl;

  /*bool hasDuplicates1 = containsDuplicateSccs(sccList1);
  if(hasDuplicates1) {
    std::cout << "Lockstep saturation gave two or more equal SCCs" << std::endl;
  }
  bool hasOverlap1 = sccListContainsDifferentSccsWithDuplicateNodes(sccList1);
  if(hasOverlap1) {
    std::cout << "Lockstep saturation gave overlapping SCCs" << std::endl;
  }
  bool foundAllSCCs1 = sccUnionIsWholeBdd(sccList1, graph.nodes);
  if(!foundAllSCCs1) {
    std::cout << "Lockstep saturation did not find SCCs covering all nodes" << std::endl;
  }*/

  auto start2 = std::chrono::high_resolution_clock::now();
  std::list<sylvan::Bdd> sccList2 = lockstepRelationUnion(graph);
  auto stop2 = std::chrono::high_resolution_clock::now();
  auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(stop2 - start2);
  std::cout << "Time elapsed (relation union): " << duration2.count() << " milliseconds" << std::endl;
  std::cout << "Found " << sccList2.size() << " SCCs" << std::endl << std::endl;

  /*bool hasDuplicates2 = containsDuplicateSccs(sccList2);
  if(hasDuplicates2) {
    std::cout << "Lockstep relation union gave two or more equal SCCs" << std::endl;
  }
  bool hasOverlap2 = sccListContainsDifferentSccsWithDuplicateNodes(sccList2);
  if(hasOverlap2) {
    std::cout << "Lockstep relation union gave overlapping SCCs" << std::endl;
  }
  bool foundAllSCCs2 = sccUnionIsWholeBdd(sccList2, graph.nodes);
  if(!foundAllSCCs2) {
    std::cout << "Lockstep relation union did not find SCCs covering all nodes" << std::endl;
  }

  if(!sccListCorrectness(sccList1, sccList2)) {
    std::cout << "SCC lists did not contain the same BDDs" << std::endl;
  }*/
}

void timeSaturationIterative(Graph &graph) {
  graph = graphPreprocessing(graph);

  auto start1 = std::chrono::high_resolution_clock::now();
  std::list<sylvan::Bdd> sccList1 = lockstepSaturationIterative(graph);
  auto stop1 = std::chrono::high_resolution_clock::now();
  auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(stop1 - start1);
  std::cout << "Time elapsed (saturation): " << duration1.count() << " milliseconds" << std::endl;
  std::cout << "Found " << sccList1.size() << " SCCs" << std::endl << std::endl;

  /*bool hasDuplicates1 = containsDuplicateSccs(sccList1);
  if(hasDuplicates1) {
    std::cout << "Lockstep saturation gave two or more equal SCCs" << std::endl;
  }
  bool hasOverlap1 = sccListContainsDifferentSccsWithDuplicateNodes(sccList1);
  if(hasOverlap1) {
    std::cout << "Lockstep saturation gave overlapping SCCs" << std::endl;
  }
  bool foundAllSCCs1 = sccUnionIsWholeBdd(sccList1, graph.nodes);
  if(!foundAllSCCs1) {
    std::cout << "Lockstep saturation did not find SCCs covering all nodes" << std::endl;
  }*/

  auto start2 = std::chrono::high_resolution_clock::now();
  std::list<sylvan::Bdd> sccList2 = lockstepRelationUnionIterative(graph);
  auto stop2 = std::chrono::high_resolution_clock::now();
  auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(stop2 - start2);
  std::cout << "Time elapsed (relation union): " << duration2.count() << " milliseconds" << std::endl;
  std::cout << "Found " << sccList2.size() << " SCCs" << std::endl << std::endl;

  /*bool hasDuplicates2 = containsDuplicateSccs(sccList2);
  if(hasDuplicates2) {
    std::cout << "Lockstep relation union gave two or more equal SCCs" << std::endl;
  }
  bool hasOverlap2 = sccListContainsDifferentSccsWithDuplicateNodes(sccList2);
  if(hasOverlap2) {
    std::cout << "Lockstep relation union gave overlapping SCCs" << std::endl;
  }
  bool foundAllSCCs2 = sccUnionIsWholeBdd(sccList2, graph.nodes);
  if(!foundAllSCCs2) {
    std::cout << "Lockstep relation union did not find SCCs covering all nodes" << std::endl;
  }

  if(!sccListCorrectness(sccList1, sccList2)) {
    std::cout << "SCC lists did not contain the same BDDs" << std::endl;
  }*/
}

Graph graphPreprocessing(const Graph &graph) {
  Graph resultGraph = graph;
  resultGraph = pruneGraph(resultGraph);
  resultGraph = pruneGraph(resultGraph);
  resultGraph = pruneGraph(resultGraph);
  resultGraph = pruneGraph(resultGraph);
  resultGraph = pruneGraph(resultGraph);

  resultGraph = sortRelations(resultGraph);

  return resultGraph;
}

bool sccListContains(sylvan::Bdd target, std::list<sylvan::Bdd> sccList) {
  for(sylvan::Bdd scc : sccList) {
    if(scc == target) {
      return true;
    }
  }
  return false;
}

bool containsDuplicateSccs(const std::list<sylvan::Bdd> sccList) {
  for(sylvan::Bdd scc1 : sccList) {
    int duplicates = 0;
    for(sylvan::Bdd scc2 : sccList) {
      if(scc1 == scc2) {
        duplicates++;
      }
    }
    if(duplicates > 1) {
      return true;
    }
  }
  return false;
}

bool sccListContainsDifferentSccsWithDuplicateNodes(const std::list<sylvan::Bdd> sccList) {
  for(sylvan::Bdd scc1 : sccList) {
    for(sylvan::Bdd scc2 : sccList) {
      sylvan::Bdd intersect = intersectBdd(scc1, scc2);
      if(intersect != leaf_false() && scc1 != scc2) {
        return true;
      }
    }
  }
  return false;
}

bool sccUnionIsWholeBdd(const std::list<sylvan::Bdd> sccList, const sylvan::Bdd nodes) {
  sylvan::Bdd bddUnion = leaf_false();
  for(sylvan::Bdd scc : sccList) {
    bddUnion = unionBdd(bddUnion, scc);
  }
  sylvan::Bdd result = differenceBdd(nodes, bddUnion);
  return result == leaf_false();
}


bool sccListCorrectness(std::list<sylvan::Bdd> sccList1, std::list<sylvan::Bdd> sccList2) {
  if(sccList1.size() != sccList2.size()) {
    return false;
  }
  if(containsDuplicateSccs(sccList1) || containsDuplicateSccs(sccList2)) {
    return false;
  }
  for(sylvan::Bdd scc : sccList1) {
    if(!sccListContains(scc, sccList2)) {
      return false;
    }
  }
  return true;
}