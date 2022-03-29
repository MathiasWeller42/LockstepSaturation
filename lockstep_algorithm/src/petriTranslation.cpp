#include <iostream>
#include <list>
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

using std::cout; using std::cin;
using std::endl; using std::string;


void PNMLtoStringLists() {
  std::string myText;

  // Read from the text file
  std::ifstream MyReadFile("/mnt/c/Users/ablum/bachelorprojekt/PNMLFiles/HealthRecord/PT/hrec_01.pnml");

  /*char tmp[256];
  getcwd(tmp, 256);
  cout << "Current working directory: " << tmp << endl;*/

  std::list<std::string> placeList = {};
  std::list<std::string> placeListDecimal = {};
  std::list<std::string> placeListBinary = {};
  int noPlaces = 0;
  if(MyReadFile.is_open()) {
    std::cout << "Opened successfully" << std::endl;
  } else {
    std::cout << "Bad file" << std::endl;
  }

  // Use a while loop together with the getline() function to read the file line by line
  while (std::getline (MyReadFile, myText)) {
    if(myText.rfind("<place ", 0) == 0) {
      noPlaces++;
      placeList.push_back(myText);
      std::string number = "";
      for (int i = 0; i < myText.length(); i++) {
        char currentChar = myText[i];
        if(currentChar == '>') {
          break;
        }
        if(std::isdigit(currentChar)) {
          number += currentChar;
        }
      }
      placeListDecimal.push_back(number);
      int numberAsInt = std::stoi(number);
      std::cout << "Number: " << numberAsInt << std::endl;
      std::string numberInBinary = std::bitset<8>(numberAsInt).to_string();
      std::cout << ".. As Binary: " << numberInBinary << std::endl;
      placeListBinary.push_back(numberInBinary);
    }
    if(myText.rfind("<transition ", 0) == 0) {
         //noTransitions++;
         //transitionList.push_back(myText);
    }
  }

  std::cout << std::endl;
  std::cout << "Number of places: " << noPlaces << std::endl;
  for (std::string v : placeListDecimal) {
    std::cout << v << "\n";
  }
  // Close the file
  MyReadFile.close();
}

