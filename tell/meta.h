#pragma once

#include <functional>
#include <iostream>
#include <tuple>
#include <type_traits>
#include <utility>

namespace tell
{
  //
  // call ith function in a tuple with the given arguments
  //
  namespace impl
  {
    template<typename F, typename... Args>
      void call_if_meqn(int m, int n, const F& fs, std::tuple<Args...>& args)
    {
      if (n < 0 || m == n) {
	std::apply([&fs](auto&&... a) {(fs(std::forward<Args>(a)...));}, args);
      }
    }
  }

  // call n-th of a tuple of functions, all if n is -1
  template<typename... Fs, typename... Args>
    void call_nth(int n, const std::tuple<Fs...>& fs, Args&&... args)
  {
    // move capture of parameter pack become possible with c++20
    auto t = std::make_tuple(std::forward<Args>(args)...);
    std::apply([n, &t](const auto&... f) {
	int m = 0;
	(impl::call_if_meqn(m++, n, f, t),...);
      },
      fs);
  }

#if 0
  // alternative approach to above
  template<typename I, typename J, typename F>
    struct Call_if_eq
    {
    Call_if_eq(I i, J j, const F& f) : i_(i), j_(j), f_(f) {}
      template<typename... Args>
      void operator()(Args&&... args) {
	if (i_ == j_) f_(std::forward<Args>(args)...);
      }
    private:
      I i_;
      J j_;
      F f_;
    };
#endif

  // for dealing with std::reference_wrapper class members
  template<typename T>
    struct Unwrap
    {
      using value_type = T;
      using store_type  = T;
    };
  
  template<typename T>
    struct Unwrap<std::reference_wrapper<T>>
    {
      using value_type = T;
      using store_type = T&;
    };

  // compile-time string
  template<char... C>
    struct ct_string
    {
      const char* c_str() const
      {
	static const char r[] = {C..., '\0'};
	return r;
      }
    };
  
  template<char... C>
    std::ostream& operator<<(std::ostream& ost, const ct_string<C...>& s)
    {
      return ost << s.c_str();
    }

  // user defined literal for compile-time strings
  template<typename CharT, CharT ...s>
    constexpr auto operator"" _S()
  {
    return ct_string<s...>{};
  }
  
  //
  // size function for tuples and index_sequences
  //

  template<typename Tuple>
    constexpr std::size_t size(const Tuple&);

  template<typename... Ts>
    constexpr std::size_t size(const std::tuple<Ts...>&)
    {
      // return std::tuple_size_v<std::tuple<Ts...>>;
      return sizeof...(Ts);
    }

  template<std::size_t... Is>
    constexpr std::size_t size(const std::index_sequence<Is...>&)
    {
      return sizeof...(Is);
    }

  //
  // get for index_sequence
  //

  namespace impl
  {
    template<std::size_t J, typename T>
      struct Getter; // undefined
    
    template<std::size_t J, std::size_t... Is>
      struct Getter<J, std::index_sequence<Is...>>; // undefined
    
    template<std::size_t I, std::size_t... Is>
      struct Getter<0, std::index_sequence<I, Is...>>
      {
	static constexpr const std::size_t value = I;
      };
    
    template<std::size_t J, std::size_t I, std::size_t... Is>
      struct Getter<J, std::index_sequence<I, Is...>>
      {
	static_assert(0 < J);
	static constexpr const std::size_t value
	  = Getter<J-1, std::index_sequence<Is...>>::value;
      };    
  }
  
  template<std::size_t J, std::size_t... Is>
    constexpr std::size_t get(const std::index_sequence<Is...>& is)
    {
      static_assert(J < size(is));
      return impl::Getter<J, std::index_sequence<Is...>>::value;
    }
  
  //
  // slice for tuples and index sequences
  //
  
  template<typename Tuple, std::size_t... Is>
    constexpr auto slice(const Tuple& t, std::index_sequence<Is...>) 
    {
      return std::make_tuple(std::get<Is>(t)...);
    }

  template<std::size_t... Ts, std::size_t... Is>
    constexpr auto slice(const std::index_sequence<Ts...>& ts,
			     std::index_sequence<Is...> is)
    {
      return std::index_sequence<get<Is>(ts)...>{};
    }
  
  //
  // pop front
  //

  template<std::size_t... Is>
    constexpr auto pop_front(std::index_sequence<Is...> is)
    {
      static_assert(size_f(is) > 0);
    }

  template<std::size_t I, std::size_t... Is>
    constexpr auto pop_front(std::index_sequence<I, Is...>)
    {
      return std::index_sequence<Is...>{};
    }

  template<typename... T>
    constexpr auto pop_front(const std::tuple<T...>& t)
    {
      constexpr auto is = pop_front(std::index_sequence_for<T...>{});
      return slice(t, is);
    }

  //
  // zip
  //

  // ordinary function to handle empty case
  constexpr auto zip(const std::tuple<>& t, const std::tuple<>& u)
  {
    return std::tuple<>{};
  }

  // template function to handle generale case using recursion
  template<typename T0, typename... T,
    typename U0, typename... U>
    constexpr auto zip(const std::tuple<T0, T...>& t,
		       const std::tuple<U0, U...>& u)
    {
      const auto head
	= std::make_tuple(std::make_pair(std::get<0>(t), std::get<0>(u)));
      const auto tail = zip(pop_front(t), pop_front(u));
      return std::tuple_cat(head, tail);
    }

  //
  // i-th column of tuple of tuples (pictured as stacked vertically)
  //
  template<std::size_t I, typename Tuple>
    constexpr auto col(const Tuple&);// undefined

