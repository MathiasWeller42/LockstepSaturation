#ifndef LOCKSTEP_H
#define LOCKSTEP_H

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>
#include <deque>

struct Graph {
  sylvan::Bdd nodes;
  sylvan::BddSet cube;
  std::deque<sylvan::Bdd> relations;
};

void lockstepSaturation(Graph graph);

#endif  //LOCKSTEP_H