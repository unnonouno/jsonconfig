#ifndef JSOCONFIG_CAST_HPP_
#define JSOCONFIG_CAST_HPP_

#include "config.hpp"

#include <pficommon/data/unordered_map.h>
#include <pficommon/lang/shared_ptr.h>

namespace jsonconfig {

class JsonConfigIarchiveCast {
 public:
  JsonConfigIarchiveCast(const Config &js): js(js), trace_error_(false) {}
  JsonConfigIarchiveCast(const Config &js, JsonConfigIarchiveCast& parent): js(js), trace_error_(parent.TraceError()), parent_(parent) {}
  const Config& Get() const { return js; }
  void SetTraceError(bool trace) { trace_error_ = trace; }
  bool TraceError() const {
    return trace_error_;
  }
  template <class T>
  void PushError(const T& e)
  {
    if (parent_) {
      parent_->PushError(e);
    } else {
      errors_.push_back(pfi::lang::shared_ptr<ConfigError>(new T(e)));
    }
  }
  std::vector<pfi::lang::shared_ptr<ConfigError> >& Errors()
  {
    if (parent_) {
      return parent_->Errors();
    } else {
      return errors_;
    }
  }
 private:
  const Config &js;
  bool trace_error_;
  std::vector<pfi::lang::shared_ptr<ConfigError> > errors_;
  pfi::data::optional<JsonConfigIarchiveCast> parent_;
};

template <typename T>
void FromConfig(const Config& conf, T& v);

template <typename T>
void FromConfig(const Config& conf, T& v, JsonConfigIarchiveCast& js);

template <typename T>
inline void serialize(JsonConfigIarchiveCast &js, T &v) {
  // TODO: insert typecheck
  pfi::data::serialization::access::serialize(js, v);
}

////////

inline bool CheckType(JsonConfigIarchiveCast& js, pfi::text::json::json::json_type_t t) {
  if (js.Get().Get().type() != t) {
    TypeError e(js.Get().GetPath(), t, js.Get().Get().type());
    if (js.TraceError()) {
      js.PushError(e);
    } else {
      throw e;
    }
    return false;
  }
  return true;
}

#define GENERATE_CONFIG_SERIALIZE_DEF(typ, json_typ)                \
  template <>                                                       \
  inline void serialize(JsonConfigIarchiveCast &js, typ &v) {       \
    if (CheckType(js, pfi::text::json::json::json_typ)) {           \
      v = pfi::text::json::json_cast<typ>(js.Get().Get());          \
    }                                                               \
  }

GENERATE_CONFIG_SERIALIZE_DEF(bool, Bool)
GENERATE_CONFIG_SERIALIZE_DEF(int, Integer)
GENERATE_CONFIG_SERIALIZE_DEF(long, Integer)
GENERATE_CONFIG_SERIALIZE_DEF(float, Float)
GENERATE_CONFIG_SERIALIZE_DEF(double, Float)
GENERATE_CONFIG_SERIALIZE_DEF(std::string, String)

template <typename T>
inline void serialize(JsonConfigIarchiveCast& js, std::vector<T>& vs) {
  size_t size = js.Get().Size();
  std::vector<T> v(size);
  for (size_t i = 0; i < size; ++i) {
    // TODO: merge all errors
    FromConfig(js.Get()[i], v[i], js);
  }
  vs.swap(v);
}

template <typename K, typename V>
inline void serialize(JsonConfigIarchiveCast& js, std::map<K, V>& m) {
  std::map<K, V> tmp;
  typedef Config::Iterator iter_t;
  for (iter_t it = js.Get().GetIterator(); it.HasNext(); it.Next()) {
    // TODO: merge all errors
    V value;
    FromConfig(it.GetValue(), value, js);
    tmp[it.GetKey()] = value;
  }
  tmp.swap(m);
}

template <typename K, typename V>
inline void serialize(JsonConfigIarchiveCast& js, pfi::data::unordered_map<K, V>& m) {
  pfi::data::unordered_map<K, V> tmp;
  typedef Config::Iterator iter_t;
  for (iter_t it = js.Get().GetIterator(); it.HasNext(); it.Next()) {
    // TODO: merge all errors
    V value;
    FromConfig(it.GetValue(), value);
    tmp[it.GetKey()] = value;
  }
  tmp.swap(m);
}

template <typename T>
inline void serialize(JsonConfigIarchiveCast &js, pfi::data::serialization::named_value<pfi::data::optional<T> >& v) {
  if (js.Get().Include(v.name)) {
    T t;
    FromConfig(js.Get()[v.name], t, js);
    v.v = t;
  } else {
    v.v = pfi::data::optional<T>();
  }
}

template <typename T>
inline void serialize(JsonConfigIarchiveCast &js, pfi::data::serialization::named_value<T>& v) {
  if (js.Get().Include(v.name)) {
    FromConfig(js.Get()[v.name], v.v, js);
  } else {
    js.PushError(NotFound(js.Get().GetPath(), v.name));
  }
}

////////

template <typename T>
void FromConfig(const Config& conf, T& v) {
  JsonConfigIarchiveCast cast(conf);
  serialize(cast, v);
}

template <typename T>
void FromConfig(const Config& conf, T& v, JsonConfigIarchiveCast& parent) {
  JsonConfigIarchiveCast cast(conf, parent);
  serialize(cast, v);
}

template <class T>
T ConfigCast(const Config& c) {
  T v;
  FromConfig(c, v);
  return v;
}

template <class T>
T ConfigCastWithError(const Config& c, std::vector<pfi::lang::shared_ptr<ConfigError> >& errors) {
  T v;
  JsonConfigIarchiveCast cast(c);
  cast.SetTraceError(true);
  serialize(cast, v);
  errors.swap(cast.Errors());
  return v;
}

}

#endif // JSOCONFIG_CAST_HPP_
