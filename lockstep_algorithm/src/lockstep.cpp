#include <iostream>
#include <list>
#include <deque>
#include <stack>
#include <vector>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "lockstep.h"
#include "bdd_utilities.h"
#include "petriTranslation.h"
#include "print.h"

sylvan::Bdd pick(const sylvan::Bdd &nodeSet, const sylvan::BddSet &cube) {
	//Find path in BDD that evaluates to true, and evaluate the decisions into new node
	return pickAssignment(nodeSet, cube);
}

//LOCKSTEP SATURATION ##########################################################################
/*std::list<sylvan::Bdd> lockstepSaturation(const Graph &graph) {
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
  }

  //Save the set that has converged
  bool frontConverged = relFrontI == relationDeque.size();
  sylvan::Bdd converged = frontConverged ? forwardSet : backwardSet;
  sylvan::Bdd nonConverged = frontConverged ? backwardSet : forwardSet;

    //Throw away the elements from the nonConverged set that won't be part of the SCC
    nonConverged = intersectBdd(converged, nonConverged);
    if(frontConverged) {
      backwardSet = nonConverged;
    } else {
      forwardSet = nonConverged;
    }
  

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
      }
    }
  }

  //Create SCC
  sylvan::Bdd scc = frontConverged ? backwardSet : forwardSet;
  std::list<sylvan::Bdd> sccList = {scc};

  //Recursive calls
  //Call 1
  sylvan::Bdd recBdd1 = differenceBdd(converged, scc);
  Graph recursiveGraph1 = {recBdd1, fullCube, relationDeque};

  std::list<sylvan::Bdd> recursiveResult1 = lockstepSaturation(recursiveGraph1);
  sccList.splice(sccList.end(), recursiveResult1);

  //Call 2
  sylvan::Bdd recBdd2 = differenceBdd(nodeSet, converged);
  Graph recursiveGraph2 = {recBdd2, fullCube, relationDeque};

  std::list<sylvan::Bdd> recursiveResult2 = lockstepSaturation(recursiveGraph2);
  sccList.splice(sccList.end(), recursiveResult2);

  //Return SCC list
  return sccList;
}




//                                      For the visually impaired - This separates the two methods






//LOCKSTEP RELATION UNION ##########################################################################
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

  //This is the "ring" that we find the next ring from
  sylvan::Bdd forwardFront = v;
  sylvan::Bdd backwardFront = v;

  //This is the next ring including the previous ring
  sylvan::Bdd forwardAcc = leaf_false();
  sylvan::Bdd backwardAcc = leaf_false();

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

      //Finds part of the next ring with the active relation
      sylvan::Bdd relResultFront = differenceBdd(intersectBdd(forwardFront.RelNext(currentRelation, currentRelationCube), nodeSet), forwardSet);
      sylvan::Bdd relResultBack = differenceBdd(intersectBdd(backwardFront.RelPrev(currentRelation, currentRelationCube), nodeSet), backwardSet);
      if(relResultFront != leaf_false()) {
        somethingChangedFront = true;
      }
      if(relResultBack != leaf_false()) {
        somethingChangedBack = true;
      }
      //We accumulate the entire ring by adding the partial rings from all relations
      forwardAcc = unionBdd(forwardAcc, relResultFront);
      backwardAcc = unionBdd(backwardAcc, relResultBack);
    }

    //Add everything to forward and backward sets
    forwardSet = unionBdd(forwardSet, forwardAcc);
    backwardSet = unionBdd(backwardSet, backwardAcc);

    //We need to subtract the previous ring from the accumulators to only get the new rings, which are then stored in the new fronts
    forwardFront = differenceBdd(forwardAcc, forwardFront);
    forwardAcc = leaf_false();
    backwardFront = differenceBdd(backwardAcc, backwardFront);
    backwardAcc = leaf_false();
  }

  //Save the set that has converged
  bool frontConverged = !somethingChangedFront;
  sylvan::Bdd converged = frontConverged ? forwardSet : backwardSet;
  sylvan::Bdd nonConverged = frontConverged ? backwardSet : forwardSet;

  //Throw away the elements from the nonConverged set that won't be part of the SCC
  nonConverged = intersectBdd(converged, nonConverged);
  if(frontConverged) {
    backwardSet = nonConverged;
  } else {
    forwardSet = nonConverged;
  }

  //Find where the non-converged overlaps the converged set
  while(somethingChangedFront || somethingChangedBack) {
    if(frontConverged) {
      somethingChangedBack = false;
      for(int i = 0 ; i < relationDeque.size(); i++) {
        currentRelation = relationDeque[i].relationBdd;
        currentRelationCube = relationDeque[i].cube;

        //Finds part of the next ring with the active relation
        sylvan::Bdd relResultBack = differenceBdd(intersectBdd(backwardFront.RelPrev(currentRelation, currentRelationCube), nodeSet), backwardSet);
        if(relResultBack != leaf_false()) {
          somethingChangedBack = true;
        }
        backwardAcc = unionBdd(backwardAcc, relResultBack);
      }
      backwardSet = unionBdd(backwardSet, backwardAcc);

      //Find new ring and reset accumulator
      backwardFront = differenceBdd(backwardAcc, backwardFront);
      backwardAcc = leaf_false();
    } else {
      somethingChangedFront = false;
      for(int i = 0 ; i < relationDeque.size(); i++) {
        currentRelation = relationDeque[i].relationBdd;
        currentRelationCube = relationDeque[i].cube;

        //Finds part of the next ring with the active relation
        sylvan::Bdd relResultFront = differenceBdd(intersectBdd(forwardFront.RelNext(currentRelation, currentRelationCube), nodeSet), forwardSet);
        if(relResultFront != leaf_false()) {
          somethingChangedFront = true;
        }
        forwardAcc = unionBdd(forwardAcc, relResultFront);
      }
      forwardSet = unionBdd(forwardSet, forwardAcc);

      //Find new ring and reset accumulator
      forwardFront = differenceBdd(forwardAcc, forwardFront);
      forwardAcc = leaf_false();
    }
  }

  //Create SCC
  sylvan::Bdd scc = frontConverged ? backwardSet : forwardSet;
  std::list<sylvan::Bdd> sccList = {scc};

  //Recursive calls
  //Call 1
  sylvan::Bdd recBdd1 = differenceBdd(converged, scc);
  Graph recursiveGraph1 = {recBdd1, fullCube, relationDeque};

  std::list<sylvan::Bdd> recursiveResult1 = lockstepSaturation(recursiveGraph1);
  sccList.splice(sccList.end(), recursiveResult1);

  //Call 2
  sylvan::Bdd recBdd2 = differenceBdd(nodeSet, converged);
  Graph recursiveGraph2 = {recBdd2, fullCube, relationDeque};

  std::list<sylvan::Bdd> recursiveResult2 = lockstepSaturation(recursiveGraph2);
  sccList.splice(sccList.end(), recursiveResult2);

  //Return SCC list
  return sccList;
}
*/

