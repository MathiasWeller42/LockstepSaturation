#include <filesystem>
#include <list>
#include <string>
#include <deque>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "lockstep.h"
#include "petriTranslation.h"

Graph smallGraph1(){
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