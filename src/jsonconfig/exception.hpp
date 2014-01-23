#ifndef JSONCONFIG_EXCEPTION_HPP_
#define JSONCONFIG_EXCEPTION_HPP_

#include <stdexcept>
#include <string>
#include <typeinfo>

#include <pficommon/text/json.h>

namespace jsonconfig {

class config_error : public std::exception {
 public:
  config_error(
      const std::string& path,
      const std::string& message);

  ~config_error() throw();

  const std::string& path() const {
    return path_;
  }

  const char* what() const throw() {
    return message_.c_str();
  }

 private:
  const std::string path_;
  const std::string message_;
};

class type_error : public config_error {
 public:
  type_error(
      const std::string& path,
      pfi::text::json::json::json_type_t expect,
      pfi::text::json::json::json_type_t actual);

  ~type_error() throw();

  pfi::text::json::json::json_type_t expect() const {
    return expect_;
  }

  pfi::text::json::json::json_type_t actual() const {
    return actual_;
  }

 private:
  const pfi::text::json::json::json_type_t expect_;
  const pfi::text::json::json::json_type_t actual_;
};

class out_of_range : public config_error {
 public:
  out_of_range(const std::string& path, size_t size, size_t index);

  ~out_of_range() throw();

  size_t size() const {
    return size_;
  }

  size_t position() const {
    return index_;
  }

 private:
  size_t size_;
  size_t index_;
};

class not_found : public config_error {
 public:
  not_found(const std::string& path, const std::string& key);

  ~not_found() throw();

  const std::string& key() const {
    return key_;
  }

 private:
  std::string key_;
};

}  // namespace jsonconfig

#endif  // JSONCONFIG_EXCEPTION_HPP_
