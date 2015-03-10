#pragma once

#include <iow/descriptor/descriptor_context.hpp>
#include <memory>
#include <functional>

namespace iow{

template<typename DataType, typename DataPtr = std::unique_ptr<DataType> >
struct pipeline_context: descriptor_context<DataType, DataPtr >
{

};

}
