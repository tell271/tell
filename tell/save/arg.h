#include <iosfwd>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>

namespace tell
{
  class Arg_base
  {
    using Table = std::map<std::string, Arg_base*>;
  public:
    class Exception;
    
    static void parse(int argc, const char* argv[]);

    Arg_base(const std::string&, const std::string&);
    
    Arg_base(const Arg_base&) = delete;
    Arg_base(Arg_base&&) = delete;
    Arg_base& operator=(const Arg_base&) = delete;
    Arg_base& operator=(Arg_base&&) = delete;

    virtual ~Arg_base() = default;
    
  private:
    static void usage(std::ostream&);
    static inline std::string progname_;
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

  class Arg_base::Exception : public std::runtime_error
  {
  public:
    Exception(const std::string& msg, const std::string& arg);
  };
}

inline
tell::Arg_base::Arg_base(const std::string& i, const std::string& c)
: id_(i)
, comment_(c)
{
  registry_[i] = this;
}

template<typename T>
T& tell::operator*(Arg_value<T>& d)
{
  return d.index() == 0 ? std::get<0>(d) : *std::get<1>(d);
}

template<typename T>
T tell::operator*(const Arg_value<T>& d)
{
  return d.index() == 0 ? std::get<0>(d) : *std::get<1>(d);
}

template<typename T>
tell::Arg<T>::Arg(const std::string& i, const std::string& c, Arg_value<T> v)
: Arg_base(i, c)
, value_(v)
{
}

template<typename T>
tell::Arg<T>::operator T() const
{
  return *value_;
}

template<typename T>
T& tell::Arg<T>::operator*()
{
  return *value_;
}

template<typename T>
T tell::Arg<T>::operator*() const
{
  return *value_;
}

template<typename T>
void tell::Arg<T>::read(int* i, const char* argv[])
{
  std::istringstream str(argv[++*i]);
  str >> *value_;
}

template<typename T>
void tell::Arg<T>::print(std::ostream& s) const
{
  s << *value_;
}

inline
tell::Arg<bool>::Arg(const std::string& i, const std::string& c, Arg_value<bool> v)
  : Arg_base(i, c)
  , value_(v)
{
}

inline
tell::Arg<bool>::operator bool() const
{
  return *value_;
}

inline
bool& tell::Arg<bool>::operator*()
{
  return *value_;
}

inline
bool tell::Arg<bool>::operator*() const
{
  return *value_;
}

inline
void tell::Arg<bool>::read(int*, const char* argv[])
{
  *value_ = *value_ ? false : true;
}

inline
void tell::Arg<bool>::print(std::ostream& s) const
{
  s << (*value_ ? "On" : "Off");
}

