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

//LOCKSTEP SATURATION ITERATIVE ##########################################################################
std::list<sylvan::Bdd> lockstepSaturation(const Graph &fullGraph) {
  std::cout << "Running lockstepSaturation" << std::endl;
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
        sylvan::Bdd relResultBack = differenceBdd(intersectBdd(backwardSet.RelPrev(relBack, relBackCube), forwardSet), backwardSet);
        if(intersectBdd(relResultBack, forwardSet) == leaf_false()) {
          relBackI++;
          relBack = relationDeque[relBackI].relationBdd;
          relBackCube = relationDeque[relBackI].cube;
        } else {
          relBackI = 0;
          relBack = relationDeque[relBackI].relationBdd;
          relBackCube = relationDeque[relBackI].cube;
          backwardSet = unionBdd(backwardSet, relResultBack);
        }
      } else {
        sylvan::Bdd relResultFront = differenceBdd(intersectBdd(forwardSet.RelNext(relFront, relFrontCube), backwardSet), forwardSet);
        if(intersectBdd(relResultFront, backwardSet) == leaf_false()) {
          relFrontI++;
          relFront = relationDeque[relFrontI].relationBdd;
          relFrontCube = relationDeque[relFrontI].cube;
        } else {
          relFrontI = 0;
          relFront = relationDeque[relFrontI].relationBdd;
          relFrontCube = relationDeque[relFrontI].cube;
          forwardSet = unionBdd(forwardSet, relResultFront);
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
  std::cout << "Running lockstepRelationUnion" << std::endl;
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

    int woop = 0;
    while(somethingChangedFront && somethingChangedBack) {
      somethingChangedFront = false;
      somethingChangedBack = false;

      for(int i = 0 ; i < relationDeque.size(); i++) {
        woop++;
        currentRelation = relationDeque[i].relationBdd;
        currentRelationCube = relationDeque[i].cube;
        //std::cout << "Initting the relation" << std::endl;

        //Finds part of the next ring with the active relation
        sylvan::Bdd relResultFront = differenceBdd(intersectBdd(forwardFront.RelNext(currentRelation, currentRelationCube), nodeSet), forwardSet);
        sylvan::Bdd relResultBack = differenceBdd(intersectBdd(backwardFront.RelPrev(currentRelation, currentRelationCube), nodeSet), backwardSet);

        //We accumulate the entire ring by adding the partial rings from all relations
        forwardAcc = unionBdd(forwardAcc, relResultFront);
        backwardAcc = unionBdd(backwardAcc, relResultBack);
      }

      //Testing
      if(forwardAcc == fullGraph.nodes) {
        //std::cout << "ForwardAcc is all nodes..." << std::endl;
      }
      if(backwardAcc == fullGraph.nodes) {
        //std::cout << "BackwardAcc is all nodes..." << std::endl;
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

    std::cout << "Used the relation " << woop << "times" << std::endl;
    std::cout << "Nodes in the forwardset after first whileLoop: " << std::to_string(countNodes(fullCube.size(), forwardSet)) << " nodes" << std::endl;
    std::cout << "Nodes in the backwardset after first whileLoop: " << std::to_string(countNodes(fullCube.size(), backwardSet)) << " nodes" << std::endl;

    //Save the set that has converged
    bool frontConverged = !somethingChangedFront;
    sylvan::Bdd converged = frontConverged ? forwardSet : backwardSet;
    sylvan::Bdd nonConverged = frontConverged ? backwardSet : forwardSet;

    //Throw away the elements from the nonConverged set that won't be part of the SCC
    nonConverged = intersectBdd(converged, nonConverged);
    if(frontConverged) {
      std::cout << "Nodes in the backwardset yoooo: " << std::to_string(countNodes(fullCube.size(), backwardSet)) << " nodes" << std::endl;
      backwardSet = nonConverged;
      std::cout << "Nodes in the backwardset yoooo2: " << std::to_string(countNodes(fullCube.size(), backwardSet)) << " nodes" << std::endl;
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
          sylvan::Bdd relResultBack = differenceBdd(intersectBdd(backwardFront.RelPrev(currentRelation, currentRelationCube), forwardSet), backwardSet);
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
          sylvan::Bdd relResultFront = differenceBdd(intersectBdd(forwardFront.RelNext(currentRelation, currentRelationCube), backwardSet), forwardSet);
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

    std::cout << "Nodes in the forwardset after second whileLoop: " << std::to_string(countNodes(fullCube.size(), forwardSet)) << " nodes" << std::endl;
    std::cout << "Nodes in the backwardset after second whileLoop: " << std::to_string(countNodes(fullCube.size(), backwardSet)) << " nodes" << std::endl;

    //Create SCC
    sylvan::Bdd scc = frontConverged ? backwardSet : forwardSet;
    //Add scc to SCC list
    sccList.push_back(scc);

    std::cout << "Found SCC with " << std::to_string(countNodes(fullCube.size(), scc)) << " nodes" << std::endl;

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

  std::cout << "Size of SCC list: " << sccList.size() << std::endl;
  //Return SCC list
  return sccList;
}

std::list<sylvan::Bdd> lockstepLiteralRelationUnion(const Graph &fullGraph) {
  std::cout << "Running lockstepLiteralRelationUnion" << std::endl;
  bool lockstep = true;
  std::list<sylvan::Bdd> result = literalRelationUnion(fullGraph, lockstep);
  std::cout << "Size of SCC list: " << result.size() << std::endl;
  return result;
}

std::list<sylvan::Bdd> xieBeerelLiteralRelationUnion(const Graph &fullGraph) {
  std::cout << "Running xieBeerelLiteralRelationUnion" << std::endl;
  bool xieBeerel = false;
  std::list<sylvan::Bdd> result = literalRelationUnion(fullGraph, xieBeerel);
  std::cout << "Size of SCC list: " << result.size() << std::endl;
  return result;
}

//Wrapper function for lockstepRelationUnion that makes a literal union of the relations before running the function
std::list<sylvan::Bdd> literalRelationUnion(const Graph &fullGraph, bool lockstep) {
  std::deque<Relation> relations = fullGraph.relations;
  sylvan::Bdd resultBdd = leaf_false();
  std::list<uint32_t> resultCubeVars = {};

  //Making union of relations and the cube for the union
  for(Relation rel : relations) {
    //Relation union
    resultBdd = unionBdd(resultBdd, rel.relationBdd);

    //std::cout << "here is rel: " << std::endl;
    //printBdd(rel.relationBdd);

    //New cube vars
    std::vector<uint32_t> varVector = rel.cube.toVector();
    std::list<uint32_t> varList(varVector.begin(), varVector.end());
    resultCubeVars = list_union(resultCubeVars, varList);
    //std::cout << "Separate cube vars: " << std::endl;
    /*for(uint32_t var : varVector) {
      std::cout << var << ", ";
    }
    std::cout << std::endl;*/
  }

  Relation rel1 = relations.front();
  //printBdd(rel1.relationBdd);
  sylvan::Bdd next1 = fullGraph.nodes.RelNext(rel1.relationBdd, rel1.cube);
  printBddAsString(fullGraph.cube.size(), next1);
  std::vector<uint32_t> varVector1 = rel1.cube.toVector();
  std::list<uint32_t> varList1(varVector1.begin(), varVector1.end());
  relations.pop_front();
  Relation rel2 = relations.front();
  //printBdd(rel2.relationBdd);
  sylvan::Bdd next2 = fullGraph.nodes.RelNext(rel2.relationBdd, rel2.cube);
  printBddAsString(fullGraph.cube.size(), next2);
  std::vector<uint32_t> varVector2 = rel2.cube.toVector();
  std::list<uint32_t> varList2(varVector2.begin(), varVector2.end());
  relations.pop_front();

  std::list<uint32_t> resultCube5 = list_union(varList1, varList2);
  sylvan::Bdd newBdd = rel1.relationBdd.Or( rel2.relationBdd);

  std::vector<uint32_t> resultCubeVarsVector12(resultCube5.begin(), resultCube5.end());
  sylvan::BddSet resultCube1 = sylvan::BddSet::fromVector(resultCubeVarsVector12);
  //printBdd(newBdd);
  sylvan::Bdd nextBoth = fullGraph.nodes.RelNext(newBdd, resultCube1);
  printBddAsString(fullGraph.cube.size(), nextBoth);

  Relation newRel = {};
  newRel.relationBdd = newBdd;
  newRel.cube = resultCube1;
  newRel.top = 1;

  relations.push_front(newRel);



  //std::cout << "here is the resulting relationBdd: " << std::endl;
  //printBdd(resultBdd);
  //std::cout << "Here is the total cube: " << std::endl;
  /*for(int variable : resultCubeVars) {
    std::cout << variable << ", ";
  }*/

  std::vector<uint32_t> resultCubeVarsVector(resultCubeVars.begin(), resultCubeVars.end());
  sylvan::BddSet resultCube = sylvan::BddSet::fromVector(resultCubeVarsVector);

  //std::cout << "Size of the real resultCube:" << resultCube.size() << std::endl;

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
  newFullGraph.relations = relations;
  newFullGraph.cube = fullGraph.cube;


  //std::cout << "NewFullGraph size:" << std::endl;
  //std::cout << std::to_string(countNodes(newFullGraph.cube.size(), newFullGraph.nodes)) << std::endl;

  if(lockstep) {
    std::list<sylvan::Bdd> result = lockstepRelationUnion(newFullGraph);
    std::cout << "Size of SCC list: " << result.size() << std::endl;
    return result;
  } else {
    std::list<sylvan::Bdd> result = xieBeerelRelationUnion(newFullGraph);
    std::cout << "Size of SCC list: " << result.size() << std::endl;
    return result;
  }
}

std::list<sylvan::Bdd> xieBeerelSaturation(const Graph &fullGraph) {
  std::cout << "Running xieBeerelSaturation" << std::endl;
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
  std::cout << "Running xieBeerelRelationUnion" << std::endl;
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
  std::cout << "Size of SCC list: " << sccList.size() << std::endl;
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

/*
//Old version
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
*/
