#pragma once

#include <functional>

// https://stackoverflow.com/questions/2590677/
//             how-do-i-combine-hash-values-in-c0x
//
namespace tell
{
  
  inline void hash_combine(std::size_t& seed)
  {
  }
  
  template <typename T, typename... Rest>
    inline void hash_combine(std::size_t& seed, const T& v, Rest... rest)
  {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    hash_combine(seed, rest...);
  }
}

namespace std
{
  template<typename T, std::size_t N> struct hash<std::array<T, N>>
    {
      size_t operator()(const std::array<T, N>& a) const
      {
	std::size_t h=0;
	for (auto g : a) {
	  tell::hash_combine(h, g);
	}
	return h;
      }
    };  
}
