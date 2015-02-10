#include <iow/logger/logstream.hpp>
#include <iostream>


int main()
{
  std::stringstream ss;
  auto writer = [&ss](const std::string& name, const std::string& type, const std::string& text){
    ss << name << " " << type << " "<< text << std::endl;
  };
  
  ::iow::logstream ls1("1", "1", nullptr);
  ls1 <<  "test";
  if ( ls1.str() != "test" )
  {
    std::cout << "1: " << ls1.str() << std::endl;
    return -1;
  }

  {
    ::iow::logstream ls2("2", "2", writer);
    ls2 <<  "test";
    if ( ls2.str() != "test" )
      return -1;
  }

  if ( ss.str() != "2 2 test\n")
  {
    std::cout << "2: [" << ss.str() << "]" << std::endl;
    return -1;
  }

  return 0;
}