//################################################## ITERATIVE ##############################################################



//LOCKSTEP SATURATION ITERATIVE ##########################################################################

std::list<sylvan::Bdd> lockstepSaturation(const Graph &fullGraph) {
  std::stack<sylvan::Bdd> callStack;
  callStack.push(fullGraph.nodes);

  std::list<sylvan::Bdd> sccList = {};
  if(fullGraph.nodes == leaf_false()) {
    return sccList;
  }

  //Things pulled out from while-loop
  const sylvan::BddSet fullCube = fullGraph.cube;
  const std::deque<Relation> relationDeque = fullGraph.relations;

  while(!callStack.empty()) {
    const sylvan::Bdd nodeSet = callStack.top();
    callStack.pop();

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
    }

    //Save the set that has converged and the one that didn't
    bool frontConverged = relFrontI == relationDeque.size();
    sylvan::Bdd converged = frontConverged ? forwardSet : backwardSet;
    sylvan::Bdd nonConverged = frontConverged ? backwardSet : forwardSet;

    //Throw away the elements from the nonConverged set that won't be part of the SCC
    nonConverged = intersectBdd(converged, nonConverged);
    if(frontConverged) {
      backwardSet = nonConverged;
    } else {
      forwardSet = nonConverged;
    }

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
        }
      }
    }

    //Create SCC
    sylvan::Bdd scc = frontConverged ? backwardSet : forwardSet;
    //Add scc to scclist
    sccList.push_back(scc);

    //Emulating recursive calls by pushing to the stack
    //"Call" 1
    sylvan::Bdd recBdd1 = differenceBdd(converged, scc);
    if(recBdd1 != leaf_false()) {
      callStack.push(recBdd1);
    }

    //"Call" 2
    sylvan::Bdd recBdd2 = differenceBdd(nodeSet, converged);
    if(recBdd2 != leaf_false()) {
      callStack.push(recBdd2);
    }
  }

  //Return SCC list
  return sccList;
}


