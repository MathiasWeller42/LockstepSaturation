#ifndef PETRI_TRANSLATION_H
#define PETRI_TRANSLATION_H

#include <filesystem>
#include <list>
#include <string>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

sylvan::Bdd makePlace(std::string &bitstring);
sylvan::Bdd makePlaces(std::list<std::string> &bitstrings);
sylvan::Bdd makeArc(std::string &bitstringFrom, std::string &bitstringTo);
sylvan::Bdd makeTransition(std::list<std::pair<std::string, std::string>> &bitstrings);
void printBdd(sylvan::Bdd bdd);
void makeGraph();
void makeGraphGreatAgain();
void PNMLtoStringLists();

#endif //PETRI_TRANSLATION_H