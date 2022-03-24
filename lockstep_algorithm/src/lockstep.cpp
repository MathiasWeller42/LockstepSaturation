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

/*
  A --> B --> D
  |
  v
  C

A: 00
B: 01
C: 10
D: 11
*/

sylvan::Bdd twoBitEdge(bool bit1, bool bit2, bool bit3, bool bit4) {
  sylvan::Bdd bdd1;
  sylvan::Bdd bdd2;
  sylvan::Bdd bdd3;
  sylvan::Bdd bdd4;

  if(bit1) {
    bdd1 = ithvar(0);
  } else {
    bdd1 = nithvar(0);
  }
  if(bit2) {
    bdd2 = ithvar(1);
  } else {
    bdd2 = nithvar(1);
  }
  if(bit3) {
    bdd3 = ithvar(2);
  } else {
    bdd3 = nithvar(2);
  }
  if(bit4) {
    bdd4 = ithvar(3);
  } else {
    bdd4 = nithvar(3);
  }
  return bdd1.And(bdd2).And(bdd3).And(bdd4);
}

void makeGraph() {
  sylvan::Bdd nodeA = nithvar(0).And(nithvar(1));
  sylvan::Bdd nodeB = nithvar(0).And(ithvar(1));
  sylvan::Bdd nodeC = ithvar(0).And(nithvar(1));
  sylvan::Bdd nodeD = ithvar(0).And(ithvar(1));
  sylvan::Bdd nodeSet = nodeA.Or(nodeB).Or(nodeC).Or(nodeD);

  std::cout << "Printing nodeSet:" << std::endl;
  printBdd(nodeSet);
  std::cout << std::endl;

  sylvan::Bdd edgeAB = twoBitEdge(false, false, false, true);
  sylvan::Bdd edgeAC = twoBitEdge(false, true, false, false);
  sylvan::Bdd edgeBD = twoBitEdge(false, true, true, true);
  sylvan::Bdd edgeSet = edgeAB.Or(edgeAC).Or(edgeBD);

  std::cout << "Printing edgeSet:" << std::endl;
  printBdd(edgeSet);
  std::cout << std::endl;

  sylvan::BddSet cube = sylvan::BddSet();
  cube.add(0);
  cube.add(1);
  cube.add(2);
  cube.add(3);

  sylvan::Bdd relnextA = nodeA.RelNext(edgeSet, cube);
  std::cout << "Printing relNextA:" << std::endl;
  printBdd(relnextA);
  std::cout << std::endl;

  sylvan::Bdd relnextB = nodeB.RelNext(edgeSet, cube);
  std::cout << "Printing relNextB:" << std::endl;
  printBdd(relnextB);

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

  std::cout << (isLeft ? "├──" : "└──" );

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