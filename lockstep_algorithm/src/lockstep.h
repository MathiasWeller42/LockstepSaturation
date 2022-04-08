#ifndef LOCKSTEP_H
#define LOCKSTEP_H
#include <deque>
#include<list>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "petriTranslation.h"

std::list<sylvan::Bdd> lockstepSaturation(const Graph &graph);
std::list<sylvan::Bdd> lockstepRelationUnion(const Graph &graph);
std::list<sylvan::Bdd> lockstepSaturationIterative(const Graph &fullGraph);
std::list<sylvan::Bdd> lockstepRelationUnionIterative(const Graph &fullGraph);

#endif  //LOCKSTEP_H