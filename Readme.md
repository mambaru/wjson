
[![tag](https://img.shields.io/github/v/tag/mambaru/wjson.svg?sort=semver)](https://github.com/mambaru/wjson/tree/master)
[![Build Status](https://github.com/mambaru/wjson/workflows/C++%20CI/badge.svg?branch=master)](https://github.com/mambaru/wjson/tree/master)
[![Build Status](https://github.com/mambaru/wjson/workflows/C++%20CI/badge.svg?branch=mambaru)](https://github.com/mambaru/wjson/tree/mambaru)
[![Build Status](https://travis-ci.com/mambaru/wjson.svg?branch=master)](https://travis-ci.com/mambaru/wjson)
[![Build Status](https://travis-ci.com/mambaru/wjson.svg?branch=mambaru)](https://travis-ci.com/mambaru/wjson)
[![codecov](https://codecov.io/gh/mambaru/wjson/branch/master/graph/badge.svg)](https://codecov.io/gh/mambaru/wjson)

# JSON сериализатор

* Статья на [хабре](https://habrahabr.ru/post/311262/) с примерами и бенчмарками.
* Документация [doxygen](https://mambaru.github.io/wjson/index.html)
* Репозитарий на [github.com](https://github.com/mambaru/wjson)
* Отчет [coverage](https://migashko.github.io/faslib/cov-report/index.html)

Простой в использовани, быстрый, декларативный сериализатор/десериализатор непосредственно в структуры данных.
Например для структуры:

```cpp
struct foo
{
  int foo1 = 0;
  std::string foo2;
  std::vector<std::string> foo3;
};
```

Нужно создать JSON-описание:
```cpp
struct foo_json
{
  JSON_NAME(foo1)
  JSON_NAME(foo2)
  JSON_NAME(foo3)

  typedef wjson::object<
    foo,
    wjson::member_list<
      wjson::member<n_foo1, foo, int, &foo::foo1>,
      wjson::member<n_foo2, foo, std::string, &foo::foo2>,
      wjson::member<n_foo3, foo, std::vector<std::string>, &foo::foo3, iow::json::vector_of_strings<10> >
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};
```
И можно сериализовывать:
```cpp
foo f;
std::string json;
foo_json::serializer()( f, std::back_inserter(json) );
```
Или сразу в stdout:
```cpp
foo_json::serializer()( f, std::ostreambuf_iterator<char>(std::cout) );
```
Для десериализации нужна пара итераторов произвольного доступа:
```cpp
wjson::error e;
foo_json::serializer()( f, json.begin(), json.end(), &e );
```
Если есть ошибка:
```cpp
  if ( e )
  {
    std::cout << "Error code: " << e.code() << std::endl;
    std::cout << "Error tail of: " << e.tail_of() << std::endl;
    std::cout << "Error position: " << wjson::strerror::where(e, json.begin(), json.end() ) << std::endl;
    std::cout << "Error expected of: " << e.expected_of() << std::endl;
    std::cout << "Error message: " << wjson::strerror::message(e) << std::endl;
    std::cout << "Error trace: " << wjson::strerror::trace(e, json.begin(), json.end()) << std::endl;
  }
```

[Подробнее...](https://github.com/mambaru/wjson/wiki)
