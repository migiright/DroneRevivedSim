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
	for (auto x0 = 0.0; x0 <= 1.01; x0 += 0.05) {
		auto x1 = 0.0;
		for (; x1 <= 1.01; x1 += 0.05) {
			auto system = make_shared<DroneSystem>(MyMath::Vector<2>{0.0, 0.0});
			Simulator<DroneSystem> simulator(system, Vector<6>{x0, x1, 0.0, 0.0, 0.0, 0.0},
				createRungeKutta<DroneSystem>(1e-3), nullptr, true, 1e-2);
			simulator.simulateTo(10);
			if (checkQ0Exceeds1(simulator.data())) {
				x1 -= 0.05;
				break;
			}
		}

		cout << boost::format("simulation started from x0:%04.2f, x1:%04.2f") % x0 % x1 << endl;

		for (; x1 <= 1.01; x1 += 0.02) {
			MatlabUtility::PlotOptions os;
			os.fontName("Times New Roman")
				.fontSize(16)
				.wholeLineProperties({{"LineWidth", "2"}})
				.prints(true)
				.legend("q_%d");
			MatlabUtility::PlotOptions oi = os;
			oi.legend({"tilde \\mu_1", "\\mu_2"});

			{
				auto system = make_shared<DroneSystem>(MyMath::Vector<2>{0.0, 0.0});
				Simulator<DroneSystem> simulator(system, Vector<6>{x0, x1, 0.0, 0.0, 0.0, 0.0},
					createRungeKutta<DroneSystem>(1e-3), &cout, true, 1e-2);
				simulator.simulateTo(10);
				auto s = (boost::format("_%03d_%03d_o") % int(x0*100) % int(x1*100)).str();
				auto mu = make_shared<MatlabUtility>("result" + s + ".m");
				auto data = make_shared<Simulator<DroneSystem>::Data>(simulator.data());
				SystemPlotter<DroneSystem> pl(mu, data);
				pl.plotState(os, "state" + s + ".csv", "state");
				pl.plotInput(oi, "input" + s + ".csv", "input");
			}
			{
				auto system = make_shared<DroneSystem>(MyMath::Vector<2>{1.0, 1.0});
				Simulator<DroneSystem> simulator(system, Vector<6>{x0, x1, 0.0, 0.0, 0.0, 0.0},
					createRungeKutta<DroneSystem>(1e-5), &cout, true, 1e-2);
				simulator.simulateTo(10);
				auto s = (boost::format("_%03d_%03d_r") % int(x0*100) % int(x1*100)).str();
				auto mu = make_shared<MatlabUtility>("result" + s + ".m");
				auto data = make_shared<Simulator<DroneSystem>::Data>(simulator.data());
				SystemPlotter<DroneSystem> pl(mu, data);
				pl.plotState(os, "state" + s + ".csv", "state");
				pl.plotInput(oi, "input" + s + ".csv", "input");
			}
			cout << boost::format("plotted x0:%04.2f, x1:%04.2f") % x0 % x1 << endl;
		}
	}
	return 0;
}
