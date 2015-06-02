#pragma once 

namespace {

std::string good_parse[] = {
  "{\"method\":\"test1\",\"params\":[{},2,3,4,{}]}",
  "{\"method\":\"test1\",\"params\":[1,2,3,4,5]}",
  "{\"jsonrpc\":\"2.0\",\"method\":\"test1\",\"params\":[1,2,3,4,5],\"id\":1}",
  "{\"result\":[1,2,{},4,5],\"id\":\"id\",\"id1\":\"id1\"}",
  "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32601,\"message\":\"Procedure not found.\"},\"id\":1}",
  "{\"jsonrpc\":\"2.0\"}"
};

std::string bad_parse[] = {
  " ",
  " {",
  "/t}",
  "[]",
  "{null}",
  "[\"a\":1]",
  "{\"method\":\"test3\",\"params\":[1,2,{3},4,5],\"id\":1}",
  "{\"method\":\"test3\",\"params\":{1,2,3,4,5},\"id\":1}"
};

std::string bad_request[][2] = {
  { 
    "{\"method\":\"test1\",\"params\":[1,2,3,4,5],\"id\":1}",
    "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32601,\"message\":\"Procedure not found.\"},\"id\":1}"
  },
  { 
    "{\"method\":\"test2\",\"params\":[1,2,3,4,5]}",
    "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32601,\"message\":\"Procedure not found.\"},\"id\":null}"
  },
  {
    "{\"method\":\"test3\",\"id\":[{},{\"id\":1}]}",
    "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32601,\"message\":\"Procedure not found.\"},\"id\":[{},{\"id\":1}]}"
  },
  {
    "{\"method\":null,\"params\":[1,2,3,4,5],\"id\":1}",
    "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32601,\"message\":\"Procedure not found.\"},\"id\":1}"
  },
  {
    "{\"params\":[1,2,3,4,5],\"id\":1}",
    "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32600,\"message\":\"Invalid Request.\"},\"id\":1}"
  },
  {
    "{\"result\":[1,2,3,4,5]}",
    "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32600,\"message\":\"Invalid Request.\"},\"id\":null}"
  }
};


std::string good_request[][2] = {
  
  { 
    "{\"method\":\"method1\",\"params\":[1,2,3,4,5],\"id\":1}",
    "{\"jsonrpc\":\"2.0\",\"result\":[5,4,3,2,1],\"id\":1}"
  },
  { 
    "{\"method\":\"method2\",\"params\":[1,2,3,4,5],\"id\":2}",
    "{\"jsonrpc\":\"2.0\",\"result\":null,\"id\":2}"
  },
  { 
    "{\"method\":\"method1\",\"params\":null,\"id\":1}",
    "{\"jsonrpc\":\"2.0\",\"result\":null,\"id\":1}"
  } 

  /*,
  { 
    "{\"method\":\"test1\",\"params\":[1,2,3,4,5],\"id\":1}",
    "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32601,\"message\":\"Procedure not found.\"},\"id\":1}"
  },
  { 
    "{\"method\":\"test2\",\"params\":[1,2,3,4,5]}",
    "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32601,\"message\":\"Procedure not found.\"},\"id\":null}"
  },
  {
    "{\"method\":\"test3\",\"id\":[{},{\"id\":1}]}",
    "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32601,\"message\":\"Procedure not found.\"},\"id\":[{},{\"id\":1}]}"
  },
  {
    "{\"method\":null,\"params\":[1,2,3,4,5],\"id\":1}",
    "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32601,\"message\":\"Procedure not found.\"},\"id\":1}"
  },
  {
    "{\"params\":[1,2,3,4,5],\"id\":1}",
    "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32600,\"message\":\"Invalid Request.\"},\"id\":1}"
  },
  {
    "{\"result\":[1,2,3,4,5]}",
    "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32600,\"message\":\"Invalid Request.\"},\"id\":null}"
  }*/
};

}