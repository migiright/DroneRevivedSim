#pragma once

#include "System.hpp"
#include <memory>
#include <boost/noncopyable.hpp>
#include <functional>
#include <list>
#include <chrono>
#include "Logger.hpp"
#include <cmath>

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
	using State = typename System::State;
	using Input = typename System::Input;

	Simulator(PSystem system, const typename System::State &initialState, Algorithm algorithm,
		std::ostream *logStream = nullptr, bool records = true, double recodringInterval = 0);
	virtual ~Simulator(){}

	std::ostream* logStream() const { return logger_.ostream(); }
	std::ostream* logStream(std::ostream *ostream) { return logger_.ostream(ostream); }
	bool records() const { return records_; }
	bool records(bool records) { return records_ = records; }
	double recordingInterval() const { return recordingInterval_; }
	double recordingInterval(double recordingInterval) { return recordingInterval_ = recordingInterval; }
	double currentTime() const { return currentTime_; }
	const State& currentState() const { return currentState_; }

	Record calcNext();
	void simulateStep();
	void simulateTo(double time);
	void simulateToConverge(double error);
	const Data &data() const;

protected:
	void step(const Record &next);

private:
	PSystem system_;
	Data data_;
	Algorithm algorithm_;
	Logger logger_;
	bool records_;
	double recordingInterval_;
	double currentTime_;
	State currentState_;
	double recordedTime_;
};

template<class System>
Simulator<System>::Simulator(PSystem system, const typename System::State &initialState,
	Algorithm algorithm, std::ostream *logStream, const bool records, const double recordingInterval)
	: system_(system)
	, data_()
	, algorithm_(algorithm)
	, logger_(logStream)
	, records_(records)
	, recordingInterval_(recordingInterval)
	, currentTime_(0)
	, currentState_(initialState)
	, recordedTime_(0)
{}

template<class System>
typename Simulator<System>::Record Simulator<System>::calcNext()
{
	auto n = algorithm_(*system_, currentTime_, currentState_);
	n.state = system_->normalize(n.state);
	return n;
}

template<class System>
void Simulator<System>::simulateStep()
{
	step(calcNext());
}

template<class System>
void Simulator<System>::simulateTo(double time)
{
	logger_ << "simulate from " << currentTime() << " to " << time << "." << std::endl;
	auto pt = std::chrono::steady_clock::now();
	while (currentTime() < time) {
		const auto ct = std::chrono::steady_clock::now();
		if (ct - pt >= std::chrono::seconds(1)) {
			logger_ << "simulated up to " << currentTime() << "." << std::endl;
			pt = ct;
		}
		simulateStep();
	}
	logger_ << "simulation finished." << std::endl;
}

template<class System>
void Simulator<System>::simulateToConverge(double error)
{
	logger_ << "simulate from " << currentTime() << " until state convergs to " << error << "." << std::endl;
	auto cpt = std::chrono::steady_clock::now();

	while (true) {
		const auto cct = std::chrono::steady_clock::now();
		if (cct - cpt >= std::chrono::seconds(1)) {
			logger_ << "simulated up to " << currentTime() << "." << std::endl;
			cpt = cct;
		}
		auto n = calcNext();
		if (MyMath::norm(n.state - currentState())/(n.time - currentTime()) <= error) break;
		step(n);
	}
	logger_ << "simulation finished." << std::endl;
}

template<class System>
const typename Simulator<System>::Data &Simulator<System>::data() const
{
	return data_;
}

template<class System>
void Simulator<System>::step(const Record &next)
{
	if (records() && (data().size() == 0 || currentTime() - recordedTime_ >= recordingInterval())) {
		data_.push_back(Record{currentTime_, currentState_, next.input});
		if (recordingInterval() > 0) {
			recordedTime_ += floor((currentTime_-recordedTime_)/recordingInterval()) * recordingInterval();
		}
	}
	currentTime_ = next.time;
	currentState_ = next.state;
}
