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
  resultList.push_back("ShieldRVs/PT/shield_s_rv_001_a_17place.pnml");                        //17
  resultList.push_back("ShieldRVt/PT/shield_t_rv_002_a_19place.pnml");                        //19
/*
  resultList.push_back("ShieldIIPt/PT/shield_t_iip_001_a_22place.pnml");                      //22
  resultList.push_back("GPUForwardProgress/PT/gpufp_04_a_24place.pnml");                      //24
  resultList.push_back("ShieldRVt/PT/shield_t_rv_003_a_27place.pnml");                        //27
  resultList.push_back("ShieldPPPt/PT/shield_t_ppp_001_a_28place.pnml");                      //28
  resultList.push_back("ShieldRVs/PT/shield_s_rv_002_a_31place.pnml");                        //31
  resultList.push_back("SimpleLoadBal/PT/simple_lbs-2_32place.pnml");                         //32
  resultList.push_back("ShieldPPPs/PT/shield_s_ppp_001_a_34place.pnml");                      //34
  resultList.push_back("ShieldRVt/PT/shield_t_rv_004_a_35place.pnml");                        //35
  resultList.push_back("SmartHome/PT/smhome_01_38place.pnml");                                //38
  resultList.push_back("GPUForwardProgress/PT/gpufp_08_a_40place.pnml");                      //40
  //resultList.push_back("Egfr/PT/Egfr-020-1-0_40place.pnml");                                  //40
  resultList.push_back("SmartHome/PT/smhome_02_41place.pnml");                                //41
  resultList.push_back("ShieldIIPt/PT/shield_t_iip_002_a_41place.pnml");                      //41
  resultList.push_back("ShieldRVt/PT/shield_t_rv_005_a_43place.pnml");                        //43
  resultList.push_back("ShieldRVs/PT/shield_s_rv_001_b_43place.pnml");                        //43
  resultList.push_back("SmartHome/PT/smhome_03_45place.pnml");                                //45
  resultList.push_back("ShieldRVs/PT/shield_s_rv_003_a_45place.pnml");                        //45
  resultList.push_back("ShieldPPPt/PT/shield_t_ppp_002_a_53place.pnml");                      //53
  resultList.push_back("ShieldRVt/PT/shield_t_rv_001_b_53place.pnml");                        //53
  resultList.push_back("NQueens/PT/NQueens-PT-05_55place.pnml");                              //55
  resultList.push_back("GPUForwardProgress/PT/gpufp_12_a_56place.pnml");                      //56
  resultList.push_back("ShieldRVs/PT/shield_s_rv_004_a_59place.pnml");                        //59
  resultList.push_back("ShieldIIPt/PT/shield_t_iip_003_a_60place.pnml");                      //60
  resultList.push_back("DiscoveryGPU/PT/discovery_06_a_63place.pnml");                        //63
  resultList.push_back("ShieldPPPs/PT/shield_s_ppp_002_a_65place.pnml");                      //65
  resultList.push_back("ShieldPPPs/PT/shield_s_ppp_001_b_71place.pnml");                      //71
  resultList.push_back("GPUForwardProgress/PT/gpufp_16_a_72place.pnml");                      //72
  resultList.push_back("DiscoveryGPU/PT/discovery_07_a_73place.pnml");                        //73
  resultList.push_back("ShieldIIPt/PT/shield_t_iip_001_b_73place.pnml");                      //73
  resultList.push_back("ShieldRVs/PT/shield_s_rv_005_a_73place.pnml");                        //73
  resultList.push_back("ShieldPPPt/PT/shield_t_ppp_003_a_78place.pnml");                      //78
  resultList.push_back("ShieldIIPt/PT/shield_t_iip_004_a_79place.pnml");                      //79
  resultList.push_back("ShieldPPPt/PT/shield_t_ppp_001_b_81place.pnml");                      //81
  resultList.push_back("ShieldRVt/PT/shield_t_rv_010_a_83place.pnml");                        //83
  resultList.push_back("ShieldRVs/PT/shield_s_rv_002_b_83place.pnml");                        //83
  resultList.push_back("DiscoveryGPU/PT/discovery_08_a_83place.pnml");                        //83
  resultList.push_back("GPUForwardProgress/PT/gpufp_20_a_88place.pnml");                      //88
  resultList.push_back("DiscoveryGPU/PT/discovery_09_a_93place.pnml");                        //93
  resultList.push_back("ShieldPPPs/PT/shield_s_ppp_003_a_96place.pnml");                      //96
  resultList.push_back("ShieldIIPt/PT/shield_t_iip_005_a_98place.pnml");                      //98
  resultList.push_back("ShieldRVt/PT/shield_t_rv_002_b_102place.pnml");                       //102
  resultList.push_back("ShieldPPPt/PT/shield_t_ppp_004_a_103place.pnml");                     //103
  resultList.push_back("DiscoveryGPU/PT/discovery_10_a_103place.pnml");                       //103
  resultList.push_back("GPUForwardProgress/PT/gpufp_24_a_104place.pnml");                     //104
  resultList.push_back("MAPKbis/PT/MAPKbis-53-1-0_106place.pnml");                            //106
  resultList.push_back("MAPKbis/PT/MAPKbis-53-2-0_106place.pnml");                            //106
  resultList.push_back("GPUForwardProgress/PT/gpufp_04_b_112place.pnml");                     //112
  resultList.push_back("NQueens/PT/NQueens-PT-08_112place.pnml");                             //112
  resultList.push_back("DiscoveryGPU/PT/discovery_11_a_113place.pnml");                       //113
  resultList.push_back("HealthRecord/PT/hrec_01_117place.pnml");                              //117
  resultList.push_back("HealthRecord/PT/hrec_02_119place.pnml");                              //119
  resultList.push_back("GPUForwardProgress/PT/gpufp_28_a_120place.pnml");                     //120
  resultList.push_back("HealthRecord/PT/hrec_03_121place.pnml");                              //121
  resultList.push_back("HealthRecord/PT/hrec_04_123place.pnml");                              //123
  resultList.push_back("DiscoveryGPU/PT/discovery_12_a_123place.pnml");                       //123
  resultList.push_back("ShieldRVs/PT/shield_s_rv_003_b_123place.pnml");                       //123
  resultList.push_back("HealthRecord/PT/hrec_05_125place.pnml");                              //125
  resultList.push_back("ShieldPPPs/PT/shield_s_ppp_004_a_127place.pnml");                     //127
  resultList.push_back("ShieldPPPt/PT/shield_t_ppp_005_a_128place.pnml");                     //128
  resultList.push_back("DiscoveryGPU/PT/discovery_13_a_133place.pnml");                       //133
  resultList.push_back("GPUForwardProgress/PT/gpufp_32_a_136place.pnml");                     //136
  resultList.push_back("ShieldPPPs/PT/shield_s_ppp_002_b_139place.pnml");                     //139
  resultList.push_back("SmartHome/PT/smhome_04_139place.pnml");                               //139
  resultList.push_back("DiscoveryGPU/PT/discovery_14_a_143place.pnml");                       //143
  resultList.push_back("ShieldIIPt/PT/shield_t_iip_002_b_143place.pnml");                     //143
  resultList.push_back("ShieldRVs/PT/shield_s_rv_010_a_143place.pnml");                       //143
  resultList.push_back("GPUForwardProgress/PT/gpufp_36_a_152place.pnml");                     //152
  resultList.push_back("ShieldRVt/PT/shield_t_rv_003_b_153place.pnml");                       //153
  resultList.push_back("DiscoveryGPU/PT/discovery_15_a_153place.pnml");                       //153
  resultList.push_back("HealthRecord/PT/hrec_06_154place.pnml");                              //154
  resultList.push_back("HealthRecord/PT/hrec_07_155place.pnml");                              //155
  resultList.push_back("HealthRecord/PT/hrec_08_156place.pnml");                              //156
  resultList.push_back("HealthRecord/PT/hrec_09_157place.pnml");                              //157
  resultList.push_back("HealthRecord/PT/hrec_10_158place.pnml");                              //158
  resultList.push_back("ShieldIIPt/PT/shield_t_iip_005_a_158place.pnml");                     //158
  resultList.push_back("ShieldPPPs/PT/shield_s_ppp_005_a_158place.pnml");                     //158
  resultList.push_back("ShieldPPPt/PT/shield_t_ppp_002_b_159place.pnml");                     //159
  resultList.push_back("HealthRecord/PT/hrec_11_159place.pnml");                              //159
  resultList.push_back("HealthRecord/PT/hrec_12_160place.pnml");                              //160
  resultList.push_back("NQueens/PT/NQueens-PT-10_160place.pnml");                             //160
  resultList.push_back("HealthRecord/PT/hrec_13_161place.pnml");                              //161
  resultList.push_back("HealthRecord/PT/hrec_14_162place.pnml");                              //162
  resultList.push_back("HealthRecord/PT/hrec_15_163place.pnml");                              //163
  resultList.push_back("ShieldRVt/PT/shield_t_rv_020_a_163place.pnml");                       //163
  resultList.push_back("ShieldRVs/PT/shield_s_rv_004_b_163place.pnml");                       //163
  resultList.push_back("NoC3x3/PT/noc3x3_1_a_165place.pnml");                                 //165
  resultList.push_back("GPUForwardProgress/PT/gpufp_40_a_168place.pnml");                     //168
  resultList.push_back("DiscoveryGPU/PT/discovery_06_b_184place.pnml");                       //184
  resultList.push_back("GPUForwardProgress/PT/gpufp_08_b_188place.pnml");                     //188
  resultList.push_back("ShieldIIPt/PT/shield_t_iip_010_a_193place.pnml");                     //193
  resultList.push_back("ShieldRVt/PT/shield_t_rv_004_b_203place.pnml");                       //203
  resultList.push_back("ShieldRVs/PT/shield_s_rv_005_b_203place.pnml");                       //203
  //resultList.push_back("Egfr/PT/Egfr-104-2-0_208place.pnml");                                 //208
  //resultList.push_back("Egfr/PT/Egfr-104-2-1_208place.pnml");                                 //208
  resultList.push_back("DiscoveryGPU/PT/discovery_07_b_212place.pnml");                       //212
  resultList.push_back("ShieldIIPt/PT/shield_t_iip_003_b_213place.pnml");                     //213
*/
  return resultList;
}