  template<std::size_t I, typename... Ps>
    constexpr auto col(const std::tuple<Ps...>& ps)
  {
    return std::apply([](auto&... p) {
	return std::make_tuple(std::get<I>(p)...);
      },
      ps);
  }
  
  //
  // index of first occurrence of type V in tuple
  //

  namespace impl
  {
    template<typename V, typename T>
      struct Index; // undefined

    // empty tuple - note that zero is one beyond the end
    template<typename V>
      struct Index<V, std::tuple<>>
      {
	static std::integral_constant<std::size_t, 0> value;
      };

    // first type matches: return 0
    template<typename V, typename... T>
      struct Index<V, std::tuple<V, T...>>
      {
	static std::integral_constant<std::size_t, 0> value;
      };

    // first type doesn't match: recursion
    template<typename V, typename H, typename... T>
      struct Index<V, std::tuple<H, T...>>
      {
	static_assert(!std::is_same<V,H>::value);
	static constexpr auto r = 1 + Index<V, std::tuple<T...>>::value;
	static_assert(r != 0);// guard against (unlikely:-) wrap-around
	static std::integral_constant<std::size_t, r> value;
      };
  }
  
  // helper variable template
  template<typename V, typename T>
    inline constexpr std::size_t index_v = impl::Index<V, T>::value;

  // helper function templates - deduce types from arguments 
  template<typename V, typename T>
    constexpr std::size_t index(const V&, const T&)
  {
    return index_v<V, T>;
  }
  
  // helper function templates - deduce type T from argument
  template<typename V, typename T>
    constexpr std::size_t index(const T&)
  {
    return index_v<V, T>;
  }

  // helper function template
  template<typename V, typename T>
    constexpr std::size_t index()
  {
    return index_v<V, T>;
  }

  //
  // find tuple by type of first element in tuple of tuples
  //
  
  namespace impl
  {
    template<typename V, typename T>
      struct Vlookup; // undefined

    // empty tuple - note that zero is one beyond the end 
    template<typename V>
      struct Vlookup<V, std::tuple<>>
      {
	static std::integral_constant<std::size_t, 0> value;
      };

    // first type matches: return 0
    template<typename V, typename... W, typename... T>
      struct Vlookup<V, std::tuple<std::tuple<V, W...>, T...>>
      {
	static std::integral_constant<std::size_t, 0> value;
      };

    // first type doesn't match: recursion
    template<typename V, typename H, typename... L, typename... T>
      struct Vlookup<V, std::tuple<std::tuple<H, L...>, T...>>
      {
	static_assert(!std::is_same<V,H>::value);
	static constexpr auto r = 1 + Vlookup<V, std::tuple<T...>>::value;
	static_assert(r != 0);// guard against (unlikely:-) wrap-around
	static std::integral_constant<std::size_t, r> value;
      };
  }

  // helper variable template
  template<typename V, typename T>
    inline constexpr std::size_t vlookup_v = impl::Vlookup<V, T>::value;

  // helper function templates - deduce types V and T from arguments 
  template<typename V, typename T>
    constexpr std::size_t vlookup(const V&, const T&)
  {
    return vlookup_v<V, T>;
  }
  
  // helper function templates - deduce type T from argument
  template<typename V, typename T>
    constexpr std::size_t vlookup(const T&)
  {
    return vlookup_v<V, T>;
  }
    
  // helper function template - specify types explicitly
  template<typename V, typename T>
    constexpr std::size_t vlookup()
  {
    return vlookup_v<V, T>;
  }
  
  //
  // add to tuple of tuples
  //

  // empty tuple of tuples
  inline constexpr auto null2D = std::make_tuple(std::tuple<>{});

  // type of empty tuple of tuples
  using Null2D = decltype(null2D);
  
  template<typename... Us>
    decltype(auto) operator<<(const Null2D&, std::tuple<Us...>&& us)
  {
    return std::make_tuple(std::move(us));
  }
  
  template<typename... Ts, typename... Us>
    decltype(auto) operator<<(std::tuple<Ts...>&& ts,
			      std::tuple<Us...>&& us)
  {
    return std::tuple_cat(std::move(ts), std::make_tuple(std::move(us)));
  }
  
  template<typename... Ts, typename... Us>
    decltype(auto) operator<<(const std::tuple<Ts...>& ts,
			      const std::tuple<Us...>&& us)
  {
    return std::tuple_cat(ts, std::make_tuple(std::move(us)));
  }
  
  template<typename... Us>
    decltype(auto) operator<<(const Null2D&, const std::tuple<Us...>& us)
  {
    return std::make_tuple(us);
  }
  
  template<typename... Ts, typename... Us>
    decltype(auto) operator<<(const std::tuple<Ts...>& ts,
			      const std::tuple<Us...>& us)
  {
    return std::tuple_cat(ts, std::make_tuple(us));
  }
  
  //
  // debugging support
  //

  template<size_t... Is>
    std::ostream& operator<<(std::ostream& ost, std::index_sequence<Is...>)
    {
      ost << "<";
      ((ost << Is << ", "),...);
      return ost << ">";
    }

  template<typename T, typename U>
    std::ostream& operator<<(std::ostream& ost, const std::pair<T,U>& p)
    {
      return ost << "[" << p.first << ", " << p.second << "]";
    }

  inline
    std::ostream& operator<<(std::ostream& ost, const std::tuple<>&)
      {
	return ost << "{}";
      }
    
  template<typename H, typename... T>
    std::ostream& operator<<(std::ostream& ost, const std::tuple<H, T...>& t)
    {
      ost << "{" << std::get<0>(t);
      const auto s = pop_front(t);
      std::apply([&ost](auto&... e) {
	  ((ost << ", " << e),...);
	},
	s
	);
      return ost << "}";
    }
}


