#include "transportation.h"

namespace transportation
{
    
bool func(pair<int, int> p, const set< pair<int, int> >& base_cells,
        pair<int, int> cell, map<pair<int, int>, bool>& visited,
        map<pair<int, int>, pair<int, int> >& prev, bool turn, bool flag = true)
{
    // debug output
    /*auto h = p;
    while (h != cell)
    {
        cout << "(" << h.first << "," << h.second << ")\t";
        h = prev[h];
    }
    cout << "(" << h.first << "," << h.second << ")\n";*/
    // -----------

    if (p == cell && flag)
    {
        return true;
    }
    int x = p.first;
    int y = p.second;
    for (auto it = base_cells.begin(); it != base_cells.end(); ++it)
    {
        if (visited[*it] || *it == p)
        {
            continue;
        }
        if (turn)
        {
            if (it->second == y)
            {
                prev[*it] = p;
                visited[*it] = true;
                if (func(*it, base_cells, cell, visited, prev, !turn))
                {
                    return true;
                }
                visited[*it] = false;
            }
        }
        else
        {
            if (it->first == x)
            {
                prev[*it] = p;
                visited[*it] = true;
                if (func(*it, base_cells, cell, visited, prev, !turn))
                {
                    return true;
                }
                visited[*it] = false;
            }
        }
    }
    return false;
}

    vector< pair<int, int> > findCircle(set< pair<int, int> > base_cells, pair<int, int> cell)
    {
        map<pair<int, int>, bool> visited;
        map<pair<int, int>, pair<int, int> > prev;

        base_cells.insert(cell);

        func(cell, base_cells, cell, visited, prev, true, false);

        vector< pair<int, int> > circle;
        auto p = cell;
        while (prev[p] != cell)
        {
            circle.push_back(p);
            p = prev[p];
        }
        circle.push_back(p);
        //reverse(circle.begin(), circle.end());
        return circle;
    }

    vvd firstPlan(const vvd& a, const vd& b, const vd& c)
    {
        double the_max = *max_element(a[0].begin(), a[0].end());
        vvd the_plan(a);

        for (auto& v : the_plan)
        {
            for (auto& d : v)
            {
                d = 0;
            }
        }
        for (size_t i = 0; i < a.size(); ++i)
        {
            double t = *max_element(a[i].begin(), a[i].end());
            the_max = max(the_max, t);
        }

        map<int, double> fabrics;
        map<int, double> shops;
        
        for (size_t i = 0; i < b.size(); ++i)
        {
            fabrics[i] = b[i];
        }
        for (size_t i = 0; i < c.size(); ++i)
        {
            shops[i] = c[i];
        }

        while (!shops.empty())
        {
            int x;
            int y;
            double the_min = the_max;
            for (auto i : fabrics)
            {
                for (auto j : shops)
                {
                    if (a[i.first][j.first] <= the_min)
                    {
                        x = i.first;
                        y = j.first;
                        the_min = a[i.first][j.first];
                    }
                }
            }
            double z = min(fabrics[x], shops[y]);
            if ((fabrics[x] -= z) == 0)
            {
                fabrics.erase(x);
            }
            if ((shops[y] -= z) == 0)
            {
                shops.erase(y);
            }
            the_plan[x][y] = z;
        }
        return the_plan;
    }
    
    vvd solve(const vvd& a, const vd& b, const vd& c)
    {
        size_t n = b.size();
        size_t m = c.size();

        vvd plan = firstPlan(a, b, c);
        bool isOptimal = false;

        set< pair<int, int> > base_cells;
        for (size_t i = 0; i < n; ++i)
        {
            for (size_t j = 0; j < m; ++j)
            {
                if (plan[i][j] != 0)
                {
                    base_cells.insert(make_pair(i, j));
                }
            }
        }

        while (!isOptimal)
        {

            vd v(n);
            vd u(m);
            set<int> solved_v;
            set<int> solved_u;
            
            set< pair<int, int> > base_cells_tmp = base_cells;

            solved_v.insert(base_cells.begin()->first);
            while (!base_cells_tmp.empty())
            {
                auto it = base_cells_tmp.begin();
                for (; it != base_cells_tmp.end(); ++it)
                {
                    if (solved_v.count(it->first))
                    {
                        u[it->second] = a[it->first][it->second] - v[it->first];
                        solved_u.insert(it->second);
                        break;
                    }
                    if (solved_u.count(it->second))
                    {
                        v[it->first] = a[it->first][it->second] - u[it->second];
                        solved_v.insert(it->first);
                        break;
                    }
                }
                base_cells_tmp.erase(it);
            }

            // check plan
            isOptimal = true;
            for (size_t i = 0; (i < n) && isOptimal; ++i)
            {
                for (size_t j = 0; (j < m) && isOptimal; ++j)
                {
                    if (a[i][j] < v[i] + u[j])
                    {
                        isOptimal = false;
                    }
                }
            }

            if (isOptimal)
            {
                break;
            }

            int x = 0;
            int y = 0;
            for (size_t i = 0; i < n; ++i)
            {
                for (size_t j = 0; j < m; ++j)
                {
                    if (v[x] + u[y] - a[x][y] < v[i] + u[j] - a[i][j])
                    {
                        x = i;
                        y = j;
                    }
                }
            }

            // find circle;
            auto circle = findCircle(base_cells, make_pair(x, y));
            base_cells.insert(make_pair(x, y));

            x = circle[1].first;
            y = circle[1].second;
            for (size_t k = 3; k < circle.size(); k += 2)
            {
                int x1 = circle[k].first;
                int y1 = circle[k].second;
                if (plan[x][y] > plan[x1][y1])
                {
                    x = x1;
                    y = y1;
                }
            }
            double delta = plan[x][y];
            for (size_t k = 0; k < circle.size(); ++k)
            {
                int x1 = circle[k].first;
                int y1 = circle[k].second;
                double potomy = delta * ((k & 1) ? -1 : +1);
                plan[x1][y1] += potomy;
            }

            base_cells.erase(make_pair(x, y));

        }

        return plan;
    }

	vvd solveBySimplex(const vvd& a, const vd& b, const vd& c)
	{
		size_t n = b.size() * c.size();
		size_t m = b.size() + c.size();
		vvd simplexA(m, vd(n));
		vd simplexB;
		vd simplexC(n);

		for (size_t i = 0; i < a.size(); ++i)
		{
			for (size_t j = 0; j < a[i].size(); ++j)
			{
				simplexC[j + i * c.size()] = -a[i][j];
			}
		}

		for (auto x : c)
		{
			simplexB.push_back(-x);
		}

		for (auto x : b)
		{
			simplexB.push_back(-x);
		}

		for (size_t i = 0; i < c.size(); ++i)
		{
			for (size_t j = 0; j < b.size(); ++j)
			{
				simplexA[i][j + i * c.size()] = 1;
			}
		}

		for (size_t i = 0; i < b.size(); ++i)
		{
			for (size_t j = 0; j < c.size(); ++j)
			{
				simplexA[i + c.size()][j + i * c.size()] = 1;
			}
		}

		linearProgramming::SlackForm form = linearProgramming::simplex(simplexA, simplexB, simplexC);
		vd res = form.getResult();
		vvd matrix = a;
		for (size_t i = 0; i < a.size(); ++i)
		{
			for (size_t j = 0; j < a[i].size(); ++j)
			{
				matrix[i][j] = res[j + i * c.size()];
			}
		}

		return matrix;
	}

	
}