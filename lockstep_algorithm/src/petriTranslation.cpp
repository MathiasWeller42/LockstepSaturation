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


void PNMLtoStringLists() {
  std::string myText;

  // Read from the text file
  std::ifstream MyReadFile("/mnt/c/Users/ablum/bachelorprojekt/PNMLFiles/HealthRecord/PT/hrec_01.pnml");

  std::map<std::string, int> placeMap = {};
  int placeIndex = 0;
  std::map<std::string, std::list<Arc>> transitionMap = {};
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
      transitionMap[id] = {};
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
      endpos = myText.find("\"/>");
      std::string target = myText.substr(startpos + 8, endpos-(startpos+8));
      arc.target = target
      //push the arc
      arcList.push_back(arc);
    }
  }
  // Close the file
  MyReadFile.close();
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
  int nodeBytes = nodes == 1 ? 1 : ceil(log2(nodes));

  std::list<std::string> placeList = {};
  std::string iInBinary;

  //Make the nodeset
  for(int i = 0; i < nodes; i++) {
    iInBinary = decimalToBinaryString(i, nodeBytes);
    placeList.push_back(iInBinary);
  }
  sylvan::Bdd places = makeNodes(placeList);

  //Make the cube
  sylvan::BddSet cube = sylvan::BddSet();
  for(int i = 0; i < nodeBytes; i++) {
    cube.add(2 * i);
  }

  //Make the relationDeque
  std::list<std::list<std::pair<std::string,std::string>>> newRelations;
  for(std::list<std::pair<int,int>> relation : relations) {
    std::list<std::pair<std::string, std::string>> newRelation;

    for(std::pair<int,int> pair : relation) {
      std::string first = decimalToBinaryString(pair.first, nodeBytes);
      std::string second = decimalToBinaryString(pair.second, nodeBytes);
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

