#include <wjson/json.hpp>
#include <wjson/strerror.hpp>
#include <iostream>

int main()
{
  typedef   std::vector<std::string> raw_vect_t;
  raw_vect_t raw_in;
  raw_vect_t raw_out;
  
  std::string json = "[ 42, null, \"абракадабра\", [1,2], {\"x\":[3,4]}, 7, true ]";
  typedef wjson::array< std::vector< wjson::raw_value<std::string> > > raw_json;
  raw_json::serializer()(raw_in, json.begin(), json.end(), nullptr );
  //for ( auto i : raw_in)
  for (size_t k=0; k < raw_in.size(); ++k)
  {
    const std::string& i = raw_in[k];
    if ( wjson::parser::is_null(i.begin(),i.end()) )
    {
      raw_out.push_back(i);
    }
    else if ( wjson::parser::is_bool(i.begin(),i.end()) )
    {
      bool val = false;
      wjson::value<bool>::serializer()(val, i.begin(),i.end(), nullptr );
      std::cout << "Bool " << std::boolalpha << val << " from " << i << std::endl;
      val=true;
      raw_out.push_back(std::string());
      wjson::value<bool>::serializer()( val, std::back_inserter(raw_out.back()) );
    }
    else if ( wjson::parser::is_number(i.begin(),i.end()) )
    {
      int val = 0;
      wjson::value<int>::serializer()(val, i.begin(),i.end(), nullptr );
      std::cout << "Number " << val << " from " << i << std::endl;
      val *= -1;
      raw_out.push_back(std::string());
      wjson::value<int>::serializer()( val, std::back_inserter(raw_out.back()) );
    }
    else if ( wjson::parser::is_string(i.begin(),i.end()) )
    {
      std::string val;
      wjson::value<std::string>::serializer()(val, i.begin(),i.end(), nullptr );
      std::cout << "String " << val << " from " << i << std::endl;
      std::reverse(val.begin(), val.end());
      raw_out.push_back(std::string());
      wjson::value<std::string>::serializer()( val, std::back_inserter(raw_out.back()) );
    }
    else if ( wjson::parser::is_array(i.begin(),i.end()) )
    {
      typedef wjson::array<std::vector< wjson::value<int> > > vjson_t;
      std::vector<int> val;
      vjson_t::serializer()(val, i.begin(),i.end(), nullptr );
      std::cout << "Array " << val[0] << "," << val[1] << " from " << i << std::endl;
      std::reverse(val.begin(), val.end());
      raw_out.push_back(std::string());
      vjson_t::serializer()( val, std::back_inserter(raw_out.back()) );
    }
    else
    {
      std::cout << "Unknown item: " << i << std::endl;
    }
  }

  std::cout << "Serialize: ";
  raw_json::serializer()(raw_out, std::ostreambuf_iterator<char>(std::cout));
  std::cout << std::endl;

}
