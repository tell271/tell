#include "tell/argrt.h"
#include <cassert>
#include <exception>
#include <functional>
#include <iostream>
#include <tuple>

void fun(int n)
{
  std::cout << "n: " << n << std::endl;
}

template<typename T>
void fun_tpl(T n)
{
  std::cout << "tpl_f: " << n << std::endl;
}

double d_glob = 3.14;
int g_glob = 2911939;
int main(int argc, const char* argv[])
{
  try {
    using namespace tell::argrt;
    Arg d("-d", "global double", std::ref(d_glob));
    Arg f("-f", "boolean flag", false);
    Arg g("-g", "global int", std::ref(g_glob));
    Arg n("-n", "integer", 42);
    Arg s("-s", "string", std::string("hello"));
    Arg help("-h", "show usage?", false);
    Arg_base::parse(argc, argv);
    if (help) {
      Arg_base::usage(std::cerr, argv[0]);
      return 1;
    }
    fun(n);
    fun_tpl(*n);
    std::cout << "len(s): " << (*s).size() << std::endl;
    assert(d == d_glob);
    std::cerr << "d: " << *d << ", d_glob: " << d_glob << std::endl;
    *d = 0.618;
    assert(d == d_glob);
  }
catch (const std::exception& x) {
    std::cerr << x.what() << std::endl;
    return 1;
  }
}
