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

//Denne metode er den hyggelige helterotte hoppe-loppe hepatitis herpes Herbert Hans Handanovics yndlingsmetode
/*Relation updateRelation(const Relation &currentRelation, const sylvan::Bdd &nodeSet) {
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
}*/

std::list<sylvan::Bdd> lockstepSaturation(const Graph &graph) {
  const sylvan::Bdd nodeSet = graph.nodes;
  const sylvan::BddSet fullCube = graph.cube;
  const std::deque<Relation> relationDeque = graph.relations;

  if(nodeSet == leaf_false()) {
    return {};
  }

  sylvan::Bdd v = pick(nodeSet, fullCube);
  if(hasOddVars(v)) {
    std::cout << "Odd vars after pick" << std::endl;
    printBdd(v);
    exit(-1);
  }

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
    sylvan::Bdd relResultFront = differenceBdd(intersectBdd(forwardSet.RelNext(relFront, relFrontCube), nodeSet), forwardSet);
    sylvan::Bdd relResultBack = differenceBdd(intersectBdd(backwardSet.RelPrev(relBack, relBackCube), nodeSet), backwardSet);

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
    /*if(hasOddVars(forwardSet)) {
      std::cout << "Odd vars in forwardSet first" << std::endl;
      for(int i : getVars(forwardSet)) {
        std::cout << i << " ";
      }
      std::cout << std::endl;
      std::cout << "Caused by relation " << relFrontI << std::endl;
      printBdd(relFront);
      exit(-1);
    }
    if(hasOddVars(backwardSet)) {
      std::cout << "Odd vars in backwardSet first" << std::endl;
      for(int i : getVars(backwardSet)) {
        std::cout << i << " ";
      }
      std::cout << std::endl;
      std::cout << "Caused by relation " << relBackI << std::endl;
      printBdd(relBack);
      exit(-1);
    }*/

    //std::cout << relFrontI << " " << relBackI << std::endl;
  }

  //Save the set that has converged
  bool frontConverged = relFrontI == relationDeque.size();
  sylvan::Bdd converged = frontConverged ? forwardSet : backwardSet;

  //Find where the non-converged overlaps the converged set
  while(relFrontI < relationDeque.size() || relBackI < relationDeque.size()) {
    if(frontConverged) {
      sylvan::Bdd relResultBack = differenceBdd(intersectBdd(backwardSet.RelPrev(relBack, relBackCube), nodeSet), backwardSet);
      if(intersectBdd(relResultBack, forwardSet) == leaf_false()) {
        relBackI++;
        relBack = relationDeque[relBackI].relationBdd;
        relBackCube = relationDeque[relBackI].cube;
      } else {
        relBackI = 0;
        relBack = relationDeque[relBackI].relationBdd;
        relBackCube = relationDeque[relBackI].cube;
        backwardSet = unionBdd(backwardSet, intersectBdd(relResultBack, forwardSet));
        /*if(hasOddVars(backwardSet)) {
          std::cout << "Odd vars in backwardSet second" << std::endl;
          for(int i : getVars(backwardSet)) {
            std::cout << i << " ";
          }
          std::cout << std::endl;
          std::cout << "Caused by relation " << relBackI << std::endl;
          printBdd(relBack);
          exit(-1);
        }*/
      }
    } else {
      sylvan::Bdd relResultFront = differenceBdd(intersectBdd(forwardSet.RelNext(relFront, relFrontCube), nodeSet), forwardSet);
      if(intersectBdd(relResultFront, backwardSet) == leaf_false()) {
        relFrontI++;
        relFront = relationDeque[relFrontI].relationBdd;
        relFrontCube = relationDeque[relFrontI].cube;
      } else {
        relFrontI = 0;
        relFront = relationDeque[relFrontI].relationBdd;
        relFrontCube = relationDeque[relFrontI].cube;
        forwardSet = unionBdd(forwardSet, intersectBdd(relResultFront, backwardSet));
        /*if(hasOddVars(forwardSet)) {
          std::cout << "Odd vars in forwardSet second" << std::endl;
          for(int i : getVars(forwardSet)) {
            std::cout << i << " ";
          }
          std::cout << std::endl;
          std::cout << "Caused by relation " << relFrontI << std::endl;
          printBdd(relFront);
          exit(-1);
        }*/
      }
    }
  }

  //Create SCC
  sylvan::Bdd scc = intersectBdd(forwardSet, backwardSet);
  std::list<sylvan::Bdd> sccList = {scc};

  //std::cout << "New SCC: " << std::endl;
  //printBigBddAsString(7, scc);
  /*
  if(hasOddVars(forwardSet)) {
    std::cout << "Odd vars in forwardSet" << std::endl;
    printBdd(forwardSet);
    exit(-1);
  }
  if(hasOddVars(backwardSet)) {
    std::cout << "Odd vars in backwardSet" << std::endl;
    printBdd(backwardSet);
    exit(-1);
  }
  if(hasOddVars(scc)) {
    std::cout << "Odd vars in scc" << std::endl;
    printBdd(scc);
    exit(-1);
  }*/

  //Recursive calls
  //Call 1
  sylvan::Bdd recBdd1 = differenceBdd(converged, scc);
  Graph recursiveGraph1 = {recBdd1, fullCube, relationDeque};

  /*if(hasOddVars(converged)) {
    std::cout << "Odd vars in converged" << std::endl;
    printBdd(converged);
    exit(-1);
  }
  if(hasOddVars(nodeSet)) {
    std::cout << "Odd vars in nodeset" << std::endl;
    printBdd(nodeSet);
    exit(-1);
  }*/

  //std::cout << "Call 1: " << std::endl;
  //printBigBddAsString(32, recursiveGraph1.nodes);

  std::list<sylvan::Bdd> recursiveResult1 = lockstepSaturation(recursiveGraph1);
  sccList.splice(sccList.end(), recursiveResult1);

  //Call 2
  sylvan::Bdd recBdd2 = differenceBdd(nodeSet, converged);
  Graph recursiveGraph2 = {recBdd2, fullCube, relationDeque};

  //std::cout << "Call 2: " << std::endl;
  //printBigBddAsString(32, recursiveGraph2.nodes);

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
      sylvan::Bdd relResultFront = differenceBdd(intersectBdd(forwardSet.RelNext(currentRelation, currentRelationCube), nodeSet), forwardSet);
      sylvan::Bdd relResultBack = differenceBdd(intersectBdd(backwardSet.RelPrev(currentRelation, currentRelationCube), nodeSet), backwardSet);
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
        sylvan::Bdd relResultBack = differenceBdd(intersectBdd(backwardSet.RelPrev(currentRelation, currentRelationCube), nodeSet), backwardSet);
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
        sylvan::Bdd relResultFront = differenceBdd(intersectBdd(forwardSet.RelNext(currentRelation, currentRelationCube), nodeSet), forwardSet);
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
  //Call 1
  sylvan::Bdd recBdd1 = differenceBdd(converged, scc);
  Graph recursiveGraph1 = {recBdd1, fullCube, relationDeque};

  std::list<sylvan::Bdd> recursiveResult1 = lockstepRelationUnion(recursiveGraph1);
  sccList.splice(sccList.end(), recursiveResult1);

  //Call 2
  sylvan::Bdd recBdd2 = differenceBdd(nodeSet, converged);
  Graph recursiveGraph2 = {recBdd2, fullCube, relationDeque};

  std::list<sylvan::Bdd> recursiveResult2 = lockstepRelationUnion(recursiveGraph2);
  sccList.splice(sccList.end(), recursiveResult2);

  //Return SCC list
  return sccList;
}