#include "testSystemSample.h"

#include <iostream>
#include "MyMath.hpp"
#include "TestSystem.hpp"

using namespace std;
using namespace MyMath;

void testSystemSample() {
	TestSystem sys;
	constexpr double Step = 1e-3;
	constexpr double Time = 10;
	constexpr size_t NumTimes = static_cast<int>(Time / Step);

	Vector<2> state{2.0, 0.0};

	cout << state.toString();
	for (size_t i = 0; i < NumTimes; i++) {
		auto input = sys.u(state);
		state += Step * sys.f(state, input);
		state = sys.normalize(state);
		cout << state.toString();
	}
}
