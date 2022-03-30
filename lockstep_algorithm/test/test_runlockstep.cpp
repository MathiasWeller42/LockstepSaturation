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
    std::cout << "skrrr";
  }
  return res; //TODO fix - does not work (yet)
}

bool testNoGraph() {
  sylvan::Bdd expected_scc = leaf_false();
  std::list<sylvan::Bdd> expectedSCCs = {expected_scc};
  const Graph graph = noGraph();

  return testGraph(graph, expectedSCCs);
}

bool testOneNodeGraph() {
  std::string bitstring = "0";
  sylvan::Bdd expected_scc = makeNode(bitstring);
  const Graph graph = oneNodeGraph();
  std::list<sylvan::Bdd> result = lockstepSaturation(graph);
  sylvan::Bdd result_scc = result.front();

  return expected_scc == result_scc;
}

bool testOneNodeGraphSelfLoop() {
  std::string bitstring = "0";
  sylvan::Bdd expected_scc = makeNode(bitstring);

  const Graph graph = oneNodeGraphSelfLoop();
  std::list<sylvan::Bdd> result = lockstepSaturation(graph);
  sylvan::Bdd result_scc = result.front();

  return expected_scc == result_scc;
}


bool testTwoNodeGraphTwoSCCs() {
  std::string a = "0";
  std::string b = "1";
  
  sylvan::Bdd scc1 = makeNode(a);
  sylvan::Bdd scc2 = makeNode(b);

  std::list<sylvan::Bdd> expectedSccList = {scc1,scc2};
  std::cout << "Da size: " << expectedSccList.size()  << std::endl;

  const Graph graph = twoNodeGraphTwoSCCs();
  return testGraph(graph, expectedSccList);
}