#include <wjson/json.hpp>
#include <wjson/strerror.hpp>
#include <iostream>

int main()
{
  typedef std::map<std::string, int> dict;
  typedef ::wjson::dict_map< ::wjson::value<int> > dict_json;
  // typedef ::wjson::dict< std::map< ::wjson::value<std::string>, ::wjson::value<int> > > dict_json;
  dict d;
  std::string json = "{\"один\":1,\"два\":2,\"три\":3}";
  std::cout << json << std::endl;
  dict_json::serializer()( d, json.begin(), json.end(), NULL );
  d["четыре"]=4;
  json.clear();
  dict_json::serializer()( d, std::back_inserter(json) );
  std::cout << json << std::endl;
  // {"один":1,"два":2,"три":3}
  // {"два":2,"один":1,"три":3,"четыре":4}
}
