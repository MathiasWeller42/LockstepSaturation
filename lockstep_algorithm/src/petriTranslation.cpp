#include <iostream>
#include <list>
#include <map>
#include <set>
#include <fstream>
#include <filesystem>
#include <bitset>
#include <unistd.h>
#include <math.h>
#include <algorithm>
#include <string>
#include <functional>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "petriTranslation.h"
#include "bdd_utilities.h"
#include "graph_creation.h"
#include "print.h"
#include "lockstep.h"

//Gets the local filePath to the file given as argument
inline std::string getPNMLFilePath(std::string file) {
  char tmp[256];
  std::string cwd = getcwd(tmp, 256);
  std::string theString(tmp);
  int chop = theString.find("lockstep");

  theString.erase(chop, std::string::npos);
  std::string newPath = theString + "PNMLFiles/" + file;
  std::cout << "Current working directory: " << newPath << std::endl;
  return newPath;
}

//List1 ∪ List2
std::list<int> list_union(std::list<int> &list1, std::list<int> &list2) {
  list1.sort();
  list2.sort();
  std::list<int> out;
  std::set_union(list1.begin(), list1.end(), list2.begin(), list2.end(), std::back_inserter(out));
  return out;
}

//Overload version with uint
std::list<uint32_t> list_union(std::list<uint32_t> &list1, std::list<uint32_t> &list2) {
  list1.sort();
  list2.sort();
  std::list<uint32_t> out;
  std::set_union(list1.begin(), list1.end(), list2.begin(), list2.end(), std::back_inserter(out));
  return out;
}

//List1 ∩ List2
inline std::list<int> list_intersect(std::list<int> &list1, std::list<int> &list2) {
  list1.sort();
  list2.sort();
  std::list<int> out;
  std::set_intersection(list1.begin(), list1.end(), list2.begin(), list2.end(), std::back_inserter(out));
  return out;
}

// List1 - List2
inline std::list<int> list_difference(std::list<int> &list1, std::list<int> &list2) {
  list1.sort();
  list2.sort();
  std::list<int> out;
  std::set_difference(list1.begin(), list1.end(), list2.begin(), list2.end(), std::back_inserter(out));
  return out;
}

//Takes a petri-net transition and makes it into a relation
//The transition has sources and targets
//- The sources go from having a mark to not having a mark
//- The targets go from not having a mark to having a mark
// The relation then is all configurations in which it flips these bits defining this transition
// Example: P0 -> | -> P1 would be 01 -> 10 while keeping all other places in the petri net fixed (these define several nodes in the "graph")
inline Relation makeRelationFromTransition(Transition transition, std::map<std::string, int> placeMap) {
  Relation result = {};

  sylvan::Bdd resultBdd = leaf_true();

  //The sets of vars included in the sources and targets in the transition
  std::list<int> sources = {};
  std::list<int> targets = {};

  //Handling self-loops
  for(Arc arc : transition.sources) {
    if(placeMap.count(arc.source)) {
      int placeNum = placeMap[arc.source];
      sources.push_back(placeNum);
    } else {
      std::cout << "Place " << arc.source << " not found in PlaceMap" << std::endl;
      std::cout << arc.source << " should be source to transition " << transition.id << std::endl;
      std::exit(-1);
    }
  }
  for(Arc arc : transition.targets) {
    if(placeMap.count(arc.target)) {
      int placeNum = placeMap[arc.target];
      targets.push_back(placeNum);
    } else {
      std::cout << "Place " << arc.target << " not found in PlaceMap" << std::endl;
      std::cout << arc.target << " should be target of transition " << transition.id << std::endl;
      std::exit(-1);
    }
  }

  //Make the cube for the relation
  std::list<int> varSet = list_union(sources, targets);
  sylvan::BddSet cube = sylvan::BddSet();
  for(int i : varSet) {
    cube.add(i);
  }
  result.cube = cube;
  result.top = varSet.front();

  //Create the relation BDD
  std::list<int> selfLoops = list_intersect(sources, targets);
  std::list<int> newSources = list_difference(sources, targets);
  std::list<int> newTargets = list_difference(targets, sources);

  for(int placeNum : selfLoops) {
    sylvan::Bdd beforeSource = ithvar(placeNum);
    resultBdd = resultBdd.And(beforeSource);

    sylvan::Bdd afterSource = ithvar(placeNum+1);
    resultBdd = resultBdd.And(afterSource);
  }

  for(int placeNum : newSources) {
    //The source places have tokens before the transition
    sylvan::Bdd beforeSource = ithvar(placeNum);
    resultBdd = resultBdd.And(beforeSource);

    //After the transition, the source places have no tokens
    sylvan::Bdd afterSource = nithvar(placeNum + 1);
    resultBdd = resultBdd.And(afterSource);
  }

  for(int placeNum : newTargets) {
    //The target places have no tokens before the transition
    //TODO: Overvej om det kan fjerererererernes.
    /*sylvan::Bdd beforeTarget = nithvar(placeNum);
    resultBdd = resultBdd.And(beforeTarget);*/

    //After the transition, the target places now have tokens
    sylvan::Bdd afterTarget = ithvar(placeNum + 1);
    resultBdd = resultBdd.And(afterTarget);
  }

  result.relationBdd = resultBdd;

  return result;
}

