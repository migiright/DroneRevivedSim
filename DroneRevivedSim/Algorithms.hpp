#pragma once

#include "Simulator.hpp"

template<class System>
Algorithm<System> createEuler(double step)
{
	return [=](System &system, double time, const typename System::State& state) {
		auto u = system.u(state);
		auto x = state + step*system.f(state, u);
		return Record<System>{time+step, x, u};
	};
}

template<class System>
Algorithm<System> createRungeKutta(double step)
{
	return [=](System &system, double time, const typename System::State& state) {
		auto u = system.u(state);
		auto k1 = system.f(state, u),
			k2 = system.f(state + step/2 * k1, system.u(state + step/2 * k1)),
			k3 = system.f(state + step/2 * k2, system.u(state + step/2 * k2)),
			k4 = system.f(state + step * k3, system.u(state + step * k3));
		auto x = state + step/6 * (k1 + 2*k2 + 2*k3 + k4);
		return Record<System>{time+step, x, u};
	};
}
