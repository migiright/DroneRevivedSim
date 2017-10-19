#pragma once

#include "System.hpp"
#include <memory>
#include <boost/noncopyable.hpp>
#include <functional>
#include <list>

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

	Simulator(PSystem system, const typename System::State &initialState, Algorithm algorithm);
	virtual ~Simulator(){}

	void simulateStep();
	void simulateTo(double time);
	const Data &data() const;

private:
	PSystem system_;
	Data data_;
	Algorithm algorithm_;
};

template<class System>
Simulator<System>::Simulator(PSystem system, const typename System::State &initialState, Algorithm algorithm)
	: system_(system)
	, data_{Record{0, initialState, system->u(initialState)}}
	, algorithm_(algorithm)
{	
}

template<class System>
void Simulator<System>::simulateStep() {
	auto &c = data_.back();
	auto n = algorithm_(*system_, c.time, c.state);
	n.state = system_->normalize(n.state);
	c.input = n.input;
	data_.push_back(n);
}

template<class System>
void Simulator<System>::simulateTo(double time) {
	while(data_.back().time < time) {
		simulateStep();
	}
}

template<class System>
const typename Simulator<System>::Data &Simulator<System>::data() const {
	return data_;
}
