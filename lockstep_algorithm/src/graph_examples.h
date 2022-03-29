#ifndef GRAPH_EXAMPLES_H
#define GRAPH_EXAMPLES_H

#include <filesystem>
#include <list>
#include <string>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

Graph noGraph();
Graph oneNodeGraph();
Graph twoNodeGraphTwoSCCs();
Graph twoNodeGraphOneSCC();
Graph twoNodeGraphTwoRelations();
Graph fourNodesOneRelation();
Graph nonConnectedGraph();

Graph graphExample1oneRel();
Graph graphExample1multRel();
#endif //GRAPH_EXAMPLES_H