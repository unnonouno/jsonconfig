
#ifndef JSONCONFIG_CONFIG_HPP_
#define JSONCONFIG_CONFIG_HPP_

#include <stdint.h>
#include <typeinfo>

#include <pficommon/text/json.h>
#include <pficommon/lang/cast.h>
#include <pficommon/lang/noncopyable.h>

namespace jsonconfig {

class config;

template <class T>
T config_cast(const config& c);

class config {
 public:
  class iterator {  // but this is const_iterator
   public:
     typedef pfi::text::json::json::const_iterator iterator_base;
    iterator(const iterator&);
    iterator(const config& parent, const pfi::text::json::json::const_iterator& it);

    const std::string& key() const;
    config value() const;

    // InputIterator
    bool operator==(const iterator& it) const {
      return it_ == it.it_;
    }

    bool operator!=(const iterator& it) const {
      return !(*this == it);
    }

    std::pair<const std::string, pfi::text::json::json> operator*() const {
      return *it_;
    }

    const std::pair<const std::string, pfi::text::json::json>* operator->() const {
      return it_.operator->();
    }
    // FowrardIterator
    const iterator& operator++() {
      ++it_;
      return *this;
    }
    const iterator operator++(int) {
      iterator temp(*this);
      ++it_;
      return temp;
    }

   private:
    const config& parent_;
    pfi::text::json::json::const_iterator it_;
  };

  template <typename T>
  T As() const {
    return config_cast<T>(*this);
  }

  config operator[](size_t index) const;

  config operator[](const std::string& key) const;

  bool contain(const std::string& key) const;

  iterator begin() const;
  iterator end() const;

  size_t size() const;
  const pfi::text::json::json& get() const { return json_; }
  const std::string& path() const { return path_; }

  template <class T>
  bool is() const {
    return pfi::text::json::is<T>(json_);
  }


  pfi::text::json::json::json_type_t type() const {
    return json_.type();
  }

 private:
  friend class config_root;

  config(const pfi::text::json::json& j,
         const std::string& path) : json_(j), path_(path) {}
  config();

  const pfi::text::json::json& json_;
  const std::string path_;
};

class config_root : public config {
 public:
  config_root(const pfi::text::json::json& j)
      : config(json_, ""), json_(j) {}

 private:
  const pfi::text::json::json json_;
};

} // jsonconfig

#endif // JSONCONFIG_CONFIG_HPP_
