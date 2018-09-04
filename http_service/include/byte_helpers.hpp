#include <vector>
#include <array>
#include <string>
#include <cstddef>

template<typename T>
static std::array<std::byte, sizeof(T)>
to_bytes(const T& object)
{
	std::array<std::byte, sizeof(T)> bytes;

	const std::byte* begin = reinterpret_cast<const std::byte*>(std::addressof(object));
	const std::byte* end   = begin + sizeof(T);
	std::copy(begin, end, std::begin(bytes));

	return bytes;
}

template<typename T>
static T&
from_bytes(const std::array<std::byte, sizeof(T)>& bytes, T& object)
{
	// http://en.cppreference.com/w/cpp/types/is_trivially_copyable
	static_assert(std::is_trivially_copyable<T>::value,
				  "not a TriviallyCopyable type");

	std::byte* begin_object = reinterpret_cast<std::byte*>(std::addressof(object));
	std::copy(std::begin(bytes), std::end(bytes), begin_object);

	return object;
}

template<typename T>
static T
from_bytes(const std::byte* bytes)
{
	static_assert(std::is_trivially_copyable<T>::value,
				  "not a TriviallyCopyable type");

	T out_obj;
	std::byte* begin_object
		= reinterpret_cast<std::byte*>(std::addressof(out_obj));
	std::copy(bytes, bytes + sizeof(T), begin_object);

	return out_obj;
}

// Use these to detect if a type is an array
template<typename T>
constexpr bool is_array = false;

template<typename T, std::size_t N>
constexpr bool is_array<std::array<T, N>> = true;

template<typename T, typename U>
constexpr bool is_array_of = false;

template<typename T, typename U, std::size_t N>
constexpr bool is_array_of<std::array<T, N>, U> = true;

template<typename T>
static std::vector<T>
operator+(const std::vector<T>& lhs, const std::vector<T>& rhs)
{
	auto vec_cpy = std::vector<T>(lhs);
	vec_cpy.reserve(lhs.size() + rhs.size());
	vec_cpy.insert(vec_cpy.end(), rhs.begin(), rhs.end());
	return vec_cpy;
}

template<typename T>
static std::vector<T>&
operator+=(std::vector<T>& lhs, const std::vector<T>& rhs)
{
	lhs.reserve(lhs.size() + rhs.size());
	lhs.insert(lhs.end(), rhs.begin(), rhs.end());
	return lhs;
}

