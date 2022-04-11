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

std::list<std::string> getPathStrings() {
  std::list<std::string> resultList = {};

  //resultList.push_back("ShieldRVt/PT/shield_t_rv_001_a_11place.pnml");      //11
  //resultList.push_back("GPUForwardProgress/PT/userdef_15place.pnml");       //15
  //resultList.push_back("ShieldRVs/PT/shield_s_rv_001_a_17place.pnml");      //17
  //resultList.push_back("ShieldRVt/PT/shield_t_rv_002_a_19place.pnml");      //19
  resultList.push_back("ShieldIIPt/PT/shield_t_iip_001_a_22place.pnml");    //22
  //resultList.push_back("GPUForwardProgress/PT/gpufp_04_a_24place.pnml");  //24
  //resultList.push_back("ShieldRVt/PT/shield_t_rv_003_a_27place.pnml");    //27

  return resultList;
}

void timeAll(const Graph &graph) {
  //Iterative saturation
  auto start3 = std::chrono::high_resolution_clock::now();
  std::list<sylvan::Bdd> sccList3 = lockstepSaturationIterative(graph);
  auto stop3 = std::chrono::high_resolution_clock::now();
  auto duration3 = std::chrono::duration_cast<std::chrono::milliseconds>(stop3 - start3);
  std::cout << "Time elapsed (iterative saturation): " << duration3.count() << " milliseconds" << std::endl;
  std::cout << "Found " << sccList3.size() << " SCCs" << std::endl << std::endl;

  //Iterative relation union
  auto start4 = std::chrono::high_resolution_clock::now();
  std::list<sylvan::Bdd> sccList4 = lockstepRelationUnionIterative(graph);
  auto stop4 = std::chrono::high_resolution_clock::now();
  auto duration4 = std::chrono::duration_cast<std::chrono::milliseconds>(stop4 - start4);
  std::cout << "Time elapsed (iterative relation union): " << duration4.count() << " milliseconds" << std::endl;
  std::cout << "Found " << sccList4.size() << " SCCs" << std::endl << std::endl;

  //Iterative LITERAL relation union
  auto start5 = std::chrono::high_resolution_clock::now();
  std::list<sylvan::Bdd> sccList5 = lockstepRelationUnionIterative(graph);
  auto stop5 = std::chrono::high_resolution_clock::now();
  auto duration5 = std::chrono::duration_cast<std::chrono::milliseconds>(stop5 - start5);
  std::cout << "Time elapsed (LITERAL iterative relation union): " << duration5.count() << " milliseconds" << std::endl;
  std::cout << "Found " << sccList5.size() << " SCCs" << std::endl << std::endl;
}

void experiment() {
  std::list<std::string> pathStrings = getPathStrings();
  for(std::string pathString : pathStrings) {
    std::cout << "###### Running experiment on file at path: " << pathString << std::endl;
    Graph graph = PNMLtoGraph(pathString);

    std::cout << "### With pre-processing (32)" << std::endl;
    Graph processedGraph1 = graphPreprocessing(graph, 32);
    timeAll(processedGraph1);
    std::cout << std::endl;

    std::cout << "### With pre-processing (16)" << std::endl;
    Graph processedGraph2 = graphPreprocessing(graph, 16);
    timeAll(processedGraph2);
    std::cout << std::endl;

    std::cout << "### With pre-processing (8)" << std::endl;
    Graph processedGraph3 = graphPreprocessing(graph, 8);
    timeAll(processedGraph3);
    std::cout << std::endl;

    std::cout << "### With pre-processing (4)" << std::endl;
    Graph processedGraph4 = graphPreprocessing(graph, 4);
    timeAll(processedGraph4);
    std::cout << std::endl;

    std::cout << "### With pre-processing (2)" << std::endl;
    Graph processedGraph5 = graphPreprocessing(graph, 2);
    timeAll(processedGraph5);
    std::cout << std::endl;

    std::cout << "### With pre-processing (1)" << std::endl;
    Graph processedGraph6 = graphPreprocessing(graph, 1);
    timeAll(processedGraph6);
    std::cout << std::endl;

    std::cout << "### No pre-processing" << std::endl;
    Graph processedGraph7 = graphPreprocessing(graph, 0);
    timeAll(processedGraph7);
    std::cout << std::endl;
  }
}

