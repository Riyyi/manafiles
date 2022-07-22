/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cassert>

namespace Util {

template<typename T>
class Singleton {
public:
	static inline T& the()
	{
		if (s_instance == nullptr) {
			s_instance = new T { s {} };
		}

		return *s_instance;
	}

	static inline void destroy()
	{
		if (s_instance) {
			delete s_instance;
		}

		s_instance = nullptr;
	}

	// Remove copy constructor and copy assignment operator
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton&&) = delete;

protected:
	Singleton() {}

	// Constructor token
	struct s {};

private:
	static T* s_instance;
};

template<typename T>
T* Singleton<T>::s_instance = nullptr;

} // namespace Util