sylvan::Bdd makePlace(std::string &bitstring) {
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

sylvan::Bdd makePlaces(std::list<std::string> &bitstrings) {
  int n = bitstrings.size();
  std::string currentBitstring;
  sylvan::Bdd currentBdd;
  sylvan::Bdd resultBdd = leaf_false();
  for(std::string currentBitstring : bitstrings) {
    currentBdd = makePlace(currentBitstring);
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

//Main function for making graphs
Graph makeGraph(const int nodes, std::list<std::list<std::pair<int,int>>> relations) {
  Graph graph {};

  //Byte translation
  int nodeBytes = ceil(log2(nodes));

  std::list<std::string> placeList = {};
  std::string iInBinary;

  //Make the nodeset
  for(int i = 0; i < nodes; i++) {
    iInBinary = decimalToBinaryString(i, nodeBytes);
    placeList.push_back(iInBinary);
  }
  sylvan::Bdd places = makePlaces(placeList);

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

////////////////////////////////////////////////////////////////////////////////
/// Pretty printing
////////////////////////////////////////////////////////////////////////////////
void __printBdd(std::string prefix, sylvan::Bdd bdd, bool isLeft) {
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

std::list<std::string> __printBddAsString(std::string currentPath, sylvan::Bdd bdd) {
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

/*
std::list<std::list<std::pair<std::int, bool>>> bddAsStringList(std::list<std::pair<std::int, bool>> currentPath, sylvan::Bdd bdd) {
  std::list<std::list<std::pair<std::int, bool>>> nodeList = {};
   if(bdd.isTerminal()){
    if(bdd.isOne()) {
      nodeList.push_front(currentPath);
    }
    return nodeList;
   }
  
  std::list<std::string> recResult1 = bddAsStringList(std::to_string(bdd.TopVar()) + "1" + currentPath, bdd.Then());
  nodeList.splice(nodeList.end(), recResult1);
  std::list<std::string> recResult2 = bddAsStringList(std::to_string(bdd.TopVar()) + "0" + currentPath, bdd.Else());
  nodeList.splice(nodeList.end(), recResult2);

  return nodeList;
}

sylvan::Bdd shiftBdd(sylvan::Bdd bdd) {
  sylvan::Bdd shiftedBdd = leaf_false();
  std::list<std::string> nodeList = bddAsStringList(bdd);
  for(std::string nodeString : nodeList) {
    sylvan::Bdd currentNode = leaf_true();
    for(int i = 0; i < nodeString.length() / 2 ; i++) {
      std::string currentVar = nodeString[i];
      std::string currentBool = nodeString[i + 1];

    }
  }
  return shiftedBdd;
}
*/

void printBddAsString(sylvan::Bdd bdd) {
    std::list<std::string> result = __printBddAsString("", bdd);
    std::cout << "Nodes: ";
    for(std::string node : result) {
      std::cout << node << ", ";
    }
    std::cout << std::endl << std::endl;
}

void printBddAsString2(int nodes, sylvan::Bdd bdd) {
    std::list<std::string> result = __printBddAsString("", bdd);
    std::cout << "Nodes new new new new: ";

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
      std::cout << " Node: ";
      for(std::string str : arr ) {
        std::cout << str << "";
      }
    }
    
    std::cout << std::endl << std::endl;
}


void printBdd(sylvan::Bdd bdd) {
  __printBdd(" ", bdd, false);
}

////////////////////////////////////////////////////////////////////////////////
/// Examples
////////////////////////////////////////////////////////////////////////////////
void makeGraph() {
  std::string aString = "00";
  sylvan::Bdd nodeA = makePlace(aString);
  std::string bString = "01";
  sylvan::Bdd nodeB = makePlace(bString);

  std::list<std::string> nodeStrings = {aString,bString};
  sylvan::Bdd nodeSet = makePlaces(nodeStrings);

  std::cout << "Printing nodeSet:" << std::endl;
  printBdd(nodeSet);
  std::cout << std::endl;

  std::list<std::pair<std::string, std::string>> arcStrings =
    {{aString, bString}};
  sylvan::Bdd edgeSet = makeRelation(arcStrings);

  std::cout << "Printing edgeSet:" << std::endl;
  printBdd(edgeSet);
  std::cout << std::endl;

  sylvan::BddSet cube = sylvan::BddSet();
  cube.add(0);
  cube.add(2);

  sylvan::Bdd relnextA = nodeA.RelNext(edgeSet, cube);
  std::cout << "Printing relNextA:" << std::endl;
  printBdd(relnextA);
  std::cout << std::endl;

  sylvan::Bdd relprevA = nodeA.RelPrev(edgeSet, cube);
  std::cout << "Printing relPrevA:" << std::endl;
  printBdd(relprevA);
  std::cout << std::endl;

  sylvan::Bdd relnextB = nodeB.RelNext(edgeSet, cube);
  std::cout << "Printing relNextB:" << std::endl;
  printBdd(relnextB);
  std::cout << std::endl;

  sylvan::Bdd relprevB = nodeB.RelPrev(edgeSet, cube);
  std::cout << "Printing relPrevB:" << std::endl;
  printBdd(relprevB);
  std::cout << std::endl;
}

//Vote Blume
void makeGraphGreatAgain() {
  std::string aString = "00";
  sylvan::Bdd nodeA = makePlace(aString);
  std::string bString = "01";
  sylvan::Bdd nodeB = makePlace(bString);
  std::string cString = "10";
  sylvan::Bdd nodeC = makePlace(cString);
  std::string dString = "11";
  sylvan::Bdd nodeD = makePlace(dString);

  std::list<std::string> nodeStrings = {aString,bString,cString,dString};
  sylvan::Bdd nodeSet = makePlaces(nodeStrings);

  std::cout << "Printing nodeSet:" << std::endl;
  printBdd(nodeSet);
  std::cout << std::endl;

  std::list<std::pair<std::string, std::string>> arcStrings =
    {{aString, bString}, {aString, cString}, {bString, dString}};
  sylvan::Bdd edgeSet = makeRelation(arcStrings);

  std::cout << "Printing edgeSet:" << std::endl;
  printBdd(edgeSet);
  std::cout << std::endl;

  sylvan::BddSet cube = sylvan::BddSet();
  cube.add(0);
  cube.add(2);

  sylvan::Bdd relnextA = nodeA.RelNext(edgeSet, cube);
  std::cout << "Printing relNextA:" << std::endl;
  printBdd(relnextA);
  std::cout << std::endl;

  sylvan::Bdd relprevA = nodeA.RelPrev(edgeSet, cube);
  std::cout << "Printing relPrevA:" << std::endl;
  printBdd(relprevA);
  std::cout << std::endl;

  sylvan::Bdd relnextB = nodeB.RelNext(edgeSet, cube);
  std::cout << "Printing relNextB:" << std::endl;
  printBdd(relnextB);
  std::cout << std::endl;

  sylvan::Bdd relprevB = nodeB.RelPrev(edgeSet, cube);
  std::cout << "Printing relPrevB:" << std::endl;
  printBdd(relprevB);
  std::cout << std::endl;

  sylvan::Bdd relnextC = nodeC.RelNext(edgeSet, cube);
  std::cout << "Printing relNextC:" << std::endl;
  printBdd(relnextC);
  std::cout << std::endl;

  sylvan::Bdd relprevC = nodeC.RelPrev(edgeSet, cube);
  std::cout << "Printing relPrevC:" << std::endl;
  printBdd(relprevC);
  std::cout << std::endl;

  sylvan::Bdd relnextD = nodeD.RelNext(edgeSet, cube);
  std::cout << "Printing relNextD:" << std::endl;
  printBdd(relnextD);
  std::cout << std::endl;

  sylvan::Bdd relprevD = nodeD.RelPrev(edgeSet, cube);
  std::cout << "Printing relPrevD:" << std::endl;
  printBdd(relprevD);
  std::cout << std::endl;
}