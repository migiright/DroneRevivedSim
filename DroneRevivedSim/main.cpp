#include <iostream>
#include "MatlabUtilitySample.h"
#include "Math2.hpp"

using namespace std;

int main() {
	auto t = calcMinimum([](const MyMath::Vector<2> &x) { return sin(x[0])*cos(x[1]); }, MyMath::Vector<2>{1.0, 1.0});
	MyMath::Vector<2> x = std::get<0>(t);
	double v = std::get<1>(t);
	printf("%.20e, %.20e, %.20e\n", x[0], x[1], v);
	return 0;
}