//LOCKSTEP RELATION UNION ITERATIVE ##########################################################################
std::list<sylvan::Bdd> lockstepRelationUnion(const Graph &fullGraph) {
  std::stack<sylvan::Bdd> callStack;
  callStack.push(fullGraph.nodes);

  std::list<sylvan::Bdd> sccList = {};
  if(fullGraph.nodes == leaf_false()) {
    return sccList;
  }

  //Things pulled out from while-loop
  const sylvan::BddSet fullCube = fullGraph.cube;
  const std::deque<Relation> relationDeque = fullGraph.relations;

  while(!callStack.empty()) {
    const sylvan::Bdd nodeSet = callStack.top();
    callStack.pop();

    sylvan::Bdd v = pick(nodeSet, fullCube);
    sylvan::Bdd forwardSet = v;
    sylvan::Bdd backwardSet = v;

    //This is the "ring" that we find the next ring from
    sylvan::Bdd forwardFront = v;
    sylvan::Bdd backwardFront = v;

    //This is the accumulator for the next ring
    sylvan::Bdd forwardAcc = leaf_false();
    sylvan::Bdd backwardAcc = leaf_false();

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

        //Finds part of the next ring with the active relation
        sylvan::Bdd relResultFront = differenceBdd(intersectBdd(forwardFront.RelNext(currentRelation, currentRelationCube), nodeSet), forwardSet);
        sylvan::Bdd relResultBack = differenceBdd(intersectBdd(backwardFront.RelPrev(currentRelation, currentRelationCube), nodeSet), backwardSet);

        //We accumulate the entire ring by adding the partial rings from all relations
        forwardAcc = unionBdd(forwardAcc, relResultFront);
        backwardAcc = unionBdd(backwardAcc, relResultBack);
      }

      if(forwardAcc != leaf_false()) {
        somethingChangedFront = true;
      }
      if(backwardAcc != leaf_false()) {
        somethingChangedBack = true;
      }
      //Add everything to forward and backward sets
      forwardSet = unionBdd(forwardSet, forwardAcc);
      backwardSet = unionBdd(backwardSet, backwardAcc);

      //We need to subtract the previous ring from the accumulators to only get the new rings, which are then stored in the new fronts
      forwardFront = differenceBdd(forwardAcc, forwardFront);
      forwardAcc = leaf_false();
      backwardFront = differenceBdd(backwardAcc, backwardFront);
      backwardAcc = leaf_false();
    }

    //Save the set that has converged
    bool frontConverged = !somethingChangedFront;
    sylvan::Bdd converged = frontConverged ? forwardSet : backwardSet;
    sylvan::Bdd nonConverged = frontConverged ? backwardSet : forwardSet;

    //Throw away the elements from the nonConverged set that won't be part of the SCC
    nonConverged = intersectBdd(converged, nonConverged);
    if(frontConverged) {
      backwardSet = nonConverged;
    } else {
      forwardSet = nonConverged;
    }

    //Find where the non-converged overlaps the converged set
    while(somethingChangedFront || somethingChangedBack) {
      if(frontConverged) {
        somethingChangedBack = false;
        for(int i = 0 ; i < relationDeque.size(); i++) {
          currentRelation = relationDeque[i].relationBdd;
          currentRelationCube = relationDeque[i].cube;

          //Finds part of the next ring with the active relation
          sylvan::Bdd relResultBack = differenceBdd(intersectBdd(backwardFront.RelPrev(currentRelation, currentRelationCube), nodeSet), backwardSet);
          if(relResultBack != leaf_false()) {
            somethingChangedBack = true;
          }
          backwardAcc = unionBdd(backwardAcc, relResultBack);
        }
        backwardSet = unionBdd(backwardSet, backwardAcc);

        //Find new ring and reset accumulator
        backwardFront = differenceBdd(backwardAcc, backwardFront);
        backwardAcc = leaf_false();
      } else {
        somethingChangedFront = false;
        for(int i = 0 ; i < relationDeque.size(); i++) {
          currentRelation = relationDeque[i].relationBdd;
          currentRelationCube = relationDeque[i].cube;

          //Finds part of the next ring with the active relation
          sylvan::Bdd relResultFront = differenceBdd(intersectBdd(forwardFront.RelNext(currentRelation, currentRelationCube), nodeSet), forwardSet);
          if(relResultFront != leaf_false()) {
            somethingChangedFront = true;
          }
          forwardAcc = unionBdd(forwardAcc, relResultFront);
        }
        forwardSet = unionBdd(forwardSet, forwardAcc);

        //Find new ring and reset accumulator
        forwardFront = differenceBdd(forwardAcc, forwardFront);
        forwardAcc = leaf_false();
      }
    }

    //Create SCC
    sylvan::Bdd scc = frontConverged ? backwardSet : forwardSet;
    //Add scc to SCC list
    sccList.push_back(scc);

    //Emulating recursive calls by pushing to the stack
    //"Call" 1
    sylvan::Bdd recBdd1 = differenceBdd(converged, scc);
    if(recBdd1 != leaf_false()) {
      callStack.push(recBdd1);
    }

    //"Call" 2
    sylvan::Bdd recBdd2 = differenceBdd(nodeSet, converged);
    if(recBdd2 != leaf_false()) {
      callStack.push(recBdd2);
    }
  }

  //Return SCC list
  return sccList;
}

