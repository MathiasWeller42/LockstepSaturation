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
  const std::list<std::list<std::pair<int, int>>> relationList = {r1};
  Graph result = makeGraph(0, relationList);
  return result;
}

Graph oneNodeGraph() {
  std::list<std::pair<int, int>> r1 = {};
  const std::list<std::list<std::pair<int, int>>> relationList = {r1};
  Graph result = makeGraph(1, relationList);
  return result;
}

Graph oneNodeGraphSelfLoop() {
  std::list<std::pair<int, int>> r1 =
    {{0,0}};
  const std::list<std::list<std::pair<int, int>>> relationList = {r1};
  Graph result = makeGraph(1, relationList);
  return result;
}

Graph twoNodeGraphTwoSCCs() {
  std::list<std::pair<int, int>> r1 =
    {{0,1}};
  const std::list<std::list<std::pair<int, int>>> relationList = {r1};
  Graph result = makeGraph(2, relationList);
  return result;
}

Graph twoNodeGraphOneSCC() {
  std::list<std::pair<int, int>> r1 =
    {{0,1},{1,0}};
  const std::list<std::list<std::pair<int, int>>> relationList = {r1};
  Graph result = makeGraph(2, relationList);
  return result;
}


Graph twoNodeGraphTwoRelations() {
  std::list<std::pair<int, int>> r1 =
    {{0,1}};
  std::list<std::pair<int, int>> r2 =
    {{1,0}};
  const std::list<std::list<std::pair<int, int>>> relationList = {r1,r2};
  Graph result = makeGraph(2, relationList);
  return result;
}

Graph threeNodesOneRelation() {
  std::list<std::pair<int, int>> r1 =
    {{0,1}, {0,2}, {1,3}, {3,0}};
  const std::list<std::list<std::pair<int, int>>> relationList = {r1};
  Graph result = makeGraph(4, relationList);
  return result;
}

Graph fourNodesOneRelation() {
  std::list<std::pair<int, int>> r1 =
    {{0,1}, {0,2}, {1,3}, {3,0}};
  const std::list<std::list<std::pair<int, int>>> relationList = {r1};
  Graph result = makeGraph(4, relationList);
  return result;
}

Graph nonConnectedGraph() {
  std::list<std::pair<int, int>> r1 = {};
  const std::list<std::list<std::pair<int, int>>> relationList = {r1};
  Graph result = makeGraph(2, relationList);
  return result;
}

Graph graphExample1oneRel() {
  std::list<std::pair<int, int>> r1 =
    {{0,1},{1,0},{0,2},{2,1},{1,3},{2,4},{4,3},{4,5},{4,6},{4,7},{5,6},{6,7},{7,5},{7,8}};
  const std::list<std::list<std::pair<int, int>>> relationList = {r1};
  Graph result = makeGraph(9, relationList);
  return result;
}

Graph graphExample1multRel() {
  std::list<std::pair<int, int>> r1 =
    {{0,1},{1,0},{0,2},{4,5},{4,6},{4,7},{5,6}};
  std::list<std::pair<int, int>> r2 =
    {{2,4},{4,3},{6,7}};
  std::list<std::pair<int, int>> r3 =
    {{2,1},{1,3},{7,5},{7,8}};
  const std::list<std::list<std::pair<int, int>>> relationList = {r1,r2,r3};
  Graph result = makeGraph(9, relationList);
  return result;
}

