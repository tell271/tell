#include "tell/argrt.h"
#include <cassert>
#include <iostream>

void tell::argrt::Arg_base::parse(int argc, const char* argv[]) 
{
  for (int i = 1; i < argc; ++i) {
    if (registry_.find(argv[i]) == registry_.end()) {
      throw
	std::runtime_error(std::string("unknown argument: ") + argv[i]);
    }
    auto p = registry_[argv[i]];
    assert(p);
    if (i < argc) {
      p->read(&i, argv);
    }
    else {
      throw
	std::runtime_error(std::string("last argument without value: ")
			   + argv[i-1]);
    }
  }
}

void tell::argrt::Arg_base::usage(std::ostream& ost, const std::string& progname)
{
  ost << "usage: " << progname << '\n';
  for (auto& next : registry_) {
    ost << '\t' << next.second->id_
	<< " <" << next.second->comment_ 
	<< ">: ";
    next.second->print(ost);
    ost << std::endl;
  }
}

