/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef JSON_FROM_JSON_H
#define JSON_FROM_JSON_H

#include <algorithm> // transform
#include <cassert>   // assert
#include <cstddef>   // nullptr_t
#include <map>
#include <string>
#include <unordered_map>
#include <utility> // forward
#include <vector>

#include "util/json/array.h"
#include "util/json/conversion.h"
#include "util/json/object.h"

namespace Json {

namespace Detail {

// Required for containers with Json::Value type
template<typename Json>
void fromJson(const Json& json, Json& value)
{
	value = json;
}

template<typename Json>
void fromJson(const Json& json, std::nullptr_t& null)
{
	assert(json.type() == Json::Type::Null);
	null = nullptr;
}

template<typename Json>
void fromJson(const Json& json, bool& boolean)
{
	assert(json.type() == Json::Type::Bool);
	boolean = json.asBool();
}

template<typename Json>
void fromJson(const Json& json, int& number)
{
	assert(json.type() == Json::Type::Number);
	number = (int)json.asDouble();
}

template<typename Json>
void fromJson(const Json& json, double& number)
{
	assert(json.type() == Json::Type::Number);
	number = json.asDouble();
}

template<typename Json>
void fromJson(const Json& json, std::string& string)
{
	assert(json.type() == Json::Type::String);
	string = json.asString();
}

template<typename Json, typename T>
void fromJson(const Json& json, std::vector<T>& array)
{
	assert(json.type() == Json::Type::Array);
	array.resize(json.size());
	std::transform(
		json.asArray().values().begin(),
		json.asArray().values().end(),
		array.begin(),
		[](const Json& json) {
			return json.template get<T>(); // (missing-dependent-template-keyword)
		});
}

struct fromJsonFunction {
	template<typename Json, typename T>
	auto operator()(const Json& json, T&& value) const
	{
		return fromJson(json, std::forward<T>(value));
	}
};

} // namespace Detail

// Anonymous namespace prevents multiple definition of the reference
namespace {
// Function object
constexpr const auto& fromJson = Detail::staticConst<Detail::fromJsonFunction>; // NOLINT (misc-definitions-in-headers)
} // namespace

} // namespace Json

#endif // JSON_FROM_JSON_H

// Customization Points
// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4381.html

// Json::fromJson is a function object, the type of which is
// Json::Detail::fromJsonFunction. In the Json::Detail namespace are the
// fromJson free functions. The function call operator of fromJsonFunction makes
// an unqualified call to fromJson which, since it shares the Detail namespace
// with the fromJson free functions, will consider those in addition to any
// overloads that are found by argument-dependent lookup.

// Variable templates are linked externally, therefor every translation unit
// will see the same address for Detail::staticConst<Detail::fromJsonFunction>.
// Since Json::fromJson is a reference to the variable template, it too will
// have the same address in all translation units.
