#include <iostream>
#include <fstream>
#include <pficommon/data/serialization.h>
#include <jsonconfig.hpp>

struct server_conf {
  struct web_conf {
    std::string host;
    int port;
    
    template <typename Ar>
    void serialize(Ar& ar) {
      ar & MEMBER(host) & MEMBER(port);
    }
  } web_server;
  
  std::vector<std::string> users;
  
  template <typename Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(web_server) & MEMBER(users);
  }
};

jsonconfig::config_root load(const std::string& path) {
  std::ifstream ifs(path.c_str());
  if (!ifs)
    throw std::runtime_error("Cannot open: \"" + path + "\"");

  pfi::text::json::json j;
  ifs >> j;
  return jsonconfig::config_root(j);
}

int main() try {
  jsonconfig::config_error_list errors;
  using namespace std;
  {
    jsonconfig::config_root conf = load("sample/sample_config.json");
    server_conf serv = jsonconfig::config_cast<server_conf>(conf);
    
    cout << serv.web_server.host << endl
         << serv.web_server.port << endl;
  }
  
  {
    jsonconfig::config_root conf = load("sample/error_config.json");
    server_conf serv = jsonconfig::config_cast<server_conf>(conf, errors);
    cout << errors.empty() << cout;
  }

} catch(std::exception& e) {
  std::cout << e.what() << std::endl;
}
