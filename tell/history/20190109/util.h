#pragma once

#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include <utility>

namespace tell
{
  //
  // bisection, instrumentable
  //

  struct NullGuard {};

  // returns a point closer than tol to a zero of f
  // pre-condition: x0 < x1, f(x0) < 0 and 0 < f(x1), 0 < tol
  template<typename Guard, typename F, typename T>
  T find_root(F f, T x0, T x1, T tol)
  {
    while (tol < x1 - x0) {
      auto guard [[maybe_unused]] = Guard{};
      const double xm = (x0+x1)/2;
      const double ym = f(xm);
      if (ym < 0) {
	x0 = xm;
      }
      else if (0 < ym) {
	x1 = xm;
      }
      else {
	return xm;
      }
    }
    return (x0+x1)/2;
  }

  template<typename F, typename T>
  T find_root(F f, T x0, T x1, T tol)
  {
    return find_root<NullGuard, F>(f, x0, x1, tol);
  }

  // largest integer with function value smaller than or equal to zero
  // pre-condition: f monotone, n0 < n1, f(n0) <= 0 and 0 < f(n1)
  template<typename Guard, typename F, typename N>
  N floor_root(F f, N n0, N n1)
  {
    while (1 < n1 - n0) {
      auto guard [[maybe_unused]] = Guard{};
      const auto nm = (n0+n1)/2;
      const double ym = f(nm);
      if (ym <= 0) {
	n0 = nm;
      }
      else {
	n1 = nm;
      }	
    }
    return n0;
  }

  template<typename F, typename N>
  N floor_root(F f, N n0, N n1)
  {
    return floor_root<NullGuard, F>(f, n0, n1);
  }

  //
  // integer sequence generator
  //
  template<typename T>
    struct Iota
    {
      Iota(T n);
      T operator()();
    private:
      T n;
      T d;
    };

  //
  // Stroustrup's finaliser
  //
  template<typename F>
    struct Final_action
    {
      Final_action(F f); 
      ~Final_action();
    private:
      F clean_;
    };

  template<typename F> auto finally(F f);

  //
  // https://stackoverflow.com/questions/12560291/set-back-default-precision-c
  //
  template<typename Stream>
    class Ios_saver
    {
    public:
      Ios_saver(Stream& stream);      
      ~Ios_saver();
    private:
      Stream& stream;
      std::ios state{nullptr};
    };

  //
  // lexical cast
  //
  template<typename S, typename T> S lexical_cast(T v);

  //
  // Safe Bool: https://www.artima.com/cppsource/safebool2.html
  //
  template<typename T>
    class Testable
    {
      using bool_type = void (Testable::*)() const;
      using pboolmem = bool (T::*);
      void bool_type_fun() const {}
    public:
      Testable(pboolmem pbm);
      operator bool_type() const;
     private:
      pboolmem pbm_;
    };

  //
  // simple timer
  //
  template<typename P = std::chrono::microseconds>
    class Stop_watch
    {
      using Clock = std::chrono::high_resolution_clock;
    public:
      Stop_watch(const char* label = "run");
      ~Stop_watch();
    private:
      const char* label;
      Clock::time_point t = Clock::now();
    };

  template<typename P>
    inline const char* precision;
  template<>
    inline const char* precision<std::chrono::nanoseconds> = "ns";
  template<>
    inline const char* precision<std::chrono::microseconds> = "mu";
  template<>
    inline const char* precision<std::chrono::milliseconds> = "ms";
  template<>
    inline const char* precision<std::chrono::seconds> = "s";

  // recursive calculation of average and standard deviation
  class Avg
  {
  public:
    Avg& operator<<(double x);
    std::pair<double,double> operator()() const;
  private:
    std::size_t n = 0;
    double mu = 0;
    double var = 0;
  };

  // timer with statistics
  template<typename P = std::chrono::microseconds>
    class Timer
    {
      using Clock = std::chrono::high_resolution_clock;
    public:
      Timer(const char* label = "run");
      ~Timer();
      static std::ostream& print_stats(std::ostream&, bool seqno = false);
    private:
      static inline std::map<const char*, Avg> stats;
      const char* const label;
      Clock::time_point t = Clock::now();
    };

  // for counting function calls / loop iterations
  struct Counter_base
  {
    static std::ostream& print_stats(std::ostream&);
  protected:
#ifndef NDEBUG
    static inline std::map<const char*, std::size_t> stats; 
#endif
  };
  
  template<const char* label>
    struct Counter : Counter_base
    {
      Counter() = default;
      ~Counter();
    };
  
  // simple histograms
  template<typename I>
  class Histo
  {
  public:
    Histo& operator<<(I);
    Histo& operator<<(const std::vector<I>& v);
    std::ostream& print(std::ostream& ost) const;
  private:
    std::map<I, std::size_t> freq;
  };

