#include <iostream>
#include <boost/range/adaptor/transformed.hpp>
#include "DroneSystem.hpp"
#include "Simulator.hpp"
#include "MatlabUtility.hpp"
#include "Algorithms.hpp"

using namespace std;
using namespace MyMath;
using namespace Drone;

Vector<7> getState(const Record<DroneSystem> &r)
{
	Vector<7> ret;
	ret[0] = r.time;
	for (size_t i = 0; i < 6; i++) {
		ret[i+1] = r.state[i];
	}
	return  ret;
}

Vector<3> getInput(const Record<DroneSystem> &r)
{
	Vector<3> ret;
	ret[0] = r.time;
	for (size_t i = 0; i < 2; i++) {
		ret[i+1] = r.input[i];
	}
	return ret;
}

int main() {
	auto system = make_shared<DroneSystem>(MyMath::Vector<2>{4.0, 4.0});
	Simulator<DroneSystem> simulator(system, Vector<6>{0.0, -0.59, 0.0, 0.1675, -0.753, -0.15}, createRungeKutta<DroneSystem>(1e-1));
	simulator.simulateTo(10);

	MatlabUtility mu("result.m");
	mu.plot("state.csv", "state", simulator.data() | boost::adaptors::transformed(getState));
	mu.plot("input.csv", "input", simulator.data() | boost::adaptors::transformed(getInput));

	return 0;
}
