#ifndef INTERFACE_H
#define INTERFACE_H

#include <list>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "benchmark.h"

/*
Interface functions for finding SCCs in graphs from PNML format petri nets and for benchmarking the SCC finding algorithms
*/

//Finds the SCCs in a PNML format petri net using each of algorithm from a list of SCC finding algorithms
std::list<sylvan::Bdd> findSCCs(std::string fileName,
                                algorithmType algorithm,
                                int pruning,
                                bool useInitialMarking);


//Benchmarks the SCC finding algorithms on a list of PNML format petri nets and writes the result to a CSV file
void benchmark(std::list<std::string> PNMLfileNames,
               std::string csvFileName,
               std::list<algorithmType> algorithms,
               int pruning=-1, bool useInitialMarking=true);

#endif //INTERFACE_H