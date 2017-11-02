#pragma once

#include <fstream>
#include <boost/core/noncopyable.hpp>
#include <boost/range/value_type.hpp>
#include "MyMath.hpp"

class MatlabUtility : boost::noncopyable_::noncopyable {
public:
	explicit MatlabUtility(const std::string &mFilePath);

	template<class ValueRange>
	void plot(const std::string &csvFileName, const std::string &title, const ValueRange &valueRange);

private:
	std::ofstream mFile_;
	int numPlotted_;
};

inline MatlabUtility::MatlabUtility(const std::string &mFilePath)
	: mFile_(mFilePath, std::ios::trunc)
	, numPlotted_(0)
{}

template<class ValueRange>
void MatlabUtility::plot(const std::string &csvFileName, const std::string &title, const ValueRange &valueRange)
{
	using namespace std;
	using V = typename boost::range_value<ValueRange>::type;
	static_assert(is_base_of_v<MyMath::VectorBase, typename boost::range_value<ValueRange>::type>,
		"valueRange is not Range of MyMath::Vector.");

	{
		ofstream csvFile(csvFileName, ios::trunc);
		for (const auto &v : valueRange) {
			csvFile << v[0];
			for (size_t i = 1; i < V::Dimension; i++) {
				csvFile << "," << v[i];
			}
			csvFile << "\n";
		}
	}

	++numPlotted_;
	const string data = title + "_data";
	mFile_ << "%% Plot " << title << "\n"
		<< data << " = " << "csvread('" << csvFileName << "');\n"
		<< "figure(" << numPlotted_ << ");\n"
		<< "plot(";
	for (size_t i = 1; i < V::Dimension - 1; i++) {
		mFile_ << data << "(:, 1), " << data << "(:, " << i+1 << "), ";
	}
	mFile_ << data << "(:, 1), " << data << "(:, " << V::Dimension << "));\n"
		<< "grid on\n"
		<< "legend(";
	for (size_t i = 0; i < V::Dimension - 2; i++) {
		mFile_ << "'\\fontname{Times} \\fontsize{14} " << i+1 << "', ";
	}
	mFile_ << "'\\fontname{Times} \\fontsize{14} " << V::Dimension-1 << "');\n\n";
	mFile_ << flush;
}
