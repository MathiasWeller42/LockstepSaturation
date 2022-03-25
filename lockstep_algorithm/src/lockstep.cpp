#include <iostream>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "lockstep.h"
#include "utilities.h"

bool testFunction() {
  sylvan::Bdd t = leaf_true();
  sylvan::Bdd f = leaf_false();
  return t.Or(f).isZero();
}

sylvan::Bdd testFunction2() {
  sylvan::Bdd x42 = ithvar(42);
  sylvan::Bdd x69 = ithvar(69);
  return x42.Or(x69);
}

sylvan::Bdd makePlace(std::string &bitstring) {
  sylvan::Bdd resBdd = leaf_true();

  bool currentBit;
  sylvan::Bdd currentBdd;
  int n = bitstring.length();
  for (int i = n - 1; i >= 0; i--) {
    currentBit = bitstring[i] == '1';
    if(currentBit) {
      currentBdd = ithvar(2*(n-1-i));
    } else {
      currentBdd = nithvar(2*(n-1-i));
    }
    resBdd = resBdd.And(currentBdd);
  }
  return resBdd;
}

sylvan::Bdd makeArc(std::string &bitstringFrom, std::string &bitstringTo) {
  sylvan::Bdd resBdd = leaf_true();

  bool currentBitFrom;
  sylvan::Bdd currentBddFrom;
  int currentIFrom;
  bool currentBitTo;
  sylvan::Bdd currentBddTo;
  int currentITo;
  int nFrom = bitstringFrom.length();
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
    resBdd = resBdd.And(currentBddFrom).And(currentBddTo);
  }
  return resBdd;
}

void makeGraph() {
  std::string aString = "00";
  sylvan::Bdd nodeA = makePlace(aString);
  std::string bString = "01";
  sylvan::Bdd nodeB = makePlace(bString);
  sylvan::Bdd nodeSet = nodeA.Or(nodeB);

  std::cout << "Printing nodeSet:" << std::endl;
  printBdd(nodeSet);
  std::cout << std::endl;

  sylvan::Bdd edgeAB = makeArc(aString, bString);
  sylvan::Bdd edgeSet = edgeAB;

  std::cout << "Printing edgeSet:" << std::endl;
  printBdd(edgeSet);
  std::cout << std::endl;

  sylvan::BddSet cube = sylvan::BddSet();
  cube.add(0);
  //cube.add(1);
  cube.add(2);
  //cube.add(3);

  sylvan::Bdd relnextA = nodeA.RelNext(edgeSet, cube);
  std::cout << "Printing relNextA:" << std::endl;
  printBdd(relnextA);
  std::cout << std::endl;

  sylvan::Bdd relnextB = nodeB.RelNext(edgeSet, cube);
  std::cout << "Printing relNextB:" << std::endl;
  printBdd(relnextB);
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
  sylvan::Bdd nodeSet = nodeA.Or(nodeB).Or(nodeC).Or(nodeD);

  std::cout << "Printing nodeSet:" << std::endl;
  printBdd(nodeSet);
  std::cout << std::endl;

  sylvan::Bdd edgeAB = makeArc(aString, bString);
  sylvan::Bdd edgeAC = makeArc(aString, cString);
  sylvan::Bdd edgeBD = makeArc(bString, dString);
  sylvan::Bdd edgeSet = edgeAB.Or(edgeAC).Or(edgeBD);

  std::cout << "Printing edgeSet:" << std::endl;
  printBdd(edgeSet);
  std::cout << std::endl;

  sylvan::BddSet cube = sylvan::BddSet();
  cube.add(0);
  //cube.add(1);
  cube.add(2);
  //cube.add(3);

  sylvan::Bdd relnextA = nodeA.RelNext(edgeSet, cube);
  std::cout << "Printing relNextA:" << std::endl;
  printBdd(relnextA);
  std::cout << std::endl;

  sylvan::Bdd relnextB = nodeB.RelNext(edgeSet, cube);
  std::cout << "Printing relNextB:" << std::endl;
  printBdd(relnextB);
  std::cout << std::endl;

  sylvan::Bdd relnextC = nodeC.RelNext(edgeSet, cube);
  std::cout << "Printing relNextC:" << std::endl;
  printBdd(relnextC);
  std::cout << std::endl;

  sylvan::Bdd relnextD = nodeD.RelNext(edgeSet, cube);
  std::cout << "Printing relNextD:" << std::endl;
  printBdd(relnextD);
  std::cout << std::endl;
}

void test3() {
  sylvan::Bdd nodeA = nithvar(0);
  sylvan::Bdd nodeB = ithvar(0);
  sylvan::Bdd nodeAB = nodeA.And(nodeB);
}


void printBdd(std::string prefix, sylvan::Bdd bdd, bool isLeft) {
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
  printBdd( prefix + newString, bdd.Then(), true);
  printBdd( prefix + newString, bdd.Else(), false);
}

void printBdd(sylvan::Bdd bdd) {
  printBdd(" ", bdd, false);
}

/*
TODO : Automatiser edgeset + nodeset creation og flyt til anden fil
TODO : Første udkast til lockstep
TODO : Lave relativt simpelt eksempel vi kan afprøve vores lockstep på
TODO : PNML pain

TODO : Græd
TODO : #Forskning
TODO : Græd i Jacos skød

TODO : ???
TODO : Profit
TODO : Søvn
*/