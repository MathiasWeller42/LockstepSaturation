#ifndef TEST_SCCLISTCORRECTNESS_H
#define TEST_SCCLISTCORRECTNESS_H

#include <list>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "graph_examples.h"
#include "../src/petriTranslation.h"

bool testSccCorrectnessLen();
bool testSccCorrectnessDuplicates();
bool testSccCorrectnessNegSameLenNoDuplicates();
bool testSccCorrectnessPosPermutation();

#endif