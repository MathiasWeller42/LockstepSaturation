#ifndef BDD_UTILITIES_H
#define BDD_UTILITIES_H

#include <vector>
#include <algorithm>
#include <string>
#include <iostream>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "petriTranslation.h"
#include "graph_creation.h"


//inspiration from adapter.h in bdd-benchmarks - tak til Steffan ;)
inline sylvan::Bdd leaf_true()
{ return sylvan::Bdd::bddOne(); }

inline sylvan::Bdd leaf_false()
{ return sylvan::Bdd::bddZero(); }

inline sylvan::Bdd ithvar(int label)
{ return sylvan::Bdd::bddVar(label); }

inline sylvan::Bdd nithvar(int label)
{ return ~sylvan::Bdd::bddVar(label); }

inline sylvan::Bdd unionBdd(const sylvan::Bdd &lhs, const sylvan::Bdd &rhs)
{ return lhs.Or(rhs); }

inline sylvan::Bdd intersectBdd(const sylvan::Bdd &lhs, const sylvan::Bdd &rhs)
{ return lhs.And(rhs); }

inline sylvan::Bdd differenceBdd(const sylvan::Bdd &lhs, const sylvan::Bdd &rhs)
{ return lhs.And(!rhs); }

inline sylvan::Bdd pickAssignment(const sylvan::Bdd &bdd, const sylvan::BddSet &cube) {
  std::vector<bool> realPath = bdd.PickOneCube(cube);
  std::reverse(realPath.begin(), realPath.end());
  std::string bitString = "";
  for(bool b : realPath) {
    if(b){
      bitString = bitString + '1';
    } else {
      bitString = bitString + '0';
    }
  }
  if(cube.size() != bitString.size()) {
    std::cout << "Something wrong in pick()" << std::endl;
    exit(-1);
  }
  return makeNode(bitString);
}

#endif //BDD_UTILITIES_H