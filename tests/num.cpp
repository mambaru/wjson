#include <wjson/json.hpp>

int main()
{
  const char json[] = "10";
  int val=0;
  ::wjson::value<int>::serializer()(val, json, json + sizeof(json), 0);
  return val;
}
