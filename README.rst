jsonconfig
==========

[![Build Status](https://travis-ci.org/unnonouno/jsonconfig.png?branch=master)](https://travis-ci.org/unnonouno/jsonconfig) [![Coverage Status](https://coveralls.io/repos/unnonouno/jsonconfig/badge.png)](https://coveralls.io/r/unnonouno/jsonconfig)

jsonconfig is a simple helper class with json object for application configuration. 

- simply wrap pficommon json object
- readable error messages with jsonpath which indicates invalid configuration

Build
-----

::

  $ ./waf configure & ./waf & ./waf install


Supported Types
---------------

pficommon json compatible.

- bool
- int
- long
- float
- double
- std::string
- std::vector
- std::map
- pfi::data::unordered_map
- pfi::data::optional


Example
-------

Assume you have a configuration file like this::

  { "web_server": {
      "host": "http://hoge.com",
      "port": 8080  },
    "users": ["saitama", "gumma", "ibaraki"]
  }

You can access it with `As` method::

  jsonconfig::config_root config = jsonconfig::load("path/to/json/file");
  // print "http://hoge.com"
  std::cout << config["web_server"]["host"].as<std::string>() << std::endl;
  // print 8080
  std::cout << config["web_server"]["port"].as<int>() << std::endl;

You can use struct to define schema of configuration explicitly::

  struct server_conf {
    struct web_conf {
      std::string host;
      int port;

      templpate <typename Ar>
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

  server_conf server = jsonconfig::config_cast<server_conf>(conf);

