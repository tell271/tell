#include "tell/arg.h"
#include <cassert>
#include <cstdlib>
#include <iostream>

tell::Arg_base::Exception::Exception(const std::string& msg,
				     const std::string& arg)
  : std::runtime_error(msg + ": " + arg)
{
}

void tell::Arg_base::parse(int argc, const char* argv[]) 
{
  Arg<bool> usg("-h", "print usage info?");
  progname_ = argv[0];
  for (int i = 1; i < argc; ++i) {
    if (registry_.find(argv[i]) == registry_.end()) {
      throw Exception("unknown argument", argv[i]);
    }
    auto p = registry_[argv[i]];
    assert(p);
    if (i < argc) {
      p->read(&i, argv);
    }
    else {
      throw Exception("last argument without value", argv[i-1]);
    }
  }
  if (usg) {
    usage(std::cerr);
    std::exit(EXIT_SUCCESS);
  }
  registry_.clear();
}

void tell::Arg_base::usage(std::ostream& ost)
{
  ost << "usage: " << progname_ << '\n';
  for (auto& next : registry_) {
    ost << '\t' << next.second->id_
	<< " <" << next.second->comment_ 
	<< ">: ";
    next.second->print(ost);
    ost << std::endl;
  }
}

