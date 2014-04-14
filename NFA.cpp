#include "NFA.h"
#include <set>

using namespace std;

void NFA::deepCopyEdges(const list<Edge*>& edges, 
			list<Edge*>& clone) {
  for (list<Edge*>::const_iterator iter = edges.begin(); 
       iter != edges.end(); ++iter) {
    const Edge *e = *iter;
    clone.push_back(new Edge(*e));
  }
}

void NFA::deleteEdges() {
  for (list<Edge*>::iterator iter = edges.begin(); 
       iter != edges.end(); ++iter) {
    Edge *e = *iter;
    delete e;
  }
  edges.clear();
}

void NFA::deepCopy(const NFA& other) {
  edges.clear();
  deepCopyEdges(other.edges, edges);
  start = other.start;
  final = other.final;
  maxState = other.maxState;
}

NFA *NFA::union_(const NFA *other) {
  incStates(other->maxState+1);
  list<Edge*> otherEdges;
  deepCopyEdges(other->edges, otherEdges);
  edges.splice(edges.end(), otherEdges);
  const int newStart = ++maxState;
  const int newFinal = ++maxState;
  edges.push_back(new Edge(new EpsilonLabel(), newStart, start));
  edges.push_back(new Edge(new EpsilonLabel(), newStart, other->start));
  edges.push_back(new Edge(new EpsilonLabel(), final, newFinal));
  edges.push_back(new Edge(new EpsilonLabel(), other->final, newFinal));
  start = newStart;
  final = newFinal;
  return this;
}

NFA *NFA::concat(const NFA *other) {
  incStates(other->maxState+1);
  list<Edge*> otherEdges;
  deepCopyEdges(other->edges, otherEdges);
  edges.splice(edges.end(), otherEdges);
  edges.push_back(new Edge(new EpsilonLabel(), final, other->start));
  final = other->final;
  return this;
}

NFA *NFA::kleene() {
  const int newStart = ++maxState;
  const int newFinal = ++maxState;
  edges.push_back(new Edge(new EpsilonLabel(), newStart, final));
  edges.push_back(new Edge(new EpsilonLabel(), final, start));
  edges.push_back(new Edge(new EpsilonLabel(), final, newFinal));
  start = newStart;
  final = newFinal;
  return this;
}

void NFA::incStates(int delta) {
  start += delta;
  final += delta;
  maxState += delta;
  for (list<Edge*>::iterator i = edges.begin(); i != edges.end(); ++i) {
    Edge *e = *i;
    e->src += delta;
    e->dst += delta;
  }
}

namespace {

  class StateSet {
    set<int> states;
  public:
    bool insert(int state) { // returns true if inserted (i.e., not in set)
      return states.insert(state).second;
    }
    bool contains(int state) const {
      return states.find(state) != states.end();
    }
    bool empty() const {return states.empty();}
  };
		
}

bool NFA::accept(const string& str) const {
  StateSet S;
  S.insert(start);

  unsigned i = 0;  // index of current char in 'str'

  while (true) {

    //
    // Augment current states S by following epsilon-transitions.
    //
    bool changed;
    do {
      changed = false;
      for (list<Edge*>::const_iterator iter = edges.begin(); 
	   iter != edges.end(); ++iter) {
	Edge *e = *iter;
	if (e->label->isEpsilon() && S.contains(e->src))
	  changed = changed || S.insert(e->dst);
      }
    } while (changed);

    if (i >= str.length()) break;

    //
    // Let T be set of all states reachable from S by
    // following edges matching c.
    //
    const int c = str[i++];
    StateSet T;
    for (list<Edge*>::const_iterator iter = edges.begin(); 
	 iter != edges.end(); ++iter) {
      Edge *e = *iter;
      if (e->label->match(c) && S.contains(e->src))
	T.insert(e->dst);
    }

    if (T.empty()) return false; // ended up nowhere

    S = T;
  }

  //
  // Accept iff final state in S.
  //
  return S.contains(final);
}

