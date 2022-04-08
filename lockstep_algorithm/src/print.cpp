#include <iostream>
#include <list>
#include <deque>
#include <map>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "print.h"

////////////////////////////////////////////////////////////////////////////////
/// Pretty printing
////////////////////////////////////////////////////////////////////////////////

//HELPER workhorse function for printing the bdd diagram
inline void __printBdd(const std::string &prefix, const sylvan::Bdd &bdd, bool isLeft) {
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
inline std::list<std::string> __printBddAsString(const std::string &currentPath, const sylvan::Bdd &bdd) {
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
    //Måske ikke relevant med mængden af nodes -  kan fjernes nemt.
    std::cout << std::endl << "Number of nodes: " << std::to_string(totalNoOfNodes);
    std::cout << std::endl << std::endl;
}

void printBigBddAsString(const int nodes, const sylvan::Bdd &bdd) {
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
  if(totalNoOfNodes >= 2) {
    //std::cout << "Nodes:  ";
    //std::cout << output;
    std::cout << "Number of nodes: " << std::to_string(totalNoOfNodes);
    std::cout << std::endl << std::endl;
  }
}

//HELPER work horse function for printing the nodes of the bdd via the true paths
inline std::list<std::pair<std::string, std::string>> __printRelationsAsString(std::pair<std::string, std::string> currentPath, const sylvan::Bdd &bdd) {
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

void printSingleRelationAsString(const sylvan::Bdd relation) {
    std::cout << "Here is a relation: " << std::endl;
    std::pair<std::string, std::string> pair("","");
    std::list<std::pair<std::string, std::string>> pathList = __printRelationsAsString(pair, relation);
    for(std::pair<std::string, std::string> path : pathList) {
      std::cout <<"Before "<< path.first << std::endl;
      std::cout <<"After  "<< path.second << std::endl;
    }
}

inline void printRelationsAsString(const std::deque<sylvan::Bdd> relations) {
  for(sylvan::Bdd relation : relations) {
    printSingleRelationAsString(relation);
  }
}

void printMap(std::map<std::string, int> map) {
  for(std::pair<std::string, int> place : map) {
    std::cout << "key: " << place.first << ", value: " << place.second << std::endl;
  }
}