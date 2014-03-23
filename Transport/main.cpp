#include <iostream>
#include <numeric>
#include "transportation.h"

using namespace std;

int main()
{
    freopen("input.txt", "r", stdin);

    int n;
    int m;

    cin >> n >> m;

    vvd a(m);
    vd b(m);
    vd c(n);

    for (int i = 0; i < m; ++i)
    {
        a[i].assign(n, 0);
        for (int j = 0; j < n; ++j)
        {
            cin >> a[i][j];
        }
        cin >> b[i];
    }

    for (int i = 0; i < n; ++i)
    {
        cin >> c[i];
    }

    if (accumulate(c.begin(), c.end(), 0) != accumulate(b.begin(), b.end(), 0))
    {
        cout << "Not balanced.\n";
        return 0;
    }

    auto plan = transportation::solve(a, b, c);

    double total_sum = 0;

    for (size_t i = 0; i < plan.size(); ++i)
    {
        for (size_t j = 0; j < plan[i].size(); ++j)
        {
            cout << plan[i][j] << "\t";
            total_sum += plan[i][j] * a[i][j];
        }
        cout << endl;
    }
    cout << total_sum << endl;

	plan = transportation::solveBySimplex(a, b, c);

	total_sum = 0;

	for (size_t i = 0; i < plan.size(); ++i)
	{
		for (size_t j = 0; j < plan[i].size(); ++j)
		{
			cout << plan[i][j] << "\t";
			total_sum += plan[i][j] * a[i][j];
		}
		cout << endl;
	}
	cout << total_sum << endl;

    return 0;
}