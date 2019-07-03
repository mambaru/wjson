#include <wjson/json.hpp>
#include <wjson/strerror.hpp>
#include <iostream>

int main()
{
  typedef std::vector< std::string > vect_t;
  typedef ::wjson::array< std::vector< ::wjson::raw_value<std::string> > > vect_json;

  vect_t inv;
  vect_t outv;

  std::string json = "[1,\"2\",[3]]";

  std::cout << json << std::endl;
  vect_json::serializer()( inv, json.begin(), json.end(), NULL );
  //for ( auto& v : inv )
  for (size_t i = 0; i < inv.size(); ++i)
  {
    const std::string& v = inv[i];
    outv.push_back("");
    if ( wjson::parser::is_number(v.begin(), v.end()) )
    {
      int num = 0;
      wjson::value<int>::serializer()( num, v.begin(), v.end(), NULL);
      ++num;
      wjson::value<int>::serializer()( num, std::back_inserter(outv.back()) );
    }
    else if ( wjson::parser::is_string(v.begin(), v.end()) )
    {
      std::string snum;
      wjson::value<std::string>::serializer()( snum, v.begin(), v.end(), NULL);
      int num = 0;
      wjson::value<int>::serializer()( num, snum.begin(), snum.end(), NULL);
      ++num;
      snum.clear();
      wjson::value<int>::serializer()( num, std::back_inserter(snum) );
      wjson::value<std::string>::serializer()( snum, std::back_inserter(outv.back()) );

    }
    else if ( wjson::parser::is_array(v.begin(), v.end()) )
    {
      std::vector<int> vnum;
      wjson::array< std::vector< wjson::value<int> > >::serializer()( vnum, v.begin(), v.end(), NULL);
      wjson::array< std::vector< wjson::value<int> > >::serializer()( vnum, std::back_inserter(outv.back()) );
    }
    else
    {
      outv.back()="null";
    }
  }

  json.clear();
  vect_json::serializer()( outv, std::back_inserter(json) );
  std::cout << json << std::endl;
  // [1,"2",[3]]
  // [2,"3",[4]]
}
