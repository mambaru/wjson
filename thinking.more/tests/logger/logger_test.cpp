#include <iow/logger/logger.hpp>
#include <iostream>
#include <sstream>

int main()
{
  std::stringstream ss;
  iow::init_log([&ss](const std::string& name, const std::string& type, const std::string& text){
    ss << name << " " << type << " "<< text << " " << std::endl;
  });
  IOW_LOG_ERROR("test log error")
  auto result = ss.str();
  std::cout << result;
  
  return 0;
}
