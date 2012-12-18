#ifndef JSONCONFIG_EXCEPTION_HPP_
#define JSONCONFIG_EXCEPTION_HPP_

#include <stdexcept>
#include <string>
#include <typeinfo>

#include <pficommon/text/json.h>

namespace jsonconfig {

class ConfigError : public std::exception {
 public:
  ConfigError(const std::string& path,
              const std::string& message);

  ~ConfigError() throw();

  const std::string& GetPath() const {
    return path_;
  }

  const std::string& GetMessage() const {
    return message_;
  }

  const char* what() const throw() {
    return message_.c_str();
  }

 private:
  const std::string path_;
  const std::string message_;
};

class TypeError : public ConfigError {
 public:
  TypeError(const std::string& path,
            pfi::text::json::json::json_type_t expect,
            pfi::text::json::json::json_type_t actual);

  ~TypeError() throw();

  pfi::text::json::json::json_type_t GetExpect() const {
    return expect_;
  }

  pfi::text::json::json::json_type_t GetActual() const {
    return actual_;
  }

  private:
  const pfi::text::json::json::json_type_t expect_;
  const pfi::text::json::json::json_type_t actual_;
};

class OutOfRange : public ConfigError {
 public:
  OutOfRange(const std::string& path,
             size_t size, size_t index);

  ~OutOfRange() throw();

  size_t GetSize() const {
    return size_;
  }

  size_t GetIndex() const {
    return index_;
  }

 private:
  size_t size_;
  size_t index_;
};

class NotFound : public ConfigError {
 public:
  NotFound(const std::string& path, const std::string& key);

  ~NotFound() throw();

  const std::string& GetKey() const {
    return key_;
  }

 private:
  std::string key_;
};

}
#endif // JSONCONFIG_EXCEPTION_HPP_
