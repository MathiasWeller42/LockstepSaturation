#ifndef LOCKSTEP_TEST_H
#define LOCKSTEP_TEST_H

#include "petriTranslation.h"

void timeSaturation(const Graph &graph);
void timeSaturationIterative(const Graph &graph);

Graph graphPreprocessing(const Graph &graph);

bool sccListCorrectness(std::list<sylvan::Bdd> sccList1, std::list<sylvan::Bdd> sccList2);
bool sccListContainsDifferentSccsWithDuplicateNodes(const std::list<sylvan::Bdd> sccList);
bool sccUnionIsWholeBdd(const std::list<sylvan::Bdd> sccList, const sylvan::Bdd nodes);
bool containsDuplicateSccs(const std::list<sylvan::Bdd> sccList);

#endif //LOCKSTEP_TEST_H