//Wrapper function for lockstepRelationUnion that makes a literal union of the relations before running the function
std::list<sylvan::Bdd> lockstepLiteralRelationUnion(const Graph &fullGraph) {
  std::deque<Relation> relations = fullGraph.relations;
  sylvan::Bdd resultBdd= leaf_false();
  std::list<uint32_t> resultCubeVars = {};

  //Making union of relations and the cube for the union
  for(Relation rel : relations) {
    //Relation union
    resultBdd = unionBdd(resultBdd, rel.relationBdd);

    //New cube vars
    std::vector<uint32_t> varVector = rel.cube.toVector();
    std::list<uint32_t> varList(varVector.begin(), varVector.end());
    resultCubeVars = list_union(resultCubeVars, varList);
  }

  std::vector<uint32_t> resultCubeVarsVector(resultCubeVars.begin(), resultCubeVars.end());
  sylvan::BddSet resultCube = sylvan::BddSet::fromVector(resultCubeVarsVector);

  //Making new Relation object to store the union, new cube and arbitrary top value
  Relation resultRelation = {};
  resultRelation.relationBdd = resultBdd;
  resultRelation.cube = resultCube;
  resultRelation.top = 1;

  //Making the new deque of size 1 with the relation union
  std::deque<Relation> relationDeque = {};
  relationDeque.push_back(resultRelation);

  Graph newFullGraph = {};
  newFullGraph.nodes = fullGraph.nodes;
  newFullGraph.relations = relationDeque;
  newFullGraph.cube = fullGraph.cube;

  return lockstepRelationUnion(newFullGraph);
}

