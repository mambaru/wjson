#include <wjson/json.hpp>
#include <fas/system/nullptr.hpp>

int main()
{
  const char json[4] = "10";
  int val=0;
  ::wjson::value<int>::serializer()(val, json, json + sizeof(json), fas_nullptr);
  return val;
}
