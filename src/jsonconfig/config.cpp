#include "config.hpp"

#include <iostream>

#include "exception.hpp"

using namespace std;

namespace jsonconfig {

Config::Iterator::Iterator(const Config& parent)
    : parent_(parent), it_(parent.json_.begin()), end_(parent.json_.end()) {
}

Config Config::Iterator::GetValue() const {
  return Config(it_->second,
                parent_.GetPath() + "." + GetKey());
}

Config Config::operator[](size_t index) const {
  try {
    if (index < json_.size()) {
      std::ostringstream os;
      os << path_ << "[" << index << "]";
      return Config(json_[index], os.str());
    } else
      throw OutOfRange(path_, json_.size(), index);
  } catch(std::bad_cast& e) {
    throw TypeError(path_, pfi::text::json::json::Array, GetActualType());
  }
}

Config Config::operator[](const std::string& key) const {
  try {
    std::ostringstream os;
    os << path_ << "." << key;
    return Config(json_[key], os.str());
  } catch(std::out_of_range& e) {
    throw NotFound(path_, key);
  } catch(std::bad_cast& e) {
    throw TypeError(path_, pfi::text::json::json::Object, GetActualType());
  }
}

bool Config::Include(const std::string& key) const {
  return json_.count(key) > 0;
}

size_t Config::Size() const {
  try {
    return json_.size();
  } catch(std::bad_cast& e) {
    throw TypeError(path_, pfi::text::json::json::Array, GetActualType());
  }
}

Config::Iterator Config::GetIterator() const {
  return Iterator(*this);
}

bool Config::IsNull() const {
  return pfi::text::json::is<pfi::text::json::json_null>(json_.get());
}

ConfigRoot Load(const std::string& path) {
  ifstream ifs(path.c_str());
  if (!ifs)
    throw runtime_error("Cannot open: \"" + path + "\"");

  pfi::text::json::json j;
  ifs >> j;
  return ConfigRoot(j);
}

}
