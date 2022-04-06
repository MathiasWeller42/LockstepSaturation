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

Relation updateRelation(const Relation &currentRelation, const sylvan::Bdd &nodeSet) {
  Relation newRelation = {};
  sylvan::Bdd currentRelationBdd = currentRelation.relationBdd;
  sylvan::Bdd newRelationBdd = intersectBdd(intersectBdd(currentRelationBdd, nodeSet), shiftBdd(nodeSet));
  newRelation.relationBdd = newRelationBdd;
  //TODO: Set cube and top somehow (maybe?)
  newRelation.cube = currentRelation.cube;
  newRelation.top = currentRelation.top;
  return newRelation;
}

std::deque<Relation> updateRelations(const std::deque<Relation> &currentRelations,
                                        const sylvan::Bdd &nodeSet) {
  std::deque<Relation> newRelations;
  for(Relation currentRelation : currentRelations) {
    Relation newCurrentRelation = updateRelation(currentRelation, nodeSet);
    newRelations.push_back(newCurrentRelation);
  }
  return newRelations;
}

std::list<sylvan::Bdd> lockstepSaturation(const Graph &graph) {
  const sylvan::Bdd nodeSet = graph.nodes;
  const sylvan::BddSet fullCube = graph.cube;
  const std::deque<Relation> relationDeque = graph.relations;

  if(nodeSet == leaf_false()) {
    return {};
  }

  sylvan::Bdd v = pick(nodeSet, fullCube);
  sylvan::Bdd forwardSet = v;
	sylvan::Bdd backwardSet = v;

  int relFrontI = 0;
  sylvan::Bdd relFront = relationDeque[relFrontI].relationBdd;
  sylvan::BddSet relFrontCube = relationDeque[relFrontI].cube;
  int relBackI = 0;
  sylvan::Bdd relBack = relationDeque[relBackI].relationBdd;
  sylvan::BddSet relBackCube = relationDeque[relBackI].cube;

  //Expand both forward and backward sets until one converges
  while(relFrontI < relationDeque.size() && relBackI < relationDeque.size()) {
    //Find images
    sylvan::Bdd relResultFront = differenceBdd(forwardSet.RelNext(relFront, relFrontCube), forwardSet);
    sylvan::Bdd relResultBack = differenceBdd(backwardSet.RelPrev(relBack, relBackCube), backwardSet);

    //Update relations
    if(relResultFront == leaf_false()) {
      relFrontI++;
      relFront = relationDeque[relFrontI].relationBdd;
      relFrontCube = relationDeque[relFrontI].cube;
    } else {
      relFrontI = 0;
      relFront = relationDeque[relFrontI].relationBdd;
      relFrontCube = relationDeque[relFrontI].cube;
    }
    if(relResultBack == leaf_false()) {
      relBackI++;
      relBack = relationDeque[relBackI].relationBdd;
      relBackCube = relationDeque[relBackI].cube;
    } else {
      relBackI = 0;
      relBack = relationDeque[relBackI].relationBdd;
      relBackCube = relationDeque[relBackI].cube;
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
      sylvan::Bdd relResultBack = differenceBdd(backwardSet.RelPrev(relBack, relBackCube), backwardSet);
      if(intersectBdd(relResultBack, forwardSet) == leaf_false()) {
        relBackI++;
        relBack = relationDeque[relBackI].relationBdd;
        relBackCube = relationDeque[relBackI].cube;
      } else {
        relBackI = 0;
        relBack = relationDeque[relBackI].relationBdd;
        relBackCube = relationDeque[relBackI].cube;
        backwardSet = unionBdd(backwardSet, intersectBdd(relResultBack, forwardSet));
      }
    } else {
      sylvan::Bdd relResultFront = differenceBdd(forwardSet.RelNext(relFront, relFrontCube), forwardSet);
      if(intersectBdd(relResultFront, backwardSet) == leaf_false()) {
        relFrontI++;
        relFront = relationDeque[relFrontI].relationBdd;
        relFrontCube = relationDeque[relFrontI].cube;
      } else {
        relFrontI = 0;
        relFront = relationDeque[relFrontI].relationBdd;
        relFrontCube = relationDeque[relFrontI].cube;
        forwardSet = unionBdd(forwardSet, intersectBdd(relResultFront, backwardSet));
      }
    }
  }

  //Create SCC
  sylvan::Bdd scc = intersectBdd(forwardSet, backwardSet);
  std::list<sylvan::Bdd> sccList = {scc};

  //Recursive calls
  sylvan::Bdd recBdd1 = differenceBdd(converged, scc);
  std::deque<Relation> recRelations1 = updateRelations(relationDeque, recBdd1);
  Graph recursiveGraph1 = {recBdd1, fullCube, recRelations1};

  std::list<sylvan::Bdd> recursiveResult1 = lockstepSaturation(recursiveGraph1);
  sccList.splice(sccList.end(), recursiveResult1);

  sylvan::Bdd recBdd2 = differenceBdd(nodeSet, converged);
  std::deque<Relation> recRelations2 = updateRelations(relationDeque, recBdd2);
  Graph recursiveGraph2 = {recBdd2, fullCube, recRelations2};

  std::list<sylvan::Bdd> recursiveResult2 = lockstepSaturation(recursiveGraph2);
  sccList.splice(sccList.end(), recursiveResult2);

  //Return SCC list
  return sccList;
}

