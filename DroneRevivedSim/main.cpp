#include <iostream>
#include "DroneSystem.hpp"
#include "Simulator.hpp"
#include "MatlabUtility.hpp"
#include "Algorithms.hpp"
#include "SystemPlotter.hpp"
#include <boost/format.hpp>

using namespace std;
using namespace MyMath;
using namespace Drone;

bool checkQ0Exceeds1(const Simulator<DroneSystem>::Data &data)
{
	for (const auto &v : data) {
		if (v.state[0] > 1.0) return true;
	}
	return false;
}

int main()
{
	MatlabUtility::PlotOptions os;
	os.fontName("Times New Roman")
		.fontSize(12)
		.wholeLineProperties({{"LineWidth", "1"}})
		.prints(true)
		.size(13, 10)
		.fileType("pdf")
		.xlim(0, 12)
		.legend("q_%d");
	MatlabUtility::PlotOptions oi = os;
	oi.legend({"tilde \\mu_1", "\\mu_2"});

	{
		auto system = make_shared<DroneSystem>(MyMath::Vector<2>{0.0, 0.0});
		Simulator<DroneSystem> simulator(system, Vector<6>{0.1, 0.86, 0.0, 0.0, 0.0, 0.0},
			createRungeKutta<DroneSystem>(1e-3), &cout, true, 1e-2);
		simulator.simulateTo(12);
		auto s = (boost::format("_%03d_%03d_o") % int(0.1*100) % int(0.86*100)).str();
		auto mu = make_shared<MatlabUtility>("result" + s + ".m");
		auto data = make_shared<Simulator<DroneSystem>::Data>(simulator.data());
		SystemPlotter<DroneSystem> pl(mu, data);
		pl.plotState(os, "state" + s + ".csv", "state");
		pl.plotInput(oi, "input" + s + ".csv", "input");
	}
	{
		auto system = make_shared<DroneSystem>(MyMath::Vector<2>{1.0, 1.0});
		Simulator<DroneSystem> simulator(system, Vector<6>{0.1, 0.86, 0.0, 0.0, 0.0, 0.0},
			createRungeKutta<DroneSystem>(1e-5), &cout, true, 1e-2);
		simulator.simulateTo(12);
		auto s = (boost::format("_%03d_%03d_r") % int(0.1*100) % int(0.86*100)).str();
		auto mu = make_shared<MatlabUtility>("result" + s + ".m");
		auto data = make_shared<Simulator<DroneSystem>::Data>(simulator.data());
		SystemPlotter<DroneSystem> pl(mu, data);
		pl.plotState(os, "state" + s + ".csv", "state");
		pl.plotInput(oi, "input" + s + ".csv", "input");
	}
	return 0;
}
