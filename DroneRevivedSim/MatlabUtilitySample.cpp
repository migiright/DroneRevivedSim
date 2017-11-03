#include "MatlabUtilitySample.h"
#include "MatlabUtility.hpp"
#include <vector>
#include "MyMath.hpp"
#include <limits>

using namespace std;

void MatlabUtilitySample() {
	constexpr double step = 0.1, end = 5;
	constexpr size_t numPlot = static_cast<int>(end / step);
	vector<MyMath::Vector<3>> data;
	data.reserve(numPlot);

	double time = 0;
	for (size_t i = 0; i < numPlot; i++) {
		data.push_back(MyMath::Vector<3>{time, time*2, time*time});
		time += step;
	}

	MatlabUtility mu("result.m");
	mu.plot("result.csv", "plot", data,
		MatlabUtility::PlotOptions()
		.wholeLineProperties({{"LineWidth", "2"}})
		.eachLineProperties({{{"Color", "[1 0 0]"}}, {{"Color", "[0 1 0]"}}})
		.legendProperties({{"FontSize", "20"}, {"FontName", "'Times New Roman'"}})
		.xlim(0, 5)
		.ylim(-2, numeric_limits<double>::infinity())
		.fontName("Times New Roman")
		.fontSize(30));
}
