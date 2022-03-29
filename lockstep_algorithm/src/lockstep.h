#ifndef LOCKSTEP_H
#define LOCKSTEP_H
#include <deque>
#include<list>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

struct Graph {
  sylvan::Bdd nodes;
  sylvan::BddSet cube;
  std::deque<sylvan::Bdd> relations;
};

std::list<sylvan::Bdd> lockstepSaturation(Graph graph);

#endif  //LOCKSTEP_H