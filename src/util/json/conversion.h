/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef JSON_CONVERSION_H
#define JSON_CONVERSION_H

namespace Json {

namespace Detail {

// Avoid ODR (One Definition Rule) violations
template<typename T>
constexpr T staticConst {};

} // namespace Detail

} // namespace Json

#endif // JSON_CONVERSION_H