// Takes a PNML file defining a petri net and converts it into a Graph
// A node in the resulting graph is a configuration of markings in the petri net
// A relation is a transition from marking to some other marking
Graph PNMLtoGraph(std::string fileString, bool useInitialMarking) {
  std::string path = getPNMLFilePath(fileString);

  // Read from the text file
  std::ifstream readFile(path);

  if(!readFile) {
    std::cout << "Invalid file name" << std::endl;
    std::exit(-1);
  }

  int placeIndex = 0;
  std::map<std::string, int> placeMap = {};
  std::map<std::string, Transition> transitionMap = {};
  std::list<Arc> arcList = {};
  std::set<int> initialMarking = {};

  std::string myText;
  // Read the PNML file and create some data structures
  while (std::getline (readFile, myText)) {
    //Places
    if(myText.rfind("<place ", 0) == 0) {
      std::string id = "";
      int startpos = myText.find("id=\"");
      int endpos = myText.find("\">");
      id = myText.substr(startpos + 4, endpos-(startpos+4));

      if(myText.find("initialMarking") != std::string::npos) {
      initialMarking.insert(placeIndex);
      }

      placeMap[id] = placeIndex;
      placeIndex = placeIndex+2;
    }

    //Transitions
    if(myText.rfind("<transition ", 0) == 0) {
      std::string id = "";
      int startpos = myText.find("id=\"");
      int endpos = myText.find("\">");
      id = myText.substr(startpos + 4, endpos-(startpos+4));
      transitionMap[id] = Transition(id);
    }

    //Arcs
    if(myText.rfind("<arc ", 0) == 0) {
      Arc arc = {};
      //find id
      int startpos = myText.find("id=\"");
      int endpos = myText.find("\" ");
      std::string id = myText.substr(startpos + 4, endpos-(startpos+4));
      arc.id = id;

      //find source
      startpos = myText.find("source=\"");
      endpos = myText.find("\" t");
      std::string source = myText.substr(startpos + 8, endpos-(startpos+8));
      arc.source = source;

      //Target format varies
      std::string searchString = "SimpleLoadBal";
      int exists = fileString.find(searchString);
      if(exists == -1){
        //It is a HealthRecord (or other)
        startpos = myText.find("target=\"");
        endpos = myText.find("\"/>");
        std::string target = myText.substr(startpos + 8, endpos-(startpos+8));
        arc.target = target;
      } else {
        //It is a SimpleLoadBal
        startpos = myText.find("target=\"");
        endpos = myText.find("\">");
        std::string target = myText.substr(startpos + 8, endpos-(startpos+8));
        arc.target = target;
      }

      //push the arc
      arcList.push_back(arc);
    }
  }

  // Close the file
  readFile.close();

  //Distribute arcs to the different transitions
  for(Arc arc : arcList) {
    if(transitionMap.count(arc.source)) {
      transitionMap[arc.source].targets.push_back(arc);
      if(!placeMap.count(arc.target)) {
        std::cout << "No place found for arc: " << arc.toString() << std::endl;
        std::exit(-1);
      }
    } else if(transitionMap.count(arc.target)) {
      transitionMap[arc.target].sources.push_back(arc);
      if(!placeMap.count(arc.source)) {
        std::cout << "No place found for arc: " << arc.toString() << std::endl;
        std::exit(-1);
      }
    } else {
      std::cout << "Found arc which is not connected to transition" << std::endl;
      std::cout << "id: " << arc.id << std::endl;
      std::cout << "source: " << arc.source << std::endl;
      std::cout << "target: " << arc.target << std::endl;
      std::exit(-1);
    }
  }

  std::deque<Relation> relations;
  for(std::pair<std::string, Transition> key_value : transitionMap) {
    //std::cout << key_value.second.toString() << std::endl;
    Relation relationObj = makeRelationFromTransition(key_value.second, placeMap);
    relations.push_back(relationObj);
  }

  int numPlaces = placeMap.size();
  std::cout << "Number of places: " << std::to_string(numPlaces) << std::endl;
  sylvan::BddSet cube = makeCube(numPlaces);
  std::cout << "Number of relations: " << relations.size() << std::endl;

  sylvan::Bdd initialBdd = leaf_true();
  for(int i = 0; i < 2*numPlaces; i=i+2) {
    sylvan::Bdd placeMarking =  initialMarking.find(i) != initialMarking.end() ? ithvar(i) : nithvar(i);
    initialBdd = initialBdd.And(placeMarking);
  }

  Graph pnmlGraph;
  pnmlGraph.relations = relations;
  pnmlGraph.cube = cube;

  if(useInitialMarking) {
    std::cout << "Using reachability from initial marking to limit state space" << std::endl;
    pnmlGraph.nodes = leaf_true();
    pnmlGraph.nodes = reachabilityForwardSaturation(pnmlGraph, initialBdd);
  } else {
    std::cout << "Using the entire state space (ignoring initial marking)" << std::endl;
    pnmlGraph.nodes = leaf_true();
  }

  return pnmlGraph;
}
