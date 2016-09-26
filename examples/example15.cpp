#include <wjson/json.hpp>
#include <wjson/strerror.hpp>
#include <iostream>

template<char S>
struct flags_json
{
  JSON_NAME2(w1, "жил")
  JSON_NAME2(w2, "был")
  JSON_NAME2(w4, "у")
  JSON_NAME2(w8, "бабушки")
  JSON_NAME2(w16, "серенький")
  JSON_NAME2(w32, "козлик")
 
  typedef ::wjson::flags<
    int,
    ::wjson::member_list<
      ::wjson::enum_value< w1, int, 1>,
      ::wjson::enum_value< w2, int, 2>,
      ::wjson::enum_value< w4, int, 4>,
      ::wjson::enum_value< w8, int, 8>,
      ::wjson::enum_value< w16, int, 16>,
      ::wjson::enum_value< w32, int, 32>
    >,
    S
  > type;

  typedef typename type::serializer serializer;
  typedef typename type::target target;
  typedef typename type::member_list member_list; 
};

int main()
{
  std::string json = "\"жил был серенький козлик\"";
  int val = 0;
  flags_json<' '>::serializer()(val, json.begin(), json.end(), 0 );
  std::cout << json << " = " << val << std::endl;

  std::cout << 63 << " = ";
  flags_json<' '>::serializer()(63, std::ostream_iterator<char>(std::cout) );
  std::cout << std::endl;

  std::cout << 48 << " = ";
  flags_json<','>::serializer()(48, std::ostream_iterator<char>(std::cout) );
  std::cout << std::endl;

  std::cout << 49 << " = ";
  flags_json<'|'>::serializer()(49, std::ostream_iterator<char>(std::cout) );
  std::cout << std::endl;

  // "жил был серенький козлик" = 51
  // 63 = "жил был у бабушки серенький козлик"
  // 48 = ["серенький","козлик"]
  // 49 = "жил|серенький|козлик"
}
