#pragma once

#include <fstream>
#include <boost/core/noncopyable.hpp>
#include <boost/range/value_type.hpp>
#include <boost/range/adaptor/indexed.hpp>
#include <boost/range/iterator_range.hpp>
#include <map>
#include <initializer_list>
#include <vector>
#include <boost/variant.hpp>
#include <boost/format.hpp>
#include <assert.h>
#include <type_traits>
#include <utility>
#include <limits>
#include "MyMath.hpp"

class MatlabUtility : boost::noncopyable_::noncopyable
{
public:
	using Properties = std::map<std::string, std::string>;

	explicit MatlabUtility(const std::string &mFilePath);

	struct PlotOptions
	{
		PlotOptions();
		Properties mWholeLineProperties;
		std::vector<Properties> mEachLineProperties;
		boost::variant<std::string, std::vector<std::string>> mLegend;
		Properties mLegendProperties;
		std::pair<double, double> mXlim, mYlim;
		PlotOptions& wholeLineProperties(const Properties &p) { mWholeLineProperties = p; return *this; }
		template<class R = std::initializer_list<Properties>> PlotOptions& eachLineProperties(const R &r);
		PlotOptions& legend(const std::string &p) { mLegend = p; return *this; }
		template<class R = std::initializer_list<std::string>,
			class T = std::enable_if_t<std::is_same_v<std::string, typename boost::range_value<R>::type>>>
		PlotOptions& legend(const R &legends);
		PlotOptions& legendProperties(const Properties &p) { mLegendProperties = p; return *this; }
		PlotOptions& xlim(double xmin, double xmax) { mXlim = {xmin, xmax}; return *this; }
		PlotOptions& ylim(double ymin, double ymax) { mYlim = {ymin, ymax}; return *this; }
	};

	template<class ValueRange>
	void plot(const std::string &csvFileName, const std::string &title, const ValueRange &valueRange,
		const PlotOptions& options = {});

private:
	std::ofstream mFile_;
	int numPlotted_;
};

inline MatlabUtility::PlotOptions::PlotOptions()
	: mWholeLineProperties()
	, mEachLineProperties()
	, mLegend("%d")
	, mXlim(-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity())
	, mYlim(-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity())
{
}

template<class R>
MatlabUtility::PlotOptions& MatlabUtility::PlotOptions::eachLineProperties(const R &r)
{
	static_assert(std::is_same_v<Properties, typename boost::range_value<R>::type>,
		"eachLineProperties is not Range of MatlabUtility::Properties.");
	mEachLineProperties = boost::copy_range<std::vector<Properties>>(r);
	return *this;
}

template<class R, class T>
MatlabUtility::PlotOptions& MatlabUtility::PlotOptions::legend(const R &legends)
{
	mLegend = boost::copy_range<std::vector<std::string>>(legends);
	return *this;
}

inline MatlabUtility::MatlabUtility(const std::string &mFilePath)
	: mFile_(mFilePath, std::ios::trunc)
	, numPlotted_(0)
{
	mFile_ << "clearvars\n" << std::endl;
}

template<class ValueRange>
void MatlabUtility::plot(const std::string &csvFileName, const std::string &title,
	const ValueRange &valueRange, const PlotOptions& options)
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
	const auto data = title + "_data";
	const auto pl = title + "_p";
	mFile_ << "%% Plot " << title << "\n"
		<< data << " = " << "csvread('" << csvFileName << "');\n"
		<< "figure(" << numPlotted_ << ");\n"
		<< pl << " = plot(";
	for (size_t i = 1; i < V::Dimension - 1; i++) {
		mFile_ << data << "(:, 1), " << data << "(:, " << i+1 << "), ";
	}
	mFile_ << data << "(:, 1), " << data << "(:, " << V::Dimension << ")";
	for (const auto &p : options.mWholeLineProperties) {
		mFile_ << ", '" << p.first << "', " << p.second;
	}
	mFile_ << ");\n";
	for (const auto &ps : options.mEachLineProperties | boost::adaptors::indexed(1)) {
		for (const auto &p : ps.value()) {
			mFile_ << pl << "(" << ps.index() << ")." << p.first << " = " << p.second << ";\n";
		}
	}
	constexpr auto inf = numeric_limits<double>::infinity();
	if(options.mXlim != make_pair(-inf, inf)) {
		mFile_ << "xlim([" << options.mXlim.first << " " << options.mXlim.second << "]);\n";
	}
	if (options.mYlim != make_pair(-inf, inf)) {
		mFile_ << "ylim([" << options.mYlim.first << " " << options.mYlim.second << "]);\n";
	}
	mFile_ << "legend({'";
	switch (options.mLegend.which()) {
	case 0:
		{
			const auto &l = boost::get<string>(options.mLegend);
			for (size_t i = 0; i < V::Dimension - 2; i++) {
				mFile_ << boost::format(l) % (i+1) << "', '";
			}
			mFile_ << boost::format(l) % (V::Dimension-1);
			break;
		}
	case 1:
		{
			const auto &ls = boost::get<vector<string>>(options.mLegend);
			for (auto &l : boost::make_iterator_range(begin(ls), prev(end(ls)))) {
				mFile_ << l << "', '";
			}
			mFile_ << ls.back();
			break;
		}
	default:
		assert(false && "options.mLegend contains unknown type.");
		break;
	}
	mFile_ <<  "'}";
	for (auto &p : options.mLegendProperties) {
		mFile_ << ", '" << p.first << "', " << p.second;
	}
	mFile_ << ");\n\n";
	mFile_ << flush;
}
