#include <string>

namespace iow{ namespace json{

template<>
struct value<char, -1>
{
  typedef char target;
  typedef serializerT< value<char, -1> > serializer;
};

template<>
struct value<unsigned char, -1>
{
  typedef unsigned char target;
  typedef serializerT< value<unsigned char, -1> > serializer;
};

template<>
struct value<short, -1>
{
  typedef short target;
  typedef serializerT< value<short, -1> > serializer;
};

template<>
struct value<unsigned short, -1>
{
  typedef unsigned short target;
  typedef serializerT< value<unsigned short, -1> > serializer;
};


template<>
struct value<int, -1>
{
  typedef int target;
  typedef serializerT< value<int, -1> > serializer;
};

template<>
struct value<unsigned int, -1>
{
  typedef unsigned int target;
  typedef serializerT< value<unsigned int, -1> > serializer;
};

template<>
struct value<long int>
{
  typedef long int target;
  typedef serializerT< value<long int, -1> > serializer;
};

template<>
struct value<unsigned long, -1>
{
  typedef unsigned long target;
  typedef serializerT< value<unsigned long, -1> > serializer;
};


template<>
struct value<long long, -1>
{
  typedef long long target;
  typedef serializerT< value<long long, -1> > serializer;
};

template<>
struct value<unsigned long long, -1>
{
  typedef unsigned long long target;
  typedef serializerT< value<unsigned long long, -1> > serializer;
};

template<int R>
struct value<float, R>
{
  typedef float target;
  typedef serializerT< value<float, R> > serializer;
};

template<int R>
struct value<double, R>
{
  typedef double target;
  typedef serializerT< value<double, R> > serializer;
};

template<int R>
struct value<long double, R>
{
  typedef long double target;
  typedef serializerT< value<long double, R> > serializer;
};


template<>
struct value<bool, -1>
{
  typedef bool target;
  typedef serializerT< value<bool, -1> > serializer;
};

template<int R>
struct value<std::string, R>
{
  typedef std::string target;
  typedef serializerT< value<std::string, R > > serializer;
};

template<int R>
struct value< std::vector<char>, R >
{
  typedef std::vector<char> target;
  typedef serializerT< value< target, R > > serializer;
};

template<int N>
struct value< char[N], -1 >
{
  typedef char target[N];
  typedef serializerT< value<char[N], -1>  > serializer;
};

template<typename J, bool SerQ, bool ReqQ>
struct raw_quoted 
{
  typedef typename J::target target;
  typedef serializerRQ< J, SerQ, ReqQ > serializer;
};

template<typename J, bool SerQ , bool ReqQ , int R >
struct quoted
{
  typedef typename J::target target;
  typedef serializerQ< J, value<std::string>, SerQ, ReqQ, R> serializer;
};


}}

