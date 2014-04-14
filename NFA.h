#ifndef NFA_H
#define NFA_H

#include <bitset>
#include <list>
#include <string>

class Label {
public:
  virtual bool match(int c) const {return false;}
  virtual bool isEpsilon() const {return false;}
};

class EpsilonLabel : public Label {
public:
  virtual bool isEpsilon() const {return true;}
};

class CharLabel : public Label {
  int ch;
public:
  CharLabel(int c) : ch(c) {}
  virtual bool match(int c) const {return c == ch;}
};

class WildLabel : public Label {
public:
  virtual bool match(int c) const {return c != '\n';}
};

class CharSet {
  std::bitset<256> chars; // 8-bit ASCII
public:
  CharSet(){};
  explicit CharSet(int c) {chars.set(c);}
  CharSet(int a, int b) {
    const int lo = std::min(a,b);
    const int hi = std::max(a,b);
    for (int c = lo; c <= hi; chars.set(c++))
      ;
  }
  bool contains(int c) const {return chars.test(c);}
  CharSet& insert(int c) {chars.set(c); return *this;}
  CharSet& insert(const CharSet& other) {chars |= other.chars; return *this;}
  CharSet& complement() {chars.flip(); return *this;}
};

class CharSetLabel : public Label {
  const CharSet cset;
public:
  CharSetLabel(const CharSet& s) : cset(s) {}
  virtual bool match(int c) const {return cset.contains(c);}
};

class NFA {
  struct Edge {
    const Label *label;
    int src, dst;
    Edge(const Label *l, int s, int d) : label(l), src(s), dst(d) {}
  };
  int start;
  int final;
  int maxState;
  std::list<Edge*> edges;
  //NFA() : start(0), final(0), maxState(0), edges() {}
public:
  NFA() : start(0), final(0), maxState(0), edges() {}
  static const int WILD = 257;
  static const int EMPTYSTRING = 258;
  explicit NFA(int c) {
    start = 0;
    final = 1;
    maxState = 1;
    if (c == WILD)
      edges.push_back(new Edge(new WildLabel(), 0, 1));
    else if (c == EMPTYSTRING)
      edges.push_back(new Edge(new EpsilonLabel(), 0, 1));
    else
      edges.push_back(new Edge(new CharLabel(c), 0, 1));
  }
  NFA(const CharSet& s) {
    start = 0;
    final = 1;
    maxState = 1;
    Label *label = new CharSetLabel(s);
    Edge *edge = new Edge(label, 0, 1);
    edges.push_back(edge);
  }
  NFA(const NFA& other) {deepCopy(other);} // copy-constructor
  NFA& operator=(const NFA& other) {       // copy-assignment
    if (this != &other) {
      deleteEdges();
      deepCopy(other);
    }
    return *this;
  }
  ~NFA() {deleteEdges();}
  NFA *clone() const {return new NFA(*this);}
  NFA *union_(const NFA *other);
  NFA *concat(const NFA *other);
  NFA *kleene();
  bool accept(const std::string& s) const;
private:
  void incStates(int delta);
  void deepCopy(const NFA& other);
  static void deepCopyEdges(const std::list<Edge*>& edges, 
			    std::list<Edge*>& clone);
  void deleteEdges();
};

#endif
