#ifndef JSONCONFIG_CONFIG_HPP_
#define JSONCONFIG_CONFIG_HPP_

#include <stdint.h>
#include <typeinfo>

#include <pficommon/text/json.h>
#include <pficommon/lang/cast.h>
#include <pficommon/lang/noncopyable.h>

#include "exception.hpp"

namespace jsonconfig {

class Config;

template <class T>
T ConfigCast(const Config& c);

class Config {
 public:
  class Iterator {
   public:
    Iterator(const Config& parent);
    
    const std::string& GetKey() const {
      return it_->first;
    }
    
    Config GetValue() const;
    
    bool HasNext() const {
      return it_ != end_;
    }
    
    void Next() {
      ++it_;
    }
    
   private:
    const Config& parent_;
    pfi::text::json::json::const_iterator it_;
    pfi::text::json::json::const_iterator end_;
  };

  template <typename T>
  T As() const {
    return ConfigCast<T>(*this);
  }

  Config operator[](size_t index) const;

  Config operator[](const std::string& key) const;

  bool Include(const std::string& key) const;

  Iterator GetIterator() const;

  size_t Size() const;

  const pfi::text::json::json& Get() const { return json_; }

  const std::string& GetPath() const { return path_; }

  bool IsNull() const;

 private:
  friend class ConfigRoot;

  Config(const pfi::text::json::json& j,
         const std::string& path) : json_(j), path_(path) {}

  Config();

  pfi::text::json::json::json_type_t GetActualType() const {
    return json_.type();
  }

  const pfi::text::json::json& json_;
  const std::string path_;
};

class ConfigRoot : public Config {
 public:
  ConfigRoot(const pfi::text::json::json& j)
      : Config(json_, ""), json_(j) {}

 private:
  const pfi::text::json::json json_;
};

ConfigRoot Load(const std::string& path);

}

#endif // JSONCONFIG_CONFIG_HPP_
