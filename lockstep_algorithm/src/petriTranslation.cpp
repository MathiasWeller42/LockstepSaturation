#include <iostream>
#include <list>
#include <map>
#include <fstream>
#include <filesystem>
#include <bitset>
#include <unistd.h>
#include <math.h>
#include <string>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "utilities.h"
#include "petriTranslation.h"

std::string getPNMLFilePath(std::string file) {
  char tmp[256];
  std::string cwd = getcwd(tmp, 256);
  std::string theString(tmp);
  int chop = theString.find("lockstep");

  theString.erase(chop, std::string::npos);
  std::string newPath = theString + "PNMLFiles/" + file;
  std::cout << "Current working directory: " << newPath << std::endl;
  return newPath;
}

sylvan::Bdd makeRelationFromTransition(Transition transition, std::map<std::string, int> placeMap) {
  sylvan::Bdd resultBdd = leaf_true();

  std::cout << std::endl << std::endl;
  std::cout << "New relation from transition " << transition.id << std::endl;
  std::cout << "True";

  for(Arc arc : transition.sources) {
      if(placeMap.count(arc.source)) {
        //The source places have tokens before the transition
        int placeNum = placeMap[arc.source];
        std::cout << " and " << placeNum;
        sylvan::Bdd beforeSource = ithvar(placeNum);
        resultBdd = resultBdd.And(beforeSource);

        //After the transition, the source places have no tokens
        std::cout << " and !" << placeNum+1;
        sylvan::Bdd afterSource = nithvar(placeNum + 1);
        resultBdd = resultBdd.And(afterSource);
      } else {
        std::cout << "Place " << arc.source << " not found in PlaceMap" << std::endl;
        std::cout << arc.source << " should be source to transition " << transition.id << std::endl;
      }
  }

  for(Arc arc : transition.targets) {
    if(placeMap.count(arc.target)) {
      //The target places have no tokens before the transition
      int placeNum = placeMap[arc.target];
      /*std::cout << " and !" << placeNum;
      sylvan::Bdd beforeTarget = nithvar(placeNum);
      resultBdd = resultBdd.And(beforeTarget);*/

      //After the transition, the target places now have tokens
      std::cout << " and " << placeNum+1;
      sylvan::Bdd afterTarget = ithvar(placeNum + 1);
      resultBdd = resultBdd.And(afterTarget);
    } else {
      std::cout << "Place " << arc.target << " not found in PlaceMap" << std::endl;
      std::cout << arc.target << " should be target of transition " << transition.id << std::endl;
    }
  }
  return resultBdd;
}

