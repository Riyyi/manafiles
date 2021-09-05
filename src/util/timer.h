#ifndef TIMER_H
#define TIMER_H

#include <chrono>  // high_resolution_clock
#include <cstdint> // uint64_t

namespace Util {

using TimePoint = std::chrono::high_resolution_clock::time_point;

class Timer {
public:
	Timer();
	Timer(const TimePoint& timePoint)
		: m_start(timePoint)
	{
	}

	Timer operator-(const Timer& timer);

	template<typename To, typename From>
	To to(From from);
	uint64_t toSeconds();
	uint64_t toMilliseconds();
	uint64_t toMicroseconds();
	uint64_t toNaneseconds();

	template<typename T>
	uint64_t elapsed();
	uint64_t elapsedSeconds();
	uint64_t elapsedMilliseconds();
	uint64_t elapsedMicroseconds();
	uint64_t elapsedNanoseconds();

	const TimePoint& start() const { return m_start; }

private:
	TimePoint m_start;
};

} // namespace Util

#endif // TIMER_H
