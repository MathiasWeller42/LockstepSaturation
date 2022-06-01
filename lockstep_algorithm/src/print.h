#ifndef PRINT_H
#define PRINT_H

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

/*
This contains:
- methods for printing BDDs in different representations (tree structure and compact 00xx1x0 form)
- counting nodes
- printing a map
*/



//This prints in a tree-like form
void printBdd(const sylvan::Bdd &bdd);
void __printBdd(const std::string &prefix, const sylvan::Bdd &bdd, bool isLeft);

//These print BDDs in the form 00xx11xx01
void printBddAsString(const int nodes, const sylvan::Bdd &bdd);
std::list<std::string> __printBddAsString(const std::string &currentPath, const sylvan::Bdd &bdd);

void printBddAsSingleString(const int nodes, const sylvan::Bdd &bdd);
void printBddListAsString(const int nodes, const std::list<sylvan::Bdd> bdds);
void printSingleRelationAsString(const sylvan::Bdd relation);

//This works on relations which have uneven vars
void printRelationsAsString(const std::deque<sylvan::Bdd> relations);
std::list<std::pair<std::string, std::string>> __printRelationsAsString(std::pair<std::string, std::string> currentPath, const sylvan::Bdd &bdd);


//Prints a map with key, value pairs on each line
void printMap(std::map<std::string, int> map);


//Node count methods for even var BDDs
long long countNodes(int numVars, const sylvan::Bdd &bdd);
long long __countNodes(long long prevTop, int maxVar, const sylvan::Bdd &bdd);


#endif //PRINT_H