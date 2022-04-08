#include <iostream>
#include <list>
#include <iterator>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "graph_examples.h"
#include "../src/lockstep.h"
#include "../src/bdd_utilities.h"
#include "../src/petriTranslation.h"
#include "../src/graph_creation.h"
#include "../src/lockstep_test.h"

bool testGraph(const Graph &graph, const std::list<sylvan::Bdd> &expectedSCCs, bool saturation, bool iterative) {
  std::list<sylvan::Bdd> result;
  if(saturation) {
    if(iterative) {
      result = lockstepSaturationIterative(graph);
    } else {
      result = lockstepSaturation(graph);
    }
  } else {
    if(iterative) {
      result = lockstepRelationUnionIterative(graph);
    } else {
      result = lockstepRelationUnion(graph);
    }
  }
  if(result.size() != expectedSCCs.size()){
    std::cout << "Too many or too few SCCs" << std::endl;
    std::cout << "Result amount: " << result.size() << std::endl;
    std::cout << "Expected amount: " << expectedSCCs.size() << std::endl;
    return false;
  }
  return sccListCorrectness(result, expectedSCCs);
}

bool testNoGraph(bool saturation, bool iterative) {
  std::list<sylvan::Bdd> expectedSCCs = {};
  const Graph graph = noGraph();
  return testGraph(graph, expectedSCCs, saturation, iterative);
}

bool testOneNodeGraph(bool saturation, bool iterative) {
  std::string bitstring = "0";
  sylvan::Bdd scc1 = makeNode(bitstring);
  std::list<sylvan::Bdd> expectedSccList = {scc1};

  const Graph graph = oneNodeGraph();
  return testGraph(graph, expectedSccList, saturation, iterative);
}

bool testOneNodeGraphSelfLoop(bool saturation, bool iterative) {
  std::string bitstring = "0";
  sylvan::Bdd scc1 = makeNode(bitstring);
  std::list<sylvan::Bdd> expectedSccList = {scc1};

  const Graph graph = oneNodeGraphSelfLoop();
  return testGraph(graph, expectedSccList, saturation, iterative);
}

bool testTwoNodeGraphOneSCC(bool saturation, bool iterative) {
  std::string a = "0"; std::string b = "1";
  std::list<std::string> nodeList = {a,b};
  sylvan::Bdd scc1 = makeNodes(nodeList);
  std::list<sylvan::Bdd> expectedSccList = {scc1};

  const Graph graph = twoNodeGraphOneSCC();
  return testGraph(graph, expectedSccList, saturation, iterative);
}

bool testTwoNodeGraphTwoSCCs(bool saturation, bool iterative) {
  std::string a = "0"; std::string b = "1";
  sylvan::Bdd scc1 = makeNode(a);
  sylvan::Bdd scc2 = makeNode(b);
  std::list<sylvan::Bdd> expectedSccList = {scc1,scc2};

  const Graph graph = twoNodeGraphTwoSCCs();
  return testGraph(graph, expectedSccList, saturation, iterative);
}

bool testTwoNodeGraphTwoRelations(bool saturation, bool iterative) {
  std::string a = "0"; std::string b = "1";
  std::list<std::string> nodeList = {a,b};
  sylvan::Bdd scc1 = makeNodes(nodeList);
  std::list<sylvan::Bdd> expectedSccList = {scc1};

  const Graph graph = twoNodeGraphTwoRelations();
  return testGraph(graph, expectedSccList, saturation, iterative);
}

bool testNonConnectedGraph(bool saturation, bool iterative) {
  std::string a = "0"; std::string b = "1";
  sylvan::Bdd scc1 = makeNode(a);
  sylvan::Bdd scc2 = makeNode(b);
  std::list<sylvan::Bdd> expectedSccList = {scc1,scc2};

  const Graph graph = nonConnectedGraph();
  return testGraph(graph, expectedSccList, saturation, iterative);
}

bool testfourNodesOneRelation(bool saturation, bool iterative) {
  std::string a = "00"; std::string b = "01";
  std::string c = "10"; std::string d = "11";
  std::list<std::string> nodeList = {a,b,c,d};
  sylvan::Bdd scc1 = makeNodes(nodeList);
  std::list<sylvan::Bdd> expectedSccList = {scc1};

  const Graph graph = twoNodeGraphTwoRelations();
  return testGraph(graph, expectedSccList, saturation, iterative);
}

