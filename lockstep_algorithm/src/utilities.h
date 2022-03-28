#ifndef UTILITIES_H
#define UTILITIES_H

#include <vector>
#include <algorithm>

#include "petriTranslation.h"

//from adapter.h in bdd-benchmarks (tak til Steffan ;) )
  inline sylvan::Bdd leaf_true()
  { return sylvan::Bdd::bddOne(); }

  inline sylvan::Bdd leaf_false()
  { return sylvan::Bdd::bddZero(); }

  inline sylvan::Bdd ithvar(int label)
  { return sylvan::Bdd::bddVar(label); }

  inline sylvan::Bdd nithvar(int label)
  { return ~sylvan::Bdd::bddVar(label); }

  inline sylvan::Bdd unionBdd(sylvan::Bdd lhs, sylvan::Bdd rhs)
  { return lhs.Or(rhs); }

  inline sylvan::Bdd intersectBdd(sylvan::Bdd lhs, sylvan::Bdd rhs)
  { return lhs.And(rhs); }

  inline sylvan::Bdd differenceBdd(sylvan::Bdd lhs, sylvan::Bdd rhs)
  { return lhs.And(!rhs); }

  inline sylvan::Bdd pickAssignment(sylvan::Bdd bdd, sylvan::BddSet cube) {
    std::vector<bool> realPath = bdd.PickOneCube(cube);
    std::reverse(realPath.begin(), realPath.end());
    for(bool b : realPath) {
      std::cout << b << std::endl;
    }
    std::string bitString = "";
    for(bool b : realPath) {
      if(b){
        bitString = bitString + '1';
      } else {
        bitString = bitString + '0';
      }
    }
    return makePlace(bitString);
  }

  #endif //UTILITIES_H