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

  //TEST
  /*std::cout << "Running lockstep" << std::endl;
  printBddAsString(cube.size(), nodeSet);*/

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
  }

  //Save the set that has converged
  sylvan::Bdd converged = (relFrontI == relationDeque.size()) ? forwardSet : backwardSet;

  while(relFrontI < relationDeque.size() || relBackI < relationDeque.size()) {
	  //Find images
    sylvan::Bdd relResultFront = differenceBdd(forwardSet.RelNext(relFront, cube), forwardSet);
    sylvan::Bdd relResultBack = differenceBdd(backwardSet.RelPrev(relBack, cube), backwardSet);

    //Update relations
    if(intersectBdd(relResultFront, backwardSet) == leaf_false()) {
      relFrontI++;
      relFront = relationDeque[relFrontI];
    } else {
      relFrontI = 0;
      relFront = relationDeque[relFrontI];
	  forwardSet = unionBdd(forwardSet, intersectBdd(relResultFront, backwardSet));
    }
    if(intersectBdd(relResultBack, forwardSet) == leaf_false()) {
      relBackI++;
      relBack = relationDeque[relBackI];
    } else {
      relBackI = 0;
      relBack = relationDeque[relBackI];
	  backwardSet = unionBdd(backwardSet, intersectBdd(relResultBack, forwardSet));
    }

	  //Add to the forward and backward sets
    forwardSet = unionBdd(forwardSet, relResultFront);
    backwardSet = unionBdd(backwardSet, relResultBack);
  }

  //Create SCC
  sylvan::Bdd scc = intersectBdd(forwardSet, backwardSet);
  std::list<sylvan::Bdd> sccList = {scc};

  //TEST
  /*std::cout << "Found SCC" << std::endl;
  printBddAsString(cube.size(), scc);
  std::cout << "Printing converged" << std::endl;
  printBddAsString(cube.size(), converged);*/

  //Recursive calls
  sylvan::Bdd recBdd1 = differenceBdd(converged, scc);
  std::deque<sylvan::Bdd> recRelations1 = updateRelations(relationDeque, recBdd1);
  Graph recursiveGraph1 = {recBdd1, cube, recRelations1};
  //TEST
  /*std::cout << "Making recursive call 1" << std::endl;
  printBddAsString(cube.size(), recBdd1);*/
  std::list<sylvan::Bdd> recursiveResult1 = lockstepSaturation(recursiveGraph1);
  sccList.splice(sccList.end(), recursiveResult1);

  sylvan::Bdd recBdd2 = differenceBdd(nodeSet, converged);
  std::deque<sylvan::Bdd> recRelations2 = updateRelations(relationDeque, recBdd2);
  Graph recursiveGraph2 = {recBdd2, cube, recRelations2};
  //TEST
  /*std::cout << "Making recursive call 2" << std::endl;
  printBddAsString(cube.size(), recBdd2);*/
  std::list<sylvan::Bdd> recursiveResult2 = lockstepSaturation(recursiveGraph2);
  sccList.splice(sccList.end(), recursiveResult2);

  //Return SCC list
  return sccList;
}