std::list<sylvan::Bdd> xieBeerelSaturation(const Graph &fullGraph) {
  std::stack<sylvan::Bdd> callStack;
  callStack.push(fullGraph.nodes);

  std::list<sylvan::Bdd> sccList = {};
  if(fullGraph.nodes == leaf_false()) {
    return sccList;
  }

  const sylvan::BddSet fullCube = fullGraph.cube;
  const std::deque<Relation> relationDeque = fullGraph.relations;

  Graph workingGraph;
  workingGraph.cube = fullCube;
  workingGraph.relations = relationDeque;
  workingGraph.nodes = leaf_false();

  while(!callStack.empty()) {
    const sylvan::Bdd nodeSet = callStack.top();
    callStack.pop();

    sylvan::Bdd v = pick(nodeSet, fullCube);
    sylvan::Bdd forwardSet = v;
    sylvan::Bdd backwardSet = v;

    workingGraph.nodes = nodeSet;
    forwardSet = reachabilityForwardSaturation(workingGraph, forwardSet);
    workingGraph.nodes = forwardSet;
    backwardSet = reachabilityBackwardSaturation(workingGraph, backwardSet);

    //Create SCC
    sylvan::Bdd scc = intersectBdd(forwardSet, backwardSet);
    //Add scc to scclist
    sccList.push_back(scc);

    //Emulating recursive calls by pushing to the stack
    //"Call" 1
    sylvan::Bdd recBdd1 = differenceBdd(forwardSet, scc);
    if(recBdd1 != leaf_false()) {
      callStack.push(recBdd1);
    }

    //"Call" 2
    sylvan::Bdd recBdd2 = differenceBdd(nodeSet, forwardSet);
    if(recBdd2 != leaf_false()) {
      callStack.push(recBdd2);
    }
  }

  return sccList;
}

std::list<sylvan::Bdd> xieBeerelRelationUnion(const Graph &fullGraph) {
  std::stack<sylvan::Bdd> callStack;
  callStack.push(fullGraph.nodes);

  std::list<sylvan::Bdd> sccList = {};
  if(fullGraph.nodes == leaf_false()) {
    return sccList;
  }

  const sylvan::BddSet fullCube = fullGraph.cube;
  const std::deque<Relation> relationDeque = fullGraph.relations;

  Graph workingGraph;
  workingGraph.cube = fullCube;
  workingGraph.relations = relationDeque;
  workingGraph.nodes = leaf_false();

  while(!callStack.empty()) {
    const sylvan::Bdd nodeSet = callStack.top();
    callStack.pop();

    sylvan::Bdd v = pick(nodeSet, fullCube);
    sylvan::Bdd forwardSet = v;
    sylvan::Bdd backwardSet = v;

    workingGraph.nodes = nodeSet;
    forwardSet = reachabilityForwardRelationUnion(workingGraph, forwardSet);
    workingGraph.nodes = forwardSet;
    backwardSet = reachabilityBackwardRelationUnion(workingGraph, backwardSet);

    //Create SCC
    sylvan::Bdd scc = intersectBdd(forwardSet, backwardSet);
    //Add scc to scclist
    sccList.push_back(scc);

    //Emulating recursive calls by pushing to the stack
    //"Call" 1
    sylvan::Bdd recBdd1 = differenceBdd(forwardSet, scc);
    if(recBdd1 != leaf_false()) {
      callStack.push(recBdd1);
    }

    //"Call" 2
    sylvan::Bdd recBdd2 = differenceBdd(nodeSet, forwardSet);
    if(recBdd2 != leaf_false()) {
      callStack.push(recBdd2);
    }
  }

  return sccList;
}

//Computes the nodes reachable from the node(s) in the Graph given using saturation
sylvan::Bdd reachabilityForwardSaturation(const Graph &graph, sylvan::Bdd nodes) {
  sylvan::BddSet cube = graph.cube;
  std::deque<Relation> relationDeque = graph.relations;

  sylvan::Bdd forwardSet = nodes;
  sylvan::Bdd nodeSet = graph.nodes;

  int relFrontI = 0;
  sylvan::Bdd relFront = relationDeque[relFrontI].relationBdd;
  sylvan::BddSet relFrontCube = relationDeque[relFrontI].cube;

  while(relFrontI < relationDeque.size()) {
    sylvan::Bdd relResultFront = differenceBdd(intersectBdd(forwardSet.RelNext(relFront, relFrontCube), nodeSet), forwardSet);

    if(relResultFront == leaf_false()) {
      relFrontI++;
      relFront = relationDeque[relFrontI].relationBdd;
      relFrontCube = relationDeque[relFrontI].cube;
    } else {
      relFrontI = 0;
      relFront = relationDeque[relFrontI].relationBdd;
      relFrontCube = relationDeque[relFrontI].cube;
    }

	  //Add to the forward set
    forwardSet = unionBdd(forwardSet, relResultFront);
  }

  return forwardSet;
}

