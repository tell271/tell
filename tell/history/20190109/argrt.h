#include <tell/meta.h>
#include <tell/util.h>
#include <iomanip>
#include <iosfwd>
#include <map>
#include <string>

namespace tell::argrt
{
  class Arg_base
  {
    using Table = std::map<std::string, Arg_base*>;
  public:
    static void parse(int argc, const char* argv[]);
    static void usage(std::ostream&, const std::string& progname);

    Arg_base(const Arg_base&) = delete;
    Arg_base(Arg_base&&) = delete;
    Arg_base& operator=(const Arg_base&) = delete;
    Arg_base& operator=(Arg_base&&) = delete;

  protected:
    Arg_base(const std::string&, const std::string&);    
    virtual ~Arg_base() = default;
    
  private:
    static inline Table registry_;
    std::string id_;
    std::string comment_;
    virtual void read(int* arg_index, const char* argv[]) = 0;
    virtual void print(std::ostream&) const = 0;
  };
  
  template<typename T>
    class Arg : public Arg_base
    {
      using U = typename ::tell::Unwrap<T>::value_type;
      using S = typename ::tell::Unwrap<T>::store_type;
    public:
      Arg(const std::string& i, const std::string& c, T&& v);
      operator U() const;
      U& operator*();
      const U& operator*() const;
    private:
      S value_;
      void read(int* arg_index, const char* argv[]) override;
      void print(std::ostream& s) const override;
    };

  template<typename T>
    void extract(T& val, int* i, const char* argv[]);
  
  template<>
    void extract(bool& val, int* i, const char* argv[]);
}

inline
tell::argrt::Arg_base::Arg_base(const std::string& i, const std::string& c)
: id_(i)
, comment_(c)
{
  registry_[i] = this;
}

template<typename T>
tell::argrt::Arg<T>::Arg(const std::string& i, const std::string& c, T&& v)
: Arg_base(i, c)
, value_(std::move(v))
{
}

template<typename T>
tell::argrt::Arg<T>::operator U() const
{
  return value_;
}

template<typename T>
const typename tell::argrt::Arg<T>::U& tell::argrt::Arg<T>::operator*() const
{
  return value_;
}

template<typename T>
typename tell::argrt::Arg<T>::U& tell::argrt::Arg<T>::operator*()
{
  return value_;
}

template<typename T>
void tell::argrt::Arg<T>::read(int* i, const char* argv[])
{
  extract(value_, i, argv);
}

template<typename T>
void tell::argrt::Arg<T>::print(std::ostream& s) const
{
  s << std::boolalpha << value_;
}

template<typename T>
void tell::argrt::extract(T& val, int* i, const char* argv[])
{
  std::cerr << "type of val: " << typeid(val).name() << std::endl;
  val = ::tell::lexical_cast<T>(argv[++*i]);
}

template<>
inline void tell::argrt::extract(bool& val, int* i, const char* argv[])
{
  val = val ? false : true;
}


