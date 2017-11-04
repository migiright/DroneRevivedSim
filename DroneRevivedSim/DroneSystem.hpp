#pragma once

#include <tuple>
#include <cmath>
#include <functional>
#include <utility>
#include "System.hpp"
#include "Math2.hpp"

namespace Drone
{

inline MyMath::Vector<6> f(const MyMath::Vector<6> &state)
{
	return MyMath::Vector<6>{
		state[1], -sin(state[4]), state[3], cos(state[4]) - 1, state[5], 0.0
	};
}

inline MyMath::Matrix<6, 2> g(const MyMath::Vector<6> &state)
{
	return MyMath::Matrix<6, 2>{
		0.0, 0.0, -sin(state[4]), 0.0, 0.0, 0.0, cos(state[4]), 0.0, 0.0, 0.0, 0.0, 1.0
	};
}

inline MyMath::Vector<8> dynamicExtend(const MyMath::Vector<6> &state, const MyMath::Vector<2> &input)
{
	return MyMath::Vector<8>{
		state[0],
			state[1],
			-(1+input[0])*sin(state[4]),
			-(1+input[0])*state[5]*cos(state[4]) - input[1]*sin(state[4]),
			state[2],
			state[3],
			(1+input[0])*cos(state[4]) - 1,
			-(1+input[0])*state[5]*sin(state[4]) + input[1]*cos(state[4]),
	};
}

inline MyMath::Matrix<4, 2> dynamicExtendInputJacobi(const MyMath::Vector<6> &state, const MyMath::Vector<2> &input)
{
	//1, 2, 5, 6行目は省略
	return {
		-sin(state[4]), 0.0,
		-state[5]*cos(state[4]), -sin(state[4]),
		cos(state[4]), 0.0,
		-state[5]*sin(state[4]), cos(state[4]),
	};
}

inline MyMath::Vector<8> systemRevive(const MyMath::Vector<8> &x, const MyMath::Vector<2> &c)
{
	const auto t1 = tan(Pi*x[0]/(2*c[0])), at1 = 1 + t1*t1,
		t2 = tan(Pi*x[4]/(2*c[1])), at2 = 1 + t2*t2;
	return MyMath::Vector<8>{
		t1,
			Pi*x[1]/(2*c[0]) * at1,
			Pi/(2*c[0]) * at1 * (x[2] + Pi*x[1]*x[1]/c[0]*t1),
			Pi/(2*c[0]) * at1 * (x[3] + 3*Pi*x[1]*x[2]/c[0]*t1 + Pi*Pi*x[1]*x[1]*x[1]/(2*c[0]*c[0])*(1+3*t1*t1)),
			t2,
			Pi*x[5]/(2*c[1]) * at2,
			Pi/(2*c[1]) * at2 * (x[6] + Pi*x[5]*x[5]/c[1]*t2),
			Pi/(2*c[1]) * at2 * (x[7] + 3*Pi*x[5]*x[6]/c[1]*t2 + Pi*Pi*x[5]*x[5]*x[5]/(2*c[1]*c[1])*(1+3*t2*t2)),
	};
}

inline MyMath::Matrix<4, 4> systemReviveJacobi(const MyMath::Vector<8> &x, const MyMath::Vector<2> &c)
{
	//1, 2, 5, 6行目, 1, 2, 5, 6列目は省略
	const auto t1 = tan(Pi*x[0]/(2*c[0])), at1 = 1 + t1*t1,
		t2 = tan(Pi*x[4]/(2*c[1])), at2 = 1 + t2*t2;
	return {
		Pi*at1/(2*c[0]), 0.0, 0.0, 0.0,
		3*Pi*Pi*t1*at1*x[1]/(2*c[0]*c[0]), Pi*at1/(2*c[0]), 0.0, 0.0,
		0.0, 0.0, Pi*at2/(2*c[1]), 0.0,
		0.0, 0.0, 3*Pi*Pi*t2*at2*x[5]/(2*c[1]*c[1]), Pi*at2/(2*c[1]),
	};
}

inline double clfForExtension(const MyMath::Vector<8> &state, const MyMath::Matrix<8, 8> &p0)
{
	return dot(state * p0, state);
}

inline MyMath::Vector<4> clfForExtensionJacobi(const MyMath::Vector<8> &state, const MyMath::Matrix<8, 8> &p0)
{
	//1, 2, 5, 6列目は省略
	constexpr std::pair<size_t, size_t> L[]{{0, 2}, {1, 3}, {2, 6}, {3, 7}};
	MyMath::Vector<4> r;
	for (auto &c : L) {
		for (size_t i = 0; i < 8; i++) {
			r[c.first] += state[i] * (p0(i, c.second) + p0(c.second, i));
		}
	}
	return r;
}

inline double dynamicClf(const MyMath::Vector<6> &state, const MyMath::Vector<2> &input,
	const MyMath::Matrix<8, 8> &p0)
{
	return clfForExtension(dynamicExtend(state, input), p0);
}

inline MyMath::Vector<2> dynamicClfInputJacobi(const MyMath::Vector<6> &state, const MyMath::Vector<2> &input,
	const MyMath::Matrix<8, 8> &p0)
{
	return clfForExtensionJacobi(dynamicExtend(state, input), p0)*dynamicExtendInputJacobi(state, input);
}

inline double dynamicClfRevived(const MyMath::Vector<6> &state, const MyMath::Vector<2> &input,
	const MyMath::Matrix<8, 8> &p0, const MyMath::Vector<2> constraint)
{
	return clfForExtension(systemRevive(dynamicExtend(state, input), constraint), p0);
}

inline MyMath::Vector<2> dynamicClfRevivedInputJacobi(const MyMath::Vector<6> &state, const MyMath::Vector<2> &input,
	const MyMath::Matrix<8, 8> &p0, const MyMath::Vector<2> &constraint)
{
	const auto x = dynamicExtend(state, input);
	return clfForExtensionJacobi(systemRevive(x, constraint), p0)
		* systemReviveJacobi(x, constraint) * dynamicExtendInputJacobi(state, input);
}

inline std::tuple<MyMath::Vector<2>, double> staticClf(const MyMath::Vector<6> &state, const MyMath::Matrix<8, 8> p0,
	const MyMath::Vector<2> &initialMinimizingInput)
{
	return calcMinimum([&](const MyMath::Vector<2> u) { return dynamicClf(state, u, p0); },
		initialMinimizingInput);
}

inline std::tuple<MyMath::Vector<2>, double> staticClfRevived(const MyMath::Vector<6> &state, const MyMath::Matrix<8, 8> p0,
	const MyMath::Vector<2> &initialMinimizingInput, const MyMath::Vector<2> constraint)
{
	return calcMinimum([&](const MyMath::Vector<2> u) { return dynamicClfRevived(state, u, p0, constraint); },
		[&](const MyMath::Vector<2> u) { return dynamicClfRevivedInputJacobi(state, u, p0, constraint); },
		initialMinimizingInput);
}

inline std::tuple<MyMath::Vector<2>, MyMath::Vector<2>> calcInputFromClf(const MyMath::Vector<6> &state,
	const double c, const MyMath::Vector<2> &initialMinimizingInput,
	std::function<std::tuple<MyMath::Vector<2>, double>(const MyMath::Vector<6>&, const MyMath::Vector<2>&)> clf)
{
	MyMath::Vector<2> minInput = initialMinimizingInput;
	auto cl = [&](const MyMath::Vector<6> x) {
		auto r = clf(x, minInput);
		minInput = std::get<0>(r);
		return std::get<1>(r);
	};
	auto lfv = MyMath::lie(f, cl, state);
	auto lgv = MyMath::lie(g, cl, state);
	auto input = -((lfv >= 0 ? 2*lfv : 0) / MyMath::dot(lgv, lgv) + c) * lgv;
	return std::make_tuple(minInput, input);
}

/**
 * \return 0: clfを最小にする入力, 1: 入力
 */
inline std::tuple<MyMath::Vector<2>, MyMath::Vector<2>> calcInput(const MyMath::Vector<6> &state,
	const double c, const MyMath::Matrix<8, 8> &p0, const MyMath::Vector<2> &initialMinimizingInput)
{
	return calcInputFromClf(state, c, initialMinimizingInput, [&](const MyMath::Vector<6> &x, const MyMath::Vector<2> &mi) {
		return staticClf(x, p0, mi);
	});
}

/**
* \return 0: clfを最小にする入力, 1: 入力
*/
inline std::tuple<MyMath::Vector<2>, MyMath::Vector<2>> calcInputRevived(const MyMath::Vector<6> &state,
	const double c, const MyMath::Matrix<8, 8> &p0,
	const MyMath::Vector<2> &initialMinimizingInput, const MyMath::Vector<2> &constraint)
{
	return calcInputFromClf(state, c, initialMinimizingInput, [&](const MyMath::Vector<6> &x, const MyMath::Vector<2> &mi) {
		return staticClfRevived(x, p0, mi, constraint);
	});
}

class DroneSystem : public System<6, 2>
{
public:
	explicit DroneSystem(
		const MyMath::Vector<2> &constraint = MyMath::Vector<2>{},
		double c = 0.7,
		const MyMath::Matrix<8, 8> &p0 = MyMath::Matrix<8, 8>{
		3.0, 4.0, 3.0, 1.0, 0.0, 0.0, 0.0, 0.0,
		4.0, 10.0, 8.0, 3.0, 0.0, 0.0, 0.0, 0.0,
		3.0, 8.0, 10.0, 4.0, 0.0, 0.0, 0.0, 0.0,
		1.0, 3.0, 4.0, 3.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 3.0, 4.0, 3.0, 1.0,
		0.0, 0.0, 0.0, 0.0, 4.0, 10.0, 8.0, 3.0,
		0.0, 0.0, 0.0, 0.0, 3.0, 8.0, 10.0, 4.0,
		0.0, 0.0, 0.0, 0.0, 1.0, 3.0, 4.0, 3.0,
	});
	~DroneSystem() override {}
	State f(const State &state, const Input &input) override;
	Input u(const State &state) override;
	State normalize(const State &state) override;

