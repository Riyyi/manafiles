/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef JSON_TO_JSON_H
#define JSON_TO_JSON_H

#include <cassert> // assert
#include <cstddef> // nullptr_t
#include <map>
#include <string>
#include <unordered_map>
#include <utility> // forward

#include "util/json/array.h"
#include "util/json/conversion.h"
#include "util/json/object.h"

namespace Json {

namespace Detail {

struct jsonConstructor {
	template<typename Json>
	static void construct(Json& json, bool boolean)
	{
		json.clear();
		json.m_type = Json::Type::Bool;
		json.m_value.asBool = boolean;
	}

	template<typename Json>
	static void construct(Json& json, int number)
	{
		json.clear();
		json.m_type = Json::Type::Number;
		json.m_value.asDouble = (double)number;
	}

	template<typename Json>
	static void construct(Json& json, double number)
	{
		json.clear();
		json.m_type = Json::Type::Number;
		json.m_value.asDouble = number;
	}

	template<typename Json>
	static void construct(Json& json, const char* string)
	{
		json.clear();
		json.m_type = Json::Type::String;
		json.m_value.asString = new std::string(string);
	}

	template<typename Json>
	static void construct(Json& json, const std::string& string)
	{
		json.clear();
		json.m_type = Json::Type::String;
		json.m_value.asString = new std::string(string);
	}

	template<typename Json>
	static void construct(Json& json, const Array& array)
	{
		json.clear();
		json.m_type = Json::Type::Array;
		json.m_value.asArray = new Array(array);
	}

	template<typename Json, typename T>
	static void construct(Json& json, const std::vector<T>& array)
	{
		json.clear();
		json.m_type = Json::Type::Array;
		json.m_value.asArray = new Array;
		json.m_value.asArray->reserve(array.size());
		for (const T& value : array) {
			json.m_value.asArray->emplace_back(value);
		}
	}

	template<typename Json>
	static void construct(Json& json, const Object& object)
	{
		json.clear();
		json.m_type = Json::Type::Object;
		json.m_value.asObject = new Object(object);
	}

	template<typename Json, typename T>
	static void construct(Json& json, const std::map<std::string, T>& object)
	{
		json.clear();
		json.m_type = Json::Type::Object;
		json.m_value.asObject = new Object;
		for (const auto& [name, value] : object) {
			json.m_value.asObject->emplace(name, value);
		}
	}

	template<typename Json, typename T>
	static void construct(Json& json, const std::unordered_map<std::string, T>& object)
	{
		json.clear();
		json.m_type = Json::Type::Object;
		json.m_value.asObject = new Object;
		for (const auto& [name, value] : object) {
			json.m_value.asObject->emplace(name, value);
		}
	}
};

template<typename Json, typename T>
void toJson(Json& json, const T& value)
{
	jsonConstructor::construct(json, value);
}

struct toJsonFunction {
	template<typename Json, typename T>
	auto operator()(Json& json, T&& value) const
	{
		return toJson(json, std::forward<T>(value));
	}
};

} // namespace Detail

// Anonymous namespace prevents multiple definition of the reference
namespace {
// Function object
constexpr const auto& toJson = Detail::staticConst<Detail::toJsonFunction>; // NOLINT (misc-definitions-in-headers)
} // namespace

} // namespace Json

#endif // JSON_TO_JSON_H

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