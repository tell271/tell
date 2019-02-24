#pragma once

#include <algorithm>
#include <array>
#include <iostream>
#include <limits>
#include <random>
#include <type_traits>
#include <utility>

namespace tell
{
  namespace random
  {
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
      using param_type = typename Distro::param_type;
      Uniform(T low = lims::min(), T high = lims::max());
      T operator()();
      void param(param_type pt);
      param_type param() const;
    private:
      Distro  distro_;
    };
    
    // random variable whose square is uniform 
    template<typename T, typename E>
      class Uniform2 : public Base<E>
    {
      static_assert(std::is_floating_point<T>::value);
      using lims = std::numeric_limits<T>;
    public:
      Uniform2(T low = 0, T high = T(std::sqrt(lims::max())));
      T operator()();
    private:
      Uniform<T,E> u_;
    };

    // pairs of random variables
    namespace impl
    {
      template<typename T, typename U, typename E>
	using UP = std::pair<Uniform<T,E>, Uniform<U,E>>;
    }
    
    template<typename T, typename U, typename E>
      class Uniform<std::pair<T,U>,E> : public Base<E>
    {
      using P = impl::UP<T,U,E>;  
    public:
      using param_type
	= std::pair<
	typename P::first_type::param_type,
	typename P::second_type::param_type>;
     
      template<typename... Args,
	std::enable_if_t<std::is_constructible<P, Args...>::value, int> = 0>
	Uniform(Args&&... args);
      std::pair<T,U> operator()();
      param_type param() const;
      void param(param_type pt);
    private:
      P p;
    };

    // random arrays
    template<typename T, std::size_t N, typename E>
      class Uniform<std::array<T,N>,E> : public Base<E>
    {
      using V = std::array<Uniform<T,E>,N>;
    public:
      Uniform(const V& v);
      Uniform(V&& v);
      std::array<T,N> operator()();
    private:
      V v;
    };

    template<typename G>
      class Polar_uniform;

    template<typename T, typename E>
      class Polar_uniform<Uniform<std::pair<T,T>,E>> : public Base<E>
      {
      public:
	Polar_uniform(const Uniform<std::pair<T,T>,E>&, T x = T{}, T y = T{});
	Polar_uniform(Uniform<std::pair<T,T>,E>&&, T x = T{}, T y = T{});
	std::pair<T,T> operator()();
      private:
	Uniform<std::pair<T,T>,E> g;
	T x;
	T y;
      };

    template<typename T, typename E>
      Polar_uniform<Uniform<std::pair<T,T>,E>>
      polar(const Uniform<std::pair<T,T>,E>& g, T x = T{}, T y = T{})
      {
	return {g, x, y};
      }

    template<typename T, typename E>
      Polar_uniform<Uniform<std::pair<T,T>,E>>
      polar(Uniform<std::pair<T,T>,E>&& g, T x = T{}, T y = T{})
      {
	return {std::move(g), x, y};
      }

    template<typename T, typename E>
      class Uniform_polar : public Base<E>
    {
      static_assert(std::is_floating_point<T>::value);
    public:
      Uniform_polar(const Uniform<T,E>& rho, const Uniform<T,E>& phi,
		    T x = 0, T y = 0);
      std::pair<T,T> operator()();
    private:
      T x;
      T y;
      Uniform<T,E> rho;
      Uniform<T,E> phi;
    };
    
    // convenient access to base engine
    template<typename E>
      class Uniform<void, E> : public Base<E>
    {
    public:
    }; 
  }
  
  template<typename T, typename E = std::default_random_engine>
    using Rand = random::Uniform<T,E>;
  
  template<typename T, typename E = std::default_random_engine>
    using Rand_polar = random::Uniform_polar<T,E>;
  
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
  tell::random::Uniform<T,E>::Uniform(T low, T high)
  : distro_(low, high)
    {
    }

template<typename T, typename E>
  T tell::random::Uniform<T,E>::operator()()
{
  return distro_(Base<E>::engine());
}

template<typename T, typename E>
  tell::random::Uniform2<T,E>::Uniform2(T low, T high)
  : u_(low*low, high*high)
    {
    }

