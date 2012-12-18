#include <gtest/gtest.h>

#include <pficommon/lang/cast.h>

#include "cast.hpp"

using namespace std;
using namespace pfi::lang;
using namespace pfi::text::json;

namespace jsonconfig {

TEST(cast, int) {
  json j(new json_integer(1));
  ConfigRoot conf(j);
  EXPECT_EQ(1, ConfigCast<int>(conf));
}

TEST(cast, string) {
  json j(new json_string("test"));
  ConfigRoot conf(j);
  EXPECT_EQ("test", ConfigCast<string>(conf));
}

TEST(cast, vector_int) {
  ConfigRoot conf(lexical_cast<json>("[1,2,3]"));
  vector<int> v;
  v.push_back(1);
  v.push_back(2);
  v.push_back(3);

  EXPECT_EQ(v, ConfigCast<vector<int> >(conf));
}

TEST(cast, map) {
  ConfigRoot conf(lexical_cast<json>("{\"height\": 160, \"weight\": 60}"));
  map<string, int> m;
  m["height"] = 160;
  m["weight"] = 60;

  EXPECT_EQ(m, (ConfigCast<map<string, int> >(conf)));
}

TEST(cast, unordered_map) {
  ConfigRoot conf(lexical_cast<json>("{\"height\": 160, \"weight\": 60}"));
  pfi::data::unordered_map<string, int> m;
  m["height"] = 160;
  m["weight"] = 60;

  pfi::data::unordered_map<string, int> v = ConfigCast<pfi::data::unordered_map<string, int> >(conf);
  EXPECT_EQ(m["height"], v["height"]);
  EXPECT_EQ(m["weight"], v["weight"]);
}

TEST(cast, error_path) {
  ConfigRoot conf(lexical_cast<json>("{\"value\": [1,2,\"test\"]}"));
  try {
    ConfigCast<map<string, vector<int> > >(conf);
  } catch(TypeError& e) {
    EXPECT_EQ(".value[2]", e.GetPath());
    EXPECT_EQ(json::String, e.GetActual());
    EXPECT_EQ(json::Integer, e.GetExpect());
  }
}

struct Person {
  string name;
  double height;
  int age;
  map<string, string> attributes;
  pfi::data::optional<string> sport;
  pfi::data::optional<string> hobby;

  bool operator ==(const Person& p) const {
    return name == p.name && height == p.height && age == p.age && attributes == p.attributes && sport == p.sport && hobby == p.hobby;
  }
  
  template <class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(name) & MEMBER(height) & MEMBER(age) & MEMBER(attributes) & MEMBER(sport) & MEMBER(hobby);
  }
};

TEST(cast, struct) {
  ConfigRoot conf(lexical_cast<json>("{\"name\": \"Taro\", \"height\": 160.0, \"age\": 20, \"attributes\": {\"address\": \"Tokyo\"}, \"sport\": \"tennis\"}"));
  Person p;
  p.name = "Taro";
  p.height = 160.0;
  p.age = 20;
  p.attributes["address"] = "Tokyo";
  p.sport = "tennis";

  EXPECT_EQ(p, ConfigCast<Person>(conf));
  
}

TEST(cast, error) {
  ConfigRoot conf(lexical_cast<json>("{\"name_\": \"Taro\", \"height_\": 160.0, \"age\": 20, \"attributes\": {\"address\": \"Tokyo\"}, \"sport\": \"tennis\"}"));
  Person p;
  p.age = 20;
  p.attributes["address"] = "Tokyo";
  p.sport = "tennis";

  std::vector<pfi::lang::shared_ptr<ConfigError> > errors;
  ConfigCastWithError<Person>(conf, errors);
  EXPECT_EQ(2, errors.size());

  for (size_t i = 0; i < errors.size(); i++) {
    cout << errors[i]->what() << endl;
  }

  EXPECT_NE("Taron", p.name);
  EXPECT_NE(160.0, p.height);
}

}
