#include <iostream>
#include <list>
#include <deque>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "lockstep.h"
#include "utilities.h"
#include "petriTranslation.h"

sylvan::Bdd pick(const sylvan::Bdd &nodeSet, const sylvan::BddSet &cube) {
	//Find path in BDD that evaluates to true, and evaluate the decisions into new node
	return pickAssignment(nodeSet, cube);
}

sylvan::Bdd updateRelation(const sylvan::Bdd &currentRelation, const sylvan::Bdd &nodeSet) {
  sylvan::Bdd newRelation = intersectBdd(intersectBdd(currentRelation, nodeSet), shiftBdd(nodeSet));
  return newRelation;
}

std::deque<sylvan::Bdd> updateRelations(const std::deque<sylvan::Bdd> &currentRelations,
                                        const sylvan::Bdd &nodeSet) {
  std::deque<sylvan::Bdd> newRelations;
  for(sylvan::Bdd currentRelation : currentRelations) {
    sylvan::Bdd newCurrentRelation = updateRelation(currentRelation, nodeSet);
    newRelations.push_back(newCurrentRelation);
  }
  return newRelations;
}

std::list<sylvan::Bdd> lockstepSaturation(const Graph &graph) {
  const sylvan::Bdd nodeSet = graph.nodes;
  const sylvan::BddSet cube = graph.cube;
  const std::deque<sylvan::Bdd> relationDeque = graph.relations;

  if(nodeSet == leaf_false()) {
    return {};
  }

  sylvan::Bdd v = pick(nodeSet, cube);
  sylvan::Bdd forwardSet = v;
	sylvan::Bdd backwardSet = v;

  int relFrontI = 0;
  sylvan::Bdd relFront = relationDeque[relFrontI];
  int relBackI = 0;
  sylvan::Bdd relBack = relationDeque[relBackI];

  //Expand both forward and backward sets until one converges
  while(relFrontI < relationDeque.size() && relBackI < relationDeque.size()) {
    //Find images
    sylvan::Bdd relResultFront = differenceBdd(forwardSet.RelNext(relFront, cube), forwardSet);
    sylvan::Bdd relResultBack = differenceBdd(backwardSet.RelPrev(relBack, cube), backwardSet);

    //Update relations
    if(relResultFront == leaf_false()) {
      relFrontI++;
      relFront = relationDeque[relFrontI];
    } else {
      relFrontI = 0;
      relFront = relationDeque[relFrontI];
    }
    if(relResultBack == leaf_false()) {
      relBackI++;
      relBack = relationDeque[relBackI];
    } else {
      relBackI = 0;
      relBack = relationDeque[relBackI];
    }

	  //Add to the forward and backward sets
    forwardSet = unionBdd(forwardSet, relResultFront);
    backwardSet = unionBdd(backwardSet, relResultBack);

    //std::cout << relFrontI << " " << relBackI << std::endl;
  }

  //Save the set that has converged
  bool frontConverged = relFrontI == relationDeque.size();
  sylvan::Bdd converged = frontConverged ? forwardSet : backwardSet;

  //Find where the non-converged overlaps the converged set
  while(relFrontI < relationDeque.size() || relBackI < relationDeque.size()) {
    if(frontConverged) {
      sylvan::Bdd relResultBack = differenceBdd(backwardSet.RelPrev(relBack, cube), backwardSet);
      if(intersectBdd(relResultBack, forwardSet) == leaf_false()) {
        relBackI++;
        relBack = relationDeque[relBackI];
      } else {
        relBackI = 0;
        relBack = relationDeque[relBackI];
        backwardSet = unionBdd(backwardSet, intersectBdd(relResultBack, forwardSet));
      }
    } else {
      sylvan::Bdd relResultFront = differenceBdd(forwardSet.RelNext(relFront, cube), forwardSet);
      if(intersectBdd(relResultFront, backwardSet) == leaf_false()) {
        relFrontI++;
        relFront = relationDeque[relFrontI];
      } else {
        relFrontI = 0;
        relFront = relationDeque[relFrontI];
        forwardSet = unionBdd(forwardSet, intersectBdd(relResultFront, backwardSet));
      }
    }
  }

  //Create SCC
  sylvan::Bdd scc = intersectBdd(forwardSet, backwardSet);
  std::list<sylvan::Bdd> sccList = {scc};

  //Recursive calls
  sylvan::Bdd recBdd1 = differenceBdd(converged, scc);
  std::deque<sylvan::Bdd> recRelations1 = updateRelations(relationDeque, recBdd1);
  Graph recursiveGraph1 = {recBdd1, cube, recRelations1};

  std::list<sylvan::Bdd> recursiveResult1 = lockstepSaturation(recursiveGraph1);
  sccList.splice(sccList.end(), recursiveResult1);

  sylvan::Bdd recBdd2 = differenceBdd(nodeSet, converged);
  std::deque<sylvan::Bdd> recRelations2 = updateRelations(relationDeque, recBdd2);
  Graph recursiveGraph2 = {recBdd2, cube, recRelations2};

  std::list<sylvan::Bdd> recursiveResult2 = lockstepSaturation(recursiveGraph2);
  sccList.splice(sccList.end(), recursiveResult2);

  //Return SCC list
  return sccList;
}

