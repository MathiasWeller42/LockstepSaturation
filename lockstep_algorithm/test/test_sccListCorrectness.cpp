#include <list>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "graph_examples.h"
#include "../src/petriTranslation.h"
#include "../src/graph_creation.h"
#include "../src/lockstep_test.h"

bool testSccCorrectnessLen() {
  std::string n0 = "00000"; std::string n1 = "00001";
  std::string n2 = "00010"; std::string n3 = "00011";
  std::string n4 = "00100"; std::string n5 = "00101";
  std::string n6 = "00110"; std::string n7 = "00111";
  std::string n8 = "01000"; std::string n9 = "01001";
  std::string n10 = "01010"; std::string n11 = "01011";
  std::string n12 = "01100"; std::string n13 = "01101";
  std::string n14 = "01110"; std::string n15 = "01111";
  std::string n16 = "10000"; std::string n17 = "10001";
  std::string n18 = "10010"; std::string n19 = "10011";
  std::string n20 = "10100";

  std::list<std::string> nodeList1 = {n0};
  std::list<std::string> nodeList2 = {n1,n2};
  std::list<std::string> nodeList3 = {n3,n4,n5};
  std::list<std::string> nodeList4 = {n6,n7,n8,n9};
  std::list<std::string> nodeList5 = {n10,n11,n12,n13,n14};
  std::list<std::string> nodeList6 = {n15,n16,n17,n18,n19,n20};
  sylvan::Bdd scc1 = makeNodes(nodeList1);
  sylvan::Bdd scc2 = makeNodes(nodeList2);
  sylvan::Bdd scc3 = makeNodes(nodeList3);
  sylvan::Bdd scc4 = makeNodes(nodeList4);
  sylvan::Bdd scc5 = makeNodes(nodeList5);
  sylvan::Bdd scc6 = makeNodes(nodeList6);

  std::list<sylvan::Bdd> sccList1 = {scc1, scc6, scc4, scc5, scc3, scc2};
  std::list<sylvan::Bdd> sccList2 = {scc1, scc6, scc4, scc3};

  return sccListCorrectness(sccList1, sccList2);
}

bool testSccCorrectnessDuplicates() {
  std::string n0 = "00000"; std::string n1 = "00001";
  std::string n2 = "00010"; std::string n3 = "00011";
  std::string n4 = "00100"; std::string n5 = "00101";
  std::string n6 = "00110"; std::string n7 = "00111";
  std::string n8 = "01000"; std::string n9 = "01001";
  std::string n10 = "01010"; std::string n11 = "01011";
  std::string n12 = "01100"; std::string n13 = "01101";
  std::string n14 = "01110"; std::string n15 = "01111";
  std::string n16 = "10000"; std::string n17 = "10001";
  std::string n18 = "10010"; std::string n19 = "10011";
  std::string n20 = "10100";

  std::list<std::string> nodeList1 = {n0};
  std::list<std::string> nodeList2 = {n1,n2};
  std::list<std::string> nodeList3 = {n3,n4,n5};
  std::list<std::string> nodeList4 = {n6,n7,n8,n9};
  std::list<std::string> nodeList5 = {n10,n11,n12,n13,n14};
  std::list<std::string> nodeList6 = {n15,n16,n17,n18,n19,n20};
  sylvan::Bdd scc1 = makeNodes(nodeList1);
  sylvan::Bdd scc2 = makeNodes(nodeList2);
  sylvan::Bdd scc3 = makeNodes(nodeList3);
  sylvan::Bdd scc4 = makeNodes(nodeList4);
  sylvan::Bdd scc5 = makeNodes(nodeList5);
  sylvan::Bdd scc6 = makeNodes(nodeList6);

  std::list<sylvan::Bdd> sccList2 = {scc1, scc6, scc4, scc3};
  std::list<sylvan::Bdd> sccList1 = {scc1, scc6, scc4, scc4};

  return sccListCorrectness(sccList1, sccList2);
}

