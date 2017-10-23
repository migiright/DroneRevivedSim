#include <iostream>
#include "MatlabUtilitySample.h"
#include "Math2.hpp"

using namespace std;
using namespace MyMath;

double v(Vector<3> x)
{
	return x[0] * x[1]*x[1] * x[2]*x[2]*x[2];
}

auto f(Vector<3> x)
{
	return Vector<3>{x[0], x[1], x[2]};
}

auto g(Vector<3> x)
{
	return Matrix<3, 2>{x[0], x[1], x[1], x[2], x[2], x[0]};
}

int main() {
	cout << lie(f, v, Vector<3>{2.0, 3.0, 5.0}) << endl;
	cout << lie(g, v, Vector<3>{2.0, 3.0, 5.0}).toString() << endl;
	return 0;
}
