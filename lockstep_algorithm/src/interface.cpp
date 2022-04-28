#include <list>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "interface.h"
#include "benchmark.h"
#include "petriTranslation.h"

/*
Takes as input a PNML file containing a 1-safe petri net and an SCC-finding algorithm.
Finds the SCC's in the Petri Net, in a manner based on the optional arguments.
Optional arguments:
- validate (default: true): Whether or not the output should be validated (n^2 time in # of SCCs).
- useInitialMarking (default: true): Finds SCCs only in the reachable state-space if true, and looks
at all (also unreachable) states if false.
- pruning (default: -1): Prunes all SCCs of size 1 by default, otherwise runs as many prunings steps as specified.
*/
std::list<sylvan::Bdd> findSCCs(std::string PNMLfileName,
                                algorithmType algorithm,
                                int pruning=-1,
                                bool useInitialMarking=true) {
  Graph originalGraph = PNMLtoGraph(PNMLfileName, useInitialMarking);
  Graph processedGraph;
  if(pruning < 0) {
    processedGraph = graphPreprocessingFixedPoint(originalGraph);
  } else {
    //TODO: Brug max-versionen i stedet?
    std::pair<Graph, int> result = graphPreprocessingFixedPointWithMax(originalGraph, pruning);
    processedGraph = result.first;
  }
  auto result = timeRun(processedGraph, algorithm);
  std::list<sylvan::Bdd> sccList = std::get<0>(result);
  return sccList;
}

/*
Takes as input: 1) a list of PNML filepaths containing 1-safe petri nets, 2) a file-name to write the results to
and 3) a list of SCC finding algorithms.
It finds the SCCs for each of the Petri nets for each of the algorithms and writes the results to fileName.csv,
based on the optional arguments.
Optional arguments:
- pruning (default = -1): Prunes all SCCs of size 1 by default, otherwise it runs as many pruning steps as specified.
- useInitialMarking (default = true): Finds SCCs only in the reachable state-space if true, and looks
at all (also unreachable) states if false.
*/
void benchmark(std::list<std::string> PNMLfileNames, std::string csvFileName,
               std::list<algorithmType> algorithms,
               int pruning, bool useInitialMarking) {
  experiment(PNMLfileNames, pruning, pruning, useInitialMarking, algorithms, csvFileName);
}
