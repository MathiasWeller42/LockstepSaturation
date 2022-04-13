#include <filesystem>
#include <list>
#include <string>
#include <deque>
#include <iostream>
#include <deque>
#include <algorithm>
#include <chrono>
#include <vector>
#include <fstream>
#include <math.h>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "lockstep_test.h"
#include "lockstep.h"
#include "petriTranslation.h"
#include "bdd_utilities.h"
#include "graph_creation.h"
#include "print.h"
#include "../test/graph_examples.h"

std::list<std::string> getPathStrings() {
  std::list<std::string> resultList = {};

  resultList.push_back("ShieldRVt/PT/shield_t_rv_001_a_11place.pnml");      //11
  resultList.push_back("GPUForwardProgress/PT/userdef_15place.pnml");       //15
  //resultList.push_back("ShieldRVs/PT/shield_s_rv_001_a_17place.pnml");      //17
  //resultList.push_back("ShieldRVt/PT/shield_t_rv_002_a_19place.pnml");      //19
  //resultList.push_back("ShieldIIPt/PT/shield_t_iip_001_a_22place.pnml");    //22
  //resultList.push_back("GPUForwardProgress/PT/gpufp_04_a_24place.pnml");  //24
  //resultList.push_back("ShieldRVt/PT/shield_t_rv_003_a_27place.pnml");    //27

  return resultList;
}

//Main experimentation function
//Runs each algorithm on a list of graphs from PNML files with varying preprocessing amounts
//Prints the results and writes them to a csv-file
void experiment() {
  //The PNML files to run experiments on
  std::list<std::string> pathStrings = getPathStrings();

  //The amount of files to run experiments on - is used to init the size of the csv-file rows
  int noFiles = pathStrings.size();

  //CSV filename and empty container for the rows and init row 0 headers
  std::string csvFileName = "test";
  std::vector<std::vector<std::string>> grid = initCsvGrid(noFiles);

  //The current csv row to insert results into
  int csvRow = 1;
  for(std::string pathString : pathStrings) {
    std::cout << "###### Running experiment on file at path: " << pathString << std::endl;
    Graph graph = PNMLtoGraph(pathString);
    
    std::string noOfPlaces = std::to_string(graph.cube.size());
    std::string noOfRelations = std::to_string(graph.relations.size());

    grid[csvRow].insert(grid[csvRow].end(), {"Saturation", noOfPlaces, noOfRelations});
    grid[csvRow+1].insert(grid[csvRow+1].end(), {"Relation Union", noOfPlaces, noOfRelations});
    grid[csvRow+2].insert(grid[csvRow+2].end(), {"Literal relation union", noOfPlaces, noOfRelations});


    //Preprocess with every 2-factor until the number specified (0 included)
    int maxPreProcess = 32;
    grid = testAndPrintWithMax(graph, maxPreProcess, grid, csvFileName, csvRow);
    //Move three rows after an experiment since we have three methods to test on
    csvRow = csvRow+3;
  }
  writeToCSV(csvFileName, grid);
}


void validateAlgoSccResults(std::list<sylvan::Bdd> resultSccList, Graph originalGraph) {

  bool hasDuplicates = containsDuplicateSccs(resultSccList);
  if(hasDuplicates) {
    std::cout << "Lockstep saturation gave two or more equal SCCs" << std::endl;
  }
  bool hasOverlap = sccListContainsDifferentSccsWithDuplicateNodes(resultSccList);
  if(hasOverlap) {
    std::cout << "Lockstep saturation gave overlapping SCCs" << std::endl;
  }
  bool foundAllSCCs = sccUnionIsWholeBdd(resultSccList, originalGraph.nodes);
  if(!foundAllSCCs) {
    std::cout << "Lockstep saturation did not find SCCs covering all nodes" << std::endl;
  }
}

void timeSaturation(const Graph &graph, int pruningSteps) {
  Graph processedGraph = graphPreprocessing(graph, pruningSteps);

  auto start1 = std::chrono::high_resolution_clock::now();
  std::list<sylvan::Bdd> sccList1 = lockstepSaturation(processedGraph);
  auto stop1 = std::chrono::high_resolution_clock::now();
  auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(stop1 - start1);
  std::cout << "Time elapsed (saturation): " << duration1.count() << " milliseconds" << std::endl;
  std::cout << "Found " << sccList1.size() << " SCCs" << std::endl << std::endl;


  //validateAlgoSccResults(sccList1, processedGraph);

  auto start2 = std::chrono::high_resolution_clock::now();
  std::list<sylvan::Bdd> sccList2 = lockstepRelationUnion(processedGraph);
  auto stop2 = std::chrono::high_resolution_clock::now();
  auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(stop2 - start2);
  std::cout << "Time elapsed (relation union): " << duration2.count() << " milliseconds" << std::endl;
  std::cout << "Found " << sccList2.size() << " SCCs" << std::endl << std::endl;

  //validateAlgoSccResults(sccList2, processedGraph);
  
  /*
  if(!sccListCorrectness(sccList1, sccList2)) {
    std::cout << "SCC lists did not contain the same BDDs" << std::endl;
  }*/
}

