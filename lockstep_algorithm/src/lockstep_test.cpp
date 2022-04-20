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

//These pathstrings were sorted with HumanSort *TM*
std::list<std::string> getPathStrings() {
  std::list<std::string> resultList = {};

  resultList.push_back("ShieldRVt/PT/shield_t_rv_001_a_11place.pnml");                        //11
  resultList.push_back("GPUForwardProgress/PT/userdef_15place.pnml");                         //15
  resultList.push_back("ShieldRVs/PT/shield_s_rv_001_a_17place.pnml");                        //17
  resultList.push_back("ShieldRVt/PT/shield_t_rv_002_a_19place.pnml");                        //19
  /*
  resultList.push_back("ShieldIIPt/PT/shield_t_iip_001_a_22place.pnml");                      //22
  resultList.push_back("GPUForwardProgress/PT/gpufp_04_a_24place.pnml");                      //24
  resultList.push_back("ShieldRVt/PT/shield_t_rv_003_a_27place.pnml");                        //27
  resultList.push_back("CloudOpsManagement/PT/CloudOpsManagement-00002by00001_27place.pnml"); //27
  resultList.push_back("CloudOpsManagement/PT/CloudOpsManagement-00005by00002_27place.pnml"); //27
  resultList.push_back("CloudOpsManagement/PT/CloudOpsManagement-00080by00040_27place.pnml"); //27
  resultList.push_back("CloudOpsManagement/PT/CloudOpsManagement-01280by00640_27place.pnml"); //27
  resultList.push_back("CloudOpsManagement/PT/CloudOpsManagement-10240by05120_27place.pnml"); //27
  resultList.push_back("CloudOpsManagement/PT/CloudOpsManagement-20480by10240_27place.pnml"); //27

  resultList.push_back("ShieldRVs/PT/shield_s_rv_002_a_31place.pnml");                        //31
  resultList.push_back("SimpleLoadBal/PT/simple_lbs-2_32place.pnml");                         //32
  resultList.push_back("ShieldIIPt/PT/shield_t_iip_002_a_41place.pnml");                      //41
  resultList.push_back("ShieldRVs/PT/shield_s_rv_001_b_43place.pnml");                        //43
  resultList.push_back("ShieldIIPt/PT/shield_t_iip_003_a_60place.pnml");                      //60
  resultList.push_back("ShieldIIPt/PT/shield_t_iip_004_a_79place.pnml");                      //79*/

  return resultList;
}

//Main experimentation function
//Runs each algorithm on a list of graphs from PNML files with varying preprocessing amounts
//Prints the results and writes them to a csv-file
void experiment(std::list<std::string> pathStrings, int maxPreprocess, int minPreprocess, std::list<algorithmType> runTypes) {
  //The amount of files and algorithms to run experiments on - is used to init the size of the csv-file rows
  int noFiles = pathStrings.size();
  int noAlgorithms = runTypes.size();

  //CSV filename and empty container for the rows and init row 0 headers
  std::string csvFileName = "test1";
  std::vector<std::vector<std::string>> grid = initCsvGrid(noFiles, noAlgorithms);

  //The current csv row to insert results into
  int csvRow = 0;
  for(std::string pathString : pathStrings) {
    std::cout << "###### Running experiment on file at path: " << pathString << std::endl;
    bool useInitialMarking = false;
    Graph graph = PNMLtoGraph(pathString, useInitialMarking);

    std::string noOfPlaces = std::to_string(graph.cube.size());
    std::string noOfRelations = std::to_string(graph.relations.size());
    /*TODO: fix below!*/
    grid[csvRow+1].insert(grid[csvRow+1].end(), {"Saturation", noOfPlaces, noOfRelations});
    grid[csvRow+2].insert(grid[csvRow+2].end(), {"Relation Union", noOfPlaces, noOfRelations});
    grid[csvRow+3].insert(grid[csvRow+3].end(), {"Literal relation union", noOfPlaces, noOfRelations});

    grid = testAndPrintWithMax(graph, maxPreprocess, minPreprocess, runTypes, grid, csvFileName, csvRow);
    //Move three rows after an experiment since we have three methods to test on
    csvRow = csvRow+noAlgorithms+1;
  }
  writeToCSV(csvFileName, grid);
}

