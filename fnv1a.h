#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include <cstdint>
#include <cstring>
#include <intrin.h>

#pragma once

#define stringify(x) #x
#define concat_impl(x, y) x##y
#define concat(x, y) concat_impl(x, y)

template<typename t, t v>
struct constant_holder
{
	enum class val_holder : t
	{
		val = v
	};
};

#define constant(value) ((decltype(value))constant_holder<decltype(value), value>::val_holder::val)

constexpr uint32_t seed = 0x45C3370D;
constexpr uint32_t prime = 0x1000193;
__declspec(dllexport) extern uint32_t runtime_basis;

inline uint32_t fnv1a_rt(const char* key) {
	const char* data = const_cast<char*>(key);
	auto hash = runtime_basis;


	for (size_t i = 0; i < strlen(key); ++i) {
		const uint8_t value = data[i];
		hash = hash ^ value;
		hash *= prime;
	}

	return hash;
}

inline uint32_t fnv1a_fl_rt(const char* key, const uint32_t length) {
	const char* data = const_cast<char*>(key);
	auto hash = runtime_basis;

	for (size_t i = 0; i < length; ++i) {
		const auto value = static_cast<uint8_t>(data[i]);
		hash = hash ^ value;
		hash *= prime;
	}

	return hash;
}

constexpr uint32_t fnv1a_ct(const char* str, const uint32_t value = seed) noexcept {
	return !*str ? value : fnv1a_ct(str + 1, static_cast<unsigned>(1ull *
		(value ^ static_cast<uint8_t>(*str))* prime));
}

#define fnv1a(s) constant(fnv1a_ct(s))