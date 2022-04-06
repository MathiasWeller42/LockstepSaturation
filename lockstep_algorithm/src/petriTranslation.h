#ifndef PETRI_TRANSLATION_H
#define PETRI_TRANSLATION_H

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
};

struct Graph {
  sylvan::Bdd nodes;
  sylvan::BddSet cube;
  std::deque<Relation> relations;
};

struct Arc {
  std::string id;
  std::string source;
  std::string target;
  std::string toString()
  {
    return "Arc:   id: " + id + " from: " + source + " to: " + target;
  }
};

struct Transition {
  std::string id;
  std::list<Arc> sources;
  std::list<Arc> targets;

  Transition(std::string idString) : id(idString), sources({}), targets({}) {}
  Transition() {}

  std::string toString()
  {
    int sourceNo = sources.size();
    int targetNo = targets.size();
    std::string sourceString = "Sources: \n";
    std::string targetString = "Targets: \n";
    for(Arc source : sources) {
      sourceString = sourceString + source.toString() + "\n";
    }
    for(Arc target : targets) {
      targetString = targetString + target.toString() + "\n";
    }
    return "I'm a transition id: " + id + "\n" + std::to_string(sourceNo) + " " + sourceString + std::to_string(targetNo) + " " + targetString;
  }
};

sylvan::Bdd makeNode(std::string &bitstring);
sylvan::Bdd makeNodes(std::list<std::string> &bitstrings);
sylvan::Bdd makeArc(std::string &bitstringFrom, std::string &bitstringTo);
Relation makeRelation(std::list<std::pair<std::string, std::string>> &bitstrings, sylvan::BddSet cube);
std::deque<Relation> makeRelations(std::list<std::list<std::pair<std::string, std::string>>> &bitstrings, sylvan::BddSet cube);

void printMap(std::map<std::string, int> map);

Graph PNMLtoGraph();
std::list<std::pair<std::string, std::string>> __printRelationsAsString(std::pair<std::string, std::string> currentPath, const sylvan::Bdd &bdd);

Graph makeGraph(const int nodes, const std::list<std::list<std::pair<int,int>>> &relations);
sylvan::BddSet makeCube(int nodeBytes);

void printBdd(const sylvan::Bdd &bdd);
void printBddAsString(int nodes, const sylvan::Bdd &bdd);
void printSingleRelationAsString(sylvan::Bdd relation);
void printRelationsAsString(std::deque<sylvan::Bdd> relations);

sylvan::Bdd shiftBdd(const sylvan::Bdd &bdd);

//List operations
std::list<int> list_union(std::list<int> &list1, std::list<int> &list2);
std::list<int> list_intersect(std::list<int> &list1, std::list<int> &list2);
std::list<int> list_difference(std::list<int> &list1, std::list<int> &list2);
#endif //PETRI_TRANSLATION_H