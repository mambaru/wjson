#include <fas/testing.hpp>
#include <iow/json/json.hpp>


UNIT(array1, "Одномерный массив")
{
  using namespace fas::testing;
  using namespace iow::json;

  std::string json;
  int nums[5]={1,2,3,4,5};
  array< value<int>[5] >::serializer ser;
  ser(nums, std::back_inserter(json) );
  t << equal<expect>( json, "[1,2,3,4,5]") << FAS_TESTING_FILE_LINE;

  std::fill_n(nums, 5, 0);
  json_error e;
  ser(nums, json.begin(), json.end(), &e);
  t << is_false<expect>(e) << e.message( json.begin(), json.end() ) << ": " << FAS_TESTING_FILE_LINE;

  json.clear();
  ser(nums, std::back_inserter(json) );
  t << equal<expect>( json, "[1,2,3,4,5]") << FAS_TESTING_FILE_LINE;

  // Проверки ошибок
  json = "1,2,3,4,5]";
  e.reset();
  ser(nums, json.begin(), json.end(), &e);
  std::string error = e.message( json.begin(), json.end() );
  t << message("Test JSON error: ") << error;
  t << is_false<expect>(e) << e.message( json.begin(), json.end() ) << ": " << FAS_TESTING_FILE_LINE;
  t << equal<expect>(error, "Expected of  '[': >>>1,2,3,4,5]") << FAS_TESTING_FILE_LINE;
  
  json = "[1,2,3,4,5";
  e.reset();
  ser(nums, json.begin(), json.end(), &e);
  t << is_false<expect>(e) << e.message( json.begin(), json.end() ) << ": " << FAS_TESTING_FILE_LINE;

  json = "[1,2,3,[4,5]";
  e.reset();
  ser(nums, json.begin(), json.end(), &e);
  t << is_false<expect>(e) << e.message( json.begin(), json.end() ) << ": " << FAS_TESTING_FILE_LINE;

  json = "[1,2,3 4,5]";
  e.reset();
  ser(nums, json.begin(), json.end(), &e);
  t << is_false<expect>(e) << e.message( json.begin(), json.end() ) << ": " << FAS_TESTING_FILE_LINE;

  json = "[1,2,'3 4',5]";
  e.reset();
  ser(nums, json.begin(), json.end(), &e);
  t << is_false<expect>(e) << e.message( json.begin(), json.end() ) << ": " << FAS_TESTING_FILE_LINE;
}

UNIT(array2, "Двумерный массив")
{
  using namespace fas::testing;
  using namespace iow::json;

  std::string json;
  int nums[2][2]={{1,2},{3,4}};
  typedef array< value<int>[2] > array1_json;
  typedef array< array1_json[2] > array2_json;
  array2_json::serializer ser;
  ser(nums, std::back_inserter(json) );
  t << equal<expect>( json, "[[1,2],[3,4]]") << json << FAS_TESTING_FILE_LINE;

  json="[[4,3],[2,1]]";
  json_error e;
  ser(nums, json.begin(), json.end(), &e);
  t << is_false<expect>(e) << e.message( json.begin(), json.end() ) << ": " << FAS_TESTING_FILE_LINE;
  json.clear();
  ser(nums, std::back_inserter(json) );
  t << equal<expect>( json, "[[1,2],[3,4]]") << json << FAS_TESTING_FILE_LINE;
}

UNIT(array3, "Трехмерный массив")
{
  using namespace fas::testing;
  using namespace iow::json;

  std::string json;
  int nums[2][2][1]={{{1},{2}},{{3},{4}}};
  typedef array< value<int>[1] > array1_json;
  typedef array< array1_json[2] > array2_json;
  typedef array< array2_json[2] > array3_json;
  array3_json::serializer ser;
  ser(nums, std::back_inserter(json) );
  t << equal<expect>( json, "[[1,2],[3,4]]") << json << FAS_TESTING_FILE_LINE;

  json="[[4,3],[2,1]]";
  json_error e;
  ser(nums, json.begin(), json.end(), &e);
  t << is_false<expect>(e) << e.message( json.begin(), json.end() ) << ": " << FAS_TESTING_FILE_LINE;
  json.clear();
  ser(nums, std::back_inserter(json) );
  t << equal<expect>( json, "[[1,2],[3,4]]") << json << FAS_TESTING_FILE_LINE;
}


BEGIN_SUITE(array, "")
  ADD_UNIT(array1)
  ADD_UNIT(array2)
  ADD_UNIT(array3)
END_SUITE(array)