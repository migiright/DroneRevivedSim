/**
* \file MyMath.hpp
* \version 1.4.1
*/

#pragma once

#include <array>
#include <type_traits>
#include <boost/format.hpp>
#include <boost/operators.hpp>

namespace MyMath {

/// すべての Matrix のベースクラス。std::is_base_of とかで使う。
class MatrixBase {};

/**
* @brief 行列 
* @tparam H 行数
* @tparam W 列数
*/
template<size_t H, size_t W>
class Matrix
	: MatrixBase
	, boost::addable<Matrix<H, W>>
	, boost::subtractable<Matrix<H, W>>
	, boost::multipliable<Matrix<H, W>, double>
{
public:
	constexpr static size_t Height = H; ///< 行数
	constexpr static size_t Width = W; ///< 列数

	std::array<double, Height*Width> v;

	/// 0で初期化する
	Matrix()
		: v{}
	{}

	/// コピーコンストラクタ
	Matrix(const Matrix<H, W> &o)
		: v(o.v)
	{}

	/**
	* \brief 初期値を指定する
	* \param args 初期化する値。1行1列, ... , 1行Width列, 2行1列, ... の順
	*/
	template<class ...Args>
	explicit Matrix(const Args &...args)
		: v{args...}
	{}

	/// 代入演算子
	Matrix<Height, Width> &operator=(const Matrix<Height, Width> &o) &
	{
		for (size_t r = 0; r < Height; r++) {
			for (size_t c = 0; c < Width; c++) {
				(*this)(r, c) = o(r, c);
			}
		}
		return *this;
	}

	/// 要素を返す
	double& operator()(size_t row, size_t column)
	{
		return v[row * Width + column];
	}

	/// 要素を返す
	double operator()(size_t row, size_t column) const
	{
		return v[row * Width + column];
	}

	constexpr static double height() { return Height; } ///< 行数を返す
	constexpr static double width() { return Width; } ///< 列数を返す

													  /// 整形して文字列として返す
	std::string toString() const
	{
		std::string s;
		for (size_t r = 0; r < Height; ++r) {
			for (size_t c = 0; c < Width; ++c) {
				s += (boost::format("%4.2f") % (*this)(r, c)).str() + " ";
			}
			s += "\n";
		}
		return s;
	}

	/// 行列同士の加算
	Matrix<Height, Width>& operator+=(const Matrix<Height, Width> &rhs) &
	{
		for (size_t r = 0; r < Height; r++) {
			for (size_t c = 0; c < Width; c++) {
				(*this)(r, c) += rhs(r, c);
			}
		}
		return *this;
	}

	/// 行列同士の減算
	Matrix<Height, Width>& operator-=(const Matrix<Height, Width> &rhs) &
	{
		for (size_t r = 0; r < Height; r++) {
			for (size_t c = 0; c < Width; c++) {
				(*this)(r, c) -= rhs(r, c);
			}
		}
		return *this;
	}

	/// スカラとの乗算
	Matrix<Height, Width>& operator*=(double rhs) &
	{
		for (size_t r = 0; r < Height; ++r) {
			for (size_t c = 0; c < Width; ++c) {
				(*this)(r, c) *= rhs;
			}
		}
		return *this;
	}

	///　反数
	Matrix<Height, Width> operator-()
	{
		Matrix<Height, Width> ret;
		for (size_t r = 0; r < Height; r++) {
			for (size_t c = 0; c < Width; c++) {
				ret(r, c) = -(*this)(r, c);
			}
		}
		return ret;
	}

};

/// 行列同士の乗算
template<size_t LhsWidth, size_t AnsHright, size_t AnsWidth>
Matrix<AnsHright, AnsWidth> operator*(
	const Matrix<AnsHright, LhsWidth>& lhs, const Matrix<LhsWidth, AnsWidth>& rhs)
{
	Matrix<AnsHright, AnsWidth> ret;
	for (size_t r = 0; r < AnsHright; ++r) {
		for (size_t c = 0; c < AnsWidth; ++c) {
			double s = 0;
			for (size_t i = 0; i < LhsWidth; ++i) {
				s += lhs(r, i) * rhs(i, c);
			}
			ret(r, c) = s;
		}
	}
	return ret;
}


/// すべての Vector のベースクラス。std::is_base_of とかで使う。
struct VectorBase {};

/**
* \brief ベクトル
* \tparam D 次元
*/
template<size_t D>
struct Vector
	: VectorBase
	, boost::addable<Vector<D>>
	, boost::subtractable<Vector<D>>
	, boost::multipliable<Vector<D>, double>
	, boost::dividable<Vector<D>, double>
{
	std::array<double, D> v;
	constexpr static size_t Dimension = D;

	/// 0で初期化する
	Vector() : v{}
	{}

	/// コピーコンストラクタ
	Vector(const Vector &o)
		: v(o.v)
	{}

	/// 初期値を指定する
	template<class ...Args>
	explicit Vector(const Args &...args)
		: v{args...}
	{}

	/// 代入演算子
	const Vector<Dimension> &operator=(const Vector &o) &
	{
		for (size_t i = 0; i < Dimension; i++) {
			(*this)[i] = o[i];
		}
		return *this;
	}

	/// 要素を返す
	double& operator[](unsigned index) { return v[index]; }

	/// 要素を返す
	const double& operator[](unsigned index) const { return v[index]; }

	/// 加算
	Vector<D>& operator+=(const Vector<D>& rhs) &
	{
		for (size_t i = 0; i < D; i++) {
			v[i] += rhs[i];
		}
		return *this;
	}

	/// 減算
	Vector<D>& operator-=(const Vector<D>& rhs) &
	{
		for (size_t i = 0; i < D; i++) {
			v[i] -= rhs[i];
		}
		return *this;
	}

	/// スカラとの乗算
	Vector<D>& operator*=(double rhs) &
	{
		for (size_t i = 0; i < D; ++i) {
			(*this)[i] *= rhs;
		}
		return *this;
	}

	/// スカラでの除算
	Vector<D>& operator/=(double rhs) &
	{
		for (size_t i = 0; i < D; ++i) {
			(*this)[i] /= rhs;
		}
		return *this;
	}

	/// 反数
	Vector<D> operator-() const {
		Vector<D> d;
		for (size_t i = 0; i < D; i++) {
			d[i] = -(*this)[i];
		}
		return d;
	}

	/// 整形して文字列として返す
	std::string toString() const
	{
		std::string s;
		for (size_t i = 0; i < D; ++i) {
			s += (boost::format("%4.2f") % (*this)[i]).str() + " ";
		}
		s += "\n";
		return s;
	}
};

/// 行列*縦ベクトルの乗算
template<size_t LhsHeight, size_t LhsWidth>
Vector<LhsHeight> operator*(const Matrix<LhsHeight, LhsWidth> &lhs, const Vector<LhsWidth> &rhs)
{
	Vector<LhsHeight> ret;
	for (size_t i = 0; i < LhsHeight; i++) {
		ret[i] = 0;
		for (size_t j = 0; j < LhsWidth; j++) {
			ret[i] += lhs(i, j) * rhs[j];
		}
	}
	return ret;
}

/// 横ベクトル*行列の乗算
template<size_t LhsHeight, size_t LhsWidth>
Vector<LhsWidth> operator*(const Vector<LhsHeight> &lhs, const Matrix<LhsHeight, LhsWidth> &rhs)
{
	Vector<LhsWidth> ret;
	for (size_t i = 0; i < LhsWidth; i++) {
		ret[i] = 0;
		for (size_t j = 0; j < LhsHeight; j++) {
			ret[i] += lhs[j] * rhs(j, i);
		}
	}
	return ret;
}

/// ベクトル同士の内積
template<size_t Dimension>
double dot(const Vector<Dimension> &lhs, const Vector<Dimension> &rhs) {
	double ret = 0;
	for (size_t i = 0; i < Dimension; i++) {
		ret += lhs[i] * rhs[i];
	}
	return ret;
}

/// ノルム
template<size_t Dimension>
double norm(const Vector<Dimension> x)
{
	double ret = 0;
	for (size_t i = 0; i < Dimension; i++) {
		ret += x[i]*x[i];
	}
	return sqrt(ret);
}

/// ノルム
inline double norm(double x)
{
	return x;
}

/// Matrixから列を抜き出してVectorにする
template<size_t Height, size_t Width>
Vector<Height> pickColumn(const Matrix<Height, Width> &matrix, size_t columnIndex)
{
	Vector<Height> ret;
	for (size_t r = 0; r < Height; r++) {
		ret[r] = matrix(r, columnIndex);
	}
	return ret;
}

/// 行を抜き出してベクトルにする
template<size_t Height, size_t Width>
Vector<Width> pickRow(const Matrix<Height, Width> &matrix, size_t rowIndex)
{
	Vector<Width> ret;
	for (size_t c = 0; c < Width; c++) {
		ret[c] = matrix(rowIndex, c);
	}
	return ret;
}

/// 微分の微小変位
constexpr double DefaultDisplacement = 1e-7;

/// 1変数関数の微分
template<class Function>
auto differential(Function function, double variable, double displacement = DefaultDisplacement)
{
	return (function(variable + displacement) - function(variable - displacement)) / (2*displacement);
}

/// ベクトル値関数のヤコビ行列
template<size_t VariableDimension, class Function>
auto jacobianMatrix(Function function, Vector<VariableDimension> variable, double displacement = DefaultDisplacement)
{
	constexpr size_t FunctionDimension = decltype(function(variable))::Dimension;
	Matrix<FunctionDimension, VariableDimension> ret;
	for (size_t i = 0; i < VariableDimension; i++) {
		auto r = differential([&](double x) {
			auto v = variable;
			v[i] = x;
			return function(v);
		}, variable[i], displacement);
		for (size_t j = 0; j < FunctionDimension; j++) {
			ret(j, i) = r[j];
		}
	}
	return ret;
}

///　スカラ値関数のヤコビ行列
template<size_t VariableDimension, class Function>
auto jacobianVector(Function function, Vector<VariableDimension> variable, double displacement = DefaultDisplacement)
{
	Vector<VariableDimension> ret;
	for (size_t i = 0; i < VariableDimension; i++) {
		ret[i] = differential([&](double x) {
			auto v = variable;
			v[i] = x;
			return function(v);
		}, variable[i], displacement);
	}
	return ret;
}

/// LfVのfがMatrixのときのLie微分
template<size_t H, class F, class V, class C = std::enable_if_t<
	std::is_base_of_v<MatrixBase, decltype(std::declval<F>()(MyMath::Vector<H>()))>>>
	auto lie(F f, V v, const Vector<H> &variable)
{
	auto fr = f(variable);
	Vector<fr.Width> ret;
	for (size_t i = 0; i < ret.Dimension; i++) {
		ret[i] = MyMath::differential([&](double x) { return v(variable + x*pickColumn(fr, i)); }, 0);
	}
	return ret;
}

/// LfVのfがVectorのときのLie微分
template<size_t Dimension, class F, class V, class C = std::enable_if_t<
	std::is_same_v<decltype(std::declval<F>()(MyMath::Vector<Dimension>())), Vector<Dimension>>>>
	double lie(F f, V v, const Vector<Dimension> &variable)
{
	Vector<Dimension> fr = f(variable);
	return MyMath::differential([&](double x) { return v(variable + x*fr); }, 0);
}

}
