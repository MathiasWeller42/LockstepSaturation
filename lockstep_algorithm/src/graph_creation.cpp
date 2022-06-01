#include <iostream>
#include <list>
#include <unistd.h>
#include <math.h>
#include <algorithm>
#include <string>
#include <limits>
#include <chrono>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "graph_creation.h"
#include "bdd_utilities.h"
#include "petriTranslation.h"
#include "print.h"


sylvan::Bdd makeNode(std::string &bitstring) {
  sylvan::Bdd resultBdd = leaf_true();
  bool currentBit;
  sylvan::Bdd currentBdd;
  int currentI;
  int n = bitstring.length();
  for (int i = n - 1; i >= 0; i--) {
    currentBit = bitstring[i] == '1';
    currentI = 2*(n-1-i);
    if(currentBit) {
      currentBdd = ithvar(currentI);
    } else {
      currentBdd = nithvar(currentI);
    }
    resultBdd = intersectBdd(resultBdd, currentBdd);
  }
  return resultBdd;
}

sylvan::Bdd makeNodes(std::list<std::string> &bitstrings) {
  int n = bitstrings.size();
  std::string currentBitstring;
  sylvan::Bdd currentBdd;
  sylvan::Bdd resultBdd = leaf_false();
  for(std::string currentBitstring : bitstrings) {
    currentBdd = makeNode(currentBitstring);
    resultBdd = unionBdd(resultBdd, currentBdd);
  }
  return resultBdd;
}

inline sylvan::Bdd makeEdge(std::string &bitstringFrom, std::string &bitstringTo) {
  int nFrom = bitstringFrom.length();
  bool currentBitFrom;
  sylvan::Bdd currentBddFrom;
  int currentIFrom;
  bool currentBitTo;
  sylvan::Bdd currentBddTo;
  int currentITo;
  sylvan::Bdd resultBdd = leaf_true();
  for (int i = nFrom - 1; i >= 0; i--) {
    currentBitFrom = bitstringFrom[i] == '1';
    currentBitTo = bitstringTo[i] == '1';
    currentIFrom = 2*(nFrom-1-i);
    currentITo = currentIFrom + 1;
    if(currentBitFrom) {
      currentBddFrom = ithvar(currentIFrom);
    } else {
      currentBddFrom = nithvar(currentIFrom);
    }
    if(currentBitTo) {
      currentBddTo = ithvar(currentITo);
    } else {
      currentBddTo = nithvar(currentITo);
    }
    resultBdd = intersectBdd(intersectBdd(resultBdd, currentBddFrom), currentBddTo);
  }
  return resultBdd;
}

Relation makeRelation(std::list<std::pair<std::string, std::string>> &bitstrings, sylvan::BddSet cube) {
  sylvan::Bdd currentBdd;
  sylvan::Bdd resultBdd = leaf_false();
  for(std::pair<std::string, std::string> currentPair : bitstrings) {
    currentBdd = makeEdge(currentPair.first, currentPair.second);
    resultBdd = unionBdd(resultBdd, currentBdd);
  }
  Relation resultRelation = {};
  resultRelation.relationBdd = resultBdd;
  resultRelation.top = 0;
  resultRelation.bottom = 0;
  resultRelation.cube = cube;
  return resultRelation;
}

inline std::deque<Relation> makeRelations(std::list<std::list<std::pair<std::string, std::string>>> &bitstrings, sylvan::BddSet cube) {
  std::deque<Relation> transitions;
  Relation currentRelation;
  for(std::list<std::pair<std::string, std::string>> currentBitstringList : bitstrings) {
    currentRelation = makeRelation(currentBitstringList, cube);
    transitions.push_back(currentRelation);
  }
  return transitions;
}

inline std::string decimalToBinaryString(int number, int bytes) {
  std::string binary = "";
  int mask = 1;
  for(int i = 0; i < bytes; i++) {
    if((mask & number) >= 1) {
      binary = "1" + binary;
    } else {
      binary = "0" + binary;
    }
    mask <<= 1;
  }
  return binary;
}

