#include <iostream>
#include <list>
#include <deque>
#include <map>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>
#include <math.h>

#include "print.h"

////////////////////////////////////////////////////////////////////////////////
//Pretty printing
////////////////////////////////////////////////////////////////////////////////

/*Main function for printing the binary BDD tree as a tree-structure. 
 Example of the structure:
└──x8
   ├──x5
   │   ├──x2
   │   └──x6
   └──x10
       ├──x9
       └──x11
*/
void printBdd(const sylvan::Bdd &bdd) {
  __printBdd(" ", bdd, false);
}

//HELPER workhorse function for printing the bdd
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

//Prints the true-paths of the BDD on the form "01x1", where x means either 0 or 1, and the total number of nodes the BDD represents
void printBddAsString(const int nodes, const sylvan::Bdd &bdd) {
    std::list<std::string> result = __printBddAsString("", bdd);
    std::cout << "Nodes: ";
    int totalNoOfNodes = 0;
    std::list<std::string> newNodes;
    for(std::string node : result) {
      int noOfNodes = 1;
      std::string arr [nodes];
      for (int i = 0; i < nodes*2; i = i+2) {
        std::string theString = std::to_string(i);
        std::string searchString = "x" + theString + "=";
        int exists = node.find(searchString);
        if(exists == -1){
          arr[nodes-1 - i/2] = "x";
          noOfNodes = noOfNodes * 2;
        } else {
          int len = searchString.length();
          arr[nodes-1 - i/2] = node.substr(exists+len, 1);
        }
      }
      totalNoOfNodes = totalNoOfNodes + noOfNodes;
      std::cout << "  ";
      for(std::string str : arr ) {
        std::cout << str << "";
      }
    }
    std::cout << std::endl << "Number of nodes: " << std::to_string(totalNoOfNodes);
    std::cout << std::endl << std::endl;
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

//This puts the output string in a single output which then can be printed
void printBddAsSingleString(const int nodes, const sylvan::Bdd &bdd) {
  std::list<std::string> result = __printBddAsString("", bdd);
  int totalNoOfNodes = 0;
  std::string output = "";
  std::list<std::string> newNodes;
  for(std::string node : result) {
    int noOfNodes = 1;
    std::string arr [nodes];
    for (int i = 0; i < nodes*2; i = i+2) {
      std::string theString = std::to_string(i);
      std::string searchString = "x" + theString + "=";
      int exists = node.find(searchString);
      if(exists == -1){
        arr[nodes-1 - i/2] = "x";
        noOfNodes = noOfNodes * 2;
      } else {
        int len = searchString.length();
        arr[nodes-1 - i/2] = node.substr(exists+len, 1);
      }
    }
    totalNoOfNodes = totalNoOfNodes + noOfNodes;
    for(std::string str : arr ) {
      output = output + str + "";
    }
    output = output + "   ";
  }
  std::cout << "Number of nodes: " << std::to_string(totalNoOfNodes);
  std::cout << std::endl << std::endl;
}

//Print all the BDDs in a list
void printBddListAsString(const int nodes, const std::list<sylvan::Bdd> bdds) {
  for(sylvan::Bdd bdd: bdds) {
    printBddAsString(nodes, bdd);
  }
}



//Print a single relation as a string
void printSingleRelationAsString(const sylvan::Bdd relation) {
    std::cout << "Here is a relation: " << std::endl;
    std::pair<std::string, std::string> pair("","");
    std::list<std::pair<std::string, std::string>> pathList = __printRelationsAsString(pair, relation);
    for(std::pair<std::string, std::string> path : pathList) {
      std::cout <<"Before "<< path.first << std::endl;
      std::cout <<"After  "<< path.second << std::endl;
    }
}

//Print a deque of relations as a string
void printRelationsAsString(const std::deque<sylvan::Bdd> relations) {
  for(sylvan::Bdd relation : relations) {
    printSingleRelationAsString(relation);
  }
}

//HELPER work horse function for printing the nodes of the bdd via the true paths
std::list<std::pair<std::string, std::string>> __printRelationsAsString(std::pair<std::string, std::string> currentPath, const sylvan::Bdd &bdd) {
   std::list<std::pair<std::string, std::string>> nodeList = {};
   if(bdd.isTerminal()) {
    if(bdd.isOne()) {
      nodeList.push_front(currentPath);
    }
    return nodeList;
   }

  if(bdd.TopVar() % 2 == 0) {
    std::string firstBefore = currentPath.first;
    currentPath.first = "x" + std::to_string(bdd.TopVar()) +"=1 " + firstBefore;
    std::list<std::pair<std::string, std::string>> recResult1 =  __printRelationsAsString(currentPath, bdd.Then());
    nodeList.splice(nodeList.end(), recResult1);

    currentPath.first = "x" + std::to_string(bdd.TopVar()) +"=0 " + firstBefore;
    std::list<std::pair<std::string, std::string>> recResult2 =  __printRelationsAsString(currentPath, bdd.Else());
    nodeList.splice(nodeList.end(), recResult2);

  } else if(bdd.TopVar() % 2 == 1) {
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



//Prints a map with key,value pairs each on a new line
void printMap(std::map<std::string, int> map) {
  for(std::pair<std::string, int> place : map) {
    std::cout << "key: " << place.first << ", value: " << place.second << std::endl;
  }
}



////////////////////////////////////////////////////////////////////////////////
//Node counting
////////////////////////////////////////////////////////////////////////////////


//Returns the number of nodes in a BDD with even numbered vars from 0 to 2*numVars
long long countNodes(int numVars, const sylvan::Bdd &bdd) {
  if(bdd.isTerminal()){
    if(bdd.isOne()) {
      return (long long) pow(2, numVars);
    } else {
      return 0;
    }
  }
  return __countNodes(-1, 2 * numVars - 2, bdd);
}

//Helper function for countNodes
long long __countNodes(long long prevTop, int maxVar, const sylvan::Bdd &bdd) {
  if(bdd.isTerminal()){
    if(bdd.isOne()) {
      return (long long) pow(2, (maxVar - prevTop) / 2);
    } else {
      return 0;
    }
  }

  long long currentTop = bdd.TopVar();
  double factor;
  int exponent;
  if(prevTop == -1) {
    exponent = (currentTop) / 2;
    factor = pow(2, exponent);
  } else {
    exponent = (currentTop - prevTop) / 2 - 1;
    factor = pow(2, exponent);
  }

  int recResult1 =  __countNodes(currentTop, maxVar, bdd.Then());
  int recResult2 =  __countNodes(currentTop, maxVar, bdd.Else());
  int recResults = (recResult1 + recResult2);

  //Check for overflow in addition
  if(recResults < 0) {
    return -1;
  }

  if((int)log2(recResults) + 1 + exponent + 1 >= 64) {
    return -1;
  }

  int result = (long long) factor * (recResult1 + recResult2);

  //Check for overflow in multiplication
  /*if((factor != 0) && (result / factor != recResults)) {
    return -1;
  }*/


  return result;
}
