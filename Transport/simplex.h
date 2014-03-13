#ifndef SIMPLEX_H
#define SIMPLEX_H

#include <vector>
#include <cmath>
#include <algorithm>
#include <exception>
#include <iostream>
#include <set>
#include <map>
#include <iterator>

using namespace std;

typedef vector<char> vc;
typedef vector<long double> vd;
typedef map<int, long double> md;
typedef vector< vector<long double> > vvd;
typedef vector< map<int, long double> > vmd;

const long double EPS = 1e-14;

namespace linearProgramming
{
using std::vector;
using std::map;
using std::set;

struct simplexException : std::exception
{
    const char *msg;
    simplexException(const char* msg = "no description.") : msg(msg) {}
    const char* what() {return msg;}
};

class SlackForm
{
public:
    void pivot(int l, int e);

    SlackForm(const vvd& a, const vd& b, const vd& c);

    bool isBounded(int e, int &l);
    bool checkB() const { for (auto a : b) if (a.second < 0) return false; return true; }
    bool checkZero() const;

    void print() const;

    int getPositiveC();
    set<int> getB() { return basicVars; }
    set<int> getN() { return nonbasicVars; }
    long double getV() const { return v; }
    vd getResult() const;

    void deleteNulls();

private:
    SlackForm() {};

    set<int> basicVars;
    set<int> nonbasicVars;
    vmd A;
    map<int, long double> b;
    map<int, long double> c;
    long double v;
    size_t n;
    size_t m;

    friend void recFunc(SlackForm& cond, SlackForm& best_cond);

};

SlackForm simplex(const vvd& a, const vd& b, const vd& c);
SlackForm allVertexesCheck(const vvd& a, const vd& b, const vd& c);

}

#endif // SIMPLEX_H
