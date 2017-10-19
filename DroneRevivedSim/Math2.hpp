﻿#pragma once

#include <tuple>
#include "MyMath.hpp"
#include "System.hpp"
#include "Simulator.hpp"
#include "Algorithms.hpp"

template<size_t Dimension>
MyMath::Vector<Dimension> operator-(MyMath::Vector<Dimension> o)
{
	for (size_t i = 0; i < Dimension; i++) {
		o[i] = -o[i];
	}
	return o;
}

template<size_t Dimension, class Function>
auto calcMinimum(Function function, MyMath::Vector<Dimension> initial)
{
	using State = MyMath::Vector<Dimension>;
	using Input = MyMath::Vector<0>;
	using System = CommonSystem<Dimension, 0>;
	auto system = std::make_shared<System>([function](const State &s, const Input &) { return -MyMath::jacobianVector(function, s); },
		[](const State &s) { return Input{}; });
	Simulator<System> simulator(system, initial, createRungeKutta<System>(1e-3));
	State error;
	simulator.simulateToConverge(1e-7);
	auto minx = simulator.data().back().state;
	return std::make_tuple(minx, function(minx));
}
