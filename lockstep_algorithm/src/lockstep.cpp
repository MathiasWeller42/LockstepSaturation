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

void lockstepSaturation(Graph graph) {
  sylvan::Bdd nodeSet = graph.nodes;
  sylvan::BddSet cube = graph.cube;
  std::deque<sylvan::Bdd> relationDeque = graph.relations;

  if(nodeSet == leaf_false()) {
    return;
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

  /*std::cout << "Printing forward" << std::endl;
  printBdd(forwardSet);
  std::cout << "Printing backward" << std::endl;
  printBdd(backwardSet);*/

  sylvan::Bdd scc = intersectBdd(forwardSet, backwardSet);
  std::cout << "Printing SCC" << std::endl;
  printBdd(scc);

  sylvan::Bdd recBdd1 = differenceBdd(converged, scc);
  //printBdd(recBdd1);
  Graph recursiveGraph1 = {recBdd1, graph.cube, graph.relations};
  lockstepSaturation(recursiveGraph1);

  sylvan::Bdd recBdd2 = differenceBdd(nodeSet, converged);
  //printBdd(recBdd2);
  Graph recursiveGraph2 = {recBdd2, graph.cube, graph.relations};
  lockstepSaturation(recursiveGraph2);
}

/*void lockstepSaturation(V) {
  Set F, B, SCC, Converged = Ø
	if(V == Ø) return
	Node v = pick(V)
	F = B = {v}

	Relation RelFront, RelBack = R1;		//Rk+1 means all levels are saturated
	while(RelFront < = Rk && RelBack <= Rk):
		RelResultFront = Img(F, RelFront) \ F       	//find image and preimage
		RelResultBack = PreImg(B, RelBack) \ B

		if(RelResultFront == Ø):    			//update relations
			RelFront = nextRelation(RelFront)
		else:
			RelFront = R1
		if(RelResultBack == Ø):
			RelBack = nextRelation(RelBack)
		else:
			RelBack = R1

		F = F U RelResultFront			//update sets with results
		B = B U RelResultBack

  Converged = (RelFront == Rk+1) ? F : B

	while(RelFront <= Rk || RelBack <= Rk):
		RelResultFront = Img(F, RelFront) \ F	//find image and preimage
		RelResultBack = PreImg(B, RelBack) \ B	//only one is necessary

		if(RelResultFront  B == Ø):			//update relations and sets
			RelFront = nextRelation(RelFront)
		else:
			F = F U (RelResultFront  B)
			RelFront = R1
		if(RelResultBack  F == Ø):
			RelBack = nextRelation(RelBack)
		else:
			B = B U (RelResultBack  F)
			RelBack = R1

	SCC = F  B // Move this up ^^
	report(SCC)
	LockStepSaturation(Converged \ SCC)
  LockStepSaturation(V \ Converged)
}*/
