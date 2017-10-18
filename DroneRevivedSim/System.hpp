#pragma once

#include "MyMath.hpp"

template<size_t StateD, size_t InputD>
class System {
public:
	constexpr static size_t StateDimension = StateD;
	constexpr static size_t InputDimension = InputD;
	using State = MyMath::Vector<StateDimension>;
	using Input = MyMath::Vector<InputDimension>;

	virtual ~System(){}

	virtual State f(const State &state, const Input &input) = 0;
	virtual Input u(const State &state) = 0;
	virtual State normalize(const State &state) { return state; }
};
