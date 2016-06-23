#include <string>

namespace iow{ namespace json{

template<>
struct value<char>
{
  typedef char target;
  typedef serializerT< value<char> > serializer;
};

template<>
struct value<unsigned char>
{
  typedef unsigned char target;
  typedef serializerT< value<unsigned char> > serializer;
};

template<>
struct value<short>
{
  typedef short target;
  typedef serializerT< value<short> > serializer;
};

template<>
struct value<unsigned short>
{
  typedef unsigned short target;
  typedef serializerT< value<unsigned short> > serializer;
};


template<>
struct value<int>
{
  typedef int target;
  typedef serializerT< value<int> > serializer;
};

template<>
struct value<unsigned int>
{
  typedef unsigned int target;
  typedef serializerT< value<unsigned int> > serializer;
};

template<>
struct value<long int>
{
  typedef long int target;
  typedef serializerT< value<long int> > serializer;
};

template<>
struct value<unsigned long>
{
  typedef unsigned long target;
  typedef serializerT< value<unsigned long> > serializer;
};


template<>
struct value<long long>
{
  typedef long long target;
  typedef serializerT< value<long long> > serializer;
};

template<>
struct value<unsigned long long>
{
  typedef unsigned long long target;
  typedef serializerT< value<unsigned long long> > serializer;
};

template<>
struct value<bool>
{
  typedef bool target;
  typedef serializerT< value<bool> > serializer;
};

template<>
struct value<std::string>
{
  typedef std::string target;
  typedef serializerT< value<std::string> > serializer;
};

template<>
struct value< std::vector<char> >
{
  typedef std::vector<char> target;
  typedef serializerT< value< target > > serializer;
};

template<int N>
struct value< char[N] >
{
  typedef char target[N];
  typedef serializerT< value<char[N]>  > serializer;
};

template<typename T>
struct value_quoted {
  typedef T target;
  typedef serializerQuoted< T > serializer;
};

}}

