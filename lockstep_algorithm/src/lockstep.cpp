#include <iostream>
#include <list>
#include <deque>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "lockstep.h"
#include "utilities.h"

sylvan::Bdd pick(sylvan::Bdd nodeSet, sylvan::BddSet cube) {
	//Find path in BDD that evaluates to true, and evaluate the decisions into new node
	return pickAssignment(nodeSet, cube);
}

/*
sylvan::Bdd shiftNodes(sylvan::Bdd nodeSet) {
  return leaf_false();
}

sylvan::Bdd updateRelation(sylvan::Bdd currentRelation, sylvan::Bdd nodeSet) {
  sylvan::Bdd newRelation = intersectBdd(intersectBdd(currentRelation, nodeSet), shiftNodes(nodeSet));
  return newRelation;
}

std::deque<sylvan::Bdd> updateRelations(std::deque<sylvan::Bdd> currentRelations, sylvan::Bdd nodeSet) {
  std::deque<sylvan::Bdd> newRelations;
  for(sylvan::Bdd currentRelation : currentRelations) {
    sylvan::Bdd newCurrentRelation = updateRelation(currentRelation, nodeSet);
    newRelations.push_back(newCurrentRelation);
  }
  return newRelations;
}*/

std::list<sylvan::Bdd> lockstepSaturation(Graph graph) {
  sylvan::Bdd nodeSet = graph.nodes;
  sylvan::BddSet cube = graph.cube;
  std::deque<sylvan::Bdd> relationDeque = graph.relations;

  //TEST
  std::cout << "Running lockstep";
  printBddAsString2(4, nodeSet);

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
  std::cout << "Found SCC" << std::endl;
  printBddAsString(scc);

  std::cout << "Printing converged" << std::endl;
  printBddAsString(converged);

  //Recursive calls
  sylvan::Bdd recBdd1 = differenceBdd(converged, scc);
  Graph recursiveGraph1 = {recBdd1, cube, relationDeque};
  //TEST
  std::cout << "Making recursive call 1" << std::endl;
  printBddAsString(recBdd1);
  std::list<sylvan::Bdd> recursiveResult1 = lockstepSaturation(recursiveGraph1);
  sccList.splice(sccList.end(), recursiveResult1);

  sylvan::Bdd recBdd2 = differenceBdd(nodeSet, converged);
  Graph recursiveGraph2 = {recBdd2, cube, relationDeque};
  //TEST
  std::cout << "Making recursive call 2" << std::endl;
  printBddAsString2(4, recBdd2);
  std::list<sylvan::Bdd> recursiveResult2 = lockstepSaturation(recursiveGraph2);
  sccList.splice(sccList.end(), recursiveResult2);

  //Return SCC list
  return sccList;
}