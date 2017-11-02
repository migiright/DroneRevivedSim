#pragma once

#include <fstream>
#include <boost/core/noncopyable.hpp>
#include <boost/range/value_type.hpp>
#include <boost/range/adaptor/indexed.hpp>
#include <map>
#include <initializer_list>
#include "MyMath.hpp"

class MatlabUtility : boost::noncopyable_::noncopyable {
public:
	using LineProperties = std::map<std::string, std::string>;

	explicit MatlabUtility(const std::string &mFilePath);

	template<class ValueRange, class EachLineProperties = std::initializer_list<LineProperties>>
	void plot(const std::string &csvFileName, const std::string &title, const ValueRange &valueRange,
		const LineProperties &wholeLineProperties = LineProperties(), const EachLineProperties &eachLineProperties = {});

private:
	std::ofstream mFile_;
	int numPlotted_;
};

inline MatlabUtility::MatlabUtility(const std::string &mFilePath)
	: mFile_(mFilePath, std::ios::trunc)
	, numPlotted_(0)
{}

template<class ValueRange, class EachLineProperties>
void MatlabUtility::plot(const std::string &csvFileName, const std::string &title, const ValueRange &valueRange,
	const LineProperties &wholeLineProperties, const EachLineProperties &eachLineProperties)
{
	using namespace std;
	using V = typename boost::range_value<ValueRange>::type;
	static_assert(is_base_of_v<MyMath::VectorBase, typename boost::range_value<ValueRange>::type>,
		"valueRange is not Range of MyMath::Vector.");
	static_assert(is_same_v<LineProperties, typename boost::range_value<EachLineProperties>::type>,
		"eachLineProperties is not Range of MatlabUtility::LineProperties.");

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
	const string pl = title + "_p";
	mFile_ << "%% Plot " << title << "\n"
		<< data << " = " << "csvread('" << csvFileName << "');\n"
		<< "figure(" << numPlotted_ << ");\n"
		<< pl << " = plot(";
	for (size_t i = 1; i < V::Dimension - 1; i++) {
		mFile_ << data << "(:, 1), " << data << "(:, " << i+1 << "), ";
	}
	mFile_ << data << "(:, 1), " << data << "(:, " << V::Dimension << ")";
	for(const auto &p : wholeLineProperties) {
		mFile_ << ", '" << p.first << "', " << p.second;
	}
	mFile_ << ");\n";
	for(const auto &ps : eachLineProperties | boost::adaptors::indexed(1)) {
		for(const auto &p : ps.value()) {
			mFile_ << pl << "(" << ps.index() << ")." << p.first << " = " << p.second << ";\n";
		}
	}
	mFile_ << "legend(";
	for (size_t i = 0; i < V::Dimension - 2; i++) {
		mFile_ << "'\\fontname{Times} \\fontsize{14} " << i+1 << "', ";
	}
	mFile_ << "'\\fontname{Times} \\fontsize{14} " << V::Dimension-1 << "');\n\n";
	mFile_ << flush;
}
