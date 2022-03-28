#include <filesystem>
#include <list>
#include <string>
#include <deque>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "lockstep.h"
#include "petriTranslation.h"
#include "utilities.h"

Graph noGraph() {
  Graph graph = {};

  //Nodes
  std::list<std::string> nodeStrings = {};
  sylvan::Bdd nodeSet = makePlaces(nodeStrings);

  //Cube
  sylvan::BddSet cube = sylvan::BddSet();
  cube.add(0);
  cube.add(2);

  //Arcs
  std::list<std::pair<std::string, std::string>> arcStrings = {};
  sylvan::Bdd edgeSet = makeTransition(arcStrings);

  //Relation
  std::deque<sylvan::Bdd> relations;
  relations.push_front(edgeSet);

  graph.nodes = nodeSet;
  graph.relations = relations;
  graph.cube = cube;
  return graph;
}

Graph oneNodeGraph() {
  Graph graph = {};

  //Nodes
  std::string aString = "00";
  std::list<std::string> nodeStrings = {aString};
  sylvan::Bdd nodeSet = makePlaces(nodeStrings);

  //Cube
  sylvan::BddSet cube = sylvan::BddSet();
  cube.add(0);
  cube.add(2);

  //Arcs
  std::list<std::pair<std::string, std::string>> arcStrings = {};
  sylvan::Bdd edgeSet = makeTransition(arcStrings);

  //Relation
  std::deque<sylvan::Bdd> relations;
  relations.push_front(edgeSet);

  graph.nodes = nodeSet;
  graph.relations = relations;
  graph.cube = cube;
  return graph;
}

Graph smallGraph1() {
  Graph graph = {};

  //Nodes
  std::string aString = "00";
  std::string bString = "01";
  std::list<std::string> nodeStrings = {aString,bString};
  sylvan::Bdd nodeSet = makePlaces(nodeStrings);

  //Cube
  sylvan::BddSet cube = sylvan::BddSet();
  cube.add(0);
  cube.add(2);

  //Arcs
  std::list<std::pair<std::string, std::string>> arcStrings = {{aString, bString}};
  sylvan::Bdd edgeSet = makeTransition(arcStrings);

  //Relation
  std::deque<sylvan::Bdd> relations;
  relations.push_front(edgeSet);

  graph.nodes = nodeSet;
  graph.relations = relations;
  graph.cube = cube;
  return graph;
}

Graph smallGraph2() {
  Graph graph = {};

  //Nodes
  std::string aString = "00";
  std::string bString = "01";
  std::list<std::string> nodeStrings = {aString,bString};
  sylvan::Bdd nodeSet = makePlaces(nodeStrings);

  //Cube
  sylvan::BddSet cube = sylvan::BddSet();
  cube.add(0);
  cube.add(2);

  //Arcs
  std::list<std::pair<std::string, std::string>> arcStrings = {{aString, bString},{bString,aString}};
  sylvan::Bdd edgeSet = makeTransition(arcStrings);

  //Relation
  std::deque<sylvan::Bdd> relations;
  relations.push_front(edgeSet);

  graph.nodes = nodeSet;
  graph.relations = relations;
  graph.cube = cube;
  return graph;
}

Graph smallGraph2Relations() {
  Graph graph = {};

  //Nodes
  std::string aString = "00";
  std::string bString = "01";
  std::list<std::string> nodeStrings = {aString,bString};
  sylvan::Bdd nodeSet = makePlaces(nodeStrings);

  //Cube
  sylvan::BddSet cube = sylvan::BddSet();
  cube.add(0);
  cube.add(2);

  //Arcs & relations
  std::list<std::pair<std::string, std::string>> arcStrings1 = {{aString, bString}};
  std::list<std::pair<std::string, std::string>> arcStrings2 = {{bString, aString}};
  std::list<std::list<std::pair<std::string, std::string>>> transList = {arcStrings1, arcStrings2};
  std::deque<sylvan::Bdd> relations = makeTransitions(transList);

  graph.nodes = nodeSet;
  graph.relations = relations;
  graph.cube = cube;
  return graph;
}