std::list<sylvan::Bdd> lockstepRelationUnion(const Graph &graph) {
  const sylvan::Bdd nodeSet = graph.nodes;
  const sylvan::BddSet fullCube = graph.cube;
  const std::deque<Relation> relationDeque = graph.relations;

  if(nodeSet == leaf_false()) {
    return {};
  }

  sylvan::Bdd v = pick(nodeSet, fullCube);
  sylvan::Bdd forwardSet = v;
	sylvan::Bdd backwardSet = v;
  sylvan::Bdd forwardFront = leaf_false();
  sylvan::Bdd backwardFront = leaf_false();

  sylvan::Bdd relResultFront;
  sylvan::Bdd relResultBack;

  sylvan::Bdd currentRelation;
  sylvan::BddSet currentRelationCube;

  //Expand both forward and backward sets until one converges
  bool somethingChangedFront = true;
  bool somethingChangedBack = true;
  while(somethingChangedFront && somethingChangedBack) {
    somethingChangedFront = false;
    somethingChangedBack = false;
    for(int i = 0 ; i < relationDeque.size(); i++) {
      currentRelation = relationDeque[i].relationBdd;
      currentRelationCube = relationDeque[i].cube;
      sylvan::Bdd relResultFront = differenceBdd(forwardSet.RelNext(currentRelation, currentRelationCube), forwardSet);
      sylvan::Bdd relResultBack = differenceBdd(backwardSet.RelPrev(currentRelation, currentRelationCube), backwardSet);
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
        currentRelation = relationDeque[i].relationBdd;
        currentRelationCube = relationDeque[i].cube;
        sylvan::Bdd relResultBack = differenceBdd(backwardSet.RelPrev(currentRelation, currentRelationCube), backwardSet);
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
      currentRelation = relationDeque[i].relationBdd;
        currentRelationCube = relationDeque[i].cube;
      sylvan::Bdd relResultFront = differenceBdd(forwardSet.RelNext(currentRelation, currentRelationCube), forwardSet);
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
  std::deque<Relation> recRelations1 = updateRelations(relationDeque, recBdd1);
  Graph recursiveGraph1 = {recBdd1, fullCube, recRelations1};

  std::list<sylvan::Bdd> recursiveResult1 = lockstepSaturation(recursiveGraph1);
  sccList.splice(sccList.end(), recursiveResult1);

  sylvan::Bdd recBdd2 = differenceBdd(nodeSet, converged);
  std::deque<Relation> recRelations2 = updateRelations(relationDeque, recBdd2);
  Graph recursiveGraph2 = {recBdd2, fullCube, recRelations2};

  std::list<sylvan::Bdd> recursiveResult2 = lockstepSaturation(recursiveGraph2);
  sccList.splice(sccList.end(), recursiveResult2);

  //Return SCC list
  return sccList;
}