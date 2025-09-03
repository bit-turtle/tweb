#include "lib/httplib.h"
#include "lib/uri_parser.h"
#include <string>
#include <sstream>

#ifndef CONNECTION_H
#define CONNECTION_H

class Connection {
  public:
    uri::Uri* uri;
    httplib::Client* cli;
    httplib::Result* res;
    ~Connection() {
      delete uri;
      delete cli;
      delete res;
    }
    Connection(std::string url) {
      uri = new uri::Uri(uri::parse_uri(url));
      if (uri->error != uri::Error::None) {
        uri->scheme = "http";
        uri->authority.port = 80;
      }
      std::ostringstream host;
      host << uri->scheme << "://" << uri->authority.authority;
      cli = new httplib::Client(host.str());
      std::ostringstream req;
      req << uri->path << uri->query_string;
      res = new httplib::Result(cli->Get(req.str()));
    }
};

#endif
