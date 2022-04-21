#ifndef LOCKSTEP_H
#define LOCKSTEP_H
#include <deque>
#include<list>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "petriTranslation.h"

//Recursive versions
/*std::list<sylvan::Bdd> lockstepSaturation(const Graph &graph);
std::list<sylvan::Bdd> lockstepRelationUnion(const Graph &graph);*/

//Iterative versions
std::list<sylvan::Bdd> lockstepSaturation(const Graph &fullGraph);
std::list<sylvan::Bdd> lockstepRelationUnion(const Graph &fullGraph);
std::list<sylvan::Bdd> lockstepLiteralRelationUnion(const Graph &fullGraph);
std::list<sylvan::Bdd> xieBeerelSaturation(const Graph &fullGraph);
std::list<sylvan::Bdd> xieBeerelRelationUnion(const Graph &fullGraph);
std::list<sylvan::Bdd> xieBeerelLiteralRelationUnion(const Graph &fullGraph);
std::list<sylvan::Bdd> literalRelationUnion(const Graph &fullGraph, bool lockstep);

sylvan::Bdd reachabilityForwardSaturation(const Graph &graph, sylvan::Bdd nodes);
sylvan::Bdd reachabilityBackwardSaturation(const Graph &graph, sylvan::Bdd nodes);
sylvan::Bdd reachabilityForwardRelationUnion(const Graph &graph, sylvan::Bdd nodes);
sylvan::Bdd reachabilityBackwardRelationUnion(const Graph &graph, sylvan::Bdd nodes);

#endif  //LOCKSTEP_H