bool testGraphExample1oneRel(bool saturation, bool iterative) {
  std::string zero = "0000";
  std::string one =  "0001";
  std::string two =  "0010";
  std::string three ="0011";
  std::string four = "0100";
  std::string five = "0101";
  std::string six =  "0110";
  std::string seven ="0111";
  std::string eight ="1000";

  std::list<std::string> nodeList1 = {zero,one,two};
  std::list<std::string> nodeList2 = {three};
  std::list<std::string> nodeList3 = {four};
  std::list<std::string> nodeList4 = {five,six,seven};
  std::list<std::string> nodeList5 = {eight};
  sylvan::Bdd scc1 = makeNodes(nodeList1);
  sylvan::Bdd scc2 = makeNodes(nodeList2);
  sylvan::Bdd scc3 = makeNodes(nodeList3);
  sylvan::Bdd scc4 = makeNodes(nodeList4);
  sylvan::Bdd scc5 = makeNodes(nodeList5);
  std::list<sylvan::Bdd> expectedSccList = {scc1, scc5, scc3, scc4, scc2};

  const Graph graph = graphExample1oneRel();
  return testGraph(graph, expectedSccList, saturation, iterative);
}

bool testGraphExample1multRel(bool saturation, bool iterative) {
  std::string zero  = "0000";
  std::string one   = "0001";
  std::string two   = "0010";
  std::string three = "0011";
  std::string four  = "0100";
  std::string five  = "0101";
  std::string six   = "0110";
  std::string seven = "0111";
  std::string eight = "1000";

  std::list<std::string> nodeList1 = {zero,one,two};
  std::list<std::string> nodeList2 = {three};
  std::list<std::string> nodeList3 = {four};
  std::list<std::string> nodeList4 = {five,six,seven};
  std::list<std::string> nodeList5 = {eight};
  sylvan::Bdd scc1 = makeNodes(nodeList1);
  sylvan::Bdd scc2 = makeNodes(nodeList2);
  sylvan::Bdd scc3 = makeNodes(nodeList3);
  sylvan::Bdd scc4 = makeNodes(nodeList4);
  sylvan::Bdd scc5 = makeNodes(nodeList5);
  std::list<sylvan::Bdd> expectedSccList = {scc1, scc5, scc3, scc4, scc2};

  const Graph graph = graphExample1multRel();
  return testGraph(graph, expectedSccList, saturation, iterative);
}

bool testGraphExample2oneRel(bool saturation, bool iterative) {
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
  std::list<sylvan::Bdd> expectedSccList = {scc1, scc6, scc4, scc5, scc3, scc2};

  const Graph graph = graphExample2oneRel();
  return testGraph(graph, expectedSccList, saturation, iterative);
}

bool testGraphExample2multRel(bool saturation, bool iterative) {
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
  std::list<sylvan::Bdd> expectedSccList = {scc1, scc6, scc4, scc3, scc2, scc5};

  const Graph graph = graphExample2multRel();
  return testGraph(graph, expectedSccList, saturation, iterative);
}

bool testGraphExample3oneRel(bool saturation, bool iterative) {
  std::string a = "000"; std::string b = "001";
  std::string c = "010"; std::string d = "011"; std::string e = "100";
  std::list<std::string> nodeList = {a,b,c,d,e};
  sylvan::Bdd scc1 = makeNodes(nodeList);
  std::list<sylvan::Bdd> expectedSccList = {scc1};

  const Graph graph = graphExample3oneRel();
  return testGraph(graph, expectedSccList, saturation, iterative);
}

bool testGraphExample3multRel(bool saturation, bool iterative) {
  std::string a = "000"; std::string b = "001";
  std::string c = "010"; std::string d = "011"; std::string e = "100";
  std::list<std::string> nodeList = {a,b,c,d,e};
  sylvan::Bdd scc1 = makeNodes(nodeList);
  std::list<sylvan::Bdd> expectedSccList = {scc1};

  const Graph graph = graphExample3multRel();
  return testGraph(graph, expectedSccList, saturation, iterative);
}

