#include "simplex.h"

using namespace std;

namespace linearProgramming
{

namespace
{

bool isZero(long double d)
{
    return std::abs(d) < EPS;
}

}

void SlackForm::print() const
{
    std::cout << "A:\n";
    for (size_t i = 0; i < A.size(); ++i)
    {
        if (A[i].empty())
            continue;
        for (auto j : A[i])
        {
            if (j.second != 0)
                std::cout << "A[" << i << "][" << j.first << "]="<< j.second << " ";
        }
        std::cout << "\n";
    }
    std::cout << "b:\n";
    for (auto j : b)
    {
        std::cout << "b[" << j.first << "]=" << j.second << " ";
    }
    std::cout << "\n";
    std::cout << "c:\n";
    for (auto j : c)
    {
        if (j.second != 0)
            std::cout << "c[" << j.first << "]=" << j.second << " ";
    }
    std::cout << "\n";
    std::cout << "N: {";
    for (int i : nonbasicVars)
    {
        std::cout << i << ", ";
    }
    std::cout << "}\n";
    std::cout << "B: {";
    for (int i : basicVars)
    {
        std::cout << i << ", ";
    }
    std::cout << "}\n";
    std::cout << "v = " << v << "\n";
}

void SlackForm::pivot(int l, int e)
{
    vmd newA(A.size());
    map<int, long double> newB;
    map<int, long double> newC;
    newB[e] = b[l] / A[l][e];
    for (int j : nonbasicVars)
    {
        if (j == e)
            continue;
        newA[e][j] = A[l][j] / A[l][e];
    }
    newA[e][l] = 1 / A[l][e];
    for (int i : basicVars)
    {
        if (i == l)
            continue;
        newB[i] = b[i] - A[i][e] * newB[e];
        //if (isZero(newB[i])) ///////////////////////////// !!!!!!!!!!!
        //{
        //    newB[i] = 0;
        //}
        for (int j : nonbasicVars)
        {
            if (j == e)
                continue;
            newA[i][j] = A[i][j] - A[i][e] * newA[e][j];
            //if (isZero(newA[i][j])) ///////////////////////////// !!!!!!!!!!!
            //{
            //    newA[i][j] = 0;
            //}
        }
        newA[i][l] = -A[i][e] * newA[e][l];
    }

    v += c[e] * newB[e];
    for (int j : nonbasicVars)
    {
        if (j == e)
            continue;
        newC[j] = c[j] - c[e] * newA[e][j];
        //if (isZero(newC[j])) ///////////////////////////// !!!!!!!!!!!
        //{
        //    newC[j] = 0;
        //}
    }
    newC[l] = - c[e] * newA[e][l];

    basicVars.insert(e);
    basicVars.erase(l);
    nonbasicVars.insert(l);
    nonbasicVars.erase(e);
    A = newA;
    b = newB;
    c = newC;
}

int SlackForm::getPositiveC()
{
    for (auto i : c)
    {
        if (i.second > 0)
            return i.first;
    }
    return -1;
}

bool SlackForm::isBounded(int e, int &l)
{
    long double INF = 1e100;
    long double delta = INF;
    for (int i : basicVars)
    {
        long double tmp = (A[i][e] > 0) ? (b[i] / A[i][e]) : INF;
        if (tmp < delta)
        {
            delta = tmp;
            l = i;
        }
    }
    return delta == INF;
}

SlackForm::SlackForm(const vvd& input_a, const vd& input_b, const vd& input_c)
{
    SlackForm condition;
    n = input_c.size();
    m = input_b.size();
    A.assign(n + m + 1, md());
    for (size_t i = 0; i < m; ++i)
    {
        for (size_t j = 0; j < n; ++j)
        {
            A[i + n + 1][j + 1] = input_a[i][j];
        }
    }

    for (size_t i = 0; i < n; ++i)
    {
        c[i + 1] = input_c[i];
    }
    for (size_t i = 0; i < m; ++i)
    {
        b[n + 1 + i] = input_b[i];
    }
    v = 0;
    for (size_t i = 1; i <= n; ++i)
        nonbasicVars.insert(i);
    for (size_t i = n + 1; i <= m + n; ++i)
        basicVars.insert(i);
    

    int k = min_element(input_b.begin(), input_b.end()) - input_b.begin() + 1;
    if (input_b[k - 1] >= 0)
    {
        return;
    }
    nonbasicVars.insert(0);
    for (size_t i : basicVars)
    {
        A[i][0] = -1;
    }
    c.clear();
    c[0] = -1;
    int l = n + k;
    
    
    pivot(l, 0);
    int e;
    while ((e = getPositiveC()) != -1)
    {
        if (isBounded(e, l))
            throw simplexException("Unbounded.");
        else
            pivot(l, e);
    }
    if (checkZero())
    {
        throw simplexException("Undecidable problem");
    }
    else
    {
        v = 0;
        c.clear();
        for (auto& p : A)
        {
            p.erase(0);
        }
        for (size_t i = 0; i < n; ++i)
        {
            c[i + 1] = input_c[i];
        }
        for (int i : basicVars)
        {
            v += b[i] * c[i];
            for (auto p : A[i])
            {
                if (p.first)
                    c[p.first] -= c[i] * p.second;
            }
            c[i] = 0;
        }
        nonbasicVars.erase(0);
        basicVars.erase(0);
        deleteNulls();
    }
}

bool SlackForm::checkZero() const
{
    if (basicVars.count(0))
    {
        return !isZero(b.at(0));
    }
    return false;
}

vd SlackForm::getResult() const
{
    vd result(n);
    vd x;
    for (size_t i = 1; i <= m + n; ++i)
    {
        x.push_back(basicVars.count(i) ? b.at(i) : 0.0);
    }
    for (size_t i = 1; i <= n; ++i)
    {
        if (basicVars.count(i))
        {
            result[i - 1] = b.at(i);
        }
        else
        {
            for (auto j : A[i])
            {
                result[i - 1] += j.second * x[j.first - 1];
            }
        }
    }
    return result;
}

void SlackForm::deleteNulls()
{
    for (auto& x : c)
        if (isZero(x.second))
            x.second = 0;
    for (auto& x : b)
        if (isZero(x.second))
            x.second = 0;
    for (auto& x : A)
    {
        for (auto& y : x)
        if (isZero(y.second))
            y.second = 0;
    }
}

SlackForm simplex(const vvd& a, const vd& b, const vd& c)
{
    SlackForm conditions(a, b, c);
    
    /*cout << "----------INPUT---------\n";
    conditions.print();
    cout << "----------END---------\n";*/
    
    int e;
    int l;
    while ((e = conditions.getPositiveC()) != -1)
    {
        if (conditions.isBounded(e, l))
            throw simplexException("Unbounded.");
        else
            conditions.pivot(l, e);
    }

    /*cout << "----------SIMPLEX---------\n";
    conditions.print();
    cout << "----------END---------\n";*/

    return conditions;
}

SlackForm allVertexesCheck(const vvd& a, const vd& b, const vd& c)
{
    SlackForm condition(a, b, c);
    SlackForm res(a, b, c);
    recFunc(condition, res);


    /*cout << "----------ALL V---------\n";
    res.print();
    cout << "----------END---------\n";*/

    return res;
}

void recFunc(SlackForm& cond, SlackForm& best_cond)
{
    static map<set<int>, bool> visited;
    if (visited[cond.getN()]) return;
    visited[cond.getN()] = true;

    if (cond.checkB() && (cond.getV() > best_cond.getV()))
    {
        best_cond = cond;
    }
    for (size_t i = 0; i < cond.A.size(); ++i)
    {
        for (auto j : cond.A[i])
        {
            if (!isZero(std::abs(j.second)))
            {
                SlackForm tmp = cond;
                tmp.pivot(i, j.first);
                tmp.deleteNulls();
                if (!visited[tmp.getN()])
                {
                    recFunc(tmp, best_cond);
                }
            }
        }
    }
}

}





























