#pragma once

template <typename T>
struct ISampler1D
{
	virtual T operator()(double u) const = 0;
};

template <typename T>
struct ISampler2D
{
	virtual T operator()(double u, double v) const = 0;
};

template <typename T>
struct ISampler3D
{
	virtual T operator()(double u, double v, double w) const = 0;
};