#ifndef PETRI_TRANSLATION_H
#define PETRI_TRANSLATION_H

#include <filesystem>
#include <list>
#include <string>
#include <deque>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "lockstep.h"

sylvan::Bdd makePlace(std::string &bitstring);
sylvan::Bdd makePlaces(std::list<std::string> &bitstrings);
sylvan::Bdd makeArc(std::string &bitstringFrom, std::string &bitstringTo);
sylvan::Bdd makeRelation(std::list<std::pair<std::string, std::string>> &bitstrings);
std::deque<sylvan::Bdd> makeRelations(std::list<std::list<std::pair<std::string, std::string>>> &bitstrings);
void printBdd(sylvan::Bdd bdd);
void makeGraph();
void makeGraphGreatAgain();
void PNMLtoStringLists();
Graph makeGraph(const int nodes, std::list<std::list<std::pair<int,int>>> relations);
std::string decimalToBinaryString(int number, int bytes);
void printBddAsString(sylvan::Bdd bdd);
void printBddAsString2(int nodes, sylvan::Bdd bdd);

#endif //PETRI_TRANSLATION_H