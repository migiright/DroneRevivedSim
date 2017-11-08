#include "Math2Sample.h"
#include <iostream>
#include <iomanip>
#include "Math2.hpp"

using namespace std;

void calcMinimumSample()
{
	cout << scientific << setprecision(20);
	auto x = calcMinimumByCg([](const MyMath::Vector<3> &x)
	{
		return MyMath::Vector<3>{2*(x[0]-1)+(x[1]-2), 2*(x[1]-2)+(x[0]-1), x[2]-10};
	}, MyMath::Vector<3>{5.0, 4.0, 3.0}, 1e-5, 1e-5);
	cout << x[0] << ", " << x[1] << ", " << x[2] << endl;
}