bool testSccCorrectnessNegSameLenNoDuplicates() {
  std::string n0 = "00000"; std::string n1 = "00001";
  std::string n2 = "00010"; std::string n3 = "00011";
  std::string n4 = "00100"; std::string n5 = "00101";
  std::string n6 = "00110"; std::string n7 = "00111";
  std::string n8 = "01000"; std::string n9 = "01001";
  std::string n10 = "01010"; std::string n11 = "01011";
  std::string n12 = "01100"; std::string n13 = "01101";
  std::string n14 = "01110"; std::string n15 = "01111";
  std::string n16 = "10000"; std::string n17 = "10001";
  std::string n18 = "10010"; std::string n19 = "10011";
  std::string n20 = "10100";

  std::list<std::string> nodeList1 = {n0};
  std::list<std::string> nodeList2 = {n1,n2};
  std::list<std::string> nodeList3 = {n3,n4,n5};
  std::list<std::string> nodeList4 = {n6,n7,n8,n9};
  std::list<std::string> nodeList5 = {n10,n11,n12,n13,n14};
  std::list<std::string> nodeList6 = {n15,n16,n17,n18,n19,n20};
  sylvan::Bdd scc1 = makeNodes(nodeList1);
  sylvan::Bdd scc2 = makeNodes(nodeList2);
  sylvan::Bdd scc3 = makeNodes(nodeList3);
  sylvan::Bdd scc4 = makeNodes(nodeList4);
  sylvan::Bdd scc5 = makeNodes(nodeList5);
  sylvan::Bdd scc6 = makeNodes(nodeList6);

  std::list<sylvan::Bdd> sccList2 = {scc1, scc6, scc4, scc5};
  std::list<sylvan::Bdd> sccList1 = {scc1, scc6, scc2, scc3};

  return sccListCorrectness(sccList1, sccList2);
}

bool testSccCorrectnessPosPermutation() {
  std::string n0 = "00000"; std::string n1 = "00001";
  std::string n2 = "00010"; std::string n3 = "00011";
  std::string n4 = "00100"; std::string n5 = "00101";
  std::string n6 = "00110"; std::string n7 = "00111";
  std::string n8 = "01000"; std::string n9 = "01001";
  std::string n10 = "01010"; std::string n11 = "01011";
  std::string n12 = "01100"; std::string n13 = "01101";
  std::string n14 = "01110"; std::string n15 = "01111";
  std::string n16 = "10000"; std::string n17 = "10001";
  std::string n18 = "10010"; std::string n19 = "10011";
  std::string n20 = "10100";

  std::list<std::string> nodeList1 = {n0};
  std::list<std::string> nodeList2 = {n1,n2};
  std::list<std::string> nodeList3 = {n3,n4,n5};
  std::list<std::string> nodeList4 = {n6,n7,n8,n9};
  std::list<std::string> nodeList5 = {n10,n11,n12,n13,n14};
  std::list<std::string> nodeList6 = {n15,n16,n17,n18,n19,n20};
  sylvan::Bdd scc1 = makeNodes(nodeList1);
  sylvan::Bdd scc2 = makeNodes(nodeList2);
  sylvan::Bdd scc3 = makeNodes(nodeList3);
  sylvan::Bdd scc4 = makeNodes(nodeList4);
  sylvan::Bdd scc5 = makeNodes(nodeList5);
  sylvan::Bdd scc6 = makeNodes(nodeList6);

  std::list<sylvan::Bdd> sccList2 = {scc1, scc2, scc3, scc4, scc5, scc6};
  std::list<sylvan::Bdd> sccList1 = {scc6, scc5, scc4, scc3, scc2, scc1};

  return sccListCorrectness(sccList1, sccList2);
}

 bool testsccListContainsDifferentSccsWithDuplicateNodesPos() {
  std::string n0 = "00000"; std::string n1 = "00001";
  std::string n2 = "00010"; std::string n3 = "00011";
  std::string n4 = "00100"; std::string n5 = "00101";
  std::string n6 = "00110"; std::string n7 = "00111";
  std::string n8 = "01000"; std::string n9 = "01001";

  std::list<std::string> nodeList1 = {n0};
  std::list<std::string> nodeList2 = {n1,n2};
  std::list<std::string> nodeList3 = {n3,n4,n5,n6};
  std::list<std::string> nodeList4 = {n6,n7,n8,n9};
  sylvan::Bdd scc1 = makeNodes(nodeList1);
  sylvan::Bdd scc2 = makeNodes(nodeList2);
  sylvan::Bdd scc3 = makeNodes(nodeList3);
  sylvan::Bdd scc4 = makeNodes(nodeList4);

  std::list<sylvan::Bdd> sccList = {scc1, scc2, scc3, scc4};

  return sccListContainsDifferentSccsWithDuplicateNodes(sccList);
 }

 bool testsccListContainsDifferentSccsWithDuplicateNodesNeg() {
  std::string n0 = "00000"; std::string n1 = "00001";
  std::string n2 = "00010"; std::string n3 = "00011";
  std::string n4 = "00100"; std::string n5 = "00101";
  std::string n6 = "00110"; std::string n7 = "00111";
  std::string n8 = "01000"; std::string n9 = "01001";

  std::list<std::string> nodeList1 = {n0};
  std::list<std::string> nodeList2 = {n1,n2};
  std::list<std::string> nodeList3 = {n3,n4,n5};
  std::list<std::string> nodeList4 = {n6,n7,n8,n9};
  sylvan::Bdd scc1 = makeNodes(nodeList1);
  sylvan::Bdd scc2 = makeNodes(nodeList2);
  sylvan::Bdd scc3 = makeNodes(nodeList3);
  sylvan::Bdd scc4 = makeNodes(nodeList4);

  std::list<sylvan::Bdd> sccList = {scc1, scc2, scc3, scc4};

  return sccListContainsDifferentSccsWithDuplicateNodes(sccList);
 }

