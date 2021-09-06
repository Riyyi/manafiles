#include <chrono>  // high_resolution_clock, seconds, milliseconds, microseconds, nanoseconds
#include <cstdint> // uint64_t
#include <cstdio> // printf

#include "util/timer.h"

namespace Util {

Timer::Timer()
{
	m_start = std::chrono::high_resolution_clock::now();
}

Timer Timer::operator-(const Timer& timer)
{
	return Timer(TimePoint { m_start - timer.start() });
}

template<typename To, typename From>
To Timer::to(From from)
{
	return std::chrono::duration_cast<To>(from);
}

uint64_t Timer::toSeconds()
{
	return to<std::chrono::seconds>(m_start.time_since_epoch()).count();
}

uint64_t Timer::toMilliseconds()
{
	return to<std::chrono::milliseconds>(m_start.time_since_epoch()).count();
}

uint64_t Timer::toMicroseconds()
{
	return to<std::chrono::microseconds>(m_start.time_since_epoch()).count();
}

uint64_t Timer::toNanoseconds()
{
	return to<std::chrono::nanoseconds>(m_start.time_since_epoch()).count();
}

template<typename T>
uint64_t Timer::elapsed()
{
	auto now = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<T>(now - m_start).count();
}

uint64_t Timer::elapsedSeconds()
{
	return elapsed<std::chrono::seconds>();
}

uint64_t Timer::elapsedMilliseconds()
{
	return elapsed<std::chrono::milliseconds>();
}

uint64_t Timer::elapsedMicroseconds()
{
	return elapsed<std::chrono::microseconds>();
}

uint64_t Timer::elapsedNanoseconds()
{
	return elapsed<std::chrono::nanoseconds>();
}

void Timer::fancyPrint(uint64_t nanoseconds)
{
	if (nanoseconds > 999999999) {
		printf("%.3fs", nanoseconds / 1000000000.0);
	}
	else if (nanoseconds > 999999) {
		printf("%.0fms", nanoseconds / 1000000.0);
	}
	else if (nanoseconds > 999) {
		printf("%.0fμs", nanoseconds / 1000.0);
	}
	else {
		printf("%luns", nanoseconds);
	}
}

} // namespace Util