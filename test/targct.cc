#include "tell/argct.h"
#include <cassert>
#include <exception>
#include <functional>
#include <iostream>

void f(int n)
{
  std::cout << "n: " << n << std::endl;
}

template<typename T>
void f_tpl(T n)
{
  std::cout << "tpl_f: " << n << std::endl;
}

int g_glob = 2911939;
double d_glob = 3.14;
int main(int argc, const char* argv[])
{
  try {
    using namespace tell::argct;
    const auto
      getopt = handle(argc, argv,
		      start
		      << decl("-d"_o, "global double", std::ref(d_glob))
		      << decl("-f"_o, "boolean flag", false)
		      << decl("-g"_o, "global int", std::ref(g_glob))
		      << decl("-n"_o, "integer", 42)
		      << decl("-s"_o, "string", std::string("hello"))
		      << decl("-h"_o, "show usage?", false));
    if (getopt("-h"_o)) {
      getopt(std::cerr, argv[0]);
      return 1;
    }

    std::cout << getopt("-s"_o) << std::endl;

    auto n = getopt("-n"_o);
    f(n);
    f_tpl(n);
    
    const auto s = getopt("-s"_o);
    std::cout << "len(s): " << s.size() << std::endl;

    auto& d = getopt("-d"_o);
    std::cout << "d: " << d << std::endl;
    assert(d == d_glob);
    d = 0.618;
    assert(d == d_glob);    
  }
  catch (const std::exception& x) {
    std::cerr << x.what() << std::endl;
    return 1;
  }
}
