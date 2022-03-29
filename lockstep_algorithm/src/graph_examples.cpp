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
 std::list<std::pair<int, int>> r1 = {};
  return makeGraph(0, {r1});
}

Graph oneNodeGraph() {
  std::list<std::pair<int, int>> r1 = {};
  return makeGraph(1, {r1});
}

Graph twoNodeGraphTwoSCCs() {
  std::list<std::pair<int, int>> r1 =
    {{0,1}};
  return makeGraph(2, {r1});
}

Graph twoNodeGraphOneSCC() {
  std::list<std::pair<int, int>> r1 =
    {{0,1},{1,0}};
  return makeGraph(2, {r1});
}


Graph twoNodeGraphTwoRelations() {
  std::list<std::pair<int, int>> r1 =
    {{0,1}};
  std::list<std::pair<int, int>> r2 =
    {{1,0}};
  return makeGraph(2, {r1, r2});
}

Graph fourNodesOneRelation() {
  std::list<std::pair<int, int>> r1 =
    {{0,1}, {0,2}, {1,3}, {3,0}};
  return makeGraph(4, {r1});
}

Graph nonConnectedGraph() {
  std::list<std::pair<int, int>> r1 = {};
  return makeGraph(2, {r1});
}

Graph graphExample1oneRel() {
  std::list<std::pair<int, int>> r1 =
    {{0,1},{1,0},{0,2},{2,1},{1,3},{2,4},{4,3},{4,5},{4,6},{4,7},{5,6},{6,7},{7,5},{7,8}};
  return makeGraph(9, {r1});
}

Graph graphExample1multRel() {
  std::list<std::pair<int, int>> r1 =
    {{0,1},{1,0},{0,2},{4,5},{4,6},{4,7},{5,6}};
  std::list<std::pair<int, int>> r2 =
    {{2,4},{4,3},{6,7}};
  std::list<std::pair<int, int>> r3 =
    {{2,1},{1,3},{7,5},{7,8}};
  return makeGraph(9, {r1,r2,r3});
}

