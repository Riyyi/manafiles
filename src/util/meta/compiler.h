/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

// Compiler
#if defined(__clang__)
	#define GCC
#elif defined(__GNUG__) || (defined(__GNUC__) && defined(__cplusplus))
	#define GCC
#elif defined(__INTEL_COMPILER) // Supports some GCC extensions
	#define GCC
#elif defined(_MSC_VER)
	#define MSVC
#endif

// Non-standard function macro
#ifdef GCC
	#define FUNCTION_MACRO __PRETTY_FUNCTION__ // GCC extension
#elif MSVC
	#define FUNCTION_MACRO __FUNCSIG__
#else
	#define FUNCTION_MACRO __func__ // C99
#endif
