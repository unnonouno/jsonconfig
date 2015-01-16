#ifndef JSOCONFIG_CAST_HPP_
#define JSOCONFIG_CAST_HPP_

#include <stdint.h>
#include <map>
#include <string>
#include <vector>

#include <pficommon/data/unordered_map.h>
#include <pficommon/lang/shared_ptr.h>

#include "config.hpp"
#include "exception.hpp"

namespace jsonconfig {

typedef std::vector<pfi::lang::shared_ptr<config_error> > config_error_list;

class json_config_iarchive_cast {
 public:
  explicit json_config_iarchive_cast(const config& js)
      : js_(js), errors_(NULL) {
  }

  json_config_iarchive_cast(const config& js, config_error_list* errors)
      : js_(js), errors_(errors) {
  }

  const config& get_config() const { return js_; }
  const pfi::text::json::json& get() const { return js_.get(); }

  bool trace_error() const {
    return errors_;
  }

  template <class T>
  void push_error(const T& e) {
    if (errors_) {
      errors_->push_back(pfi::lang::shared_ptr<config_error>(new T(e)));
    }
  }

  config_error_list* errors() const {
    return errors_;
  }

 private:
  const config& js_;
  config_error_list* errors_;
};

template <typename T>
void json_from_config(const config& conf, T& v);

template <typename T>
void json_from_config(const config& conf, T& v, json_config_iarchive_cast& js);

template <typename T>
inline void serialize(json_config_iarchive_cast& js, T& v) {
  // TODO(unno): insert typecheck
  pfi::data::serialization::access::serialize(js, v);
}

namespace detail {
inline bool check_json_type(
    json_config_iarchive_cast& js,
    pfi::text::json::json::json_type_t t) {
  if (js.get().type() != t) {
    // int -> float is valid
    if (js.get().type() == pfi::text::json::json::Integer &&
        t == pfi::text::json::json::Float) {
      return true;
    }
    type_error e(js.get_config().path(), t, js.get().type());
    if (js.trace_error()) {
      js.push_error(e);
    } else {
      throw e;
    }
    return false;
  }
  return true;
}

}  // namespace detail

#define GENERATE_CONFIG_SERIALIZE_DEF(typ, json_typ)                    \
  template <>                                                           \
  inline void serialize(json_config_iarchive_cast& js, typ& v) {        \
    if (detail::check_json_type(js, pfi::text::json::json::json_typ)) { \
      v = pfi::text::json::json_cast<typ>(js.get());                    \
    }                                                                   \
  }

GENERATE_CONFIG_SERIALIZE_DEF(bool, Bool)
GENERATE_CONFIG_SERIALIZE_DEF(int32_t, Integer)
GENERATE_CONFIG_SERIALIZE_DEF(int64_t, Integer)
GENERATE_CONFIG_SERIALIZE_DEF(float, Float)
GENERATE_CONFIG_SERIALIZE_DEF(double, Float)
GENERATE_CONFIG_SERIALIZE_DEF(std::string, String)

#undef GENERATE_CONFIG_SERIALIZE_DEF

template <typename T>
inline void serialize(json_config_iarchive_cast& js, std::vector<T>& vs) {
  // check errors
  if (!detail::check_json_type(js, pfi::text::json::json::Array)) {
    return;
  }

  size_t size = js.get_config().size();
  std::vector<T> v(size);
  for (size_t i = 0; i < size; ++i) {
    json_from_config(js.get_config()[i], v[i], js.errors());
  }
  vs.swap(v);
}

template <typename K, typename V>
inline void serialize(json_config_iarchive_cast& js, std::map<K, V>& m) {
  if (!detail::check_json_type(js, pfi::text::json::json::Object)) {
    return;
  }

  std::map<K, V> tmp;
  typedef config::iterator iter_t;
  for (iter_t it = js.get_config().begin(), end = js.get_config().end();
       it != end; ++it) {
    V value = V();
    json_from_config(it.value(), value, js.errors());
    tmp[it.key()] = value;
  }
  tmp.swap(m);
}

template <typename K, typename V>
inline void serialize(
    json_config_iarchive_cast& js,
    pfi::data::unordered_map<K, V>& m) {
  if (!detail::check_json_type(js, pfi::text::json::json::Object)) {
    return;
  }

  pfi::data::unordered_map<K, V> tmp;
  typedef config::iterator iter_t;
  for (iter_t it = js.get_config().begin(), end = js.get_config().end();
       it != end; ++it) {
    V value = V();
    json_from_config(it.value(), value, js.errors());
    tmp[it.key()] = value;
  }
  tmp.swap(m);
}

template <typename T>
inline void serialize(
    json_config_iarchive_cast& js,
    pfi::data::serialization::named_value<pfi::data::optional<T> >& v) {
  using pfi::text::json::json;
  if (js.get_config().contain(v.name)
      && (js.get_config()[v.name].get().type() != json::Null)) {
    T value = T();
    json_from_config(js.get_config()[v.name], value, js.errors());
    v.v = value;
  } else {
    // optional can be null
    v.v = pfi::data::optional<T>();
  }
}

template <typename T>
inline void serialize(
    json_config_iarchive_cast& js,
    pfi::data::serialization::named_value<T>& v) {
  if (js.get_config().contain(v.name)) {
    json_from_config(js.get_config()[v.name], v.v, js.errors());
  } else {
    not_found e(js.get_config().path(), v.name);
    if (js.trace_error()) {
      js.push_error(e);
    } else {
      throw e;
    }
  }
}

template <typename T>
void json_from_config(const config& conf, T& v) {
  json_config_iarchive_cast cast(conf);
  serialize(cast, v);
}

template <typename T>
void json_from_config(const config& conf, T& v, config_error_list* errors) {
  json_config_iarchive_cast cast(conf, errors);
  serialize(cast, v);
}

template <class T>
T config_cast(const config& c) {
  T value = T();
  json_from_config(c, value);
  return value;
}

template <class T>
T config_cast(const config& c, config_error_list& errors) {
  T value = T();
  json_config_iarchive_cast cast(c, &errors);
  serialize(cast, value);
  return value;
}

}  // namespace jsonconfig

#endif  // JSOCONFIG_CAST_HPP_
