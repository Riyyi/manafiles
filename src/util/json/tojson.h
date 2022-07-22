/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cassert> // assert
#include <cstddef> // nullptr_t
#include <map>
#include <string>
#include <unordered_map>
#include <utility> // forward

#include "util/json/array.h"
#include "util/json/object.h"
#include "util/meta/odr.h"

namespace Util::JSON {

namespace Detail {

struct jsonConstructor {
	template<typename Json>
	static void construct(Json& json, bool boolean)
	{
		json.destroy();
		json.m_type = Json::Type::Bool;
		json.m_value.boolean = boolean;
	}

	template<typename Json>
	static void construct(Json& json, int number)
	{
		json.destroy();
		json.m_type = Json::Type::Number;
		json.m_value.number = (double)number;
	}

	template<typename Json>
	static void construct(Json& json, double number)
	{
		json.destroy();
		json.m_type = Json::Type::Number;
		json.m_value.number = number;
	}

	template<typename Json>
	static void construct(Json& json, const char* string)
	{
		json.destroy();
		json.m_type = Json::Type::String;
		json.m_value.string = new std::string(string);
	}

	template<typename Json>
	static void construct(Json& json, const std::string& string)
	{
		json.destroy();
		json.m_type = Json::Type::String;
		json.m_value.string = new std::string(string);
	}

	template<typename Json>
	static void construct(Json& json, const Array& array)
	{
		json.destroy();
		json.m_type = Json::Type::Array;
		json.m_value.array = new Array(array);
	}

	template<typename Json, typename T>
	static void construct(Json& json, const std::vector<T>& array)
	{
		json.destroy();
		json.m_type = Json::Type::Array;
		json.m_value.array = new Array;
		json.m_value.array->reserve(array.size());
		for (const T& value : array) {
			json.m_value.array->emplace_back(value);
		}
	}

	template<typename Json>
	static void construct(Json& json, const Object& object)
	{
		json.destroy();
		json.m_type = Json::Type::Object;
		json.m_value.object = new Object(object);
	}

	template<typename Json, typename T>
	static void construct(Json& json, const std::map<std::string, T>& object)
	{
		json.destroy();
		json.m_type = Json::Type::Object;
		json.m_value.object = new Object;
		for (const auto& [name, value] : object) {
			json.m_value.object->emplace(name, value);
		}
	}

	template<typename Json, typename T>
	static void construct(Json& json, const std::unordered_map<std::string, T>& object)
	{
		json.destroy();
		json.m_type = Json::Type::Object;
		json.m_value.object = new Object;
		for (const auto& [name, value] : object) {
			json.m_value.object->emplace(name, value);
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
constexpr const auto& toJson = Util::Detail::staticConst<Detail::toJsonFunction>; // NOLINT(misc-definitions-in-headers,clang-diagnostic-unused-variable)
} // namespace

} // namespace Util::JSON

// Customization Points
// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4381.html

// Json::toJson is a function object, the type of which is
// Json::Detail::toJsonFunction. In the Json::Detail namespace are the toJson
// free functions. The function call operator of toJsonFunction makes an
// unqualified call to toJson which, since it shares the Detail namespace with
// the toJson free functions, will consider those in addition to any overloads
// that are found by argument-dependent lookup.

// Variable templates are linked externally, therefor every translation unit
// will see the same address for Detail::staticConst<Detail::toJsonFunction>.
// Since Json::toJson is a reference to the variable template, it too will have
// the same address in all translation units.
