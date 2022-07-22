/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <chrono>  // high_resolution_clock
#include <cstdint> // uint64_t

namespace Util {

using TimePoint = std::chrono::high_resolution_clock::time_point;

class Timer {
public:
	Timer();
	Timer(const TimePoint& timePoint);

	Timer operator-(const Timer& timer);

	static TimePoint now();
	void pause();
	void resume();

	template<typename To, typename From>
	To to(From from);
	uint64_t toSeconds();
	uint64_t toMilliseconds();
	uint64_t toMicroseconds();
	uint64_t toNanoseconds();

	template<typename T>
	uint64_t elapsed();
	uint64_t elapsedSeconds();
	uint64_t elapsedMilliseconds();
	uint64_t elapsedMicroseconds();
	uint64_t elapsedNanoseconds();

	static void fancyPrint(uint64_t nanoseconds);

	const TimePoint& start() const { return m_start; }

private:
	bool m_running { true };
	TimePoint m_accumulated;
	TimePoint m_start;
};

} // namespace Util
