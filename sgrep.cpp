#include <cstdlib>
#include <sstream>
#include <string>
#include <iostream>

#include "NFA.h"

using namespace std;

NFA R(istream &in, NFA &nfa1);
NFA *U(istream &in, NFA &nfa1);
NFA *C(istream &in, NFA &nfa1);
NFA *K(istream &in, NFA &nfa1);
NFA *L(istream &in, NFA &nfa1);
NFA *M(istream &in, NFA &nfa1);
CharSet S(istream &in);

//#define DEBUG

void match(istream &in, char c1){
	//compares chars, shouldn't conflict with Dr. Cochran's implementation
	char c2;
	in >> c2;
	if (c1 != c2){
		cout << "Error: Expected " << c1 << " and got " << c2 << endl; 
		exit(1);
	}
}

int main(int argc, char *argv[]){
	if (argc != 2) {
		cerr << "Usage: " << argv[0] << " regex" << endl;
		exit(1);
	}
    string line;
    stringstream ss(argv[1]);
//	cout << ss.str() << " string" << endl;	
	NFA nfa;
	nfa = R(ss,nfa); 
	while(getline(cin,line)){
                const int n = line.length();
                if (n > 0 && line[n-1] == '\n') // chomp
                        line.resize(n-1);
                if (nfa.accept(line))
                        cout << line << endl;
    }
    return 0;
}

/* Implementations may seem backwards */
NFA R(istream &in, NFA &nfa1) {
	bool beginAnchor = false;
	if (in.peek() == '^') {beginAnchor = true; match(in, '^');}
	NFA *newNfa = U(in, nfa1);
	bool endAnchor = false;
	if (in.peek() == '$') {endAnchor = true; match(in,'$');}
	// modify nfa based on (lack of) anchors
	if(!beginAnchor){
		
#ifdef DEBUG
	cout << "No starting anchor" << endl;
#endif
		NFA *any = (new NFA(NFA::WILD))->kleene();
                any->concat(newNfa);
                newNfa = any;
	}
	if(!endAnchor){
#ifdef DEBUG
		cout << "No end anchor" << endl;
#endif
		NFA *any = (new NFA(NFA::WILD))->kleene();
                newNfa->concat(any);
                delete any;
	}
	return *newNfa;
}

NFA *U(istream &in, NFA &nfa1) {
#ifdef DEBUG
	cout << "Got here! U" << endl;
#endif
	NFA *UnewNfa = C(in, nfa1);
	while (in.peek() == '|'){
		//cout << "Saw | " << endl;	
		match(in, '|');
		NFA temp; //needed a new nfa for this
		NFA *tempPtr = U(in,temp);
		UnewNfa = UnewNfa->union_(tempPtr);
	}
	return UnewNfa;
}

NFA *C(istream &in, NFA &nfa1) {
#ifdef DEBUG
	cout << "Got here! C" << endl;
#endif	
	NFA *CnewNfa = K(in,nfa1);
	//check test cases
	while(in.peek() != '\n' && in.peek() != ')' && in.peek() != '|' && in.peek() != '$' && !in.eof()) {
	//add eof
		(*CnewNfa).concat(K(in,nfa1));
	}
	return CnewNfa;
}

NFA *K(istream &in, NFA &nfa1) {
#ifdef DEBUG	
	cout << "Got here! K" << endl;
#endif	
	NFA *LnewNfa = L(in, nfa1);
	if(in.peek() == '*'){
		match(in, '*');
		LnewNfa = LnewNfa->kleene();
	} else if (in.peek() =='+'){
#ifdef DEBUG
		cout << "Saw + " << endl;
#endif
		match(in, '+');
		NFA *nfaCopy = LnewNfa->clone();
		LnewNfa->concat(nfaCopy->kleene());
		delete nfaCopy;
	} else if (in.peek() == '?'){
		match(in, '?');
		NFA *e = new NFA(NFA::EMPTYSTRING);
		LnewNfa = LnewNfa->union_(e);
		delete e;
	}
	return LnewNfa;
}

NFA *L(istream &in, NFA &nfa1) {
#ifdef DEBUG	
	cout << "Got here! L" << endl;
#endif
	if(in.peek() == '['){
		match(in, '[');
		if(in.peek() == '^'){
			match(in,'^');
			CharSet cc = S(in).complement();
			NFA *nfa = new NFA(cc);
			//match(in, ']');
			return nfa;
		} else {
			CharSet cc = S(in);
			NFA *nfa = new NFA(cc);
			//match(in, ']');
			return nfa;
		}	
	} else if(in.peek() == '('){
		match(in, '(');
		NFA *nfa = U(in, nfa1);
		match(in, ')');
		return nfa;
	}
	return M(in, nfa1);
}

NFA *M(istream &in, NFA &nfa1) {
#ifdef DEBUG	
	cout << "Got here! M" << endl;
#endif
	char c;
	if(in.peek() == '.'){
		match(in, '.');
		NFA *nfa = new NFA(NFA::WILD);
		return nfa;
	} else if(in.peek() == '\\'){
		match(in, '\\'); //escaped character
		in >> c;
		NFA *nfa =  new NFA(c);
		return nfa;
	} else {
		in >> c;
#ifdef DEBUG	
		cout << "Putting in this char in M: " << c << endl;
#endif
		NFA *nfa =  new NFA(c);
		return nfa;
	}
}

CharSet S(istream &in) {
#ifdef DEBUG	
	cout << "Got here! S" << endl;
#endif
	CharSet cs;
	while (in.peek() != ']'){
		//CharSet cs;
		char c;
		in >> c;
		if (c == '\\'){
			in >> c; //escaped char
		}
		if (in.peek() == '-'){
			match(in, '-');
			//realized I needed to check for
			//this if it is a char in the class
			if(in.peek() == ']'){
				cs.insert('-');
			} else {
				char c2;
				in >> c2;
#ifdef DEBUG		
				cout << "CharSet  " << c  << "-" << c2<< endl;
#endif	
				cs.insert(CharSet(c, c2));
			}
		} else {
#ifdef DEBUG
			cout << "inserted " << c << endl;
#endif
			cs = cs.insert(c);
		}
	}
	match(in,']');
	return cs;
}

