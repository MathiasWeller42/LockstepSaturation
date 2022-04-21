#include <list>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "bdd_utilities.h"

std::list<int> __getVars(const sylvan::Bdd &bdd) {
  std::list<int> varList = {};
  if(bdd.isTerminal()){
    return varList;
  }
  varList.push_back(bdd.TopVar());
  sylvan::Bdd bddThen = bdd.Then();
  std::list<int> recResult1 = __getVars(bddThen);
  varList.splice(varList.end(), recResult1);

  sylvan::Bdd bddElse = bdd.Else();
  std::list<int> recResult2 = __getVars(bddElse);
  varList.splice(varList.end(), recResult2);

  return varList;
}

std::list<int> getVars(const sylvan::Bdd &bdd) {
  std::list<int> varList = __getVars(bdd);
  varList.sort();
  return varList;
}

bool hasOddVars(const sylvan::Bdd &bdd) {
  std::list<int> varList = getVars(bdd);
  for(int i : varList) {
    if(i % 2 == 1) {
      return true;
    }
  }
  return false;
}

//Returns each of the true paths in the BDD as a list of pairs of the form {{x0,0},{x2,1},...,}
//TODO: Har vi brug for den? Vi bruger den ingen steder
inline std::list<std::list<std::pair<int, bool>>> bddAsList(std::list<std::pair<int, bool>> &currentPath,
                                                     const sylvan::Bdd &bdd) {
  std::list<std::list<std::pair<int, bool>>> nodeList = {};
  if(bdd.isTerminal()) {
    if(bdd.isOne()) {
      nodeList.push_front(currentPath);
    }
    return nodeList;
  }

  std::list<std::pair<int, bool>> recPath1(currentPath);
  recPath1.push_back({bdd.TopVar(), true});
  std::list<std::list<std::pair<int, bool>>> recResult1 = bddAsList(recPath1, bdd.Then());
  nodeList.splice(nodeList.end(), recResult1);

  std::list<std::pair<int, bool>> recPath2(currentPath);
  recPath2.push_back({bdd.TopVar(), false});
  std::list<std::list<std::pair<int, bool>>> recResult2 = bddAsList(recPath2, bdd.Else());
  nodeList.splice(nodeList.end(), recResult2);

  return nodeList;
}