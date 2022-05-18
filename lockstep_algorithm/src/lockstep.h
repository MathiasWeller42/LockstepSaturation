#ifndef LOCKSTEP_H
#define LOCKSTEP_H
#include <deque>
#include<list>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "petriTranslation.h"

/*
This contains:
- Iterative versions of SCC-finding algorithms lockstep and Xie Beerel, both with and without Saturation
- Functions for computing forward and backward sets
- Pick function
*/

//Lockstep
std::pair<std::list<sylvan::Bdd>, int> lockstepSaturation(const Graph &fullGraph);
std::pair<std::list<sylvan::Bdd>, int> lockstepRelationUnion(const Graph &fullGraph);

//Xie-Beerel
std::pair<std::list<sylvan::Bdd>, int> xieBeerelForwardSaturation(const Graph &fullGraph);
std::pair<std::list<sylvan::Bdd>, int> xieBeerelForwardRelationUnion(const Graph &fullGraph);
std::pair<std::list<sylvan::Bdd>, int> xieBeerelSaturation(const Graph &fullGraph);
std::pair<std::list<sylvan::Bdd>, int> xieBeerelRelationUnion(const Graph &fullGraph);

//Reachability
std::pair<sylvan::Bdd, int> reachabilityForwardSaturation(const Graph &graph, sylvan::Bdd nodes);
std::pair<sylvan::Bdd, int> reachabilityBackwardSaturation(const Graph &graph, sylvan::Bdd nodes);
std::pair<sylvan::Bdd, int> reachabilityForwardRelationUnion(const Graph &graph, sylvan::Bdd nodes);
std::pair<sylvan::Bdd, int> reachabilityBackwardRelationUnion(const Graph &graph, sylvan::Bdd nodes);

//Pick a single node from a nodeSet
sylvan::Bdd pick(const sylvan::Bdd &nodeSet, const sylvan::BddSet &cube);


//BDD counting
std::pair<std::list<sylvan::Bdd>, int> lockstepRelationUnionBDDSize(const Graph &fullGraph);
std::pair<std::list<sylvan::Bdd>, int> lockstepSaturationBDDSize(const Graph &fullGraph);
std::pair<std::list<sylvan::Bdd>, int> xieBeerelSaturationBDDSize(const Graph &fullGraph);
std::pair<std::list<sylvan::Bdd>, int> xieBeerelRelationUnionBDDSize(const Graph &fullGraph);

std::pair<sylvan::Bdd, unsigned long long> reachabilityBackwardSaturationBDDSize(const Graph &graph, sylvan::Bdd nodes);
std::pair<sylvan::Bdd, unsigned long long> reachabilityForwardSaturationBDDSize(const Graph &graph, sylvan::Bdd nodes);
std::pair<sylvan::Bdd, unsigned long long> reachabilityBackwardRelationUnionBDDSize(const Graph &graph, sylvan::Bdd nodes);
std::pair<sylvan::Bdd, unsigned long long> reachabilityForwardRelationUnionBDDSize(const Graph &graph, sylvan::Bdd nodes);


#endif  //LOCKSTEP_H