//Main function for making graphs, takes as input (noOfNodes, relations={{(0,1),(1,0),(2,0)}, {(2,3)}})
Graph makeGraph(const int nodes, const std::list<std::list<std::pair<int,int>>> &relations) {
  Graph graph {};

  //Byte translation
  int nodeBits = nodes == 1 ? 1 : ceil(log2(nodes));

  std::list<std::string> placeList = {};
  std::string iInBinary;

  //Make the nodeset
  for(int i = 0; i < nodes; i++) {
    iInBinary = decimalToBinaryString(i, nodeBits);
    placeList.push_back(iInBinary);
  }
  sylvan::Bdd places = makeNodes(placeList);

  //Make the cube
  sylvan::BddSet cube = makeCube(nodeBits);

  //Make the relationDeque
  std::list<std::list<std::pair<std::string,std::string>>> newRelations;
  for(std::list<std::pair<int,int>> relation : relations) {
    std::list<std::pair<std::string, std::string>> newRelation;

    for(std::pair<int,int> pair : relation) {
      std::string first = decimalToBinaryString(pair.first, nodeBits);
      std::string second = decimalToBinaryString(pair.second, nodeBits);
      std::pair<std::string, std::string> newPair(first,second);
      newRelation.push_back(newPair);
    }
    newRelations.push_back(newRelation);
  }
  std::deque<Relation> realRelations = makeRelations(newRelations, cube);

  graph.nodes = places;
  graph.relations = realRelations;
  graph.cube = cube;
  return graph;
}

sylvan::BddSet makeCube(int nodeBits) {
  sylvan::BddSet cube = sylvan::BddSet();
  for(int i = 0; i < nodeBits; i++) {
    cube.add(2 * i);
  }
  return cube;
}

//Removes (hopefully) most (some) 1-node SCC's from the graph
//Removes the nodes that have in-degree or out-degree 0
Graph pruneGraph(const Graph &graph) {
  sylvan::Bdd nodes = graph.nodes;
  sylvan::BddSet cube = graph.cube;
  std::deque<Relation> relations = graph.relations;

  Graph resultGraph = {};

  sylvan::Bdd frontRes = leaf_false();
  sylvan::Bdd backRes = leaf_false();

  auto start2 = std::chrono::high_resolution_clock::now();
  for(Relation relation : relations) {
    frontRes = unionBdd(nodes.RelNext(relation.relationBdd, relation.cube), frontRes);
    backRes = unionBdd(nodes.RelPrev(relation.relationBdd, relation.cube), backRes);
  }

  sylvan::Bdd nodesWithoutSomeSingletonSccs = intersectBdd(frontRes, backRes);

  resultGraph.nodes = nodesWithoutSomeSingletonSccs;
  resultGraph.cube = cube;
  resultGraph.relations = relations;

  return resultGraph;
}

Graph fixedPointPruning(const Graph &graph) {
  /*auto start = std::chrono::high_resolution_clock::now();
  auto stop = std::chrono::high_resolution_clock::now();
  std::chrono::duration<long, std::milli> duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);*/

  sylvan::Bdd oldNodes = graph.nodes;

  Graph resultGraph = pruneGraph(graph);
  int pruningSteps = 1;

  while(resultGraph.nodes != oldNodes) {
    /*stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    if((int)duration.count() > 60000) {
      std::cout << "Took too long pruning " << (int)duration.count() << std::endl;
      resultGraph.nodes = leaf_false();
      return resultGraph;
    }*/

    oldNodes = resultGraph.nodes;
    resultGraph = pruneGraph(resultGraph);
    pruningSteps++;
  }

  std::cout << "Finished fixed-point pruning after " << pruningSteps << " pruning steps" << std::endl;
  return resultGraph;
}

std::pair<Graph, int> fixedPointPruningWithMax(const Graph &graph, int maxPruning) {
  sylvan::Bdd oldNodes = graph.nodes;

  Graph resultGraph = pruneGraph(graph);
  int pruningSteps = 1;

  while(resultGraph.nodes != oldNodes && pruningSteps < maxPruning) {
    oldNodes = resultGraph.nodes;
    resultGraph = pruneGraph(resultGraph);
    pruningSteps++;
  }

  std::cout << "Finished fixed-point pruning after " << pruningSteps << " pruning steps with max number of steps: " << maxPruning << std::endl;
  std::pair<Graph, int> result = {resultGraph, pruningSteps};
  return result;
}

Graph sortRelations(const Graph &graph) {
  sylvan::Bdd nodes = graph.nodes;
  sylvan::BddSet cube = graph.cube;
  std::deque<Relation> relations = graph.relations;

  //Sort relations in reverse order of top (largest top first)
  std::sort(relations.begin(),relations.end());
  std::reverse(relations.begin(),relations.end());

  Graph resultGraph = {};
  resultGraph.nodes = nodes;
  resultGraph.cube = cube;
  resultGraph.relations = relations;

  return resultGraph;
}