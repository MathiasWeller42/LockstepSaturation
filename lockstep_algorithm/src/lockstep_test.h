#ifndef LOCKSTEP_TEST_H
#define LOCKSTEP_TEST_H

#include "petriTranslation.h"

void experiment();

void timeSaturation(const Graph &graph);
void timeSaturationIterative(const Graph &graph);

Graph graphPreprocessing(const Graph &graph, int pruningSteps);

bool sccListCorrectness(std::list<sylvan::Bdd> sccList1, std::list<sylvan::Bdd> sccList2);
bool sccListContainsDifferentSccsWithDuplicateNodes(const std::list<sylvan::Bdd> sccList);
bool sccUnionIsWholeBdd(const std::list<sylvan::Bdd> sccList, const sylvan::Bdd nodes);
bool containsDuplicateSccs(const std::list<sylvan::Bdd> sccList);

void writeToCSV(std::string fileName, std::vector<std::vector<std::string>> grid);
std::vector<std::vector<std::string>> testAndPrintWithMax(Graph &graph, int maxPruning, std::vector<std::vector<std::string>> grid, std::string fileName, int row);
std::vector<std::vector<std::string>> timeAll2(const Graph &graph, std::vector<std::vector<std::string>> grid, int row);

#endif //LOCKSTEP_TEST_H