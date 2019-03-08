[![Coverage Status](https://coveralls.io/repos/github/mambaru/wjson/badge.svg?branch=master)](https://coveralls.io/github/mambaru/wjson?branch=master)

# JSON сериализатор

Статья на [хабре](https://habrahabr.ru/post/311262/) с примерами и бенчмарками.
Репозитарий на [github.com](https://github.com/mambaru/wjson)
Документация [wiki](https://github.com/mambaru/wjson/wiki)
Документация [doxygen](https://mambaru.github.io/wjson/index.html)
Отчет [coverage(TODO)](https://mambaru.github.io/wjson/cov-report/index.html)

Простой в использовании, быстрый, декларативный сериализатор/десериализатор непосредственно в структуры данных. 
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
      wjson::member<n_foo3, foo, std::vector<std::string>, &foo::foo3, wjson::vector_of_strings<10> >
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