void timeSaturationIterative(const Graph &graph, int pruningSteps) {
  Graph processedGraph = graphPreprocessing(graph, pruningSteps);

  auto start1 = std::chrono::high_resolution_clock::now();
  std::list<sylvan::Bdd> sccList1 = lockstepSaturationIterative(processedGraph);
  auto stop1 = std::chrono::high_resolution_clock::now();
  auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(stop1 - start1);
  std::cout << "Time elapsed (iterative saturation): " << duration1.count() << " milliseconds" << std::endl;
  std::cout << "Found " << sccList1.size() << " SCCs" << std::endl << std::endl;


  auto start2 = std::chrono::high_resolution_clock::now();
  std::list<sylvan::Bdd> sccList2 = lockstepRelationUnionIterative(processedGraph);
  auto stop2 = std::chrono::high_resolution_clock::now();
  auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(stop2 - start2);
  std::cout << "Time elapsed (iterative relation union): " << duration2.count() << " milliseconds" << std::endl;
  std::cout << "Found " << sccList2.size() << " SCCs" << std::endl << std::endl;

  bool hasDuplicates2 = containsDuplicateSccs(sccList2);
  if(hasDuplicates2) {
    std::cout << "Lockstep relation union gave two or more equal SCCs" << std::endl;
  }
  bool hasOverlap2 = sccListContainsDifferentSccsWithDuplicateNodes(sccList2);
  if(hasOverlap2) {
    std::cout << "Lockstep relation union gave overlapping SCCs" << std::endl;
  }
  bool foundAllSCCs2 = sccUnionIsWholeBdd(sccList2, processedGraph.nodes);
  if(!foundAllSCCs2) {
    std::cout << "Lockstep relation union did not find SCCs covering all nodes" << std::endl;
  }

  auto start3 = std::chrono::high_resolution_clock::now();
  std::list<sylvan::Bdd> sccList3 = lockstepRelationUnionIterative(processedGraph);
  auto stop3 = std::chrono::high_resolution_clock::now();
  auto duration3 = std::chrono::duration_cast<std::chrono::milliseconds>(stop3 - start3);
  std::cout << "Time elapsed (iterative literal relation union): " << duration3.count() << " milliseconds" << std::endl;
  std::cout << "Found " << sccList3.size() << " SCCs" << std::endl << std::endl;

  bool hasDuplicates3 = containsDuplicateSccs(sccList3);
  if(hasDuplicates3) {
    std::cout << "Lockstep literal relation union gave two or more equal SCCs" << std::endl;
  }
  bool hasOverlap3 = sccListContainsDifferentSccsWithDuplicateNodes(sccList3);
  if(hasOverlap3) {
    std::cout << "Lockstep literal relation union gave overlapping SCCs" << std::endl;
  }
  bool foundAllSCCs3 = sccUnionIsWholeBdd(sccList3, processedGraph.nodes);
  if(!foundAllSCCs3) {
    std::cout << "Lockstep literal relation union did not find SCCs covering all nodes" << std::endl;
  }

  if(!sccListCorrectness(sccList2, sccList3)) {
    std::cout << "SCC lists did not contain the same BDDs" << std::endl;
  }
}






//Sorts the relations by their top and prunes the graph pruningSteps times
Graph graphPreprocessing(const Graph &graph, int pruningSteps) {
  Graph resultGraph = graph;
  resultGraph = sortRelations(resultGraph);

  for(int i = 0; i < pruningSteps; i++) {
    resultGraph = pruneGraph(resultGraph);
  }

  std::cout << "Finished pre-processing of graph" << std::endl;
  return resultGraph;
}

bool sccListContains(sylvan::Bdd target, std::list<sylvan::Bdd> sccList) {
  for(sylvan::Bdd scc : sccList) {
    if(scc == target) {
      return true;
    }
  }
  return false;
}

bool containsDuplicateSccs(const std::list<sylvan::Bdd> sccList) {
  for(sylvan::Bdd scc1 : sccList) {
    int duplicates = 0;
    for(sylvan::Bdd scc2 : sccList) {
      if(scc1 == scc2) {
        duplicates++;
      }
    }
    if(duplicates > 1) {
      return true;
    }
  }
  return false;
}

bool sccListContainsDifferentSccsWithDuplicateNodes(const std::list<sylvan::Bdd> sccList) {
  for(sylvan::Bdd scc1 : sccList) {
    for(sylvan::Bdd scc2 : sccList) {
      sylvan::Bdd intersect = intersectBdd(scc1, scc2);
      if(intersect != leaf_false() && scc1 != scc2) {
        return true;
      }
    }
  }
  return false;
}

