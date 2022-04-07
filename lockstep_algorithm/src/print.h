#ifndef PRINT_H
#define PRINT_H

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

void printBdd(const sylvan::Bdd &bdd);
void printBddAsString(const int nodes, const sylvan::Bdd &bdd);
void printBigBddAsString(const int nodes, const sylvan::Bdd &bdd);
void printSingleRelationAsString(const sylvan::Bdd relation);
void printRelationsAsString(const std::deque<sylvan::Bdd> relations);
void printMap(std::map<std::string, int> map);

#endif //PRINT_H