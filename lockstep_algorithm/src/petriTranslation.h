#ifndef PETRI_TRANSLATION_H
#define PETRI_TRANSLATION_H

#include <filesystem>
#include <list>
#include <string>
#include <deque>
#include <map>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "graph_creation.h"

struct Arc {
  std::string id;
  std::string source;
  std::string target;
  std::string toString()
  {
    return "Arc:   id: " + id + " from: " + source + " to: " + target;
  }
};

struct Transition {
  std::string id;
  std::list<Arc> sources;
  std::list<Arc> targets;

  Transition(std::string idString) : id(idString), sources({}), targets({}) {}
  Transition() {}

  std::string toString()
  {
    int sourceNo = sources.size();
    int targetNo = targets.size();
    std::string sourceString = "Sources: \n";
    std::string targetString = "Targets: \n";
    for(Arc source : sources) {
      sourceString = sourceString + source.toString() + "\n";
    }
    for(Arc target : targets) {
      targetString = targetString + target.toString() + "\n";
    }
    return "I'm a transition id: " + id + "\n" + std::to_string(sourceNo) + " " + sourceString + std::to_string(targetNo) + " " + targetString;
  }
};

Graph PNMLtoGraph(std::string fileString);

#endif //PETRI_TRANSLATION_H