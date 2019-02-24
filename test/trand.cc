#include <tell/rand.h>
#include <iostream>

int main(int argc, const char* argv[])
{
  tell::Rand<void>::seed(2911939);
  tell::Rand<int> X;
  for (int i = 0; i != 10; ++i) 
    std::cout << X() << std::endl;
  auto Y = tell::Rand<double>{0,1};
  for (int i = 0; i != 10; ++i)
    std::cout << Y() << std::endl;
}
