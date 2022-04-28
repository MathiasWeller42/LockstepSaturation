#ifndef LOCKSTEP_H
#define LOCKSTEP_H
#include <deque>
#include<list>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "petriTranslation.h"

//Iterative versions
std::pair<std::list<sylvan::Bdd>, int> lockstepSaturation(const Graph &fullGraph);
std::pair<std::list<sylvan::Bdd>, int> lockstepRelationUnion(const Graph &fullGraph);
std::pair<std::list<sylvan::Bdd>, int> xieBeerelSaturation(const Graph &fullGraph);
std::pair<std::list<sylvan::Bdd>, int> xieBeerelRelationUnion(const Graph &fullGraph);
std::pair<std::list<sylvan::Bdd>, int> xieBeerelBackwardSaturation(const Graph &fullGraph);
std::pair<std::list<sylvan::Bdd>, int> xieBeerelBackwardRelationUnion(const Graph &fullGraph);

std::pair<sylvan::Bdd, int> reachabilityForwardSaturation(const Graph &graph, sylvan::Bdd nodes);
std::pair<sylvan::Bdd, int> reachabilityBackwardSaturation(const Graph &graph, sylvan::Bdd nodes);
std::pair<sylvan::Bdd, int> reachabilityForwardRelationUnion(const Graph &graph, sylvan::Bdd nodes);
std::pair<sylvan::Bdd, int> reachabilityBackwardRelationUnion(const Graph &graph, sylvan::Bdd nodes);

sylvan::Bdd pick(const sylvan::Bdd &nodeSet, const sylvan::BddSet &cube);

#endif  //LOCKSTEP_H