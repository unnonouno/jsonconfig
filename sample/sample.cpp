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

int main() try {
  std::vector<pfi::lang::shared_ptr<jsonconfig::ConfigError> > errors;
  using namespace std;
  {
    jsonconfig::ConfigRoot conf = jsonconfig::Load("sample/sample_config.json");
    server_conf serv = jsonconfig::ConfigCast<server_conf>(conf);
    
    cout << serv.web_server.host << endl
         << serv.web_server.port << endl;
  }
  
  {
    jsonconfig::ConfigRoot conf = jsonconfig::Load("sample/error_config.json");
    server_conf serv = jsonconfig::ConfigCastWithError<server_conf>(conf, errors);
    cout << errors.empty() << cout;
  }

} catch(std::exception& e) {
  std::cout << e.what() << std::endl;
}
