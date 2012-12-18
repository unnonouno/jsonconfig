#include "exception.hpp"

using namespace std;
using pfi::text::json::json;

namespace jsonconfig {

ConfigError::ConfigError(const std::string& path,
                         const std::string& message)
    : path_(path),
      message_(message + " (" + path_ + ")") {
}

ConfigError::~ConfigError() throw() {}

////////

namespace {

const char* TypeToName(json::json_type_t t) {
  switch (t) {
    case json::Null:
      return "Null";
    case json::Integer:
      return "Integer";
    case json::Float:
      return "Float";
    case json::Bool:
      return "Bool";
    case json::String:
      return "String";
    case json::Array:
      return "Array";
    case json::Object:
      return "Object";
    default:
      return "Unknown Type";
  }
}

std::string MakeTypeErrorMessage(json::json_type_t expect,
                                 json::json_type_t actual) {
  return string(TypeToName(expect)) + " is expected, but "
      + TypeToName(actual) + " is given.";
}

}

TypeError::TypeError(const std::string& path,
                     json::json_type_t expect,
                     json::json_type_t actual)
    : ConfigError(path, MakeTypeErrorMessage(expect, actual)),
      expect_(expect), actual_(actual) {}

TypeError::~TypeError() throw() {}

////////

std::string MakeOutOfRangeMessage(size_t size, size_t index) {
  std::ostringstream os;
  os << "Out of range 0.." << size << ": " << index;
  return os.str();
}

OutOfRange::OutOfRange(const std::string& path,
                       size_t size, size_t index)
    : ConfigError(path, MakeOutOfRangeMessage(size, index)),
      size_(size), index_(index) {}

OutOfRange::~OutOfRange() throw() {}

////////

std::string MakeNotFoundMessage(const std::string& key) {
  return "\"" + key + "\" is not found";
}

NotFound::NotFound(const std::string& path, const std::string& key)
    : ConfigError(path, MakeNotFoundMessage(key)), key_(key) {}

NotFound::~NotFound() throw() {}

}
