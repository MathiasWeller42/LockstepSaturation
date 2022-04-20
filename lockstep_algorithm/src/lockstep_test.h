#ifndef LOCKSTEP_TEST_H
#define LOCKSTEP_TEST_H

#include "petriTranslation.h"

enum algorithmType
{
  lockstepSat,
  lockstepRelUnion,
  lockstepLitRelUnion,
  xbSat,
  xbRelUnion,
};

void experiment(std::list<std::string> pathStrings, int maxPreprocess, int minPreprocess, std::list<algorithmType> runTypes);

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

std::vector<std::vector<std::string>> testAndPrintWithMax(const Graph &graph, int maxPruning, int minPruning, std::list<algorithmType> runTypes, std::vector<std::vector<std::string>> grid, std::string fileName, int row);
std::vector<std::vector<std::string>> timeAll(const Graph &graph, std::list<algorithmType> runTypes, std::vector<std::vector<std::string>> grid, int row);

std::list<std::string> getPathStrings();

#endif //LOCKSTEP_TEST_H