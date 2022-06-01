#ifndef GRAPH_EXAMPLES_H
#define GRAPH_EXAMPLES_H

#include <filesystem>
#include <list>
#include <string>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "../src/petriTranslation.h"

Graph noGraph();
Graph oneNodeGraph();
Graph oneNodeGraphSelfLoop();
Graph twoNodeGraphTwoSCCs();
Graph twoNodeGraphOneSCC();
Graph twoNodeGraphTwoRelations();
Graph fourNodesOneRelation();
Graph nonConnectedGraph();

Graph graphExample1oneRel();
Graph graphExample1multRel();

Graph graphExample2oneRel();
Graph graphExample2multRel();

Graph graphExample3oneRel();
Graph graphExample3multRel();

#endif //GRAPH_EXAMPLES_H