//Main experimentation function
//Runs each algorithm on a list of graphs from PNML files with varying preprocessing amounts
//Prints the results and writes them to a csv-file
void experiment(std::list<std::string> pathStrings, int maxPreprocess, int minPreprocess, bool useInitialMarking, std::list<algorithmType> runTypes) {
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
    Graph graph = PNMLtoGraph(pathString, useInitialMarking);

    std::string noOfPlaces = std::to_string(graph.cube.size());
    std::string noOfRelations = std::to_string(graph.relations.size());
    /*TODO: fix below!*/

    int i = 1;
    for(algorithmType algo : runTypes) {
      grid[csvRow+i].insert(grid[csvRow+i].end(), {algoToString(algo), noOfPlaces, noOfRelations});
      i++;
    }

    grid = testAndPrintWithMax(graph, maxPreprocess, minPreprocess, runTypes, grid, csvFileName, csvRow);
    //Move three rows after an experiment since we have three methods to test on
    csvRow = csvRow+noAlgorithms+2;
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
    std::cout << "the size of the scc lists were different" << std::endl;
    return false;
  }
  if(containsDuplicateSccs(sccList1) || containsDuplicateSccs(sccList2)) {
    std::cout << "One of the lists contained duplicate sccs" << std::endl;
    return false;
  }
  for(sylvan::Bdd scc : sccList1) {
    if(!sccListContains(scc, sccList2)) {
      std::cout << "The lists didn't contain the same scc" << std::endl;
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

  for(int i = 0; i < noOfRows; i = i+noOfAlgorithms+2) {
    grid[i].insert(grid[i].end(), {"Algorithm run", "Places", "Relations"});
  }
  return grid;
}

std::pair<std::list<sylvan::Bdd>, std::chrono::duration<long, std::milli>> timeRun(const Graph &graph, algorithmType runType) {
  std::pair<std::list<sylvan::Bdd>, int> sccAndSteps;
  std::list<sylvan::Bdd> sccList;
  auto start = std::chrono::high_resolution_clock::now();
  switch(runType) {
    case lockstepSat:
      sccAndSteps = lockstepSaturation(graph);
      sccList = sccAndSteps.first;
      break;
    case lockstepRelUnion:
      sccAndSteps = lockstepRelationUnion(graph);
      sccList = sccAndSteps.first;
      break;
    case xbSat:
      sccAndSteps = xieBeerelSaturation(graph);
      sccList = sccAndSteps.first;
      break;
    case xbRelUnion:
      sccAndSteps = xieBeerelRelationUnion(graph);
      sccList = sccAndSteps.first;
      break;
    case xbBackwardSat:
      sccAndSteps = xieBeerelBackwardSaturation(graph);
      sccList = sccAndSteps.first;
      break;
    case xbBackwardRelUnion:
      sccAndSteps = xieBeerelBackwardRelationUnion(graph);
      sccList = sccAndSteps.first;
      break;
  }

  auto stop = std::chrono::high_resolution_clock::now();
  std::chrono::duration<long, std::milli> duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
  std::cout << "Time elapsed (" << algoToString(runType) << "): " << duration.count() << " milliseconds" << std::endl;
  std::cout << "Found " << sccList.size() << " SCCs" << std::endl << std::endl;
  std::pair<std::list<sylvan::Bdd>, std::chrono::duration<long, std::milli>> result = {sccList, duration};
  return result;
}

//Finds Scc's on a graph with each algorithm, prints the timings and results, and appends them to the CSV-grid which is returned
std::vector<std::vector<std::string>> timeAll(const Graph &graph, std::list<algorithmType> runTypes, std::vector<std::vector<std::string>> grid, int row) {
  int i = 1;
  for(algorithmType runType : runTypes) {
    std::cout << "Running on algorithm: " << algoToString(runType) << std::endl;
    std::pair<std::list<sylvan::Bdd>, std::chrono::duration<long, std::milli>> runResults = timeRun(graph, runType);
    std::list<sylvan::Bdd> sccList = runResults.first;
    std::chrono::duration<long, std::milli> duration = runResults.second;

    std::string annoyingFloatString =  std::to_string((float)sccList.size() / duration.count());
    std::replace(annoyingFloatString.begin(), annoyingFloatString.end(), '.', ',');
    grid[row+i].insert(grid[row+i].end(), {std::to_string(sccList.size()), std::to_string(duration.count()), annoyingFloatString});

    i++;
  }

  return grid;
}
