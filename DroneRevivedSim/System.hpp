#pragma once

#include "MyMath.hpp"
#include <functional>

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

template<size_t StateD, size_t InputD>
class CommonSystem : public System<StateD, InputD> {
public:
	CommonSystem(
		std::function<State(const State&, const Input&)> f,
		std::function<Input(const State&)> u,
		std::function<State(const State&)> normalize = [](const State& state) { return state; }
	)
		: f_(f), u_(u), normalize_(normalize)
	{}

	State f(const State &state, const Input &input) override {
		return f_(state, input);
	}
	Input u(const State &state) override {
		return u_(state);
	}
	State normalize(const State &state) override {
		return normalize_(state);
	}

private:
	std::function<State(const State&, const Input&)> f_;
	std::function<Input(const State&)> u_;
	std::function<State(const State&)> normalize_;
};
