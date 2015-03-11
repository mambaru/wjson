#pragma once

#include <iow/descriptor/descriptor_context.hpp>
#include <memory>
#include <functional>

namespace iow{ namespace pipeline{

template<typename DataType, typename DataPtr = std::unique_ptr<DataType> >
struct context: descriptor_context<DataType, DataPtr >
{

};

}}
