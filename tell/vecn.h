#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>

//
// rudimentary n-dimensional vector algebra
//

namespace tell
{  
  // vector addition
  template<typename C, std::size_t N>
    auto operator+(const std::array<C,N>& u, const std::array<C,N>& v);

  // vector subtraction
  template<typename C, std::size_t N>
    auto operator-(const std::array<C,N>& u, const std::array<C,N>& v);

  // left multiplication by scalar
  template<typename C, std::size_t N, typename S>
    auto operator*(S s, const std::array<C,N>& u);

  // right multiplication by scalar
  template<typename C, std::size_t N, typename S>
    auto operator*(const std::array<C,N>& u, S s);

  // division by scalar
  template<typename C, std::size_t N, typename S>
    auto operator/(const std::array<C,N>& u, S s);

  // scalar product
  template<typename C, std::size_t N>
  auto operator*(const std::array<C,N>& u, const std::array<C,N>& v);
  
  // Euclidean length
  template<typename C, std::size_t N>
    auto abs(const std::array<C,N>& u);

  // L1 norm
  template<typename C, std::size_t N>
    auto abs1(const std::array<C,N>& u);
  
  // L2 norm
  template<typename C, std::size_t N>
    auto abs2(const std::array<C,N>& u);
  
  // Linfinity norm
  template<typename C, std::size_t N>
    auto abs8(const std::array<C,N>& u);
}

template<typename C, std::size_t N>
  auto tell::operator+(const std::array<C,N>& u, const std::array<C,N>& v)
{
  std::array<C,N> w{};
  std::transform(u.begin(), u.end(), v.begin(), w.begin(), std::plus<C>());
  return w;
}

template<typename C, std::size_t N>
  auto tell::operator-(const std::array<C,N>& u, const std::array<C,N>& v)
{
  std::array<C,N> w{};
  std::transform(u.begin(), u.end(), v.begin(), w.begin(), std::minus<C>());
  return w;
}

template<typename C, std::size_t N, typename S>
  auto tell::operator*(S s, const std::array<C,N>& u)
{
  using W = decltype(S()*C());
  std::array<W,N> w;
  std::transform(u.begin(), u.end(), w.begin(), [s](auto x){
      return s*x;
    });
  return w;
}

template<typename C, std::size_t N, typename S>
  auto tell::operator*(const std::array<C,N>& u, S s)
{
  return s*u;
}

template<typename C, std::size_t N, typename S>
  auto tell::operator/(const std::array<C,N>& u, S s)
{
  using W = decltype(C()/S());
  std::array<W,N> w;
  std::transform(u.begin(), u.end(), w.begin(), [s](auto x){
      return x/s;
    });
  return w;
}

template<typename C, std::size_t N>
  auto tell::operator*(const std::array<C,N>& u, const std::array<C,N>& v)
{
  return std::inner_product(u.begin(), u.end(), v.begin(), C{});
}

template<typename C, std::size_t N>
  auto tell::abs(const std::array<C,N>& u)
{
  return abs2(u);
}

template<typename C, std::size_t N>
  auto tell::abs1(const std::array<C,N>& u)
{
  return std::accumulate(u.begin(), u.end(), C{}, [](auto x, auto y){
      return x + std::abs(y);
    });
}

template<typename C, std::size_t N>
  auto tell::abs2(const std::array<C,N>& u)
{
  return std::sqrt(u*u);
}

template<typename C, std::size_t N>
  auto tell::abs8(const std::array<C,N>& u)
{
  return std::accumulate(u.begin(), u.end(), C{}, [](auto x, auto y){
      return std::max(x, std::abs(y));
    });
}


