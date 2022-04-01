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

#endif  //LOCKSTEP_H