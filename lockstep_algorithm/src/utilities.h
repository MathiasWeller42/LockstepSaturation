#ifndef UTILITIES_H
#define UTILITIES_H

//from adapter.h in bdd-benchmarks (tak til Steffan ;) )
  inline sylvan::Bdd leaf_true()
  { return sylvan::Bdd::bddOne(); }

  inline sylvan::Bdd leaf_false()
  { return sylvan::Bdd::bddZero(); }

  inline sylvan::Bdd ithvar(int label)
  { return sylvan::Bdd::bddVar(label); }

  inline sylvan::Bdd nithvar(int label)
  { return ~sylvan::Bdd::bddVar(label); }

  inline sylvan::Bdd ite(const sylvan::Bdd &f, const sylvan::Bdd &g, const sylvan::Bdd &h)
  { return f.Ite(g,h); }

  inline sylvan::Bdd negate(const sylvan::Bdd &b)
  { return ~b; }

  inline sylvan::Bdd exists(const sylvan::Bdd &b, int label)
  { return b.ExistAbstract(sylvan::Bdd::bddVar(label)); }