	bool revives() const;

private:
	MyMath::Matrix<8, 8> p0_;
	MyMath::Vector<2> constraint_;
	double c_;
	MyMath::Vector<2> prevMinimizingInput_;
};

inline DroneSystem::DroneSystem(const MyMath::Vector<2> &constraint, const double c, const MyMath::Matrix<8, 8> &p0)
	: p0_(p0)
	, constraint_(constraint)
	, c_(c)
	, prevMinimizingInput_(MyMath::Vector<2>{0.0, 0.0})
{}

inline DroneSystem::State DroneSystem::f(const State &state, const Input &input)
{
	return Drone::f(state) + g(state)*input;
}

inline DroneSystem::Input DroneSystem::u(const State &state)
{
	auto a = revives()
		? calcInputRevived(state, c_, p0_, prevMinimizingInput_, constraint_)
		: calcInput(state, c_, p0_, prevMinimizingInput_);
	prevMinimizingInput_ = std::get<0>(a);
	return std::get<1>(a);
}

inline DroneSystem::State DroneSystem::normalize(const State &state)
{
	auto r = state;
	r[4] = fmod(fmod(state[4]+Pi, Pi*2) + Pi*2, Pi*2) - Pi;
	return r;
}

inline bool DroneSystem::revives() const
{
	if (constraint_[0] > 0 && constraint_[1] > 0) return true;
	return false;
}

}
