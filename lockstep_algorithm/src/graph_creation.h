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

sylvan::Bdd makeNode(std::string &bitstring);
sylvan::Bdd makeNodes(std::list<std::string> &bitstrings);
sylvan::Bdd makeArc(std::string &bitstringFrom, std::string &bitstringTo);
Relation makeRelation(std::list<std::pair<std::string, std::string>> &bitstrings, sylvan::BddSet cube);
std::deque<Relation> makeRelations(std::list<std::list<std::pair<std::string, std::string>>> &bitstrings, sylvan::BddSet cube);
sylvan::BddSet makeCube(int nodeBytes);
Graph makeGraph(const int nodes, const std::list<std::list<std::pair<int,int>>> &relations);

Graph sortRelations(const Graph &graph);
Graph pruneGraph(const Graph &graph);

#endif //GRAPH_CREATION_H