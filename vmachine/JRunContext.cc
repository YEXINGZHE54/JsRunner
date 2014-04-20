#include "vmachine/JRunContext.hpp"
#include "vmachine/JObject.hpp"

using namespace jrun::vmachine;

const JRunContextPtr JRunContext::instance()
{
  return JObject::instance();
}