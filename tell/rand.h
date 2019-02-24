#pragma once

#include <tell/util.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <limits>
#include <random>
#include <type_traits>
#include <utility>

namespace tell
{
  namespace rand
  {
    using Internal_real = double;

    template<typename E>
      class Base
      {
      public:
	using Engine = E;
	static void seed(unsigned int s);
	static Engine& engine();
      private:
	static inline Engine engine_;
      };

    // uniform random variable, Stroustrup III p129 
    template<typename T, typename E>
      class Uniform : public Base<E>
    {
      using lims = std::numeric_limits<T>;
      using Distro = typename
	std::conditional<(std::is_floating_point<T>::value),
	std::uniform_real_distribution<T>,
	std::uniform_int_distribution<T>>::type;
    public:
      using value_type = T;
      Uniform(T low = lims::min(), T high = lims::max());
      T operator()();
    private:
      Distro  distro_;
    };
    
    // random variable with uniform square 
    template<typename T, typename E>
      class Uniform2 : public Base<E>
    {
      using lims = std::numeric_limits<T>;
    public:
      using value_type = T;
      Uniform2(T low = std::sqrt(lims::min()), T high = std::sqrt(lims::max()));
      T operator()();
    private:
      Uniform<T,E> rep_;
    };

    // random variable with uniform square 
    template<typename T, typename E>
      class Uniform3 : public Base<E>
    {
      using lims = std::numeric_limits<T>;
    public:
      using value_type = T;
      Uniform3(T low = std::pow(lims::min(), 1.0/3),
	       T high = std::pow(lims::max(), 1.0/3));
      T operator()();
    private:
      Uniform<T,E> rep_;
    };

    // random variable with uniform sin
    template<typename T, typename E>
      class Uniform_sin : public Base<E>
    {
      inline static const auto pi = 4*std::atan(1);
      using lims = std::numeric_limits<T>;
    public:
      using value_type = T;
      Uniform_sin(T low = 0.0, T high = pi/2);
      T operator()();
    private:
      Uniform<T,E> rep_;
    };
    
    // random pairs
    template<typename G, typename H>
      class Pair : public Base<typename G::Engine>
    {
      static_assert(std::is_same<
		    typename G::Engine,
		    typename H::Engine>::value);
      using T = typename G::value_type;
      using U = typename H::value_type;
      using Rep = std::pair<G,H>;
    public:
      using value_type = std::pair<T,U>;
      Pair(const G& g, const H& h);
      value_type operator()();
    private:
      Rep rep_;
    };

    // random arrays
    template<typename G, std::size_t N>
      class Array : public Base<typename G::Engine>
    {
      using T = typename G::value_type;
      using Rep = std::array<G,N>;
    public:
      using value_type = std::array<T,N>;
      template<typename... Args> Array(const Args&...);
      value_type operator()();
    private:
      Rep rep_;
    };

    // helper class for randomisation in polar coordinates
    template<typename T>
      class Polar
      {
      public:
	Polar(T x, T y, T r, T phi);
	T operator[](std::size_t) const;
      private:
	T x_;
	T y_;
      };
    
    // helper class for randomisation in spherical coordinates
    template<typename T>
      class Spherical
      {
      public:
	Spherical(T x, T y, T z, T r, T phi, T theta);
	T operator[](std::size_t) const;
      private:
	T x_;
	T y_;
	T z_;
      };
    
    // uniform random variable on disc 
    template<typename T, typename E>
      class Uniform<Polar<T>,E> : public Base<E>
    {
      using lims = std::numeric_limits<T>;
      inline static const auto pi = 4*std::atan(1);
    public:
      using value_type = std::array<T,2>;
      Uniform(T x = 0, T y = 0,
	      T r0 = 0, T r1 = lims::max(),
	      T phi0 = 0, T phi1 = 2*pi);
      value_type operator()();
    private:
      Uniform2<Internal_real,E> r_;
      Uniform<Internal_real,E> phi_;
      T x_;
      T y_;
    };

    // uniform random variable on ball 
    template<typename T, typename E>
      class Uniform<Spherical<T>,E> : public Base<E>
    {
      using lims = std::numeric_limits<T>;
      inline static const auto pi = 4*std::atan(1);
    public:
      using value_type = std::array<T,3>;
      Uniform(T x = 0, T y = 0, T z = 0,
	      T r0 = 0, T r1 = lims::max(),
	      T phi0 = 0, T phi1 = 2*pi,
	      T theta0 = 0, T theta1 = pi/2);
      value_type operator()();
    private:
      Uniform3<Internal_real,E> r_;
      Uniform<Internal_real,E> phi_;
      Uniform_sin<Internal_real,E> theta_;
      T x_;
      T y_;
      T z_;
    };

    // convenient access to base engine
    template<typename E>
      class Uniform<void, E> : public Base<E>
    {
    public:
    }; 
  }

  namespace impl
  {
    template<typename T, typename E>
      struct Rand_var
      {
	using type = rand::Uniform<T,E>;
      };
    
    template<typename T, typename U, typename E>
      struct Rand_var<std::pair<T,U>, E>
      {
	using type = rand::Pair<rand::Uniform<T,E>, rand::Uniform<U,E>>;
      };
    
    template<typename T, std::size_t N, typename E>
      struct Rand_var<std::array<T,N>, E>
      {
	using type = rand::Array<rand::Uniform<T,E>, N>;
      };
  }
  
  template<typename T, typename E = std::default_random_engine>
    using Rand = typename impl::Rand_var<T,E>::type;

