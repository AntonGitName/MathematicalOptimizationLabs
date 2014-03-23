#include <iostream>
#include <fstream>
#include <string>
#include "simplex.h"

void printTask(const vvd& a, const vd& b, const vd& c, const std::string& filename = "task.txt")
{
    size_t n = c.size();
    size_t m = a.size();
    std::ofstream out(filename);
    for (size_t i = 0; i < n; ++i)
    {
        auto& t = c[i];
        if (std::abs(t) < EPS)
        {
            continue;
        }
        if (t > 0)
        {
            out << "+\t" << t;
        }
        if (t < 0)
        {
            out << "-\t" << -t;
        }
        out << " * x" << i + 1 << "\t";
    }
    out << "-----> MAX\n";
    for (size_t i = 0; i < m; ++i)
    {
        out << "x" << n + i + 1 << "\t=\t" << b[i] << "\t";
        for (size_t j = 0; j < n; ++j)
        {
            auto t = -a[i][j];
            if (std::abs(t) < EPS)
            {
                continue;
            }
            if (t > 0)
            {
                out << "+\t" << t;
            }
            if (t < 0)
            {
                out << "-\t" << -t;
            }
            out << " * x" << j + 1 << "\t";
        }
        out << endl;
    }
}

void getDualProblem(const vvd& a, vd& b, vd& c, vvd& dual_a, vd& dual_b, vd& dual_c)
{
    for (auto x : b)
    {
        dual_c.push_back(-x);
    }
    for (auto x : c)
    {
        dual_b.push_back(-x);
    }
    dual_a.assign(a[0].size(), vd());
    for (size_t i = 0; i < a.size(); ++i)
    {
        for (size_t j = 0; j < a[i].size(); ++j)
        {
            dual_a[j].push_back(-a[i][j]);
        }
    }
}

void getCanonicalMatrix(vvd& a, vd& b, vd& c, const vc& sign, const vc& type)
{
    vvd a1;
    vd b1;
    vd c1 = c;

    for (size_t i = 0; i < type.size(); ++i)
    {
        if (type[i] == '<')
        {
            a1.push_back(a[i]);
            b1.push_back(b[i]);
        }
        if (type[i] == '>')
        {
            a1.push_back(a[i]);
            for (auto& j : a1.back())
            {
                j = -j;
            }
            b1.push_back(-b[i]);
        }
        if (type[i] == '=')
        {
            a1.push_back(a[i]);
            b1.push_back(b[i]); 
            a1.push_back(a[i]);
            for (auto& j : a1.back())
            {
                j = -j;
            }
            b1.push_back(-b[i]);
        }
    }

    for (int i = (int)sign.size() - 1; i >= 0; --i)
    {
        if (sign[i] == '0')
        {
            for (auto& v : a1)
            {
                v.insert(v.begin() + i + 1, -v[i]);
            }
            c1.insert(c1.begin() + i + 1, -c1[i]);
        }
    }

    a = a1;
    b = b1;
    c = c1;    
}

void printResult(const linearProgramming::SlackForm& condition, const vc& sign, bool f = true)
{
    auto vars = condition.getResult();
    for (size_t i = 0, k = 0; i < sign.size(); ++i)
    {
        if (sign[i] == '+')
        {
            cout << vars[k++];
        }
        else
        {
            cout << vars[k] - vars[k + 1] << endl;
        }
        cout << "\t";
    }
    cout << endl;
	if (f)
	{
		cout << "MAX = " << condition.getV() << endl;
	}
	else
	{
		cout << "MAX = " << -condition.getV() << endl;
	}
}

int main()
{
    /// INPUT START
    freopen("input.txt", "r", stdin);
    freopen("debug.txt", "w", stderr);
    int n;
    int m;
    //cout << "input conditions:\n";
    cin >> n >> m;

    vvd a(m, vd(n));
    vd b(m);
    vc type(m);
    vc sign(n);

    for (int i = 0; i < m; ++i)
    {
        cin >> type[i];
        for (int j = 0; j < n; ++j)
            cin >> a[i][j];
        cin >> b[i];
    }

    for (int i = 0; i < n; ++i)
    {
        cin >> sign[i];
    }

    vd c(n);
    for (int i = 0; i < n; ++i)
    {
        cin >> c[i];
    }
    /// INPUT END

    vvd dual_a;
    vd dual_b;
    vd dual_c;

    getCanonicalMatrix(a, b, c, sign, type);
    printTask(a, b, c);

    getDualProblem(a, b, c, dual_a, dual_b, dual_c);
    printTask(dual_a, dual_b, dual_c, "dual_task.txt");

    cout << "Simplex\n";
    printResult(linearProgramming::simplex(a, b, c), sign);
    cout << "\nAll vertexes\n";
    printResult(linearProgramming::allVertexesCheck(a, b, c), sign);
    cout << "\nDual problem simplex\n";
	printResult(linearProgramming::simplex(dual_a, dual_b, dual_c), sign, false);


    return 0;
}
