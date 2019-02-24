#include <tell/meta.h>
#include <tell/util.h>
#include <cstring>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <tuple>

namespace tell::argct
{
  // compile-time string representing command line option
  template<char... C>
    using Option = ct_string<C...>;

  // user defined literal for command line option
  template<typename CharT, CharT ...s>
    constexpr auto operator"" _o();
  
  // read argument from command line string
  template<typename Arg>
    bool extract(const char* flag, Arg& val, int* i, const char* argv[]);
  template<>
    bool extract(const char* flag, bool& val, int* i, const char* argv[]);

  // functor for accessing argument value
  template<typename Args>
    struct Getopt
    {  
      Getopt(Args&& args_);
      // return option value
      template<typename Opt> auto& operator()(const Opt& o) const;
      // print usage info
      void operator()(std::ostream& ost, const char* progname) const;
    private:
      static inline std::optional<Args> args{};
    };

  // parse command line arguments, given list of argument specs (declarations)
  template<typename... S>
    decltype(auto)
    handle(int argc, const char* argv[], std::tuple<S...>&& specs);

  // declare argument
  auto decl = [](auto&&... d) {return std::make_tuple(std::move(d)...);};
  
  // start list of argument specifiers
  inline constexpr auto start = std::tuple<std::tuple<>>{};
}

template<typename CharT, CharT ...s>
  constexpr auto tell::argct::operator"" _o()
{
  return Option<s...>{};
}

template<typename Arg>
bool
tell::argct::extract(const char* flag, Arg& val, int* i, const char* argv[])
{
  if (!std::strcmp(flag, argv[*i])) {
    val = ::tell::lexical_cast<Arg>(argv[++*i]);
    return true;
  }
  return false;
}

template<>
bool
tell::argct::extract(const char* flag, bool& val, int* i, const char* argv[])
{
  if (!std::strcmp(flag, argv[*i])) {
    val = val ? false : true;
    return true;
  }
  return false;
}

template<typename Args>
tell::argct::Getopt<Args>::Getopt(Args&& args_)
{
  args = std::move(args_);
}

template<typename Args>
template<typename Opt>
auto& tell::argct::Getopt<Args>::operator()(const Opt& o) const
{
  constexpr auto i = ::tell::vlookup(o, *args);
  static_assert(i < ::tell::size(*args), "unknown option");
  return std::get<2>(std::get<i>(*args));
}

template<typename Args>
void
tell::argct::Getopt<Args>::operator()(std::ostream& ost, const char* progname)
const
{
  ost << "usage: " << progname << '\n';
  std::apply([&ost](const auto&... a) {
      ((ost << '\t' << std::get<0>(a)
	<< " <" << std::get<1>(a)
	<< ">: " << std::boolalpha << std::get<2>(a) << '\n'
	),...);
    },
    *args);
}

template<typename... S>
decltype(auto)
tell::argct::handle(int argc, const char* argv[], std::tuple<S...>&& specs)
{
  using namespace tell;
  for (int i = 1; i < argc; ++i) {
    bool found = false;
    std::apply([&found, &i, argv](auto&... o) mutable {
  	found =
  	  ((extract(std::get<0>(o).c_str(), std::get<2>(o), &i, argv)) || ...);
      },
      specs);
    if (!found) {
      throw std::runtime_error(std::string("unkown argument: ") + argv[i]);
    }
    if (i == argc) {
      throw std::runtime_error(std::string("last argument without value: ")
			       + argv[i-1]);
    }
  }
  return Getopt{std::move(specs)};
}
