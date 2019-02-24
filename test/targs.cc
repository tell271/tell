#include "args.h"
#include <iostream>

int main(int argc, const char* argv[])
{
  try {
    tell::Args args(argc, argv);
    auto x = args.get<double>("x", "x value", 3.14);
    args.usage(); 
    std::cout << "x: " << x << std::endl;
    return 0;
  }
  catch (const std::exception& x) {
    std::cerr << x.what() << std::endl;
    return 1;
  }
}
