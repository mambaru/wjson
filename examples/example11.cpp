#include <wjson/json.hpp>
#include <wjson/strerror.hpp>
#include <iostream>

int main()
{
  typedef std::vector< std::pair<std::string, std::string> > dict;
  typedef std::map< std::string, int > dict2;
  typedef ::wjson::dict_vector< ::wjson::raw_value<> > dict_json;
  typedef ::wjson::dict_map< ::wjson::value<int> > dict2_json;
  
  dict d;
  dict2 d2;
  std::string json = "{\"один\":1,\"два\":\"2\",\"три\":[3]}";
  std::cout << json << std::endl;
  dict_json::serializer()( d, json.begin(), json.end(), 0 );
  for ( auto& v : d )
  {
    if ( wjson::parser::is_number(v.second.begin(), v.second.end()) )
    {
      int num = 0;
      wjson::value<int>::serializer()( num, v.second.begin(), v.second.end(), 0);
      ++num;
      d2[v.first]=num;
    }
    else if ( wjson::parser::is_string(v.second.begin(), v.second.end()) )
    {
      std::string snum;
      wjson::value<std::string>::serializer()( snum, v.second.begin(), v.second.end(), 0);
      int num = 0;
      wjson::value<int>::serializer()( num, snum.begin(), snum.end(), 0);
      ++num;
      d2[v.first]=num;
    }
    else if ( wjson::parser::is_array(v.second.begin(), v.second.end()) )
    {
      std::vector<int> vnum;
      wjson::array< std::vector< wjson::value<int> > >::serializer()( vnum, v.second.begin(), v.second.end(), 0);
      ++vnum[0];
      d2[v.first]=vnum[0];
    }
  }
  
  json.clear();
  dict2_json::serializer()( d2, std::back_inserter(json) );
  std::cout << json << std::endl;
  // {"один":1,"два":2,"три":3}
  // {"один":1,"два":2,"три":3,"четыре":4}
}
