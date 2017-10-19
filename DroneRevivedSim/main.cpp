#include <iostream>
#include "MyMath.hpp"
#include "Simulator.hpp"
#include "TestSystem.hpp"
#include "Algorithms.hpp"

using namespace std;
using namespace MyMath;

int main() {
	auto system = make_shared<TestSystem>();
	Simulator<TestSystem> simulator(system, Vector<2>{2.0, 0.0}, createRungeKutta<TestSystem>(0.2));
	simulator.simulateTo(10);
	auto &data = simulator.data();
	for(auto &record : data) {
		cout << record.time << " " << record.state[0] << " " << record.state[1] << " " << record.input[0] << endl;
	}
	return 0;
}
