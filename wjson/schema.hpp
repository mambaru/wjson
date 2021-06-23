//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once
#include <fas/typemanip/has_member_function.hpp>
#include <fas/integral/bool_.hpp>
#include <vector>
#include <string>

namespace wjson{

namespace detail{
FAS_HAS_STATIC_FUNCTION0T(has_schema0, create_schema)
FAS_HAS_STATIC_FUNCTION1T(has_schema1, create_schema)
FAS_HAS_STATIC_FUNCTION2T(has_schema2, create_schema)
FAS_HAS_STATIC_FUNCTION0T(has_get_schema_list, get_schema_list)
} // namespace helper

class schema
{
  typedef schema self;
public:

  // Detect: static T create_schema()
  template<typename T>
  struct has_schema1_t: detail::has_schema0<T, T> {};

  // Detect static T create_schema(const std::string&);
  template<typename T>
  struct has_schema2_t: detail::has_schema1<T, void, T& > {};

  template<typename T>
  struct has_schema3_t: detail::has_schema1<T, T, const std::string& > {};

  template<typename T>
  struct has_schema4_t: detail::has_schema2<T, bool, T&, const std::string& > {};

  template<typename T>
  struct has_schema_t
  {
    enum { value = static_cast<int>(has_schema1_t<T>::value) +
                   static_cast<int>(has_schema2_t<T>::value) +
                   static_cast<int>(has_schema3_t<T>::value) +
                   static_cast<int>(has_schema4_t<T>::value)
    };
  };

  template<typename T>
  struct has_get_schema_list_t: detail::has_get_schema_list<T, std::vector<std::string> > {};

  template<typename T>
  static bool has_schema()
  {
    return has_schema_t<T>::value != 0;
  }

  template<typename T>
  static bool has_get_schema_list()
  {
    return has_get_schema_list_t<T>::value != 0;
  }

  template<typename T>
  static bool has_schema1()
  {
    return has_schema1_t<T>::value != 0;
  }

  template<typename T>
  static bool has_schema2()
  {
    return has_schema2_t<T>::value != 0;
  }

  template<typename T>
  static bool has_schema3()
  {
    return has_schema3_t<T>::value != 0;
  }

  template<typename T>
  static bool has_schema4()
  {
    return has_schema4_t<T>::value != 0;
  }

  template<typename T>
  static std::vector<std::string> get_schema_list()
  {
    return self::get_schema_list_<T>(fas::bool_<has_get_schema_list_t<T>::value>() );
  }

  template<typename T>
  static bool get_schema_list(std::vector<std::string>& schemas)
  {
    typedef std::vector<std::string>::const_iterator citr_t;
    bool has_new = false;
    std::vector<std::string> cur = self::get_schema_list<T>();
    for ( citr_t itr1 = cur.begin(); itr1!=cur.end(); ++itr1)
    {
      bool found = false;
      for ( citr_t itr2 = schemas.begin(); itr2!=schemas.end(); ++itr2)
      {
        if ( *itr1 == *itr2 )
        {
          found = true;
          break;
        }
      }
      if ( !found )
      {
        has_new = true;
        schemas.push_back(*itr1);
      }
    }
    return has_new;
  }

  template<typename T>
  static T create_schema(const std::string& name = std::string())
  {
    T obj = T();
    create_schema4_<T>(obj, name, fas::bool_<has_schema3_t<T>::value>() );
    return obj;
  }

  template<typename T>
  static bool create_schema(T& obj, const std::string& name = std::string() )
  {
    return create_schema4_<T>(obj, name, fas::bool_<has_schema4_t<T>::value>() );
  }

  template<typename T>
  static bool create_schema(T& obj, const std::vector<std::string>& names )
  {
    if ( !names.empty() )
    {
      typedef std::vector<std::string>::const_iterator citr_t;
      std::vector<std::string> schamas = self::get_schema_list<T>();
      if ( !schamas.empty() )
      {
        for ( citr_t itr2 = names.begin(); itr2!=names.end(); ++itr2 )
        {
          for ( citr_t itr1 = schamas.begin(); itr1!=schamas.end(); ++itr1 )
          {
            if ( *itr1 == *itr2 )
            {
              return self::create_schema<T>(obj, *itr2);
            }
          }
        }
      }
      return self::create_schema<T>(obj, names.back());
    }
    return self::create_schema<T>(obj);
  }

private:

  template<typename T>
  static std::vector<std::string> get_schema_list_(fas::true_)
  {
    return T::get_schema_list();
  }

  template<typename T>
  static std::vector<std::string> get_schema_list_(fas::false_)
  {
    return std::vector<std::string>();
  }

  template<typename T>
  static bool create_schema4_(T& obj, const std::string& name, fas::true_)
  {
    return T::create_schema(obj, name);
  }

  template<typename T>
  static bool create_schema4_(T& obj, const std::string& name, fas::false_)
  {
    return create_schema3_<T>(obj, name, fas::bool_<has_schema3_t<T>::value>() );
  }

  template<typename T>
  static bool create_schema3_(T& obj, const std::string& name, fas::true_)
  {
    obj = T::create_schema(name);
    return true;
  }

  template<typename T>
  static bool create_schema3_(T& obj, const std::string& name, fas::false_)
  {
    return create_schema2_<T>(obj, name, fas::bool_<has_schema2_t<T>::value>() );
  }

  template<typename T>
  static bool create_schema2_(T& obj, const std::string&, fas::true_)
  {
    T::create_schema(obj);
    return true;
  }

  template<typename T>
  static bool create_schema2_(T& obj, const std::string& name, fas::false_)
  {
    return create_schema1_<T>(obj, name, fas::bool_<has_schema1_t<T>::value>() );
  }

  template<typename T>
  static bool create_schema1_(T& obj, const std::string&, fas::true_)
  {
    obj = T::create_schema();
    return true;
  }

  template<typename T>
  static bool create_schema1_(T& obj, const std::string&, fas::false_)
  {
    obj = T();
    return true;
  }
};

}
