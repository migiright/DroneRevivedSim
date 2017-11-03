#pragma once

#include <memory>
#include <string>
#include <boost/range/adaptor/transformed.hpp>
#include "Simulator.hpp"
#include "MatlabUtility.hpp"

template<class System>
class SystemPlotter
{
public:
	using Simulator = Simulator<System>;
	using Record = typename Simulator::Record;
	using Data = typename Simulator::Data;

	SystemPlotter(std::shared_ptr<MatlabUtility> matlabUtility, std::shared_ptr<Data> data,
		const std::string &csvFilePathPrefix = "");

	void plotInput(const std::string &csvFilePath = "input.csv", const std::string &title = "input");
	void plotState(const std::string &csvFilePath = "state.csv", const std::string &title = "state");

private:
	std::shared_ptr<MatlabUtility> matlabUtility_;
	std::shared_ptr<Data> data_;
	std::string csvFilePathPrefix_;
};

template<class System>
SystemPlotter<System>::SystemPlotter(const std::shared_ptr<MatlabUtility> matlabUtility,
	const std::shared_ptr<Data> data, const std::string &csvFilePathPrefix)
	: matlabUtility_(matlabUtility)
	, data_(data)
	, csvFilePathPrefix_(csvFilePathPrefix)
{}

template<class System>
void SystemPlotter<System>::plotInput(const std::string &csvFilePath, const std::string &title)
{
	matlabUtility_->plot(csvFilePathPrefix_ + csvFilePath, title, *data_
		| boost::adaptors::transformed([](const Record &r) {
		MyMath::Vector<System::InputDimension+1> ret;
		ret[0] = r.time;
		for (size_t i = 0; i < System::InputDimension; i++) {
			ret[i+1] = r.input[i];
		}
		return ret;
	}));
}

template<class System>
void SystemPlotter<System>::plotState(const std::string &csvFilePath, const std::string &title)
{
	matlabUtility_->plot(csvFilePathPrefix_ + csvFilePath, title, *data_
		| boost::adaptors::transformed([](const Record &r) {
		MyMath::Vector<System::StateDimension+1> ret;
		ret[0] = r.time;
		for (size_t i = 0; i < System::StateDimension; i++) {
			ret[i+1] = r.state[i];
		}
		return ret;
	}));
}
