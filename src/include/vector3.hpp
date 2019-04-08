#pragma once

#include <array>


class Vector3
{
	std::array< double, 3 > e;	// elements

public:
	double operator[](int i) const
	{
		return e[i];
	}

	double & operator[](int i)
	{
		return e[i];
	}

	friend Vector3 operator*(double s, Vector3 v);
	friend Vector3 operator+(Vector3 u, Vector3 v);

	friend std::ostream & operator<<(std::ostream & os, const Vector3);
};


std::ostream & operator<<(std::ostream & os, const Vector3 v)
{
	os << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
	return os;
}


Vector3 operator*(double s, Vector3 v)
{
	Vector3 u = v;
	u[0] *= s;
	u[1] *= s;
	u[2] *= s;
	return u;
}

// Vector3 operator+(const Vector3 & u, const Vector3 & v)
Vector3 operator+(Vector3 u, Vector3 v)
{
	Vector3 sum;
	sum[0] = u[0] + v[0];
	sum[1] = u[1] + v[1];
	sum[2] = u[2] + v[2];
	return sum;
}
