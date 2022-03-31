#include <iostream>
#include <list>
#include <iterator>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "../src/lockstep.h"
#include "../src/utilities.h"
#include "../src/graph_examples.h"
#include "../src/petriTranslation.h"

bool testGraph(const Graph &graph, const std::list<sylvan::Bdd> &expectedSCCs) {
  bool res = true;

  std::list<sylvan::Bdd> result = lockstepSaturation(graph);
  if(result.size() != expectedSCCs.size()){
    std::cout << "Too many or too few SCCs" << std::endl;
    std::cout << "Result amount: " << result.size() << std::endl;
    std::cout << "Expected amount: " << expectedSCCs.size() << std::endl;
    return false;
  }
  std::list<sylvan::Bdd>::iterator it1 = result.begin();
  std::list<sylvan::Bdd>::const_iterator it2 = expectedSCCs.begin();
  for(; it1 != result.end() && it2 != expectedSCCs.end(); ++it1, ++it2) {
    if(!(*it1 == *it2)) {
      std::cout << "SCC's were not equal" << std::endl;
      std::cout << "Result: " << std::endl;
      printBdd(*it1);
      std::cout << "Expected: " << std::endl;
      printBdd(*it2);
      res = false;
    }
  }
  return res;
}

bool testNoGraph() {
  std::list<sylvan::Bdd> expectedSCCs = {};
  const Graph graph = noGraph();
  return testGraph(graph, expectedSCCs);
}

bool testOneNodeGraph() {
  std::string bitstring = "0";
  sylvan::Bdd scc1 = makeNode(bitstring);
  std::list<sylvan::Bdd> expectedSccList = {scc1};

  const Graph graph = oneNodeGraph();
  return testGraph(graph, expectedSccList);
}

bool testOneNodeGraphSelfLoop() {
  std::string bitstring = "0";
  sylvan::Bdd scc1 = makeNode(bitstring);
  std::list<sylvan::Bdd> expectedSccList = {scc1};

  const Graph graph = oneNodeGraphSelfLoop();
  return testGraph(graph, expectedSccList);
}

bool testTwoNodeGraphOneSCC() {
  std::string a = "0"; std::string b = "1";
  std::list<std::string> nodeList = {a,b};
  sylvan::Bdd scc1 = makeNodes(nodeList);
  std::list<sylvan::Bdd> expectedSccList = {scc1};

  const Graph graph = twoNodeGraphOneSCC();
  return testGraph(graph, expectedSccList);
}

bool testTwoNodeGraphTwoSCCs() {
  std::string a = "0"; std::string b = "1";
  sylvan::Bdd scc1 = makeNode(a);
  sylvan::Bdd scc2 = makeNode(b);
  std::list<sylvan::Bdd> expectedSccList = {scc1,scc2};

  const Graph graph = twoNodeGraphTwoSCCs();
  return testGraph(graph, expectedSccList);
}

bool testTwoNodeGraphTwoRelations() {
  std::string a = "0"; std::string b = "1";
  std::list<std::string> nodeList = {a,b};
  sylvan::Bdd scc1 = makeNodes(nodeList);
  std::list<sylvan::Bdd> expectedSccList = {scc1};

  const Graph graph = twoNodeGraphTwoRelations();
  return testGraph(graph, expectedSccList);
}

bool testNonConnectedGraph() {
  std::string a = "0"; std::string b = "1";
  sylvan::Bdd scc1 = makeNode(a);
  sylvan::Bdd scc2 = makeNode(b);
  std::list<sylvan::Bdd> expectedSccList = {scc1,scc2};

  const Graph graph = nonConnectedGraph();
  return testGraph(graph, expectedSccList);
}

bool testfourNodesOneRelation() {
  std::string a = "00"; std::string b = "01";
  std::string c = "10"; std::string d = "11";
  std::list<std::string> nodeList = {a,b,c,d};
  sylvan::Bdd scc1 = makeNodes(nodeList);
  std::list<sylvan::Bdd> expectedSccList = {scc1};

  const Graph graph = twoNodeGraphTwoRelations();
  return testGraph(graph, expectedSccList);

}

bool testGraphExample1oneRel() {
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
  return testGraph(graph, expectedSccList);
}

bool testGraphExample1multRel() {
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
  return testGraph(graph, expectedSccList);
}