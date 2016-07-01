
#include <iow/json/json.hpp>

int main()
{
  constexpr const char json[] = "10";
  int val=0;
  ::iow::json::value<int>::serializer()(val, json, json + sizeof(json), 0);
  return val;
}