void validateAlgoSccResults(const std::list<sylvan::Bdd> resultSccList, const Graph originalGraph) {
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

//Sorts the relations by their top and prunes the graph pruningSteps times
Graph graphPreprocessing(const Graph &graph, int pruningSteps) {
  Graph resultGraph = graph;
  resultGraph = sortRelations(resultGraph);

  for(int i = 0; i < pruningSteps; i++) {
    resultGraph = pruneGraph(resultGraph);
  }

  long long prunedNodes = countNodes(graph.cube.size(), differenceBdd(graph.nodes, resultGraph.nodes));
  std::cout << "Pruned " << prunedNodes << " nodes" << std::endl;
  std::cout << "Finished pre-processing of graph" << std::endl;
  return resultGraph;
}

//Sorts the relations by their top and prunes the graph until it has no more 1-node SCCs
Graph graphPreprocessingFixedPoint(const Graph &graph) {
  Graph resultGraph = graph;
  resultGraph = sortRelations(resultGraph);

  resultGraph = fixedPointPruning(resultGraph);

  long long prunedNodes = countNodes(graph.cube.size(), differenceBdd(graph.nodes, resultGraph.nodes));
  std::cout << "Pruned " << prunedNodes << " nodes" << std::endl;
  std::cout << "Finished pre-processing of graph" << std::endl;
  return resultGraph;
}

//Sorts the relations by their top and prunes the graph until it has no more 1-node SCCs
std::pair<Graph, int> graphPreprocessingFixedPointWithMax(const Graph &graph, int maxPruning) {
  Graph resultGraph = graph;
  resultGraph = sortRelations(resultGraph);

  std::pair<Graph, int> result = fixedPointPruningWithMax(resultGraph, maxPruning);

  long long prunedNodes = countNodes(graph.cube.size(), differenceBdd(graph.nodes, resultGraph.nodes));
  std::cout << "Pruned " << prunedNodes << " nodes" << std::endl;
  std::cout << "Finished pre-processing of graph" << std::endl;
  return result;
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

bool sccListCorrectness(const std::list<sylvan::Bdd> sccList1, const std::list<sylvan::Bdd> sccList2) {
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

//CSV related stuff
std::vector<std::vector<std::string>> testAndPrintWithMax(const Graph &graph, int maxPruning, int minPruning, std::list<algorithmType> runTypes, std::vector<std::vector<std::string>> grid, std::string fileName, int row) {
  if(minPruning < 0) {
    minPruning = 0;
  }
  if(maxPruning < 0) {
    std::cout << "### With pre-processing (fixed point) " << std::endl;
    Graph processedGraph = graphPreprocessingFixedPoint(graph);
    grid = timeAll(processedGraph, runTypes, grid, row);
    std::cout << std::endl;
    grid[row].insert(grid[row].end(), {"SCC's", "Fixed-point pruning steps (ms)", "SSC/ms" });
  }
  else {
    if(maxPruning == 0) {
      Graph processedGraph = graphPreprocessing(graph, 0);
      grid = timeAll(graph, runTypes, grid, row);
      grid[row].insert(grid[row].end(), {"SCC's", std::to_string(0) + " pruning steps (ms)", "SCC/ms" });
    }
    else {
      std::cout << "### With pre-processing (" << std::to_string(maxPruning) << " or fixed-point) " << std::endl;
      std::pair<Graph, int> result = graphPreprocessingFixedPointWithMax(graph, maxPruning);
      Graph processedGraph = result.first;
      int newMax = result.second;

      int newMax2Pow = pow(2,floor(log2(newMax-1)));

      grid = timeAll(processedGraph, runTypes, grid, row);
      std::cout << std::endl;
      grid[row].insert(grid[row].end(), {"SCC's", std::to_string(newMax) + " pruning steps (ms)", "SCC/ms" });

      for(int i = newMax2Pow; i >= minPruning; i = floor(i/2)) {
        std::cout << "### With pre-processing (" << std::to_string(i) << ") " << std::endl;
        processedGraph = graphPreprocessing(graph, i);
        grid = timeAll(processedGraph, runTypes, grid, row);
        std::cout << std::endl;

        grid[row].insert(grid[row].end(), {"SCC's", std::to_string(i) + " pruning steps (ms)", "SCC/ms" });

        if(i == 0 || i == minPruning) {
          break;
        }
      }
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
std::vector<std::vector<std::string>> initCsvGrid(int noOfExperimentGraphs, int noOfAlgorithms) {
  int noOfRows = (noOfExperimentGraphs) * (noOfAlgorithms+2) + 1;

  std::vector<std::vector<std::string>> grid(noOfRows, std::vector<std::string>(0));

  for(int i = 0; i < noOfRows; i = i+5) {
    grid[i].insert(grid[i].end(), {"Algorithm run", "Places", "Relations"});
  }
  std::cout << "No of rows in new grid:" << std::to_string(noOfRows) << std::endl;
  return grid;
}

std::pair<std::list<sylvan::Bdd>, float> timeRun(const Graph &graph, algorithmType runType) {
  auto start = std::chrono::high_resolution_clock::now();
  std::list<sylvan::Bdd> sccList;
  std::string runTypeStr = "";

  if(runType == lockstepSat) {
    sccList = lockstepSaturation(graph);
    runTypeStr = "lockstep saturation";
  }
  else if(runType == lockstepRelUnion) {
    sccList = lockstepRelationUnion(graph);
    runTypeStr = "lockstep relation union";
  }
  else if(runType == lockstepLitRelUnion) {
    sccList = lockstepLiteralRelationUnion(graph);
    runTypeStr = "lockstep literal relation union";
  }
  else if(runType == xbSat) {
    sccList = xieBeerelSaturation(graph);
    runTypeStr = "Xie-Beerel saturation";
  }
  else if(runType == xbRelUnion) {
    sccList = xieBeerelRelationUnion(graph);
    runTypeStr = "Xie-Beerel relation union";
  }

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
  std::cout << "Time elapsed (" << runTypeStr << "): " << duration.count() << " milliseconds" << std::endl;
  std::cout << "Found " << sccList.size() << " SCCs" << std::endl << std::endl;
  std::pair<std::list<sylvan::Bdd>, float> result = {sccList, (float) duration.count()};
  return result;
}

//Finds Scc's on a graph with each algorithm, prints the timings and results, and appends them to the CSV-grid which is returned
std::vector<std::vector<std::string>> timeAll(const Graph &graph, std::list<algorithmType> runTypes, std::vector<std::vector<std::string>> grid, int row) {
  int i = 1;
  for(algorithmType runType : runTypes) {
    std::pair<std::list<sylvan::Bdd>, float> runResults = timeRun(graph, runType);
    std::list<sylvan::Bdd> sccList = runResults.first;
    float duration = runResults.second;

    std::string annoyingFloatString =  std::to_string((float)sccList.size() / duration);
    std::replace(annoyingFloatString.begin(), annoyingFloatString.end(), '.', ',');
    grid[row+i].insert(grid[row+i].end(), {std::to_string(sccList.size()), std::to_string(duration), annoyingFloatString});

    i++;
  }

  return grid;
}