//Computes the nodes reachable from the node(s) in the Graph given using saturation
sylvan::Bdd reachabilityBackwardSaturation(const Graph &graph, sylvan::Bdd nodes) {
  sylvan::BddSet cube = graph.cube;
  std::deque<Relation> relationDeque = graph.relations;

  sylvan::Bdd backwardSet = nodes;
  sylvan::Bdd nodeSet = graph.nodes;

  int relBackI = 0;
  sylvan::Bdd relBack = relationDeque[relBackI].relationBdd;
  sylvan::BddSet relBackCube = relationDeque[relBackI].cube;

  while(relBackI < relationDeque.size()) {
  //Find images
    sylvan::Bdd relResultBack = differenceBdd(intersectBdd(backwardSet.RelPrev(relBack, relBackCube), nodeSet), backwardSet);

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
    backwardSet = unionBdd(backwardSet, relResultBack);
  }

  return backwardSet;
}

//Computes the nodes reachable from the node(s) in the Graph given using relation union
sylvan::Bdd reachabilityForwardRelationUnion(const Graph &graph, sylvan::Bdd nodes) {
  sylvan::BddSet cube = graph.cube;
  std::deque<Relation> relationDeque = graph.relations;

  sylvan::Bdd forwardSet = nodes;
  sylvan::Bdd nodeSet = graph.nodes;

  sylvan::Bdd forwardFront = nodes;
  sylvan::Bdd forwardAcc = leaf_false();
  sylvan::Bdd relResultFront;

  sylvan::Bdd currentRelation;
  sylvan::BddSet currentRelationCube;

  bool somethingChanged = true;
  while(somethingChanged) {
    somethingChanged = false;

    for(int i = 0 ; i < relationDeque.size(); i++) {
      currentRelation = relationDeque[i].relationBdd;
      currentRelationCube = relationDeque[i].cube;

      sylvan::Bdd relResultFront = differenceBdd(intersectBdd(forwardFront.RelNext(currentRelation, currentRelationCube), nodeSet), forwardSet);

      forwardAcc = unionBdd(forwardAcc, relResultFront);
    }

    if(forwardAcc != leaf_false()) {
      somethingChanged = true;
    }
    forwardSet = unionBdd(forwardSet, forwardAcc);

    forwardFront = differenceBdd(forwardAcc, forwardFront);
    forwardAcc = leaf_false();
  }

  return forwardSet;
}

//Computes the nodes reachable from the node(s) in the Graph given using relation union
sylvan::Bdd reachabilityBackwardRelationUnion(const Graph &graph, sylvan::Bdd nodes) {
  sylvan::BddSet cube = graph.cube;
  std::deque<Relation> relationDeque = graph.relations;

  sylvan::Bdd backwardSet = nodes;
  sylvan::Bdd nodeSet = graph.nodes;

  sylvan::Bdd backwardFront = nodes;
  sylvan::Bdd backwardAcc = leaf_false();
  sylvan::Bdd relResultFront;

  sylvan::Bdd currentRelation;
  sylvan::BddSet currentRelationCube;

  bool somethingChanged = true;
  while(somethingChanged) {
    somethingChanged = false;

    for(int i = 0 ; i < relationDeque.size(); i++) {
      currentRelation = relationDeque[i].relationBdd;
      currentRelationCube = relationDeque[i].cube;

      sylvan::Bdd relResultFront = differenceBdd(intersectBdd(backwardFront.RelPrev(currentRelation, currentRelationCube), nodeSet), backwardSet);

      backwardAcc = unionBdd(backwardAcc, relResultFront);
    }

    if(backwardAcc != leaf_false()) {
      somethingChanged = true;
    }
    backwardSet = unionBdd(backwardSet, backwardAcc);

    backwardFront = differenceBdd(backwardAcc, backwardFront);
    backwardAcc = leaf_false();
  }

  return backwardSet;
}
