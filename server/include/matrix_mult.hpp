#pragma once

#include <cassert>

#include <array2d.hpp>

template<size_t K, typename T>
struct multi_tmp
{
	using sub_type = multi_tmp<K - 1, T>;

	multi_tmp(const T& val) : value(val), sub(val) {}

	T value;
	sub_type sub;
};

template<typename T>
struct multi_tmp<0, T>
{
	multi_tmp(const T& val) {}
};
///////////////////////////
template<size_t Index1, size_t MaxIndex1, size_t Index2, size_t MaxIndex2>
struct loopback
{
	static const size_t next_index1 = Index1;
	static const size_t next_index2 = Index2 + 1;
};
template<size_t Index1, size_t MaxIndex1, size_t MaxIndex2>
struct loopback<Index1, MaxIndex1, MaxIndex2, MaxIndex2>
{
	static const size_t next_index1 = Index1 + 1;
	static const size_t next_index2 = 0;
};
///////////////////////////
template<size_t Index1, size_t MaxIndex1, size_t Index2, size_t MaxIndex2>
struct mult_block
{
	using loop = loopback<Index1, MaxIndex1, Index2, MaxIndex2>;
	using next = mult_block<loop::next_index1, MaxIndex1, loop::next_index2, MaxIndex2>;

	template<typename T, typename Matrix>
	void operator()(T& tmp, const Matrix& A, const Matrix& B, size_t i, size_t k, size_t j) {
		tmp.value += A(i + Index1, k) * B(k, j + Index2);
		next()(tmp.sub, A, B, i, k, j);
	}
	
	template<typename T, typename Matrix>
	void update(const T& tmp, Matrix& C, size_t i, size_t j) {
		C(i + Index1, j + Index2) = tmp.value;
		next().update(tmp.sub, C, i, j);
	}
};

template<size_t Index1, size_t MaxIndex1, size_t MaxIndex2>
struct mult_block<Index1, MaxIndex1, MaxIndex2, MaxIndex2>
{
	using next = mult_block<Index1 + 1, MaxIndex1, 0, MaxIndex2>;

	template<typename T, typename Matrix>
	void operator()(T& tmp, const Matrix& A, const Matrix& B, size_t i, size_t k, size_t j) {
		tmp.value += A(i + Index1, k) * B(k, j + MaxIndex2);
		next()(tmp.sub, A, B, i, k, j);
	}
	
	template<typename T, typename Matrix>
	void update(const T& tmp, Matrix& C, size_t i, size_t j) {
		C(i + Index1, j + MaxIndex2) = tmp.value;
		next().update(tmp.sub, C, i, j);
	}
};

template<size_t MaxIndex1, size_t MaxIndex2>
struct mult_block<MaxIndex1, MaxIndex1, MaxIndex2, MaxIndex2>
{
	template<typename T, typename Matrix>
	void operator()(T& tmp, const Matrix& A, const Matrix& B, size_t i, size_t k, size_t j) {
		tmp.value += A(i + MaxIndex1, k) * B(k, j + MaxIndex2);
	}
	
	template<typename T, typename Matrix>
	void update(const T& tmp, Matrix& C, size_t i, size_t j) {
		C(i + MaxIndex1, j + MaxIndex2) = tmp.value;
	}
};

template<size_t N, size_t M, typename Matrix>
void mult_meta(const Matrix& A, const Matrix& B, Matrix& C) {
	assert(A.size(1) == B.size(2));
	using value_type = typename Matrix::value_type;
	size_t Rang = A.size(1);
	mult_block<0, N - 1, 0, M - 1> block;
	for (size_t i = 0; i < Rang; i += N)
	{
		for (size_t j = 0; j < Rang; j += M)
		{	
			multi_tmp<N*M, value_type> tmp(value_type{});	
			for (size_t k = 0; k < Rang; ++k)
			{
				block(tmp, A, B, i, k, j);
			}
			block.update(tmp, C, i, j);
		}
	}
}

template<size_t N, size_t M, typename Matrix>
void mult_siml(const Matrix& A, const Matrix& B, Matrix& C) {
	assert(A.size(1) == B.size(2));
	using value_type = typename Matrix::value_type;
	size_t Rang = A.size(1);
	for (size_t i = 0; i < Rang; i ++)
	{
		for (size_t j = 0; j < Rang; j ++)
		{	
			value_type tmp{};
			for (size_t k = 0; k < Rang; ++k)
			{
				tmp += A.at(i, k)*B.at(k, j);
			}
			C.at(i, j) = tmp;;
		}
	}
}