Graph PNMLtoStringLists() {
  std::string myText;

  std::string path = getPNMLFilePath("SimpleLoadBal/PT/simple_lbs-2.pnml");

  //Anna computer:
  // /mnt/c/Users/ablum/bachelorprojekt/PNMLFiles/HealthRecord/PT/hrec_01.pnml

  // Read from the text file
  std::ifstream MyReadFile(path);

  std::map<std::string, int> placeMap = {};
  int placeIndex = 0;
  std::map<std::string, Transition> transitionMap = {};
  std::list<Arc> arcList = {};

  // Read the PNML file and create some data structures
  while (std::getline (MyReadFile, myText)) {
    //Places
    if(myText.rfind("<place ", 0) == 0) {
      std::string id = "";
      int startpos = myText.find("id=\"");
      int endpos = myText.find("\">");
      id = myText.substr(startpos + 4, endpos-(startpos+4));
      placeMap[id] = placeIndex;
      placeIndex = placeIndex+2;
    }
    //Transitions
    if(myText.rfind("<transition ", 0) == 0) {
      std::string id = "";
      int startpos = myText.find("id=\"");
      int endpos = myText.find("\">");
      id = myText.substr(startpos + 4, endpos-(startpos+4));
      transitionMap[id] = Transition(id);
    }
    //Arcs
    if(myText.rfind("<arc ", 0) == 0) {
      Arc arc = {};
      //find id
      int startpos = myText.find("id=\"");
      int endpos = myText.find("\" ");
      std::string id = myText.substr(startpos + 4, endpos-(startpos+4));
      arc.id = id;
      //find source
      startpos = myText.find("source=\"");
      endpos = myText.find("\" t");
      std::string source = myText.substr(startpos + 8, endpos-(startpos+8));
      arc.source = source;
      //find target
      startpos = myText.find("target=\"");
      endpos = myText.find("\">");
      std::string target = myText.substr(startpos + 8, endpos-(startpos+8));
      arc.target = target;
      //push the arc
      arcList.push_back(arc);

      /*
       startpos = myText.find("target=\"");
      endpos = myText.find("\"/>");
      std::string target = myText.substr(startpos + 8, endpos-(startpos+8));
      arc.target = target;
      */
    }
  }

  //Distribute arcs to the different transitions
  for(Arc arc : arcList) {
    if(transitionMap.count(arc.source)) {
      transitionMap[arc.source].targets.push_back(arc);
      if(! placeMap.count(arc.target)) {
        std::cout << "no place found for arc: " << arc.toString() << std::endl;
      }
    } else if(transitionMap.count(arc.target)) {
      transitionMap[arc.target].sources.push_back(arc);
      if(! placeMap.count(arc.source)) {
        std::cout << "no place found for arc: " << arc.toString() << std::endl;
      }
    } else {
      std::cout << "Found arc which is not connected to transition" << std::endl;
      std::cout << "id: " << std::endl;
      std::cout << arc.id << std::endl;
      std::cout << "source: " << std::endl;
      std::cout << arc.source << std::endl;
      std::cout << "target: " << std::endl;
      std::cout << arc.target << std::endl;
      break;
    }
  }

  std::deque<sylvan::Bdd> relations;
  for(std::pair<std::string, Transition> key_value : transitionMap) {
    //std::cout << key_value.second.toString() << std::endl;
    sylvan::Bdd relation = makeRelationFromTransition(key_value.second, placeMap);
    //printSingleRelationAsString(relation);
    //printBdd(relation);
    relations.push_back(relation);
  }

  int numPlaces = placeMap.size();
  std::cout << "numPlaces: " << std::to_string(numPlaces) << std::endl;
  sylvan::BddSet cube = makeCube(numPlaces);

  Graph pnmlGraph;
  pnmlGraph.relations = relations;
  pnmlGraph.cube = cube;
  pnmlGraph.nodes = leaf_true();
  // Close the file
  MyReadFile.close();

  //std::cout << "All states in the graph:" << std::endl;
  //printBddAsString(pnmlGraph.cube.size(), pnmlGraph.nodes);
  std::cout << "Number of relations: " << pnmlGraph.relations.size() << std::endl;
  //printRelationsAsString(pnmlGraph.relations);
  //printMap(placeMap);
  return pnmlGraph;
}

void printMap(std::map<std::string, int> map) {
  for(std::pair<std::string, int> place : map) {
    std::cout << "key: " << place.first << ", value: " << place.second << std::endl;
  }
}

sylvan::Bdd makeNode(std::string &bitstring) {
  sylvan::Bdd resultBdd = leaf_true();
  bool currentBit;
  sylvan::Bdd currentBdd;
  int currentI;
  int n = bitstring.length();
  for (int i = n - 1; i >= 0; i--) {
    currentBit = bitstring[i] == '1';
    currentI = 2*(n-1-i);
    if(currentBit) {
      currentBdd = ithvar(currentI);
    } else {
      currentBdd = nithvar(currentI);
    }
    resultBdd = resultBdd.And(currentBdd);
  }
  return resultBdd;
}

sylvan::Bdd makeNodes(std::list<std::string> &bitstrings) {
  int n = bitstrings.size();
  std::string currentBitstring;
  sylvan::Bdd currentBdd;
  sylvan::Bdd resultBdd = leaf_false();
  for(std::string currentBitstring : bitstrings) {
    currentBdd = makeNode(currentBitstring);
    resultBdd = resultBdd.Or(currentBdd);
  }
  return resultBdd;
}

sylvan::Bdd makeArc(std::string &bitstringFrom, std::string &bitstringTo) {
  int nFrom = bitstringFrom.length();
  bool currentBitFrom;
  sylvan::Bdd currentBddFrom;
  int currentIFrom;
  bool currentBitTo;
  sylvan::Bdd currentBddTo;
  int currentITo;
  sylvan::Bdd resultBdd = leaf_true();
  for (int i = nFrom - 1; i >= 0; i--) {
    currentBitFrom = bitstringFrom[i] == '1';
    currentBitTo = bitstringTo[i] == '1';
    currentIFrom = 2*(nFrom-1-i);
    currentITo = currentIFrom + 1;
    if(currentBitFrom) {
      currentBddFrom = ithvar(currentIFrom);
    } else {
      currentBddFrom = nithvar(currentIFrom);
    }
    if(currentBitTo) {
      currentBddTo = ithvar(currentITo);
    } else {
      currentBddTo = nithvar(currentITo);
    }
    resultBdd = resultBdd.And(currentBddFrom).And(currentBddTo);
  }
  return resultBdd;
}

