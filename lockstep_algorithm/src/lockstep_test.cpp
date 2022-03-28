#include <filesystem>
#include <list>
#include <string>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>
#include <deque>

#include "lockstep.h"
#include "graph_examples.h"
#include "petriTranslation.h"

void simpleTest(){
    Graph graph = smallGraph2Relations();
    sylvan::Bdd nodes = graph.nodes;
    printBdd(nodes);
    lockstepSaturation(graph);
}