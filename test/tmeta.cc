#include "tell/util.h"
#include "tell/meta.h"
#include <array>
#include <cassert>
#include <cstring>
#include <exception>
#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

using namespace tell;

struct A{};
struct B{};
struct C{};
struct X{};
struct Y{};
struct Z{};

std::ostream& operator<<(std::ostream& ost, A)
{
  return ost << "A";
}

std::ostream& operator<<(std::ostream& ost, B)
{
  return ost << "B";
}

std::ostream& operator<<(std::ostream& ost, C)
{
  return ost << "C";
}

std::ostream& operator<<(std::ostream& ost, X)
{
  return ost << "X";
}

std::ostream& operator<<(std::ostream& ost, Y)
{
  return ost << "Y";
}

std::ostream& operator<<(std::ostream& ost, Z)
{
  return ost << "Z";
}

void test_pop_front()
{
  constexpr const auto empty = std::index_sequence<>{};
  std::cout << empty << std::endl;

  // caught by static_assert 
  // constexpr const auto error = pop_front(empty);

  constexpr const auto null [[maybe_unused]] = std::tuple<>{};
  // caught by static_assert
  // constexpr const auto = pop_front(null);
  
  // constexpr auto error = get<0>(empty);
  // caught by static_assert in get function
  // static_assert(error == errval);

  constexpr const auto singleton = std::index_sequence<42>{};
  static_assert(size(singleton) == 1);
  constexpr auto answer [[maybe_unused]] = get<0>(singleton);
  static_assert(answer == 42);
  
  constexpr const auto is0 = std::index_sequence<0, 1, 2>{};
  static_assert(size(is0) == 3);
  static_assert(get<0>(is0) == 0);
  static_assert(get<1>(is0) == 1);
  static_assert(get<2>(is0) == 2);
  // caught by static_assert in get function:
  // static_assert(get<3>(is0) == errval);
  
  std::cout << "is0: " << is0 << std::endl;

  constexpr const auto is1 = pop_front(is0);
  std::cout << "is1: " << is1 << std::endl;
  
  constexpr const auto is2 = pop_front(is1);
  std::cout << "is2: " << is2 << std::endl;
  
  constexpr auto tup1 = std::tuple<A,B,C>{};
  std::cout << "tup1: " << tup1 << std::endl;

  constexpr auto tup10 = pop_front(tup1);
  std::cout << "tup10: " << tup10 << std::endl;
}

void test_tuple_slice()
{
  constexpr const auto ts = std::index_sequence<1,1,2,3,5,8,13>{};
  constexpr auto i = get<0>(ts);
  constexpr auto j = get<i>(ts);
  std::cerr << "i: " << i << ", j: " << j << std::endl;
  constexpr const auto is = std::index_sequence<1,3,5>{};
  constexpr const auto ts0 = std::make_index_sequence<5>();
  std::cerr << "ts0: " << ts0 << std::endl;
  constexpr auto ks [[maybe_unused]] = slice(ts, is);
  std::cerr << "ks: " << ks << std::endl;
  constexpr auto k = get<0>(ks);
  std::cerr << "k: " << k << std::endl;
}

void test_concat()
{
  constexpr auto start = std::make_tuple(std::tuple<>{});

  const auto t = start
  << std::make_tuple(1)
  << std::make_tuple(2)
  << std::make_tuple(3);
  
  std::cout << "t: " << t << std::endl;
}

struct Foo
{
  Foo(const std::tuple<int, float>& tup)
    : i_(std::get<0>(tup))
    , f_(std::get<1>(tup))
  {
    std::cout << "Constructed a Foo from a tuple" << std::endl;
  }

  Foo(int i, float f)
    : i_(i)
    , f_(f)
  {
    std::cout << "Constructed a Foo from an int and a float" << std::endl;
  }
  int i_;
  float f_;
};

std::ostream& operator<<(std::ostream& ost, const Foo& f)
{
  return ost << "{" << f.i_ << ", " << f.f_ << "}";
}

