/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef META_ODR_H
#define META_ODR_H

namespace Util {

namespace Detail {

// Avoid ODR (One Definition Rule) violations,
// variable templates are required to have external linkage
template<typename T>
constexpr T staticConst {};

} // namespace Detail

} // namespace Util

#endif // META_ODR_H
