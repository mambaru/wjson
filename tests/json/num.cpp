#include <iow/json/json.hpp>

int main()
{
  constexpr char json[] = "10";
  int val = 0;
  ::iow::json::value<int>::serializer()(val, json, json + 2, 0);
  return val;
}