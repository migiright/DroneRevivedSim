#pragma once

#include <tuple>
#include "MyMath.hpp"
#include "System.hpp"
#include "Simulator.hpp"
#include "Algorithms.hpp"
#include <iostream>
#include <numeric>

constexpr double Pi = 3.14159265358979323846;

template<class Function>
double solve(Function function, double x0, double x1, double error)
{
	double f0 = function(x0), f1 = function(x1);
	int k = 0;
	while (abs(f1) > error) {
		const auto t0 = x0;
		x0 = x1;
		x1 -= f1 * (x1-t0)/(f1-f0);
		f0 = f1;
		f1 = function(x1);
		++k;
		if(k >= 100) {
			return std::numeric_limits<double>::quiet_NaN();
		}
	}
	return x1;
}

template<size_t Dimension, class Function, class Jacobi>
auto calcMinimum(Function function, Jacobi jacobi, const MyMath::Vector<Dimension> &initial)
{
	using State = MyMath::Vector<Dimension>;
	using Input = MyMath::Vector<0>;
	using System = CommonSystem<Dimension, 0>;
	auto system = std::make_shared<System>([jacobi](const State &s, const Input &) { return -jacobi(s); },
		[](const State &s) { return Input{}; });
	Simulator<System> simulator(system, initial, createRungeKutta<System>(1e-3), nullptr, false);
	State error;
	simulator.simulateToConverge(1e-7);
	auto minx = simulator.currentState();
	return std::make_tuple(minx, function(minx));
}

template<size_t Dimension, class Function>
auto calcMinimum(Function function, const MyMath::Vector<Dimension> &initial)
{
	return calcMinimum(function,
		[function](const MyMath::Vector<Dimension> &s) { return MyMath::jacobianVector(function, s); },
		initial);
}

template<size_t Dimension, class Jacobi>
auto calcMinimumByCg(Jacobi jacobi, const MyMath::Vector<Dimension> &initial, double initialH = 1e-7, double error = 1e-5)
{
	auto x = initial;
	const auto g0 = jacobi(x);
	auto png = dot(g0, g0);
	auto p = -g0;
	while (png > error*error) {
		x = x + p*solve([&](double a) { return dot(p, jacobi(x + a*p)); }, 0, initialH, error*error);
		const auto g = jacobi(x);
		const auto cng = dot(g, g);
		p = -g + cng / png * p;
		png = cng;
	}
	return x;
}

template<size_t Dimension>
bool isfinite(const MyMath::Vector<Dimension> &x)
{
	for (size_t i = 0; i < Dimension; i++) {
		if (!std::isfinite(x[i])) return false;
	}
	return true;
}
