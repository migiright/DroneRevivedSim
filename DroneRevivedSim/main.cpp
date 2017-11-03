#include <iostream>
#include "DroneSystem.hpp"
#include "Simulator.hpp"
#include "MatlabUtility.hpp"
#include "Algorithms.hpp"
#include "SystemPlotter.hpp"

using namespace std;
using namespace MyMath;
using namespace Drone;

int main()
{
	auto system = make_shared<DroneSystem>(MyMath::Vector<2>{1.0, 1.0});
	Simulator<DroneSystem> simulator(system, Vector<6>{0.9, 0.0, 0.0, 0.0, 0.0, 0.0},
		createRungeKutta<DroneSystem>(1e-5), &cout, true, 1e-1);
	simulator.simulateTo(10);

	auto mu = make_shared<MatlabUtility>("result.m");
	auto data = make_shared<Simulator<DroneSystem>::Data>(simulator.data());

	SystemPlotter<DroneSystem> pl(mu, data);
	pl.plotState();
	pl.plotInput();
	return 0;
}