sylvan::Bdd makeRelation(std::list<std::pair<std::string, std::string>> &bitstrings) {
  sylvan::Bdd currentBdd;
  sylvan::Bdd resultBdd = leaf_false();
  for(std::pair<std::string, std::string> currentPair : bitstrings) {
    currentBdd = makeArc(currentPair.first, currentPair.second);
    resultBdd = resultBdd.Or(currentBdd);
  }
  return resultBdd;
}

std::deque<sylvan::Bdd> makeRelations(std::list<std::list<std::pair<std::string, std::string>>> &bitstrings) {
  std::deque<sylvan::Bdd> transitions;
  sylvan::Bdd currentBdd;
  for(std::list<std::pair<std::string, std::string>> currentBitstringList : bitstrings) {
    currentBdd = makeRelation(currentBitstringList);
    transitions.push_back(currentBdd);
  }
  return transitions;
}

std::string decimalToBinaryString(int number, int bytes) {
  std::string binary = "";
  int mask = 1;
  for(int i = 0; i < bytes; i++) {
    if((mask & number) >= 1) {
      binary = "1" + binary;
    } else {
      binary = "0" + binary;
    }
    mask <<= 1;
  }
  return binary;
}

//Main function for making graphs, takes as input (noOfNodes, relations={{(0,1),(1,0),(2,0)}, {(2,3)}})
Graph makeGraph(const int nodes, const std::list<std::list<std::pair<int,int>>> &relations) {
  Graph graph {};

  //Byte translation
  int nodeBits = nodes == 1 ? 1 : ceil(log2(nodes));

  std::list<std::string> placeList = {};
  std::string iInBinary;

  //Make the nodeset
  for(int i = 0; i < nodes; i++) {
    iInBinary = decimalToBinaryString(i, nodeBits);
    placeList.push_back(iInBinary);
  }
  sylvan::Bdd places = makeNodes(placeList);

  //Make the cube
  sylvan::BddSet cube = makeCube(nodeBits);

  //Make the relationDeque
  std::list<std::list<std::pair<std::string,std::string>>> newRelations;
  for(std::list<std::pair<int,int>> relation : relations) {
    std::list<std::pair<std::string, std::string>> newRelation;

    for(std::pair<int,int> pair : relation) {
      std::string first = decimalToBinaryString(pair.first, nodeBits);
      std::string second = decimalToBinaryString(pair.second, nodeBits);
      std::pair<std::string, std::string> newPair(first,second);
      newRelation.push_back(newPair);
    }
    newRelations.push_back(newRelation);
  }
  std::deque<sylvan::Bdd> realRelations = makeRelations(newRelations);

  graph.nodes = places;
  graph.relations = realRelations;
  graph.cube = cube;
  return graph;
}

sylvan::BddSet makeCube(int nodeBits) {
  sylvan::BddSet cube = sylvan::BddSet();
  for(int i = 0; i < nodeBits; i++) {
    cube.add(2 * i);
  }
  return cube;
}

