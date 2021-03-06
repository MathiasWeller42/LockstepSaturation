#include <iostream>
#include <list>
#include <deque>
#include <stack>
#include <vector>
#include <chrono>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "lockstep.h"
#include "bdd_utilities.h"
#include "petriTranslation.h"
#include "print.h"

//Picks a single assignment in a given nodeSet. It takes the false path in the BDD as much as possible until finding a true leaf,
// and uses this path as assignment
sylvan::Bdd pick(const sylvan::Bdd &nodeSet, const sylvan::BddSet &cube) {
	//Find path in BDD that evaluates to true, and evaluate the decisions into new node
  sylvan::Bdd picked = pickAssignment(nodeSet, cube);
	return picked;
}

//LOCKSTEP SATURATION ITERATIVE ##########################################################################
std::pair<std::list<sylvan::Bdd>, int> lockstepSaturation(const Graph &fullGraph) {
  /*auto start = std::chrono::high_resolution_clock::now();
  auto stop = std::chrono::high_resolution_clock::now();
  std::chrono::duration<long, std::milli> duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);*/

  int symbolicSteps = 0;

  std::stack<sylvan::Bdd> callStack;
  callStack.push(fullGraph.nodes);

  std::list<sylvan::Bdd> sccList = {};
  if(fullGraph.nodes == leaf_false()) {
    std::pair<std::list<sylvan::Bdd>, int> result = {sccList, symbolicSteps};
    return result;
  }

  //Things pulled out from while-loop
  const sylvan::BddSet fullCube = fullGraph.cube;
  const std::deque<Relation> relationDeque = fullGraph.relations;

  while(!callStack.empty()) {
    /*stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    if((int)duration.count() > 900000) {
      std::cout << "Took too long " << (int)duration.count() << std::endl;
      return {{},0};
    }*/

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
      symbolicSteps = symbolicSteps + 2;

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
        relBack = relationDeque[0].relationBdd;
        relBackCube = relationDeque[0].cube;
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


    while(relFrontI < relationDeque.size()) {
      sylvan::Bdd relResultFront = differenceBdd(intersectBdd(forwardSet.RelNext(relFront, relFrontCube), backwardSet), forwardSet);
      symbolicSteps++;
      if(relResultFront == leaf_false()) {
        relFrontI++;
        relFront = relationDeque[relFrontI].relationBdd;
        relFrontCube = relationDeque[relFrontI].cube;
      } else {
        relFrontI = 0;
        relFront = relationDeque[0].relationBdd;
        relFrontCube = relationDeque[0].cube;
        forwardSet = unionBdd(forwardSet, relResultFront);
      }
    }

    while(relBackI < relationDeque.size()) {
      sylvan::Bdd relResultBack = differenceBdd(intersectBdd(backwardSet.RelPrev(relBack, relBackCube), forwardSet), backwardSet);
      symbolicSteps++;
      if(relResultBack == leaf_false()) {
        relBackI++;
        relBack = relationDeque[relBackI].relationBdd;
        relBackCube = relationDeque[relBackI].cube;
      } else {
        relBackI = 0;
        relBack = relationDeque[0].relationBdd;
        relBackCube = relationDeque[0].cube;
        backwardSet = unionBdd(backwardSet, relResultBack);
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

  std::cout << "[LockstepSat] Number of symbolic steps: " << symbolicSteps << std::endl;
  //Return SCC list and number of symbolic steps
  std::pair<std::list<sylvan::Bdd>, int> result = {sccList, symbolicSteps};
  return result;
}

//LOCKSTEP RELATION UNION ITERATIVE ##########################################################################
std::pair<std::list<sylvan::Bdd>, int> lockstepRelationUnion(const Graph &fullGraph) {
  int symbolicSteps = 0;

  std::stack<sylvan::Bdd> callStack;
  callStack.push(fullGraph.nodes);

  std::list<sylvan::Bdd> sccList = {};
  if(fullGraph.nodes == leaf_false()) {
    std::pair<std::list<sylvan::Bdd>, int> result = {sccList, symbolicSteps};
    return result;
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
        symbolicSteps = symbolicSteps + 2;

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

    while(somethingChangedFront) {
      somethingChangedFront = false;
        for(int i = 0 ; i < relationDeque.size(); i++) {
          currentRelation = relationDeque[i].relationBdd;
          currentRelationCube = relationDeque[i].cube;

          //Finds part of the next ring with the active relation
          sylvan::Bdd relResultFront = differenceBdd(intersectBdd(forwardFront.RelNext(currentRelation, currentRelationCube), backwardSet), forwardSet);
          symbolicSteps++;
          forwardAcc = unionBdd(forwardAcc, relResultFront);
        }
        if(forwardAcc != leaf_false()) {
          somethingChangedFront = true;
        }
        forwardSet = unionBdd(forwardSet, forwardAcc);

        //Find new ring and reset accumulator
        forwardFront = differenceBdd(forwardAcc, forwardFront);
        forwardAcc = leaf_false();
    }

    while(somethingChangedBack) {
      somethingChangedBack = false;
        for(int i = 0 ; i < relationDeque.size(); i++) {
          currentRelation = relationDeque[i].relationBdd;
          currentRelationCube = relationDeque[i].cube;

          //Finds part of the next ring with the active relation
          sylvan::Bdd relResultBack = differenceBdd(intersectBdd(backwardFront.RelPrev(currentRelation, currentRelationCube), forwardSet), backwardSet);
          symbolicSteps++;
          backwardAcc = unionBdd(backwardAcc, relResultBack);
        }
        if(backwardAcc != leaf_false()) {
          somethingChangedBack = true;
        }
        backwardSet = unionBdd(backwardSet, backwardAcc);

        //Find new ring and reset accumulator
        backwardFront = differenceBdd(backwardAcc, backwardFront);
        backwardAcc = leaf_false();
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

  std::cout << "[LockstepRelUn] Number of symbolic steps: " << symbolicSteps << std::endl;
  //Return SCC list and number of symbolic steps
  std::pair<std::list<sylvan::Bdd>, int> result = {sccList, symbolicSteps};
  return result;
}

std::pair<std::list<sylvan::Bdd>, int> xieBeerelForwardSaturation(const Graph &fullGraph) {
  int symbolicSteps = 0;

  std::stack<sylvan::Bdd> callStack;
  callStack.push(fullGraph.nodes);

  std::list<sylvan::Bdd> sccList = {};
  if(fullGraph.nodes == leaf_false()) {
    std::pair<std::list<sylvan::Bdd>, int> result = {sccList, symbolicSteps};
    return result;
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
    std::pair<sylvan::Bdd, int> res1 = reachabilityForwardSaturation(workingGraph, forwardSet);
    forwardSet = res1.first;
    symbolicSteps = symbolicSteps + res1.second;

    workingGraph.nodes = forwardSet;

    std::pair<sylvan::Bdd, int> res2 = reachabilityBackwardSaturation(workingGraph, backwardSet);
    backwardSet = res2.first;
    symbolicSteps = symbolicSteps + res2.second;

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

  std::cout << "[xbForwardSat] Number of symbolic steps: " << symbolicSteps << std::endl;
  std::pair<std::list<sylvan::Bdd>, int> result = {sccList, symbolicSteps};
  return result;
}

std::pair<std::list<sylvan::Bdd>, int> xieBeerelSaturation(const Graph &fullGraph) {
  int symbolicSteps = 0;

  std::stack<sylvan::Bdd> callStack;
  callStack.push(fullGraph.nodes);

  std::list<sylvan::Bdd> sccList = {};
  if(fullGraph.nodes == leaf_false()) {
    std::pair<std::list<sylvan::Bdd>, int> result = {sccList, symbolicSteps};
    return result;
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

    std::pair<sylvan::Bdd, int> res2 = reachabilityBackwardSaturation(workingGraph, backwardSet);
    backwardSet = res2.first;
    symbolicSteps = symbolicSteps + res2.second;

    workingGraph.nodes = backwardSet;

    std::pair<sylvan::Bdd, int> res1 = reachabilityForwardSaturation(workingGraph, forwardSet);
    forwardSet = res1.first;
    symbolicSteps = symbolicSteps + res1.second;

    //Create SCC
    sylvan::Bdd scc = intersectBdd(forwardSet, backwardSet);
    //Add scc to scclist
    sccList.push_back(scc);

    //Emulating recursive calls by pushing to the stack
    //"Call" 1
    sylvan::Bdd recBdd1 = differenceBdd(backwardSet, scc);
    if(recBdd1 != leaf_false()) {
      callStack.push(recBdd1);
    }

    //"Call" 2
    sylvan::Bdd recBdd2 = differenceBdd(nodeSet, backwardSet);
    if(recBdd2 != leaf_false()) {
      callStack.push(recBdd2);
    }
  }

  std::cout << "[xbSat] Number of symbolic steps: " << symbolicSteps << std::endl;
  std::pair<std::list<sylvan::Bdd>, int> result = {sccList, symbolicSteps};
  return result;
}


std::pair<std::list<sylvan::Bdd>, int> xieBeerelForwardRelationUnion(const Graph &fullGraph) {
  int symbolicSteps = 0;

  std::stack<sylvan::Bdd> callStack;
  callStack.push(fullGraph.nodes);

  std::list<sylvan::Bdd> sccList = {};
  if(fullGraph.nodes == leaf_false()) {
    std::pair<std::list<sylvan::Bdd>, int> result = {sccList, symbolicSteps};
    return result;
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

    std::pair<sylvan::Bdd, int> res1 = reachabilityForwardRelationUnion(workingGraph, forwardSet);
    forwardSet = res1.first;
    symbolicSteps = symbolicSteps + res1.second;

    workingGraph.nodes = forwardSet;

    std::pair<sylvan::Bdd, int> res2 = reachabilityBackwardRelationUnion(workingGraph, backwardSet);
    backwardSet = res2.first;
    symbolicSteps = symbolicSteps + res2.second;

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

  std::cout << "[XBForwardRelUn] Number of symbolic steps: " << symbolicSteps << std::endl;
  std::pair<std::list<sylvan::Bdd>, int> result = {sccList, symbolicSteps};
  return result;
}

std::pair<std::list<sylvan::Bdd>, int> xieBeerelRelationUnion(const Graph &fullGraph) {
  int symbolicSteps = 0;

  std::stack<sylvan::Bdd> callStack;
  callStack.push(fullGraph.nodes);

  std::list<sylvan::Bdd> sccList = {};
  if(fullGraph.nodes == leaf_false()) {
    std::pair<std::list<sylvan::Bdd>, int> result = {sccList, symbolicSteps};
    return result;
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

    std::pair<sylvan::Bdd, int> res2 = reachabilityBackwardRelationUnion(workingGraph, backwardSet);
    backwardSet = res2.first;
    symbolicSteps = symbolicSteps + res2.second;

    workingGraph.nodes = backwardSet;

    std::pair<sylvan::Bdd, int> res1 = reachabilityForwardRelationUnion(workingGraph, forwardSet);
    forwardSet = res1.first;
    symbolicSteps = symbolicSteps + res1.second;

    //Create SCC
    sylvan::Bdd scc = intersectBdd(forwardSet, backwardSet);
    //Add scc to scclist
    sccList.push_back(scc);

    //Emulating recursive calls by pushing to the stack
    //"Call" 1
    sylvan::Bdd recBdd1 = differenceBdd(backwardSet, scc);
    if(recBdd1 != leaf_false()) {
      callStack.push(recBdd1);
    }

    //"Call" 2
    sylvan::Bdd recBdd2 = differenceBdd(nodeSet, backwardSet);
    if(recBdd2 != leaf_false()) {
      callStack.push(recBdd2);
    }
  }

  std::cout << "[XBRelUn] Number of symbolic steps: " << symbolicSteps << std::endl;
  std::pair<std::list<sylvan::Bdd>, int> result = {sccList, symbolicSteps};
  return result;
}

//Computes the nodes reachable from the node(s) in the Graph given using saturation
std::pair<sylvan::Bdd, int> reachabilityForwardSaturation(const Graph &graph, sylvan::Bdd nodes) {
  sylvan::BddSet cube = graph.cube;
  std::deque<Relation> relationDeque = graph.relations;

  sylvan::Bdd forwardSet = nodes;
  sylvan::Bdd nodeSet = graph.nodes;

  int relFrontI = 0;
  sylvan::Bdd relFront = relationDeque[relFrontI].relationBdd;
  sylvan::BddSet relFrontCube = relationDeque[relFrontI].cube;

  int symbolicSteps = 0;

  /*auto start = std::chrono::high_resolution_clock::now();
  auto stop = std::chrono::high_resolution_clock::now();
  std::chrono::duration<long, std::milli> duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);*/

  while(relFrontI < relationDeque.size()) {
    /*stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    if((int)duration.count() > 300000) {
      std::cout << "Took too long reachability " << (int)duration.count() << std::endl;
      return {leaf_false(),0};
    }*/

    sylvan::Bdd relResultFront = differenceBdd(intersectBdd(forwardSet.RelNext(relFront, relFrontCube), nodeSet), forwardSet);
    symbolicSteps++;

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

  std::pair<sylvan::Bdd, int> result = {forwardSet, symbolicSteps};
  return result;
}

//Computes the nodes reachable from the node(s) in the Graph given using saturation
std::pair<sylvan::Bdd, int> reachabilityBackwardSaturation(const Graph &graph, sylvan::Bdd nodes) {
  sylvan::BddSet cube = graph.cube;
  std::deque<Relation> relationDeque = graph.relations;

  sylvan::Bdd backwardSet = nodes;
  sylvan::Bdd nodeSet = graph.nodes;

  int relBackI = 0;
  sylvan::Bdd relBack = relationDeque[relBackI].relationBdd;
  sylvan::BddSet relBackCube = relationDeque[relBackI].cube;

  int symbolicSteps = 0;

  while(relBackI < relationDeque.size()) {
  //Find images
    sylvan::Bdd relResultBack = differenceBdd(intersectBdd(backwardSet.RelPrev(relBack, relBackCube), nodeSet), backwardSet);
    symbolicSteps++;

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

  std::pair<sylvan::Bdd, int> result = {backwardSet, symbolicSteps};
  return result;
}

//Computes the nodes reachable from the node(s) in the Graph given using relation union
std::pair<sylvan::Bdd, int> reachabilityForwardRelationUnion(const Graph &graph, sylvan::Bdd nodes) {
  sylvan::BddSet cube = graph.cube;
  std::deque<Relation> relationDeque = graph.relations;

  sylvan::Bdd forwardSet = nodes;
  sylvan::Bdd nodeSet = graph.nodes;

  sylvan::Bdd forwardFront = nodes;
  sylvan::Bdd forwardAcc = leaf_false();
  sylvan::Bdd relResultFront;

  sylvan::Bdd currentRelation;
  sylvan::BddSet currentRelationCube;

  int symbolicSteps = 0;

  bool somethingChanged = true;
  while(somethingChanged) {
    somethingChanged = false;

    for(int i = 0 ; i < relationDeque.size(); i++) {
      currentRelation = relationDeque[i].relationBdd;
      currentRelationCube = relationDeque[i].cube;

      sylvan::Bdd relResultFront = differenceBdd(intersectBdd(forwardFront.RelNext(currentRelation, currentRelationCube), nodeSet), forwardSet);
      symbolicSteps++;
      forwardAcc = unionBdd(forwardAcc, relResultFront);
    }

    if(forwardAcc != leaf_false()) {
      somethingChanged = true;
    }
    forwardSet = unionBdd(forwardSet, forwardAcc);

    forwardFront = differenceBdd(forwardAcc, forwardFront);
    forwardAcc = leaf_false();
  }
  std::pair<sylvan::Bdd, int> result = {forwardSet, symbolicSteps};
  return result;
}

//Computes the nodes reachable from the node(s) in the Graph given using relation union
std::pair<sylvan::Bdd, int> reachabilityBackwardRelationUnion(const Graph &graph, sylvan::Bdd nodes) {
  sylvan::BddSet cube = graph.cube;
  std::deque<Relation> relationDeque = graph.relations;

  sylvan::Bdd backwardSet = nodes;
  sylvan::Bdd nodeSet = graph.nodes;

  sylvan::Bdd backwardFront = nodes;
  sylvan::Bdd backwardAcc = leaf_false();
  sylvan::Bdd relResultFront;

  sylvan::Bdd currentRelation;
  sylvan::BddSet currentRelationCube;

  int symbolicSteps = 0;

  bool somethingChanged = true;
  while(somethingChanged) {
    somethingChanged = false;

    for(int i = 0 ; i < relationDeque.size(); i++) {
      currentRelation = relationDeque[i].relationBdd;
      currentRelationCube = relationDeque[i].cube;

      sylvan::Bdd relResultFront = differenceBdd(intersectBdd(backwardFront.RelPrev(currentRelation, currentRelationCube), nodeSet), backwardSet);
      symbolicSteps++;
      backwardAcc = unionBdd(backwardAcc, relResultFront);
    }

    if(backwardAcc != leaf_false()) {
      somethingChanged = true;
    }
    backwardSet = unionBdd(backwardSet, backwardAcc);

    backwardFront = differenceBdd(backwardAcc, backwardFront);
    backwardAcc = leaf_false();
  }

  std::pair<sylvan::Bdd, int> result = {backwardSet, symbolicSteps};
  return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
// BDD Sizes
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////

//Takes a best so far BDD size and a new BDD to test against
//Returns either the size of the new BDD or the best so far size, depending on which is bigger
unsigned long long findLargestBdd(sylvan::Bdd bdd, unsigned long long peak) {
  unsigned long long newVal = bdd.NodeCount();
  if(newVal > peak) {
    return newVal;
  }
  return peak;
}

//LOCKSTEP SATURATION ITERATIVE ##########################################################################
std::pair<std::list<sylvan::Bdd>, int> lockstepSaturationBDDSize(const Graph &fullGraph) {
  /*auto start = std::chrono::high_resolution_clock::now();
  auto stop = std::chrono::high_resolution_clock::now();
  std::chrono::duration<long, std::milli> duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);*/

  unsigned long long peakBdd = 0;

  std::stack<sylvan::Bdd> callStack;
  callStack.push(fullGraph.nodes);

  std::list<sylvan::Bdd> sccList = {};
  if(fullGraph.nodes == leaf_false()) {
    std::pair<std::list<sylvan::Bdd>, int> result = {sccList, (int)peakBdd};
    return result;
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
        relBack = relationDeque[0].relationBdd;
        relBackCube = relationDeque[0].cube;
      }

      //Add to the forward and backward sets
      forwardSet = unionBdd(forwardSet, relResultFront);
      backwardSet = unionBdd(backwardSet, relResultBack);
      //peakBdd = findLargestBdd(forwardSet, peakBdd);
      peakBdd = findLargestBdd(backwardSet, peakBdd);
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

    while(relFrontI < relationDeque.size()) {
      sylvan::Bdd relResultFront = differenceBdd(intersectBdd(forwardSet.RelNext(relFront, relFrontCube), backwardSet), forwardSet);
      if(relResultFront == leaf_false()) {
        relFrontI++;
        relFront = relationDeque[relFrontI].relationBdd;
        relFrontCube = relationDeque[relFrontI].cube;
      } else {
        relFrontI = 0;
        relFront = relationDeque[0].relationBdd;
        relFrontCube = relationDeque[0].cube;
        forwardSet = unionBdd(forwardSet, relResultFront);
        //peakBdd = findLargestBdd(forwardSet, peakBdd);
      }
    }

    while(relBackI < relationDeque.size()) {
      sylvan::Bdd relResultBack = differenceBdd(intersectBdd(backwardSet.RelPrev(relBack, relBackCube), forwardSet), backwardSet);
      if(relResultBack == leaf_false()) {
        relBackI++;
        relBack = relationDeque[relBackI].relationBdd;
        relBackCube = relationDeque[relBackI].cube;
      } else {
        relBackI = 0;
        relBack = relationDeque[0].relationBdd;
        relBackCube = relationDeque[0].cube;
        backwardSet = unionBdd(backwardSet, relResultBack);
        peakBdd = findLargestBdd(backwardSet, peakBdd);
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

  std::cout << "[LockstepSat] Peak BDD size: " << peakBdd << std::endl;
  //Return SCC list and number of symbolic steps
  std::pair<std::list<sylvan::Bdd>, int> result = {sccList, (int)peakBdd};
  return result;
}

//LOCKSTEP RELATION UNION ITERATIVE ##########################################################################
std::pair<std::list<sylvan::Bdd>, int> lockstepRelationUnionBDDSize(const Graph &fullGraph) {
  unsigned long long peakBdd = 0;

  std::stack<sylvan::Bdd> callStack;
  callStack.push(fullGraph.nodes);

  std::list<sylvan::Bdd> sccList = {};
  if(fullGraph.nodes == leaf_false()) {
    std::pair<std::list<sylvan::Bdd>, int> result = {sccList, (int)peakBdd};
    return result;
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
        peakBdd = findLargestBdd(forwardFront, peakBdd);
        peakBdd = findLargestBdd(backwardFront, peakBdd);
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

    while(somethingChangedFront) {
      somethingChangedFront = false;
        for(int i = 0 ; i < relationDeque.size(); i++) {
          currentRelation = relationDeque[i].relationBdd;
          currentRelationCube = relationDeque[i].cube;

          //Finds part of the next ring with the active relation
          sylvan::Bdd relResultFront = differenceBdd(intersectBdd(forwardFront.RelNext(currentRelation, currentRelationCube), backwardSet), forwardSet);
          peakBdd = findLargestBdd(forwardFront, peakBdd);
          forwardAcc = unionBdd(forwardAcc, relResultFront);
        }
        if(forwardAcc != leaf_false()) {
          somethingChangedFront = true;
        }
        forwardSet = unionBdd(forwardSet, forwardAcc);

        //Find new ring and reset accumulator
        forwardFront = differenceBdd(forwardAcc, forwardFront);
        forwardAcc = leaf_false();
    }

    while(somethingChangedBack) {
      somethingChangedBack = false;
        for(int i = 0 ; i < relationDeque.size(); i++) {
          currentRelation = relationDeque[i].relationBdd;
          currentRelationCube = relationDeque[i].cube;

          //Finds part of the next ring with the active relation
          sylvan::Bdd relResultBack = differenceBdd(intersectBdd(backwardFront.RelPrev(currentRelation, currentRelationCube), forwardSet), backwardSet);
          peakBdd = findLargestBdd(backwardFront, peakBdd);
          backwardAcc = unionBdd(backwardAcc, relResultBack);
        }
        if(backwardAcc != leaf_false()) {
          somethingChangedBack = true;
        }
        backwardSet = unionBdd(backwardSet, backwardAcc);

        //Find new ring and reset accumulator
        backwardFront = differenceBdd(backwardAcc, backwardFront);
        backwardAcc = leaf_false();
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

  std::cout << "[LockstepRelUn] Peak BDD size: " << peakBdd << std::endl;
  //Return SCC list and number of symbolic steps
  std::pair<std::list<sylvan::Bdd>, int> result = {sccList, (int)peakBdd};
  return result;
}

//Computes the nodes reachable from the node(s) in the Graph given using saturation
std::pair<sylvan::Bdd, unsigned long long> reachabilityForwardSaturationBDDSize(const Graph &graph, sylvan::Bdd nodes) {
  sylvan::BddSet cube = graph.cube;
  std::deque<Relation> relationDeque = graph.relations;

  sylvan::Bdd forwardSet = nodes;
  sylvan::Bdd nodeSet = graph.nodes;

  int relFrontI = 0;
  sylvan::Bdd relFront = relationDeque[relFrontI].relationBdd;
  sylvan::BddSet relFrontCube = relationDeque[relFrontI].cube;

  unsigned long long peakBdd = 0;

  while(relFrontI < relationDeque.size()) {
    sylvan::Bdd relResultFront = differenceBdd(intersectBdd(forwardSet.RelNext(relFront, relFrontCube), nodeSet), forwardSet);
    peakBdd = findLargestBdd(forwardSet, peakBdd);

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

  std::pair<sylvan::Bdd, int> result = {forwardSet, peakBdd};
  return result;
}

//Computes the nodes reachable from the node(s) in the Graph given using saturation
std::pair<sylvan::Bdd, unsigned long long> reachabilityBackwardSaturationBDDSize(const Graph &graph, sylvan::Bdd nodes) {
  sylvan::BddSet cube = graph.cube;
  std::deque<Relation> relationDeque = graph.relations;

  sylvan::Bdd backwardSet = nodes;
  sylvan::Bdd nodeSet = graph.nodes;

  int relBackI = 0;
  sylvan::Bdd relBack = relationDeque[relBackI].relationBdd;
  sylvan::BddSet relBackCube = relationDeque[relBackI].cube;

  unsigned long long peakBdd = 0;

  while(relBackI < relationDeque.size()) {
  //Find images
    sylvan::Bdd relResultBack = differenceBdd(intersectBdd(backwardSet.RelPrev(relBack, relBackCube), nodeSet), backwardSet);
    peakBdd = findLargestBdd(backwardSet, peakBdd);

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

  std::pair<sylvan::Bdd, int> result = {backwardSet, peakBdd};
  return result;
}

std::pair<std::list<sylvan::Bdd>, int> xieBeerelSaturationBDDSize(const Graph &fullGraph) {
  unsigned long long peakBdd = 0;

  std::stack<sylvan::Bdd> callStack;
  callStack.push(fullGraph.nodes);

  std::list<sylvan::Bdd> sccList = {};
  if(fullGraph.nodes == leaf_false()) {
    std::pair<std::list<sylvan::Bdd>, int> result = {sccList, (int)peakBdd};
    return result;
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

    std::pair<sylvan::Bdd, int> res2 = reachabilityBackwardSaturationBDDSize(workingGraph, backwardSet);
    backwardSet = res2.first;
    if(res2.second > peakBdd){
      peakBdd = res2.second;
    }

    workingGraph.nodes = backwardSet;

    std::pair<sylvan::Bdd, int> res1 = reachabilityForwardSaturationBDDSize(workingGraph, forwardSet);
    forwardSet = res1.first;
    /*if(res1.second > peakBdd) {
      peakBdd = res1.second;
    }*/

    //Create SCC
    sylvan::Bdd scc = intersectBdd(forwardSet, backwardSet);
    //Add scc to scclist
    sccList.push_back(scc);

    //Emulating recursive calls by pushing to the stack
    //"Call" 1
    sylvan::Bdd recBdd1 = differenceBdd(backwardSet, scc);
    if(recBdd1 != leaf_false()) {
      callStack.push(recBdd1);
    }

    //"Call" 2
    sylvan::Bdd recBdd2 = differenceBdd(nodeSet, backwardSet);
    if(recBdd2 != leaf_false()) {
      callStack.push(recBdd2);
    }
  }

  std::cout << "[xbSat] Peak BDD size: " << peakBdd << std::endl;
  std::pair<std::list<sylvan::Bdd>, int> result = {sccList, (int)peakBdd};
  return result;
}



//Computes the nodes reachable from the node(s) in the Graph given using relation union
std::pair<sylvan::Bdd, unsigned long long> reachabilityForwardRelationUnionBDDSize(const Graph &graph, sylvan::Bdd nodes) {
  sylvan::BddSet cube = graph.cube;
  std::deque<Relation> relationDeque = graph.relations;

  sylvan::Bdd forwardSet = nodes;
  sylvan::Bdd nodeSet = graph.nodes;

  sylvan::Bdd forwardFront = nodes;
  sylvan::Bdd forwardAcc = leaf_false();
  sylvan::Bdd relResultFront;

  sylvan::Bdd currentRelation;
  sylvan::BddSet currentRelationCube;

  unsigned long long peakBdd = 0;


  bool somethingChanged = true;
  while(somethingChanged) {
    somethingChanged = false;

    for(int i = 0 ; i < relationDeque.size(); i++) {
      currentRelation = relationDeque[i].relationBdd;
      currentRelationCube = relationDeque[i].cube;

      sylvan::Bdd relResultFront = differenceBdd(intersectBdd(forwardFront.RelNext(currentRelation, currentRelationCube), nodeSet), forwardSet);
      peakBdd = findLargestBdd(forwardFront, peakBdd);
      forwardAcc = unionBdd(forwardAcc, relResultFront);
    }

    if(forwardAcc != leaf_false()) {
      somethingChanged = true;
    }
    forwardSet = unionBdd(forwardSet, forwardAcc);

    forwardFront = differenceBdd(forwardAcc, forwardFront);
    forwardAcc = leaf_false();
  }
  std::pair<sylvan::Bdd, int> result = {forwardSet, peakBdd};
  return result;
}

//Computes the nodes reachable from the node(s) in the Graph given using relation union
std::pair<sylvan::Bdd, unsigned long long> reachabilityBackwardRelationUnionBDDSize(const Graph &graph, sylvan::Bdd nodes) {
  sylvan::BddSet cube = graph.cube;
  std::deque<Relation> relationDeque = graph.relations;

  sylvan::Bdd backwardSet = nodes;
  sylvan::Bdd nodeSet = graph.nodes;

  sylvan::Bdd backwardFront = nodes;
  sylvan::Bdd backwardAcc = leaf_false();
  sylvan::Bdd relResultFront;

  sylvan::Bdd currentRelation;
  sylvan::BddSet currentRelationCube;

  unsigned long long peakBdd = 0;

  bool somethingChanged = true;
  while(somethingChanged) {
    somethingChanged = false;

    for(int i = 0 ; i < relationDeque.size(); i++) {
      currentRelation = relationDeque[i].relationBdd;
      currentRelationCube = relationDeque[i].cube;

      sylvan::Bdd relResultFront = differenceBdd(intersectBdd(backwardFront.RelPrev(currentRelation, currentRelationCube), nodeSet), backwardSet);
      peakBdd = findLargestBdd(backwardFront, peakBdd);
      backwardAcc = unionBdd(backwardAcc, relResultFront);
    }

    if(backwardAcc != leaf_false()) {
      somethingChanged = true;
    }
    backwardSet = unionBdd(backwardSet, backwardAcc);

    backwardFront = differenceBdd(backwardAcc, backwardFront);
    backwardAcc = leaf_false();
  }

  std::pair<sylvan::Bdd, int> result = {backwardSet, peakBdd};
  return result;
}

std::pair<std::list<sylvan::Bdd>, int> xieBeerelRelationUnionBDDSize(const Graph &fullGraph) {
  unsigned long long peakBdd = 0;

  std::stack<sylvan::Bdd> callStack;
  callStack.push(fullGraph.nodes);

  std::list<sylvan::Bdd> sccList = {};
  if(fullGraph.nodes == leaf_false()) {
    std::pair<std::list<sylvan::Bdd>, int> result = {sccList, (int)peakBdd};
    return result;
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

    std::pair<sylvan::Bdd, int> res2 = reachabilityBackwardRelationUnionBDDSize(workingGraph, backwardSet);
    backwardSet = res2.first;
    if(res2.second > peakBdd) {
      peakBdd = res2.second;
    }

    workingGraph.nodes = backwardSet;

    std::pair<sylvan::Bdd, int> res1 = reachabilityForwardRelationUnionBDDSize(workingGraph, forwardSet);
    forwardSet = res1.first;
    if(res1.second > peakBdd) {
      peakBdd = res1.second;
    }

    //Create SCC
    sylvan::Bdd scc = intersectBdd(forwardSet, backwardSet);
    //Add scc to scclist
    sccList.push_back(scc);

    //Emulating recursive calls by pushing to the stack
    //"Call" 1
    sylvan::Bdd recBdd1 = differenceBdd(backwardSet, scc);
    if(recBdd1 != leaf_false()) {
      callStack.push(recBdd1);
    }

    //"Call" 2
    sylvan::Bdd recBdd2 = differenceBdd(nodeSet, backwardSet);
    if(recBdd2 != leaf_false()) {
      callStack.push(recBdd2);
    }
  }

  std::cout << "[XBRelUn] Peak BDD size: " << peakBdd << std::endl;
  std::pair<std::list<sylvan::Bdd>, int> result = {sccList, (int)peakBdd};
  return result;
}




//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
// OPTIMIZED SATURATION
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////


//Takes a list of relations, and some relation R to find overlap with
//Finds the first relation, starting from R1, that overlaps with R, returning R if none before do
int findFirstOverlap(int relI, std::deque<Relation> relationDeque) {
  int rel = relI;
  for(int i = 0 ; i < relI ; i++) {
      if(relationDeque[relI].bottom < relationDeque[i].top) {
        continue;
      } else {
        rel = i;
        break;
      }
  }
  return rel;
}



//LOCKSTEP SATURATION ITERATIVE OPTIMIZED ##########################################################################
std::pair<std::list<sylvan::Bdd>, int> lockstepSaturationOptimized(const Graph &fullGraph) {
  /*auto start = std::chrono::high_resolution_clock::now();
  auto stop = std::chrono::high_resolution_clock::now();
  std::chrono::duration<long, std::milli> duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);*/

  int symbolicSteps = 0;

  std::stack<sylvan::Bdd> callStack;
  callStack.push(fullGraph.nodes);

  std::list<sylvan::Bdd> sccList = {};
  if(fullGraph.nodes == leaf_false()) {
    std::pair<std::list<sylvan::Bdd>, int> result = {sccList, symbolicSteps};
    return result;
  }

  //Things pulled out from while-loop
  const sylvan::BddSet fullCube = fullGraph.cube;
  const std::deque<Relation> relationDeque = fullGraph.relations;

  while(!callStack.empty()) {
    /*stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    if((int)duration.count() > 900000) {
      std::cout << "Took too long " << (int)duration.count() << std::endl;
      return {{},0};
    }*/

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
      symbolicSteps = symbolicSteps + 2;

      //Update relations
      if(relResultFront == leaf_false()) {
        relFrontI++;
        relFront = relationDeque[relFrontI].relationBdd;
        relFrontCube = relationDeque[relFrontI].cube;
      } else {
        relFrontI = findFirstOverlap(relFrontI, relationDeque);
        relFront = relationDeque[relFrontI].relationBdd;
        relFrontCube = relationDeque[relFrontI].cube;
      }
      if(relResultBack == leaf_false()) {
        relBackI++;
        relBack = relationDeque[relBackI].relationBdd;
        relBackCube = relationDeque[relBackI].cube;
      } else {
        relBackI = findFirstOverlap(relBackI, relationDeque);
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


    while(relFrontI < relationDeque.size()) {
      sylvan::Bdd relResultFront = differenceBdd(intersectBdd(forwardSet.RelNext(relFront, relFrontCube), backwardSet), forwardSet);
      symbolicSteps++;
      if(relResultFront == leaf_false()) {
        relFrontI++;
        relFront = relationDeque[relFrontI].relationBdd;
        relFrontCube = relationDeque[relFrontI].cube;
      } else {
        relFrontI = findFirstOverlap(relFrontI, relationDeque);
        relFront = relationDeque[relFrontI].relationBdd;
        relFrontCube = relationDeque[relFrontI].cube;
        forwardSet = unionBdd(forwardSet, relResultFront);
      }
    }

    while(relBackI < relationDeque.size()) {
      sylvan::Bdd relResultBack = differenceBdd(intersectBdd(backwardSet.RelPrev(relBack, relBackCube), forwardSet), backwardSet);
      symbolicSteps++;
      if(relResultBack == leaf_false()) {
        relBackI++;
        relBack = relationDeque[relBackI].relationBdd;
        relBackCube = relationDeque[relBackI].cube;
      } else {
        relBackI = findFirstOverlap(relBackI, relationDeque);
        relBack = relationDeque[relBackI].relationBdd;
        relBackCube = relationDeque[relBackI].cube;
        backwardSet = unionBdd(backwardSet, relResultBack);
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

  std::cout << "[LockstepSatOpt] Number of symbolic steps: " << symbolicSteps << std::endl;
  //Return SCC list and number of symbolic steps
  std::pair<std::list<sylvan::Bdd>, int> result = {sccList, symbolicSteps};
  return result;
}




//XIE-BEEREL SATURATION ITERATIVE OPTIMIZED ##########################################################################
std::pair<std::list<sylvan::Bdd>, int> xieBeerelSaturationOptimized(const Graph &fullGraph) {
  int symbolicSteps = 0;

  std::stack<sylvan::Bdd> callStack;
  callStack.push(fullGraph.nodes);

  std::list<sylvan::Bdd> sccList = {};
  if(fullGraph.nodes == leaf_false()) {
    std::pair<std::list<sylvan::Bdd>, int> result = {sccList, symbolicSteps};
    return result;
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

    std::pair<sylvan::Bdd, int> res2 = reachabilityBackwardSaturationOpt(workingGraph, backwardSet);
    backwardSet = res2.first;
    symbolicSteps = symbolicSteps + res2.second;

    workingGraph.nodes = backwardSet;

    std::pair<sylvan::Bdd, int> res1 = reachabilityForwardSaturationOpt(workingGraph, forwardSet);
    forwardSet = res1.first;
    symbolicSteps = symbolicSteps + res1.second;

    //Create SCC
    sylvan::Bdd scc = intersectBdd(forwardSet, backwardSet);
    //Add scc to scclist
    sccList.push_back(scc);

    //Emulating recursive calls by pushing to the stack
    //"Call" 1
    sylvan::Bdd recBdd1 = differenceBdd(backwardSet, scc);
    if(recBdd1 != leaf_false()) {
      callStack.push(recBdd1);
    }

    //"Call" 2
    sylvan::Bdd recBdd2 = differenceBdd(nodeSet, backwardSet);
    if(recBdd2 != leaf_false()) {
      callStack.push(recBdd2);
    }
  }

  std::cout << "[xbSatOpt] Number of symbolic steps: " << symbolicSteps << std::endl;
  std::pair<std::list<sylvan::Bdd>, int> result = {sccList, symbolicSteps};
  return result;
}




//Computes the nodes reachable from the node(s) in the Graph given using saturation
std::pair<sylvan::Bdd, int> reachabilityForwardSaturationOpt(const Graph &graph, sylvan::Bdd nodes) {
  sylvan::BddSet cube = graph.cube;
  std::deque<Relation> relationDeque = graph.relations;

  sylvan::Bdd forwardSet = nodes;
  sylvan::Bdd nodeSet = graph.nodes;

  int relFrontI = 0;
  sylvan::Bdd relFront = relationDeque[relFrontI].relationBdd;
  sylvan::BddSet relFrontCube = relationDeque[relFrontI].cube;

  int symbolicSteps = 0;


  while(relFrontI < relationDeque.size()) {

    sylvan::Bdd relResultFront = differenceBdd(intersectBdd(forwardSet.RelNext(relFront, relFrontCube), nodeSet), forwardSet);
    symbolicSteps++;

    if(relResultFront == leaf_false()) {
      relFrontI++;
      relFront = relationDeque[relFrontI].relationBdd;
      relFrontCube = relationDeque[relFrontI].cube;
    } else {
      relFrontI = findFirstOverlap(relFrontI, relationDeque);
      relFront = relationDeque[relFrontI].relationBdd;
      relFrontCube = relationDeque[relFrontI].cube;
    }

	  //Add to the forward set
    forwardSet = unionBdd(forwardSet, relResultFront);
  }

  std::pair<sylvan::Bdd, int> result = {forwardSet, symbolicSteps};
  return result;
}



//Computes the nodes reachable from the node(s) in the Graph given using saturation
std::pair<sylvan::Bdd, int> reachabilityBackwardSaturationOpt(const Graph &graph, sylvan::Bdd nodes) {
  sylvan::BddSet cube = graph.cube;
  std::deque<Relation> relationDeque = graph.relations;

  sylvan::Bdd backwardSet = nodes;
  sylvan::Bdd nodeSet = graph.nodes;

  int relBackI = 0;
  sylvan::Bdd relBack = relationDeque[relBackI].relationBdd;
  sylvan::BddSet relBackCube = relationDeque[relBackI].cube;

  int symbolicSteps = 0;

  while(relBackI < relationDeque.size()) {
  //Find images
    sylvan::Bdd relResultBack = differenceBdd(intersectBdd(backwardSet.RelPrev(relBack, relBackCube), nodeSet), backwardSet);
    symbolicSteps++;

    if(relResultBack == leaf_false()) {
      relBackI++;
      relBack = relationDeque[relBackI].relationBdd;
      relBackCube = relationDeque[relBackI].cube;
    } else {
      relBackI = findFirstOverlap(relBackI, relationDeque);
      relBack = relationDeque[relBackI].relationBdd;
      relBackCube = relationDeque[relBackI].cube;
    }

    //Add to the forward and backward sets
    backwardSet = unionBdd(backwardSet, relResultBack);
  }

  std::pair<sylvan::Bdd, int> result = {backwardSet, symbolicSteps};
  return result;
}