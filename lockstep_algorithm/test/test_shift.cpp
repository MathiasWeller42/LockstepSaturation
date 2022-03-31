#include <iostream>
#include <list>
#include <deque>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "../src/lockstep.h"
#include "../src/utilities.h"
#include "../src/graph_examples.h"
#include "../src/petriTranslation.h"

bool testShiftOneNode() {
    sylvan::Bdd nodes = oneNodeGraph().nodes;
    sylvan::Bdd expected = nithvar(1);
    return shiftBdd(nodes) == expected;
}

bool testShiftTwoNodes() {
    sylvan::Bdd nodes = twoNodeGraphOneSCC().nodes;
    sylvan::Bdd expected = leaf_true();
    return shiftBdd(nodes) == expected;
}

bool testShiftFourNodes() {
    sylvan::Bdd nodes = fourNodesOneRelation().nodes;
    sylvan::Bdd expected = leaf_true().Or(nithvar(3)).Or(nithvar(1));
    return shiftBdd(nodes) == expected;
}

bool testShiftNineNodes() {
    sylvan::Bdd nodes = graphExample1oneRel().nodes;
    sylvan::Bdd expected = leaf_false().Or(nithvar(7)).Or(ithvar(7).And(nithvar(5)).And(nithvar(3)).And(nithvar(1)));
    return shiftBdd(nodes) == expected;
}







