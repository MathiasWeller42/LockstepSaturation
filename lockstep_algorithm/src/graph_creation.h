#ifndef GRAPH_CREATION_H
#define GRAPH_CREATION_H

#include <filesystem>
#include <list>
#include <string>
#include <deque>
#include <map>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>


/*
Graph creation functions
Contains functions for:
- Creating Graphs from nodes, edges and relations
- Sorting the relations in a graph (mainly relevant for SCC algorithms using saturation)- Pruning graphso

*/


struct Relation {
  sylvan::Bdd relationBdd;
  sylvan::BddSet cube;
  int top;
  bool operator <(const Relation &relation) const
  {
    return top < relation.top;
  }
};

struct Graph {
  sylvan::Bdd nodes;
  sylvan::BddSet cube;
  std::deque<Relation> relations;
};

//Graph building functions
sylvan::Bdd makeNode(std::string &bitstring);
sylvan::Bdd makeNodes(std::list<std::string> &bitstrings);
sylvan::Bdd makeEdge(std::string &bitstringFrom, std::string &bitstringTo);
sylvan::BddSet makeCube(int nodeBytes);

Relation makeRelation(std::list<std::pair<std::string, std::string>> &bitstrings, sylvan::BddSet cube);
std::deque<Relation> makeRelations(std::list<std::list<std::pair<std::string, std::string>>> &bitstrings, sylvan::BddSet cube);

//Main graph buidling function and sorting relations by their top
Graph sortRelations(const Graph &graph);

Graph makeGraph(const int nodes, const std::list<std::list<std::pair<int,int>>> &relations);



//Pruning
Graph pruneGraph(const Graph &graph);
Graph fixedPointPruning(const Graph &graph);
std::pair<Graph, int> fixedPointPruningWithMax(const Graph &graph, int maxPruning);

#endif //GRAPH_CREATION_H