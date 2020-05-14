#pragma once

template <typename T>
class ISampler1D
{
	virtual T operator()(double u) = 0;
};

template <typename T>
class ISampler2D
{
	virtual T operator()(double u, double v) = 0;
};

template <typename T>
class ISampler3D
{
	virtual T operator()(double u, double v, double w) = 0;
};