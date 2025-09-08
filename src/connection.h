#include <cstddef>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "lib/httplib.h"
#include "lib/uri_parser.h"
#include "lib/termbox2.h"
#include <string>
#include <sstream>
#include <vector>

#ifndef CONNECTION_H
#define CONNECTION_H

enum class FileType {
  Error,
  Text,
  Html,
  Markdown,
  Invalid
};

class Connection {
  public:
    FileType type = FileType::Error;
    std::vector<std::string> links;
    uri::Uri* uri = nullptr;
    httplib::Client* cli = nullptr;
    httplib::Result res;
    std::string dat;
    ~Connection() {
      delete uri;
      delete cli;
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
      if (!cli->is_valid()) {
        dat = "Error: Failed to connect to server!";
        return;
      }
      std::ostringstream req;
      if (uri->path[0] == '/') req << uri->path;
      else req << "/";
      if (uri->query_string != "")
        req << "?" << uri->query_string;
      if (uri->fragment != "")
        req << "#" << uri->fragment;
      res = cli->Get(req.str());
      if (res.error() != httplib::Error::Success) {
        std::ostringstream error;
        error << "Error: Connection Error\n" << host.str() << req.str();
        dat = error.str();
        return;
      }
      // Determine Type (Default: Html)
      type = FileType::Html;
      // By Mime Type
      if (res.value().has_header("Content-Type")) {
        std::string mime_type = res.value().get_header_value("Content-Type");
        if (mime_type == "text/plain") type = FileType::Text;
      }
      // By File Extension
      else if (std::size_t ext = uri->path.find_last_of('.') != std::string::npos) {
        std::string extension = uri->path.substr(ext, uri->path.length());
        if (extension == ".txt") type = FileType::Text;
      }
        
      dat = res.value().body;
    }

    // Rendering
    void render(int* scroll) {
      if (type == FileType::Error)
        tb_print(0, 0, TB_WHITE, TB_RED, dat.c_str());
      else if (type == FileType::Text)
        text_render(scroll);
      else if (type == FileType::Html)
        text_render(scroll);
      int x = tb_width();
      tb_print(x-=uri->path.length(), 0, TB_GREEN, 0, uri->path.c_str());
      tb_print(x-=uri->authority.host.length(), 0, TB_GREEN, 0, uri->authority.host.c_str());
    }

    void text_render(int* scroll) {
      size_t line = 0;
      size_t prevline = 0;
      int linenum = 0;
      while (++linenum <= *scroll) {
        size_t nextline = dat.find('\n', line)+1;
        if (nextline < line) {
          *scroll -= *scroll-linenum;
          line = prevline;
        }
        else {
          prevline = line;
          line = nextline;
        }
      }
      tb_print(0, 0, 0, 0, dat.substr(line, dat.length()-line).c_str());
    }

    void html_render(int* scroll) {
      tb_print(0, 0, 0, 0, dat.c_str());
    }
};

#endif
