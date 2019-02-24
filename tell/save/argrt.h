#include <iosfwd>
#include <map>
#include <sstream>
#include <string>
#include <variant>

namespace tell::argrt
{
  class Arg_base
  {
    using Table = std::map<std::string, Arg_base*>;
  public:
    static void parse(int argc, const char* argv[]);

    Arg_base(const std::string&, const std::string&);
    
    Arg_base(const Arg_base&) = delete;
    Arg_base(Arg_base&&) = delete;
    Arg_base& operator=(const Arg_base&) = delete;
    Arg_base& operator=(Arg_base&&) = delete;

    virtual ~Arg_base() = default;
    
    static void usage(std::ostream&, const std::string& progname);
  private:
    static inline Table registry_;
    std::string id_;
    std::string comment_;
    virtual void read(int* arg_index, const char* argv[]) = 0;
    virtual void print(std::ostream&) const = 0;
  };

  // argument value, stored or tied to existing variable
  template<typename T>
    using Arg_value = std::variant<T, T*>;

  template<typename T>
    T& value(Arg_value<T>& d);
  
  template<typename T>
    T operator*(const Arg_value<T>& d);
  
  template<typename T>
    class Arg : public Arg_base
    {
    public:
      Arg(const std::string& i, const std::string& c, Arg_value<T> v = T{});
      operator T() const;
      T& operator*();
      T operator*() const;
    private:
      Arg_value<T> value_;
      void read(int* arg_index, const char* argv[]) override;
      void print(std::ostream& s) const override;
    };
  
  template<>
    class Arg<bool> : public Arg_base
  {
  public:
    Arg(const std::string& i, const std::string& c, Arg_value<bool> v = false);
    operator bool() const;    
    bool& operator*();
    bool operator*() const;
  private:
    Arg_value<bool> value_;
    void read(int* arg_index, const char* argv[]) override;
    void print(std::ostream& s) const override;
  };
}

inline
tell::argrt::Arg_base::Arg_base(const std::string& i, const std::string& c)
: id_(i)
, comment_(c)
{
  registry_[i] = this;
}

template<typename T>
T& tell::argrt::operator*(Arg_value<T>& d)
{
  return d.index() == 0 ? std::get<0>(d) : *std::get<1>(d);
}

template<typename T>
T tell::argrt::operator*(const Arg_value<T>& d)
{
  return d.index() == 0 ? std::get<0>(d) : *std::get<1>(d);
}

template<typename T>
tell::argrt::Arg<T>::Arg(const std::string& i, const std::string& c, Arg_value<T> v)
: Arg_base(i, c)
, value_(v)
{
}

template<typename T>
tell::argrt::Arg<T>::operator T() const
{
  return *value_;
}

template<typename T>
T& tell::argrt::Arg<T>::operator*()
{
  return *value_;
}

template<typename T>
T tell::argrt::Arg<T>::operator*() const
{
  return *value_;
}

template<typename T>
void tell::argrt::Arg<T>::read(int* i, const char* argv[])
{
  std::istringstream str(argv[++*i]);
  str >> *value_;
}

template<typename T>
void tell::argrt::Arg<T>::print(std::ostream& s) const
{
  s << *value_;
}

inline
tell::argrt::Arg<bool>::Arg(const std::string& i, const std::string& c,
			    Arg_value<bool> v)
  : Arg_base(i, c)
  , value_(v)
{
}

inline
tell::argrt::Arg<bool>::operator bool() const
{
  return *value_;
}

inline
bool& tell::argrt::Arg<bool>::operator*()
{
  return *value_;
}

inline
bool tell::argrt::Arg<bool>::operator*() const
{
  return *value_;
}

inline
void tell::argrt::Arg<bool>::read(int*, const char* argv[])
{
  *value_ = *value_ ? false : true;
}

inline
void tell::argrt::Arg<bool>::print(std::ostream& s) const
{
  s << (*value_ ? "On" : "Off");
}