  template<typename E = std::default_random_engine> 
    class Tracking_engine : public E
    {
    public:
    auto operator()();
    std::size_t calls() const;
    ~Tracking_engine();
    private:
    std::size_t calls_ = 0;
    };
}

template<typename T, typename E>
  tell::rand::Uniform<T,E>::Uniform(T low, T high)
  : distro_(low, high)
    {
    }

template<typename T, typename E>
  T tell::rand::Uniform<T,E>::operator()()
{
  return distro_(Base<E>::engine());
}

template<typename T, typename E>
  tell::rand::Uniform2<T,E>::Uniform2(T low, T high)
  : rep_(low*low, high*high)
  {
    assert(low <= high);
    assert(high == 0 || high < std::numeric_limits<T>::max() / high); 
  }

template<typename T, typename E>
  T tell::rand::Uniform2<T,E>::operator()()
{
  // T may be integral
  return static_cast<T>(std::sqrt(rep_()));
}

template<typename T, typename E>
  tell::rand::Uniform3<T,E>::Uniform3(T low, T high)
  : rep_(low*low*low, high*high*high)
  {
    assert(low <= high);
    assert(high == 0 || high < std::numeric_limits<T>::max() / (high*high)); 
  }

template<typename T, typename E>
  T tell::rand::Uniform3<T,E>::operator()()
{
  // T may be integral
  return static_cast<T>(std::pow(rep_(), 1.0/3));
}

template<typename T, typename E>
  tell::rand::Uniform_sin<T,E>::Uniform_sin(T low, T high)
  : rep_(std::sin(low), std::sin(high))
  {
    assert(low <= high);
  }

template<typename T, typename E>
  T tell::rand::Uniform_sin<T,E>::operator()()
{
  // T may be integral
  return static_cast<T>(std::asin(rep_()));
}

template<typename G, typename H>
  tell::rand::Pair<G,H>::Pair(const G& g, const H& h)
  : rep_{g,h}
{
}

template<typename G, typename H>
  typename tell::rand::Pair<G,H>::value_type
  tell::rand::Pair<G,H>::operator()()
{
  return {rep_.first(), rep_.second()};
}

template<typename G, std::size_t N>
template<typename... Args>
tell::rand::Array<G,N>::Array(const Args&... args)
  : rep_{tell::make_array(args...)}
{
}

template<typename G, std::size_t N>
  typename tell::rand::Array<G,N>::value_type
  tell::rand::Array<G,N>::Array::operator()()
{
  std::array<T,N> r;
  std::transform(rep_.begin(), rep_.end(), r.begin(), [](auto& g){
      return g();
    });
  return r;
}

template<typename T>
tell::rand::Polar<T>::Polar(T x, T y, T r, T phi)
: x_(x+r*std::cos(phi))
, y_(y+r*std::sin(phi))
{
}

template<typename T>
T tell::rand::Polar<T>::operator[](std::size_t n) const
{
  assert(n < 2);
  return n == 0 ? x_ : y_;
}

template<typename T>
tell::rand::Spherical<T>::Spherical(T x, T y, T z, T r, T phi, T theta)
: x_(x+r*std::cos(theta)*std::cos(phi))
, y_(y+r*std::cos(theta)*std::sin(phi))
, z_(z+r*std::sin(theta))
{
}

template<typename T>
T tell::rand::Spherical<T>::operator[](std::size_t n) const
{
  assert(n < 3);
  switch (n) {
  case 0:
    return x_;
  case 1:
    return y_;
  default:
    return z_;
  }
}

template<typename T, typename E>
  tell::rand::Uniform<tell::rand::Polar<T>, E>::
  Uniform(T x, T y, T r0, T r1, T phi0, T phi1)
  : x_(x)
  , y_(y)
  , r_(r0, r1)
  , phi_(phi0, phi1)
  {
  }

template<typename T, typename E>
  typename tell::rand::Uniform<tell::rand::Polar<T>,E>::value_type
  tell::rand::Uniform<tell::rand::Polar<T>,E>::operator()()
{
  const Polar<T> p(x_, y_, r_(), phi_());
  return {p[0], p[1]};  
}

template<typename T, typename E>
  tell::rand::Uniform<tell::rand::Spherical<T>, E>::
  Uniform(T x, T y, T z, T r0, T r1, T phi0, T phi1, T theta0, T theta1)
  : x_(x)
  , y_(y)
  , z_(z)
  , r_(r0, r1)
  , phi_(phi0, phi1)
  , theta_(theta0, theta1)
  {
  }

template<typename T, typename E>
  typename tell::rand::Uniform<tell::rand::Spherical<T>,E>::value_type
  tell::rand::Uniform<tell::rand::Spherical<T>,E>::operator()()
{
  const Spherical<T> p(x_, y_, z_, r_(), phi_(), theta_());
  return {p[0], p[1], p[2]};  
}

template<typename E>
void tell::rand::Base<E>::seed(unsigned int s) 
{
  engine_.seed(s);
}

template<typename E>
E& tell::rand::Base<E>::engine() 
{
  return engine_;
}

template<typename E>
auto tell::Tracking_engine<E>::operator()()
{
  ++calls_;
  return E::operator()();
}

template<typename E>
std::size_t tell::Tracking_engine<E>::calls() const
{
  return calls_;
}

template<typename E>
tell::Tracking_engine<E>::~Tracking_engine()
{
  std::cerr << "engine calls: " << calls_ << std::endl;
}