// what's the point of this?
void test_piecewise_construct()
{
  std::pair<std::string, std::vector<int>>
    foo(
	std::piecewise_construct,
	std::forward_as_tuple("sample"),
	std::forward_as_tuple(2, 100)
	);
  std::cout << "foo.first: " << foo.first << std::endl;
  std::cout << "foo.second:";
  for (auto x: foo.second) {
    std::cout << ' ' << x;
  }
  std::cout << '\n';
  
  std::tuple<int, float> t(1, 3.14);
  std::tuple<int, float> u(2, 2.71);
  std::pair<Foo, Foo> p1(t, u);
  std::pair<Foo, Foo> p2(std::piecewise_construct, t, u);
  std::cout << "p2: " << p2 << std::endl;
  std::pair<Foo, Foo> p3({1, 3.14}, {2, 2.71});
  std::cout << "p3: " << p3 << std::endl;
}

void test_zip()
{
  constexpr const auto tup1 = std::tuple<A,B,C>{};
  constexpr const auto tup2 = std::tuple<X,Y,Z>{};
 
  constexpr const auto tup12 = zip(tup1, tup2);
  static_assert(size(tup12) == 3);
  std::cout << "tup12: " << tup12 << std::endl;

  constexpr const auto p1 = col<1>(tup12);
  
  std::cout << "p1: " << p1 << std::endl;
  
  constexpr auto empty = std::make_tuple();
  static_assert(size(empty) == 0);
  
  constexpr auto e2 = zip(empty, empty);
  static_assert(size(e2) == 0);
}

#if 0

//
// alternative for index, based on std::appy: no improvement
//

constexpr const auto index_error = std::numeric_limits<std::size_t>::max();

template<typename... Ts>
constexpr auto first_row(const std::tuple<Ts...>& t)
{
  return std::apply([](auto&... x) {
      return std::make_tuple((std::get<0>(x))...);
    },
    t);
}

// this is the technique used to implement std::apply;
// can't use std::apply here, because the index i is needed alongside t[i]
template<typename V, typename... Ts, std::size_t... Is>
constexpr  std::size_t
do_find_type(const V& v, const std::tuple<Ts...>& t, std::index_sequence<Is...>)
{
  auto ind = [](auto t, auto i) {
    return std::is_same<V, decltype(t)>::value ? i : index_error;
  };
  std::size_t j = index_error;
  // ((j = std::min( j, ind(std::get<0>(std::get<Is>(t)), Is) )),...);
  ((j = std::min( j, ind(std::get<Is>(t), Is) )),...);
  return j;
}

template<typename V, typename... Ts>
constexpr std::size_t find_type(const V& v, const std::tuple<Ts...>& t)
{
  return do_find_type(v, t, std::make_index_sequence<sizeof... (Ts)>{});
}

template<typename V, typename... Ts>
auto hlookup(const V& v, const std::tuple<Ts...>& t)
{
  const auto r1 = first_row(t);
  constexpr auto i = tell::index(r1, v);
  // constexpr auto i = find_type(v, r1);
  static_assert(i != index_error);
  return std::get<i>(t);
}
#endif

void test_index()
{
  using tell::index;
  
  {
    constexpr auto tup = std::tuple<>{};
    static_assert(index(A{}, tup) == 0);
    static_assert(index<A>(tup) == 0);
    using Tp = typename std::remove_const<decltype(tup)>::type;
    static_assert(index<A, Tp>() == 0);
  }

  {
    constexpr auto tup = std::tuple<A>{};
    static_assert(index(A{}, tup) == 0);
    static_assert(index<A>(tup) == 0);
    using Tp = typename std::remove_const<decltype(tup)>::type;
    static_assert(index<A,Tp>() == 0); 
    static_assert(index(B{}, tup) == 1);
    static_assert(index<B>(tup) == 1);
    static_assert(index<B, std::tuple<A>>() == 1);
  }

  {
    constexpr auto tup = std::tuple<A,B>{};
    static_assert(index(A{}, tup) == 0);
    static_assert(index(B{}, tup) == 1);
    static_assert(index(X{}, tup) == 2);
  }
  
  {
    constexpr auto tup = std::tuple<A,B,C>{};
    static_assert(index(A{}, tup) == 0);
    static_assert(index(B{}, tup) == 1);
    static_assert(index(C{}, tup) == 2);
  }
}

int main(int argc, const char* argv[])
{
  using namespace tell;  
  try {
    test_pop_front();
    test_tuple_slice();
    test_piecewise_construct();
    test_zip();
    test_concat();
    test_index();
    return 0;
  }
  catch (std::exception& x) {
    std::cerr << x.what() << std::endl;
  }
  return 1;
}



