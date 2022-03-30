#ifndef PETRI_TRANSLATION_H
#define PETRI_TRANSLATION_H

#include <filesystem>
#include <list>
#include <string>
#include <deque>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

struct Graph {
  sylvan::Bdd nodes;
  sylvan::BddSet cube;
  std::deque<sylvan::Bdd> relations;
};

sylvan::Bdd makePlace(std::string &bitstring);
sylvan::Bdd makePlaces(std::list<std::string> &bitstrings);
sylvan::Bdd makeArc(std::string &bitstringFrom, std::string &bitstringTo);
sylvan::Bdd makeRelation(std::list<std::pair<std::string, std::string>> &bitstrings);
std::deque<sylvan::Bdd> makeRelations(std::list<std::list<std::pair<std::string, std::string>>> &bitstrings);

void PNMLtoStringLists();

Graph makeGraph(const int nodes, const std::list<std::list<std::pair<int,int>>> &relations);

void printBdd(const sylvan::Bdd &bdd);
void printBddAsString2(const sylvan::Bdd &bdd);
void printBddAsString(int nodes, const sylvan::Bdd &bdd);

sylvan::Bdd shiftBdd(const sylvan::Bdd &bdd);

#endif //PETRI_TRANSLATION_H