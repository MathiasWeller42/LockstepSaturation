#ifndef PRINT_H
#define PRINT_H

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

void printBdd(const sylvan::Bdd &bdd);

void printBddAsString(const int nodes, const sylvan::Bdd &bdd);
std::list<std::string> __printBddAsString(const std::string &currentPath, const sylvan::Bdd &bdd);

void printBddAsSingleString(const int nodes, const sylvan::Bdd &bdd);
void printBddListAsString(const int nodes, const std::list<sylvan::Bdd> bdds);
void printSingleRelationAsString(const sylvan::Bdd relation);

void printRelationsAsString(const std::deque<sylvan::Bdd> relations);
std::list<std::pair<std::string, std::string>> __printRelationsAsString(std::pair<std::string, std::string> currentPath, const sylvan::Bdd &bdd);

void printMap(std::map<std::string, int> map);

long long countNodes(int numVars, const sylvan::Bdd &bdd);
long long __countNodes(long long prevTop, int maxVar, const sylvan::Bdd &bdd);

void printBdd(const sylvan::Bdd &bdd);
void __printBdd(const std::string &prefix, const sylvan::Bdd &bdd, bool isLeft);

#endif //PRINT_H