std::list<sylvan::Bdd> lockstepRelationUnion(const Graph &graph) {
  const sylvan::Bdd nodeSet = graph.nodes;
  const sylvan::BddSet cube = graph.cube;
  const std::deque<sylvan::Bdd> relationDeque = graph.relations;

  if(nodeSet == leaf_false()) {
    return {};
  }

  sylvan::Bdd v = pick(nodeSet, cube);
  sylvan::Bdd forwardSet = v;
	sylvan::Bdd backwardSet = v;
  sylvan::Bdd forwardFront = leaf_false();
  sylvan::Bdd backwardFront = leaf_false();

  sylvan::Bdd relResultFront;
  sylvan::Bdd relResultBack;

  sylvan::Bdd relFront;
  sylvan::Bdd relBack;

  //Expand both forward and backward sets until one converges
  bool somethingChangedFront = true;
  bool somethingChangedBack = true;
  while(somethingChangedFront && somethingChangedBack) {
    somethingChangedFront = false;
    somethingChangedBack = false;
    for(int i = 0 ; i < relationDeque.size(); i++) {
      relFront = relationDeque[i];
      relBack = relationDeque[i];
      sylvan::Bdd relResultFront = differenceBdd(forwardSet.RelNext(relFront, cube), forwardSet);
      sylvan::Bdd relResultBack = differenceBdd(backwardSet.RelPrev(relBack, cube), backwardSet);
      if(relResultFront != leaf_false()) {
        somethingChangedFront = true;
      }
      if(relResultBack != leaf_false()) {
        somethingChangedBack = true;
      }
      forwardFront = unionBdd(forwardFront, relResultFront);
      backwardFront = unionBdd(backwardFront, relResultBack);
    }
    forwardSet = unionBdd(forwardSet, forwardFront);
    forwardFront = leaf_false();
    backwardSet = unionBdd(backwardSet, backwardFront);
    backwardFront = leaf_false();
  }

  //Save the set that has converged
  bool frontConverged = !somethingChangedFront;
  sylvan::Bdd converged = frontConverged ? forwardSet : backwardSet;

  //Find where the non-converged overlaps the converged set
  while(somethingChangedFront || somethingChangedBack) {
    if(frontConverged) {
      somethingChangedBack = false;
      for(int i = 0 ; i < relationDeque.size(); i++) {
        relBack = relationDeque[i];
        sylvan::Bdd relResultBack = differenceBdd(backwardSet.RelPrev(relBack, cube), backwardSet);
        if(relResultBack != leaf_false()) {
          somethingChangedBack = true;
        }
        backwardFront = unionBdd(backwardFront, relResultBack);
      }
      backwardSet = unionBdd(backwardSet, backwardFront);
      backwardFront = leaf_false();
    } else {
      somethingChangedFront = false;
      for(int i = 0 ; i < relationDeque.size(); i++) {
      relFront = relationDeque[i];
      sylvan::Bdd relResultFront = differenceBdd(forwardSet.RelNext(relFront, cube), forwardSet);
      if(relResultFront != leaf_false()) {
        somethingChangedFront = true;
      }
      forwardFront = unionBdd(forwardFront, relResultFront);
    }
      forwardSet = unionBdd(forwardSet, forwardFront);
      forwardFront = leaf_false();
    }
  }

  //Create SCC
  sylvan::Bdd scc = intersectBdd(forwardSet, backwardSet);
  std::list<sylvan::Bdd> sccList = {scc};

  //Recursive calls
  sylvan::Bdd recBdd1 = differenceBdd(converged, scc);
  std::deque<sylvan::Bdd> recRelations1 = updateRelations(relationDeque, recBdd1);
  Graph recursiveGraph1 = {recBdd1, cube, recRelations1};

  std::list<sylvan::Bdd> recursiveResult1 = lockstepSaturation(recursiveGraph1);
  sccList.splice(sccList.end(), recursiveResult1);

  sylvan::Bdd recBdd2 = differenceBdd(nodeSet, converged);
  std::deque<sylvan::Bdd> recRelations2 = updateRelations(relationDeque, recBdd2);
  Graph recursiveGraph2 = {recBdd2, cube, recRelations2};

  std::list<sylvan::Bdd> recursiveResult2 = lockstepSaturation(recursiveGraph2);
  sccList.splice(sccList.end(), recursiveResult2);

  //Return SCC list
  return sccList;
}