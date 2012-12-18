#include <gtest/gtest.h>

#include <sstream>
#include <pficommon/text/json.h>

#include "config.hpp"
#include "cast.hpp"

using namespace pfi::text::json;

namespace jsonconfig {

TEST(jsonconfig, bool) {
  json j = json(new json_bool(false));
  EXPECT_EQ(false, ConfigRoot(j).As<bool>());
}

TEST(jsonconfig, bool_type_error) {
  try {
    json j = json(new json_bool(false));
    ConfigRoot(j).As<int>();
    FAIL();
  } catch (TypeError& e) {
    EXPECT_EQ(json::Bool, e.GetActual());
    EXPECT_EQ("", e.GetPath());
  }
}

TEST(jsonconfig, int) {
  json j = json(new json_integer(10));
  EXPECT_EQ(10, ConfigRoot(j).As<int>());
}

TEST(jsonconfig, int_type_error) {
  try {
    json j = json(new json_integer(10));
    ConfigRoot(j).As<std::string>();
    FAIL();
  } catch (TypeError& e) {
    EXPECT_EQ(json::Integer, e.GetActual());
    EXPECT_EQ("", e.GetPath());
  }
}

TEST(jsonconfig, float) {
  json j = json(new json_float(0.1));
  EXPECT_FLOAT_EQ(0.1, ConfigRoot(j).As<float>());
}

TEST(jsonconfig, float_type_error) {
  try {
    json j = json(new json_float(0.1));
    ConfigRoot(j).As<std::string>();
    FAIL();
  } catch (TypeError& e) {
    EXPECT_EQ(json::Float, e.GetActual());
    EXPECT_EQ("", e.GetPath());
  }
}

TEST(jsonconfig, array) {
  json j = json(new json_array());
  j.add(json(new json_integer(10)));
  EXPECT_EQ(10, ConfigRoot(j)[0].As<int>());
}

TEST(jsonconfig, array_out_of_range) {
  json j = json(new json_array());
  try {
    ConfigRoot(j)[1].As<int>();
    FAIL();
  } catch(OutOfRange& e) {
    EXPECT_EQ(0u, e.GetSize());
    EXPECT_EQ(1u, e.GetIndex());
    EXPECT_EQ("", e.GetPath());
  }
}

TEST(jsonconfig, array_type_error) {
  json j = json(new json_integer(10));
  ConfigRoot c(j);
  try {
    c[1];
    FAIL();
  } catch(TypeError& e) {
    EXPECT_EQ(json::Array, e.GetExpect());
    EXPECT_EQ("", e.GetPath());
  }
}

TEST(jsonconfig, object) {
  json j = json(new json_object());
  j["age"] = json(new json_integer(10));
  EXPECT_EQ(10, ConfigRoot(j)["age"].As<int>());
}

TEST(jsonconfig, object_not_found) {
  json j = json(new json_object());
  j["age"] = json(new json_integer(10));
  ConfigRoot c(j);
  try {
    c["name"];
    FAIL();
  } catch(NotFound& e) {
    EXPECT_EQ("name", e.GetKey());
    EXPECT_EQ("", e.GetPath());
  }
}

TEST(jsonconfig, object_type_error) {
  json j = json(new json_integer(10));
  ConfigRoot c(j);
  try {
    c["age"];
    FAIL();
  } catch(TypeError& e) {
    EXPECT_EQ(json::Object, e.GetExpect());
    EXPECT_EQ("", e.GetPath());
  }
}

TEST(jsonconfig, error_path) {
  json j = json(new json_array());
  j.add(json(new json_object()));
  j[0]["user"] = json(new json_object());
  j[0]["user"]["age"] = json(new json_integer(10));
  ConfigRoot c(j);
  try {
    c[0]["user"]["age"].As<std::string>();
    FAIL();
  } catch(TypeError& e) {
    EXPECT_EQ("[0].user.age", e.GetPath());
  }
}

}