  //
  // stream input/output for std::vectors
  //
  template<typename T>
    std::ostream& operator<<(std::ostream& ost, const std::vector<T>& v);
  template<typename T>
    std::istream& operator>>(std::istream& ist, std::vector<T>& v);
}

template<typename T>
tell::Iota<T>::Iota(T n) : n(n), d(d)
{
}

template<typename T>
T tell::Iota<T>::operator()()
{
  return n++;
}

template<typename F> auto
tell::finally(F f)
{
  return Final_action<F>(f);
}

template<typename F>
tell::Final_action<F>::Final_action(F f) : clean_(f)
{
}
  
template<typename F>
tell::Final_action<F>::~Final_action()
{
  clean_();
}

template<typename S>
tell::Ios_saver<S>::Ios_saver(S& stream)
: stream(stream)
{
  state.copyfmt(stream);
}

template<typename S>
tell::Ios_saver<S>::~Ios_saver()
{
  stream.copyfmt(state);
}

template<typename S, typename T>
  S tell::lexical_cast(T v)
{
  std::stringstream os;
  os << v;
  S s;
  os >> s;
  return s;
}

template<typename T>
tell::Testable<T>::Testable(pboolmem pbm) : pbm_(pbm)
{
}

template<typename T>
tell::Testable<T>::operator bool_type() const
{
  auto This = static_cast<const T*>(this);
  return This->*pbm_ ? &Testable::bool_type_fun : nullptr;
}

template<typename P>
tell::Stop_watch<P>::Stop_watch(const char* label)
: label(label)
{
}

template<typename P>
tell::Stop_watch<P>::~Stop_watch()
{
  const auto d = Clock::now() - t;
  std::cout 
    << "timing for " << std::right << std::setw(12) << label << ": "
    << std::right << std::setw(20)
    << std::chrono::duration_cast<P>(d).count() 
    << precision<P> << std::endl;
}

inline tell::Avg& tell::Avg::operator<<(double x)
{
  const double h{n == 0 ? 0.0 : 1.0/n};
  ++n;
  mu += (x-mu)/n;
  const double d{x-mu};
  var += h*d*d - var/n;
  return *this;
}

inline std::pair<double,double> tell::Avg::operator()() const
{
  return {mu, std::sqrt(var)};
}

template<typename P>
tell::Timer<P>::Timer(const char* label)
: label(label)
{
}

template<typename P>
tell::Timer<P>::~Timer()
{
  const auto d = Clock::now() - t;
  const double x = std::chrono::duration_cast<P>(d).count();
  stats[label] << x;
}

template<typename P>
std::ostream& tell::Timer<P>::print_stats(std::ostream& ost, bool seqno)
{
  std::size_t i = 0;
  for (const auto& p : stats) {
    double mu, sigma;
    std::tie(mu, sigma) = (p.second)();
    ost
      << "timing for " << std::right << std::setw(10) << p.first;
    if (seqno) {
      ost
	<< " (" << std::right << std::setw(2) << i++ << ')';
    }
    ost
      << std::right << std::setw(15) << mu << precision<P>
      << " +-" << std::right << std::setw(8) << sigma << '\n';
  }
  return ost;
}

template<const char* label>
tell::Counter<label>::~Counter()
{
#ifndef NDEBUG
  stats[label]++;
#endif
}

inline std::ostream& tell::Counter_base::print_stats(std::ostream& ost)
{
#ifndef NDEBUG
  for (const auto& p : stats) {
    ost
      << "count for " << std::right << std::setw(15) << p.first << ": "
      << std::right << std::setw(10) << p.second
      << '\n';
  }
#endif
  return ost;
}

template<typename I>
tell::Histo<I>& tell::Histo<I>::operator<<(I i)
{
  freq[i]++;
  return *this;
}

template<typename I>
tell::Histo<I>& tell::Histo<I>::operator<<(const std::vector<I>& v)
{
  for (auto i : v) {
    *this << i;
  }
  return *this;
}

template<typename I>
std::ostream& tell::Histo<I>::print(std::ostream& ost) const
{
  for (const auto& f : freq) {
    ost << std::setw(5) << f.first << ": ";
    for (decltype(f.second) i = 0; i != f.second; ++i) {
      ost << 'x';
    }
    ost << '\n';
  }
  return ost;
}

template<typename T>
std::ostream& tell::operator<<(std::ostream& ost, const std::vector<T>& v)
{
  ost << '[';
  bool first_time{true};
  for(auto x: v) {
    ost << (first_time ? "" : ", ") << x;
    first_time = false;
  }
  return ost << ']';
}

template<typename T>
std::istream& tell::operator>>(std::istream& ist, std::vector<T>& v)
{
  char c;
  T t;
  ist >> c;
  while (ist) {
    if (ist >> t) {
      v.push_back(t);
    }
    if (ist >> c && c == ']') {
      break;
    }
  }
  return ist;
}