//Returns each of the true paths in the BDD as a list of pairs of the form {{x0,0},{x2,1},...,}
std::list<std::list<std::pair<int, bool>>> bddAsList(std::list<std::pair<int, bool>> &currentPath,
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

//Uses list of pairs of the form {{x0,0},{x2,1},...,{xn,0}}, makes it into {{x1,0},{x3,1},...,{xn+1,0}} and reconstructs the BDD from this
sylvan::Bdd shiftBdd(const sylvan::Bdd &bdd) {
  sylvan::Bdd shiftedBdd = leaf_false();
  std::list<std::pair<int, bool>> startPath = {};
  std::list<std::list<std::pair<int, bool>>> nodeList = bddAsList(startPath, bdd);
  for(std::list<std::pair<int, bool>> node : nodeList) {
    sylvan::Bdd currentNode = leaf_true();
    for(std::pair<int, bool> variable : node) {
      sylvan::Bdd currentBdd;
      if(variable.second) {
        currentBdd = ithvar(variable.first+1);
      } else {
        currentBdd = nithvar(variable.first+1);
      }
      currentNode = currentNode.And(currentBdd);
    }
    shiftedBdd = shiftedBdd.Or(currentNode);
  }
  return shiftedBdd;
}

////////////////////////////////////////////////////////////////////////////////
/// Pretty printing
////////////////////////////////////////////////////////////////////////////////

//HELPER workhorse function for printing the bdd diagram
void __printBdd(const std::string &prefix, const sylvan::Bdd &bdd, bool isLeft) {
  std::cout << prefix;

  std::cout << (isLeft ? "├─T─" : "└─F─" );

  if(bdd.isTerminal()){
    std::cout << bdd.isOne() << std::endl;
    return;
  } else {
    std::cout << "x" << bdd.TopVar() << std::endl;
  }
  std::string newString = isLeft ? "│   " : "    ";
  // enter the next tree level - left and right branch
  __printBdd( prefix + newString, bdd.Then(), true);
  __printBdd( prefix + newString, bdd.Else(), false);
}

//Main function for printing the binary BDD tree
void printBdd(const sylvan::Bdd &bdd) {
  __printBdd(" ", bdd, false);
}

//HELPER work horse function for printing the nodes of the bdd via the true paths
std::list<std::string> __printBddAsString(const std::string &currentPath, const sylvan::Bdd &bdd) {
   std::list<std::string> nodeList = {};
   if(bdd.isTerminal()){
    if(bdd.isOne()) {
      nodeList.push_front(currentPath);
    }
    return nodeList;
   }
  std::list<std::string> recResult1 = __printBddAsString("x" + std::to_string(bdd.TopVar()) +"=1 " + currentPath, bdd.Then());
  nodeList.splice(nodeList.end(), recResult1);

  std::list<std::string> recResult2 = __printBddAsString("x" + std::to_string(bdd.TopVar()) + "=0 " + currentPath, bdd.Else());
  nodeList.splice(nodeList.end(), recResult2);

  return nodeList;
}

//Prints the true-paths of the BDD on the form "01x1", where x means either 0 or 1.
void printBddAsString(int nodes, const sylvan::Bdd &bdd) {
    std::list<std::string> result = __printBddAsString("", bdd);
    std::cout << "Nodes: ";

    std::list<std::string> newNodes;
    for(std::string node : result) {
      std::string arr [nodes];
      for (int i = 0; i < nodes*2; i = i+2) {
        std::string searchString = "x" + std::to_string(i);
        int exists = node.find(searchString);
        if(exists == -1){
          arr[nodes-1 - i/2] = "x";
        } else {
          arr[nodes-1 - i/2] = node.substr(exists+3, 1);
        }
      }
      std::cout << "  ";
      for(std::string str : arr ) {
        std::cout << str << "";
      }
    }
    std::cout << std::endl << std::endl;
}

//HELPER work horse function for printing the nodes of the bdd via the true paths
std::list<std::pair<std::string, std::string>> __printRelationsAsString(std::pair<std::string, std::string> currentPath, const sylvan::Bdd &bdd) {
   std::list<std::pair<std::string, std::string>> nodeList = {};
   if(bdd.isTerminal()){
    if(bdd.isOne()) {
      nodeList.push_front(currentPath);
    }
    return nodeList;
   }

  if(bdd.TopVar() % 2 == 0){
    std::string firstBefore = currentPath.first;
    currentPath.first = "x" + std::to_string(bdd.TopVar()) +"=1 " + firstBefore;
    std::list<std::pair<std::string, std::string>> recResult1 =  __printRelationsAsString(currentPath, bdd.Then());
    nodeList.splice(nodeList.end(), recResult1);

    currentPath.first = "x" + std::to_string(bdd.TopVar()) +"=0 " + firstBefore;
    std::list<std::pair<std::string, std::string>> recResult2 =  __printRelationsAsString(currentPath, bdd.Else());
    nodeList.splice(nodeList.end(), recResult2);
  } else if(bdd.TopVar() % 2 == 1){

    std::string secondBefore = currentPath.second;
    currentPath.second = "x" + std::to_string(bdd.TopVar()-1) +"=1 " + secondBefore;
    std::list<std::pair<std::string, std::string>> recResult1 =  __printRelationsAsString(currentPath, bdd.Then());
    nodeList.splice(nodeList.end(), recResult1);

    currentPath.second = "x" + std::to_string(bdd.TopVar()-1) +"=0 " + secondBefore;
    std::list<std::pair<std::string, std::string>> recResult2 =  __printRelationsAsString(currentPath, bdd.Else());
    nodeList.splice(nodeList.end(), recResult2);
  }
  return nodeList;
}

void printSingleRelationAsString(sylvan::Bdd relation) {
    std::cout << "Here is a relation: " << std::endl;
    std::pair<std::string, std::string> pair("","");
    std::list<std::pair<std::string, std::string>> pathList = __printRelationsAsString(pair, relation);
    for(std::pair<std::string, std::string> path : pathList) {
      std::cout <<"Before "<< path.first << std::endl;
      std::cout <<"After  "<< path.second << std::endl;
    }
}

void printRelationsAsString(std::deque<sylvan::Bdd> relations) {
  for(sylvan::Bdd relation : relations) {
    printSingleRelationAsString(relation);
  }
}