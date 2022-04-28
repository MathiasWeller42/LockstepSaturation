#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <chrono>

#include "petriTranslation.h"

enum algorithmType
{
  lockstepSat,
  lockstepRelUnion,
  xbSat,
  xbRelUnion,
  xbBackwardSat,
  xbBackwardRelUnion
};

inline const std::string algoToString(algorithmType runType) {
  switch (runType)
  {
    case lockstepSat:
      return "Lockstep saturation";
    case lockstepRelUnion:
      return "Lockstep relation union";
    case xbSat:
      return "Xie-Beerel saturation";
    case xbRelUnion:
      return "Xie-Beerel relation union";
    case xbBackwardSat:
      return "Xie-Beerel backward saturation";
    case xbBackwardRelUnion:
      return "Xie-Beerel backward relation union";
    default:
      return "[Unknown Algorithm type]";
  }
}

void experiment(std::list<std::string> pathStrings, int minPreprocess, int maxPreprocess,
                bool useInitialMarking, std::list<algorithmType> runTypes, std::string fileName);

Graph graphPreprocessing(const Graph &graph, int pruningSteps);
Graph graphPreprocessingFixedPoint(const Graph &graph);
std::pair<Graph, int> graphPreprocessingFixedPointWithMax(const Graph &graph, int maxPruning);

bool sccListCorrectness(const std::list<sylvan::Bdd> sccList1, const std::list<sylvan::Bdd> sccList2);
bool sccListContainsDifferentSccsWithDuplicateNodes(const std::list<sylvan::Bdd> sccList);
bool sccUnionIsWholeBdd(const std::list<sylvan::Bdd> sccList, const sylvan::Bdd nodes);
bool containsDuplicateSccs(const std::list<sylvan::Bdd> sccList);
void validateAlgoSccResults(const std::list<sylvan::Bdd> resultSccList, const Graph originalGraph);

void writeToCSV(std::string fileName, std::vector<std::vector<std::string>> grid);
std::vector<std::vector<std::string>> initCsvGrid(int noOfExperimentGraphs, int noOfAlgorithms);

std::vector<std::vector<std::string>> preprocessAndRun(const Graph &graph, int maxPruning, int minPruning,
                                                       std::list<algorithmType> runTypes,
                                                       std::vector<std::vector<std::string>> grid, int row);
std::vector<std::vector<std::string>> timeAll(const Graph &graph, std::list<algorithmType> runTypes,
                                              std::vector<std::vector<std::string>> grid, int row);
std::tuple<std::list<sylvan::Bdd>, std::chrono::duration<long, std::milli>, int> timeRun(const Graph &graph,
                                                                                         algorithmType runType);

std::list<std::string> getPathStringsAll();
std::list<std::string> getPathStringsFast();

#endif //BENCHMARK_H