bool testUnionIsWholeBddPos() {
  std::string n0 = "00000"; std::string n1 = "00001";
  std::string n2 = "00010"; std::string n3 = "00011";
  std::string n4 = "00100"; std::string n5 = "00101";
  std::string n6 = "00110"; std::string n7 = "00111";
  std::string n8 = "01000"; std::string n9 = "01001";

  std::list<std::string> nodeList1 = {n0};
  std::list<std::string> nodeList2 = {n1,n2};
  std::list<std::string> nodeList3 = {n3,n4,n5};
  std::list<std::string> nodeList4 = {n6,n7,n8,n9};
  std::list<std::string> nodeList = {n0, n1, n2, n3,n4,n5,n6,n7,n8,n9};
  sylvan::Bdd scc1 = makeNodes(nodeList1);
  sylvan::Bdd scc2 = makeNodes(nodeList2);
  sylvan::Bdd scc3 = makeNodes(nodeList3);
  sylvan::Bdd scc4 = makeNodes(nodeList4);

  std::list<sylvan::Bdd> sccList = {scc1, scc2, scc3, scc4};
  sylvan::Bdd nodeSet =  makeNodes(nodeList);

  return sccUnionIsWholeBdd(sccList, nodeSet);;
}

bool testUnionIsWholeBddNeg() {
  std::string n0 = "00000"; std::string n1 = "00001";
  std::string n2 = "00010"; std::string n3 = "00011";
  std::string n4 = "00100"; std::string n5 = "00101";
  std::string n6 = "00110"; std::string n7 = "00111";
  std::string n8 = "01000"; std::string n9 = "01001";

  std::list<std::string> nodeList1 = {n0};
  std::list<std::string> nodeList2 = {n1,n2};
  std::list<std::string> nodeList3 = {n3,n4,n5};
  std::list<std::string> nodeList4 = {n6,n7,n8};
  std::list<std::string> nodeList = {n0, n1, n2, n3,n4,n5,n6,n7,n8,n9};
  sylvan::Bdd scc1 = makeNodes(nodeList1);
  sylvan::Bdd scc2 = makeNodes(nodeList2);
  sylvan::Bdd scc3 = makeNodes(nodeList3);
  sylvan::Bdd scc4 = makeNodes(nodeList4);

  std::list<sylvan::Bdd> sccList = {scc1, scc2, scc3, scc4};
  sylvan::Bdd nodeSet =  makeNodes(nodeList);

  return sccUnionIsWholeBdd(sccList, nodeSet);
}





