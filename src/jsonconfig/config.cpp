#include "config.hpp"
#include "exception.hpp"

using namespace std;

namespace jsonconfig {

config::iterator::iterator(const config::iterator& it)
  : parent_(it.parent_), it_(it.it_) {
}

config::iterator::iterator(const config& parent, const pfi::text::json::json::const_iterator& it)
  : parent_(parent), it_(it) {
}

const std::string& config::iterator::iterator::key() const {
  return it_->first;
}

config config::iterator::value() const {
  return config(it_->second,
                parent_.path() + "." + key());
}

config config::operator[](size_t index) const {
  try {
    if (index < json_.size()) {
      std::ostringstream os;
      os << path_ << "[" << index << "]";
      return config(json_[index], os.str());
    } else
      throw out_of_range(path_, json_.size(), index);
  } catch (const std::bad_cast& e) {
    throw type_error(path_, pfi::text::json::json::Array, type());
  }
}

config config::operator[](const std::string& key) const {
  try {
    std::ostringstream os;
    os << path_ << "." << key;
    return config(json_[key], os.str());
  } catch (const std::out_of_range& e) {
    throw not_found(path_, key);
  } catch (const std::bad_cast& e) {
    throw type_error(path_, pfi::text::json::json::Object, type());
  }
}

bool config::contain(const std::string& key) const {
  return json_.count(key) > 0;
}

size_t config::size() const {
  try {
    return json_.size();
  } catch (const std::bad_cast& e) {
    throw type_error(path_, pfi::text::json::json::Array, type());
  }
}

config::iterator config::begin() const {
  return iterator(*this, json_.begin());
}

config::iterator config::end() const {
  return iterator(*this, json_.end());
}

} // jsonconfig
