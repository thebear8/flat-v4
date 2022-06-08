#pragma once
#include <stdint.h>

#pragma warning(push)
#pragma warning(disable : 4455)

inline constexpr char operator "" ss(unsigned long long arg) noexcept
{
	return static_cast<char>(arg);
}

inline constexpr unsigned char operator "" uss(unsigned long long arg) noexcept
{
	return static_cast<unsigned char>(arg);
}

inline constexpr short operator "" s(unsigned long long arg) noexcept
{
	return static_cast<short>(arg);
}

inline constexpr unsigned short operator "" us(unsigned long long arg) noexcept
{
	return static_cast<unsigned short>(arg);
}

#pragma warning(pop)