bool sccUnionIsWholeBdd(const std::list<sylvan::Bdd> sccList, const sylvan::Bdd nodes) {
  sylvan::Bdd bddUnion = leaf_false();
  for(sylvan::Bdd scc : sccList) {
    bddUnion = unionBdd(bddUnion, scc);
  }
  sylvan::Bdd result = differenceBdd(nodes, bddUnion);
  return result == leaf_false();
}


bool sccListCorrectness(std::list<sylvan::Bdd> sccList1, std::list<sylvan::Bdd> sccList2) {
  if(sccList1.size() != sccList2.size()) {
    return false;
  }
  if(containsDuplicateSccs(sccList1) || containsDuplicateSccs(sccList2)) {
    return false;
  }
  for(sylvan::Bdd scc : sccList1) {
    if(!sccListContains(scc, sccList2)) {
      return false;
    }
  }
  return true;
}


//CSV related stuff - work in progress

std::vector<std::vector<std::string>> testAndPrintWithMax(Graph &graph, int maxPruning, std::vector<std::vector<std::string>> grid, std::string fileName, int row) {
  std::cout << "testAndPrint!! #########################" << std::endl;

  for(int i = maxPruning; i >= 0; i = floor(i/2)) {
    std::cout << "### With pre-processing (" << std::to_string(i) << ") " << std::endl;
    Graph processedGraph1 = graphPreprocessing(graph, i);
    grid = timeAll(processedGraph1, grid, row);
    std::cout << std::endl;


    grid[0].insert(grid[0].end(), {"SCC's", std::to_string(i) + " pruning steps (ms)" });
    

    if(i == 0) {
      break;
    }
  }
  return grid;
}

void writeToCSV(std::string fileName, std::vector<std::vector<std::string>> grid) {
  std::cout << "Called writeToCSV" << std::endl;

  std::ofstream myFile;
  std::string csvFile = fileName + ".csv";
  myFile.open(csvFile);

  //Takes semi-colon separated cells row by row and writes to .csv file
  for(std::vector<std::string> row : grid) {
    for(std::string cell : row) {
      myFile << cell + ";";
    }
    myFile << "\n";

  }
  myFile.close();
}

//Initializes an empty csv grid with the appropriate amount of rows to insert into
std::vector<std::vector<std::string>> initCsvGrid(int noOfExperimentGraphs) {
  int noOfAlgorithms = 3;
  int noOfRows = noOfExperimentGraphs * noOfAlgorithms + 1;

  std::vector<std::vector<std::string>> grid(noOfRows, std::vector<std::string>(0));
  grid[0].insert(grid[0].end(), {"Algorithm run", "Places", "Relations"});
  
  return grid;
}




//Finds Scc's on a graph with each algorithm, prints the timings and results, and appends them to the CSV-grid which is returned
std::vector<std::vector<std::string>> timeAll(const Graph &graph, std::vector<std::vector<std::string>> grid, int row) {
  //Iterative saturation
  auto start3 = std::chrono::high_resolution_clock::now();
  std::list<sylvan::Bdd> sccList3 = lockstepSaturationIterative(graph);
  auto stop3 = std::chrono::high_resolution_clock::now();
  auto duration3 = std::chrono::duration_cast<std::chrono::milliseconds>(stop3 - start3);
  std::cout << "Time elapsed (iterative saturation): " << duration3.count() << " milliseconds" << std::endl;
  std::cout << "Found " << sccList3.size() << " SCCs" << std::endl << std::endl;

  grid[row].insert(grid[row].end(), {std::to_string(sccList3.size()), std::to_string(duration3.count())});

  //Iterative relation union
  auto start4 = std::chrono::high_resolution_clock::now();
  std::list<sylvan::Bdd> sccList4 = lockstepRelationUnionIterative(graph);
  auto stop4 = std::chrono::high_resolution_clock::now();
  auto duration4 = std::chrono::duration_cast<std::chrono::milliseconds>(stop4 - start4);
  std::cout << "Time elapsed (iterative relation union): " << duration4.count() << " milliseconds" << std::endl;
  std::cout << "Found " << sccList4.size() << " SCCs" << std::endl << std::endl;

  grid[row+1].insert(grid[row+1].end(), {std::to_string(sccList4.size()), std::to_string(duration4.count())});

  //Iterative LITERAL relation union
  auto start5 = std::chrono::high_resolution_clock::now();
  std::list<sylvan::Bdd> sccList5 = lockstepRelationUnionIterative(graph);
  auto stop5 = std::chrono::high_resolution_clock::now();
  auto duration5 = std::chrono::duration_cast<std::chrono::milliseconds>(stop5 - start5);
  std::cout << "Time elapsed (LITERAL iterative relation union): " << duration5.count() << " milliseconds" << std::endl;
  std::cout << "Found " << sccList5.size() << " SCCs" << std::endl << std::endl;

  grid[row+2].insert(grid[row+2].end(), {std::to_string(sccList5.size()), std::to_string(duration5.count())});

  return grid;
}