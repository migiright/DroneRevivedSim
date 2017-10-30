#pragma once

#include "System.hpp"
#include <memory>
#include <boost/noncopyable.hpp>
#include <functional>
#include <list>
#include <chrono>
#include "Logger.hpp"

template<class System>
struct Record
{
	double time;
	typename System::State state;
	typename System::Input input;
};

template<class System>
using Algorithm = std::function<Record<System>(System &system, double, const typename System::State&)>;

template<class System>
class Simulator : boost::noncopyable
{
public:
	using PSystem = std::shared_ptr<System>;
	using Record = Record<System>;
	using Data = std::list<Record>;
	using Algorithm = Algorithm<System>;

	Simulator(PSystem system, const typename System::State &initialState, Algorithm algorithm,
		std::ostream *logStream = nullptr, bool records = true);
	virtual ~Simulator(){}

	std::ostream* logStream() const { return logger_.ostream(); }
	std::ostream* logStream(std::ostream *ostream) { return logger_.ostream(ostream); }
	bool records() const { return records_; }
	bool records(bool records) { return records_ = records; }

	void simulateStep();
	void simulateTo(double time);
	void simulateToConverge(double error);
	const Data &data() const;

protected:
	void step();

private:
	PSystem system_;
	Data data_;
	Algorithm algorithm_;
	Logger logger_;
	bool records_;
};

template<class System>
Simulator<System>::Simulator(PSystem system, const typename System::State &initialState,
	Algorithm algorithm, std::ostream *logStream, const bool records)
	: system_(system)
	, data_{Record{0, initialState, system->u(initialState)}}
	, algorithm_(algorithm)
	, logger_(logStream)
	, records_(records)
{	
}

template<class System>
void Simulator<System>::simulateStep() {
	step();
	if (!records()) data_.erase(std::prev(std::end(data_), 2));
}

template<class System>
void Simulator<System>::simulateTo(double time) {
	logger_ << "simulate from " << data_.back().time << " to " << time << "." << std::endl;
	auto pt = std::chrono::steady_clock::now();
	while(data_.back().time < time) {
		const auto ct = std::chrono::steady_clock::now();
		if(ct - pt >= std::chrono::seconds(1)) {
			logger_ << "simulated up to " << data_.back().time << "." << std::endl;
			pt = ct;
		}
		simulateStep();
	}
	logger_ << "simulation finished." << std::endl;
}

template<class System>
void Simulator<System>::simulateToConverge(double error) {
	logger_ << "simulate from " << data_.back().time 
		<< " until state convergs to " << error << "." << std::endl;
	auto pt = std::chrono::steady_clock::now();
	step();
	auto c = data_.back();
	auto p = *std::prev(std::end(data_), 2);
	while(MyMath::norm((c.state - p.state)/(c.time - p.time)) > error) {
		const auto ct = std::chrono::steady_clock::now();
		if (ct - pt >= std::chrono::seconds(1)) {
			logger_ << "simulated up to " << data_.back().time << "." << std::endl;
			pt = ct;
		}
		p = data_.back();
		simulateStep();
		c = data_.back();
	}
	if(!records()) data_.erase(std::prev(std::end(data_), 2));
	logger_ << "simulation finished." << std::endl;
}

template<class System>
const typename Simulator<System>::Data &Simulator<System>::data() const {
	return data_;
}

template<class System>
void Simulator<System>::step() {
	auto &c = data_.back();
	auto n = algorithm_(*system_, c.time, c.state);
	n.state = system_->normalize(n.state);
	c.input = n.input;
	data_.push_back(n);
}
