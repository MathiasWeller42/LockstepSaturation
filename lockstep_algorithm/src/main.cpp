#include <iostream>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "lockstep.h"
#include "petriTranslation.h"
#include "benchmark.h"
#include "print.h"
#include "../test/graph_examples.h"
#include "interface.h"
#include "../test/test_sccListCorrectness.h"

int main() {

  // Init LACE
  lace_start(1, 10000000);

  const size_t memory_bytes = 1024u * 1024u * 1024u;

  // Init Sylvan
  sylvan::sylvan_set_limits(memory_bytes, // Set memory limit
                            6,            // Set (exponent) of cache ratio
                            0);           // Initialise unique node table to full size
  sylvan::sylvan_set_granularity(1);
  sylvan::sylvan_init_package();
  sylvan::sylvan_init_bdd();

  std::cout << "Hello SCC-finding World!" << std::endl;


  std::list<std::string> pathStringsFast = getPathStringsFast();
  std::list<algorithmType> fastRunTypes = {lockstepSat/*, xbSat*/};

  /*for(algorithmType algo : fastRunTypes) {
    std::list<algorithmType> algorithms = {algo};
    std::string fileName = algoToString(algo) + "_xb_slow_1";
    benchmark(pathStringsFast, fileName, algorithms, 0);
  }
  for(algorithmType algo : fastRunTypes) {
    std::list<algorithmType> algorithms = {algo};
    std::string fileName = algoToString(algo) + "_xb_slow_2";
    benchmark(pathStringsFast, fileName, algorithms, 0);
  }
  for(algorithmType algo : fastRunTypes) {
    std::list<algorithmType> algorithms = {algo};
    std::string fileName = algoToString(algo) + "_xb_slow_3";
    benchmark(pathStringsFast, fileName, algorithms, 0);
  }*/

  std::list<std::string> pathStringsAll = getPathStringsAllNoCount();
  std::list<algorithmType> satRunTypes = {xbSat, lockstepSat};
  std::list<algorithmType> relUnionRunTypes = {xbRelUnion, lockstepRelUnion};
  std::list<algorithmType> allRunTypes = {xbSat, lockstepSat, xbRelUnion, lockstepRelUnion};

  for(algorithmType algo : satRunTypes) {
    std::list<algorithmType> algorithms = {algo};
    std::string fileName = algoToString(algo) + "_large_1";
    benchmark(pathStringsAll, fileName, algorithms);
  }
  for(algorithmType algo : satRunTypes) {
    std::list<algorithmType> algorithms = {algo};
    std::string fileName = algoToString(algo) + "_large_no_pruning_1";
    benchmark(pathStringsAll, fileName, algorithms, 0);
  }
  for(algorithmType algo : satRunTypes) {
    std::list<algorithmType> algorithms = {algo};
    std::string fileName = algoToString(algo) + "_large_2";
    benchmark(pathStringsAll, fileName, algorithms);
  }
  for(algorithmType algo : satRunTypes) {
    std::list<algorithmType> algorithms = {algo};
    std::string fileName = algoToString(algo) + "_large_no_pruning_2";
    benchmark(pathStringsAll, fileName, algorithms, 0);
  }
  for(algorithmType algo : satRunTypes) {
    std::list<algorithmType> algorithms = {algo};
    std::string fileName = algoToString(algo) + "_large_3";
    benchmark(pathStringsAll, fileName, algorithms);
  }
  for(algorithmType algo : satRunTypes) {
    std::list<algorithmType> algorithms = {algo};
    std::string fileName = algoToString(algo) + "_large_no_pruning_3";
    benchmark(pathStringsAll, fileName, algorithms, 0);
  }

  std::cout << "Goodbye :)" << std::endl;

  sylvan::sylvan_quit();
  lace_stop();
  return 0;
}