#include <string>
#include <list>
#include <iostream>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "petriTranslation.h"
#include "utilities.h"

#include <vector>
#include <algorithm>
//#include <boost/algorithm/string/join.hpp>

void testUnion() {
  std::string a = "000";
  std::string b = "001";
  std::string c = "010";
  std::string d = "011";
  std::string e = "100";
  std::string f = "101";

  std::list<std::string> placeList1 = {a,b,c,d};
  sylvan::Bdd bdd1 = makeNodes(placeList1);
  std::list<std::string> placeList2 = {c,d,e,f};
  sylvan::Bdd bdd2 = makeNodes(placeList2);

  sylvan::Bdd res = unionBdd(bdd1, bdd2);
  printBdd(res);
}

void testIntersect() {
  std::string a = "000";
  std::string b = "001";
  std::string c = "010";
  std::string d = "011";
  std::string e = "100";
  std::string f = "101";

  std::list<std::string> placeList1 = {a,b,c,d};
  sylvan::Bdd bdd1 = makeNodes(placeList1);
  std::list<std::string> placeList2 = {c,d,e,f};
  sylvan::Bdd bdd2 = makeNodes(placeList2);

  sylvan::Bdd res = intersectBdd(bdd1, bdd2);
  printBdd(res);
}

void testDifference() {
  std::string a = "000";
  std::string b = "001";
  std::string c = "010";
  std::string d = "011";
  std::string e = "100";
  std::string f = "101";

  std::list<std::string> placeList1 = {a,b,c,d};
  sylvan::Bdd bdd1 = makeNodes(placeList1);
  std::list<std::string> placeList2 = {c,d,e,f};
  sylvan::Bdd bdd2 = makeNodes(placeList2);

  sylvan::Bdd res = differenceBdd(bdd1, bdd2);
  printBdd(res);
}

void testPick() {
  std::string a = "000";
  std::string b = "001";
  std::string c = "010";
  std::string d = "011";
  std::string e = "100";
  std::string f = "101";

  std::list<std::string> placeList1 = {a,b,c,d};
  sylvan::Bdd bdd1 = makeNodes(placeList1);
  std::list<std::string> placeList2 = {c,d,e,f};
  sylvan::Bdd bdd2 = makeNodes(placeList2);

  sylvan::Bdd bdd3 = makeNode(a);

  sylvan::BddSet cube = sylvan::BddSet();
  cube.add(0);
  cube.add(2);
  cube.add(4);

  sylvan::Bdd res = pickAssignment(bdd2, cube);

  printBdd(res);
}