template<typename T, typename E>
  T tell::random::Uniform2<T,E>::operator()()
{
  return std::sqrt(u_());
}

template<typename T, typename E>
  void tell::random::Uniform<T,E>::param(param_type pt)
{
  distro_.param(pt);
}

template<typename T, typename E>
  typename tell::random::Uniform<T,E>::param_type
  tell::random::Uniform<T,E>::param() const
{
  return distro_.param();
}

template<typename E>
void tell::random::Base<E>::seed(unsigned int s) 
{
  engine_.seed(s);
}

template<typename E>
E& tell::random::Base<E>::engine() 
{
  return engine_;
}

template<typename T, typename U, typename E>
  template<typename... Args,
  std::enable_if_t<
  std::is_constructible<tell::random::impl::UP<T,U,E>, Args...>::value,
  int> = 0>
  tell::random::Uniform<std::pair<T,U>,E>::Uniform(Args&&... args)
  : p(std::forward<Args>(args)...)
{
}

template<typename T, typename U, typename E>
  std::pair<T,U> tell::random::Uniform<std::pair<T,U>,E>::operator()()
{
  return {p.first(), p.second()};
}

template<typename T, typename U, typename E>
  typename tell::random::Uniform<std::pair<T,U>,E>::param_type
  tell::random::Uniform<std::pair<T,U>,E>::param() const
{
  return {p.first.param(), p.second.param()};
}

template<typename T, typename U, typename E>
  void tell::random::Uniform<std::pair<T,U>,E>::param(param_type pt)
{
  p.first.param(pt.first);
  p.second.param(pt.second);
}

template<typename T, std::size_t N, typename E>
  tell::random::Uniform<std::array<T,N>,E>::Uniform(const V& v) 
  : v(v)
  {
  }

template<typename T, std::size_t N, typename E>
  tell::random::Uniform<std::array<T,N>,E>::Uniform(V&& v) 
  : v(std::move(v))      
  {
  }

template<typename T, std::size_t N, typename E>
  std::array<T,N> tell::random::Uniform<std::array<T,N>,E>::operator()()
{
  std::array<T,N> r;
  std::transform(v.begin(), v.end(), r.begin(), [](auto& g){
      return g();
    });
  return r;
}

template<typename T, typename E>
  tell::random::Polar_uniform<tell::random::Uniform<std::pair<T,T>,E>>
  ::Polar_uniform(const Uniform<std::pair<T,T>,E>& g0, T x, T y)
  : g(g0)
  , x(x)
  , y(y)
{
    const auto [p,q] = g.param();
    const decltype(p) p1(p.a()*p.a(), p.b()*p.b());
    g.param({p1 ,q});
}

template<typename T, typename E>
  tell::random::Polar_uniform<tell::random::Uniform<std::pair<T,T>,E>>
  ::Polar_uniform(Uniform<std::pair<T,T>,E>&& g0, T x, T y)
  : g(std::move(g0))
  , x(x)
  , y(y)
  {
    const auto [p,q] = g.param();
    const decltype(p) p1(p.a()*p.a(), p.b()*p.b());
    g.param({p1 ,q});
  }

template<typename T, typename E>
std::pair<T,T>
  tell::random::Polar_uniform<tell::random::Uniform<std::pair<T,T>,E>>
  ::operator()()
{
  auto [r2, phi] = g();
  const auto r = std::sqrt(r2);
  return {x+r*cos(phi), y+r*sin(phi)};  
}

template<typename T, typename E>
  tell::random::Uniform_polar<T,E>::Uniform_polar(const Uniform<T,E>& rho,
						  const Uniform<T,E>& phi,
						  T x,
						  T y)
  : rho(rho), phi(phi), x(x), y(y)
  {
    const auto p = rho.param();
    using param_type = typename Uniform<T,E>::param_type;
    this->rho.param(param_type(p.a()*p.a(), p.b()*p.b()));
  }

template<typename T, typename E>
  std::pair<T,T> tell::random::Uniform_polar<T,E>::operator()()
{
  const auto rho0 = std::sqrt(rho());
  const auto phi0 = phi();
  return {x+rho0*cos(phi0), y+rho0*sin(phi0)};
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