void timeSaturation(const Graph &graph, int pruningSteps) {
  Graph processedGraph = graphPreprocessing(graph, pruningSteps);

  auto start1 = std::chrono::high_resolution_clock::now();
  std::list<sylvan::Bdd> sccList1 = lockstepSaturation(processedGraph);
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
  bool foundAllSCCs1 = sccUnionIsWholeBdd(sccList1, processedGraph.nodes);
  if(!foundAllSCCs1) {
    std::cout << "Lockstep saturation did not find SCCs covering all nodes" << std::endl;
  }*/

  auto start2 = std::chrono::high_resolution_clock::now();
  std::list<sylvan::Bdd> sccList2 = lockstepRelationUnion(processedGraph);
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
  bool foundAllSCCs2 = sccUnionIsWholeBdd(sccList2, processedGraph.nodes);
  if(!foundAllSCCs2) {
    std::cout << "Lockstep relation union did not find SCCs covering all nodes" << std::endl;
  }

  if(!sccListCorrectness(sccList1, sccList2)) {
    std::cout << "SCC lists did not contain the same BDDs" << std::endl;
  }*/
}

void timeSaturationIterative(const Graph &graph, int pruningSteps) {
  Graph processedGraph = graphPreprocessing(graph, pruningSteps);

  auto start1 = std::chrono::high_resolution_clock::now();
  std::list<sylvan::Bdd> sccList1 = lockstepSaturationIterative(processedGraph);
  auto stop1 = std::chrono::high_resolution_clock::now();
  auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(stop1 - start1);
  std::cout << "Time elapsed (iterative saturation): " << duration1.count() << " milliseconds" << std::endl;
  std::cout << "Found " << sccList1.size() << " SCCs" << std::endl << std::endl;

  /*bool hasDuplicates1 = containsDuplicateSccs(sccList1);
  if(hasDuplicates1) {
    std::cout << "Lockstep saturation gave two or more equal SCCs" << std::endl;
  }
  bool hasOverlap1 = sccListContainsDifferentSccsWithDuplicateNodes(sccList1);
  if(hasOverlap1) {
    std::cout << "Lockstep saturation gave overlapping SCCs" << std::endl;
  }
  bool foundAllSCCs1 = sccUnionIsWholeBdd(sccList1, processedGraph.nodes);
  if(!foundAllSCCs1) {
    std::cout << "Lockstep saturation did not find SCCs covering all nodes" << std::endl;
  }*/

  auto start2 = std::chrono::high_resolution_clock::now();
  std::list<sylvan::Bdd> sccList2 = lockstepRelationUnionIterative(processedGraph);
  auto stop2 = std::chrono::high_resolution_clock::now();
  auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(stop2 - start2);
  std::cout << "Time elapsed (iterative relation union): " << duration2.count() << " milliseconds" << std::endl;
  std::cout << "Found " << sccList2.size() << " SCCs" << std::endl << std::endl;

  bool hasDuplicates2 = containsDuplicateSccs(sccList2);
  if(hasDuplicates2) {
    std::cout << "Lockstep relation union gave two or more equal SCCs" << std::endl;
  }
  bool hasOverlap2 = sccListContainsDifferentSccsWithDuplicateNodes(sccList2);
  if(hasOverlap2) {
    std::cout << "Lockstep relation union gave overlapping SCCs" << std::endl;
  }
  bool foundAllSCCs2 = sccUnionIsWholeBdd(sccList2, processedGraph.nodes);
  if(!foundAllSCCs2) {
    std::cout << "Lockstep relation union did not find SCCs covering all nodes" << std::endl;
  }

  auto start3 = std::chrono::high_resolution_clock::now();
  std::list<sylvan::Bdd> sccList3 = lockstepRelationUnionIterative(processedGraph);
  auto stop3 = std::chrono::high_resolution_clock::now();
  auto duration3 = std::chrono::duration_cast<std::chrono::milliseconds>(stop3 - start3);
  std::cout << "Time elapsed (iterative literal relation union): " << duration3.count() << " milliseconds" << std::endl;
  std::cout << "Found " << sccList3.size() << " SCCs" << std::endl << std::endl;

  bool hasDuplicates3 = containsDuplicateSccs(sccList3);
  if(hasDuplicates3) {
    std::cout << "Lockstep literal relation union gave two or more equal SCCs" << std::endl;
  }
  bool hasOverlap3 = sccListContainsDifferentSccsWithDuplicateNodes(sccList3);
  if(hasOverlap3) {
    std::cout << "Lockstep literal relation union gave overlapping SCCs" << std::endl;
  }
  bool foundAllSCCs3 = sccUnionIsWholeBdd(sccList3, processedGraph.nodes);
  if(!foundAllSCCs3) {
    std::cout << "Lockstep literal relation union did not find SCCs covering all nodes" << std::endl;
  }

  if(!sccListCorrectness(sccList2, sccList3)) {
    std::cout << "SCC lists did not contain the same BDDs" << std::endl;
  }
}

Graph graphPreprocessing(const Graph &graph, int pruningSteps) {
  Graph resultGraph = graph;
  resultGraph = sortRelations(resultGraph);

  for(int i = 0; i < pruningSteps; i++) {
    resultGraph = pruneGraph(resultGraph);
  }

  std::cout << "Finished pre-processing of graph" << std::endl;
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