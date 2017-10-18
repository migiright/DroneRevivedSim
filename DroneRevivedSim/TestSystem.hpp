#pragma once

#include "System.hpp"

class TestSystem final : public System<2, 1> {

public:
	State f(const State &state, const Input &input) override {
		return State{state[1], input[0]};
	}
	Input u(const State &state) override {
		return Input{-state[0]};
	}
};
