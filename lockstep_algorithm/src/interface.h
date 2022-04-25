#ifndef INTERFACE_H
#define INTERFACE_H

#include <list>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "benchmark.h"

std::list<sylvan::Bdd> findSCCs(std::string fileName,
                                algorithmType algorithm,
                                int pruning,
                                bool useInitialMarking);

void benchmark(std::list<std::string> PNMLfileNames,
               std::string csvFileName,
               std::list<algorithmType> algorithms,
               int pruning, bool useInitialMarking);

#endif //INTERFACE_H