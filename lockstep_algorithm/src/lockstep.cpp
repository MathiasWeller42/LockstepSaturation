#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "lockstep.h"

bool testFunction() {
  sylvan::Bdd t = leaf_true();
  sylvan::Bdd f = leaf_false();
  return t.And(f).isZero();
}

sylvan::Bdd testFunction2() {
  sylvan::Bdd x42 = ithvar(42);
  sylvan::Bdd x69 = ithvar(69);
  
  return x42.Or(x69);
}



