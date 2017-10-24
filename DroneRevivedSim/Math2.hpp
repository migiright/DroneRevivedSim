#pragma once

#include <tuple>
#include <type_traits>
#include "MyMath.hpp"
#include "System.hpp"
#include "Simulator.hpp"
#include "Algorithms.hpp"

constexpr double Pi = 3.14159265358979323846;

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

template<size_t H, class F, class V, class C = std::enable_if_t<
	std::is_base_of_v<MyMath::MatrixBase, decltype(std::declval<F>()(MyMath::Vector<H>()))>>>
	auto lie(F f, V v, const MyMath::Vector<H> &variable)
{
	auto fr = f(variable);
	MyMath::Vector<fr.Width> ret;
	for (size_t i = 0; i < ret.Dimension; i++) {
		MyMath::Vector<H> fri;
		for (size_t j = 0; j < H; j++) {
			fri[j] = fr(j, i);
		}
		ret[i] = MyMath::differential([&](double x) { return v(variable + x*fri); }, 0);
	}
	return ret;
}

template<size_t Dimension, class F, class V, class C = std::enable_if_t<
	std::is_same_v<decltype(std::declval<F>()(MyMath::Vector<Dimension>())), MyMath::Vector<Dimension>>>>
	double lie(F f, V v, const MyMath::Vector<Dimension> &variable)
{
	MyMath::Vector<Dimension> fr = f(variable);
	return MyMath::differential([&](double x) { return v(variable + x*fr); }, 0);
}
