#include "connection.h"
#include <stack>

#ifndef INSTANCE_H
#define INSTANCE_H

class Instance {
  public:
    Connection* con = nullptr;
    std::stack<std::string> hist;
    std::stack<std::string> fwd;
    ~Instance() {
      delete con;
    }
    Instance(std::string url) {
      open(url);
    }
    void open(std::string url) {
      delete con;
      hist.push(url);
      con = new Connection(url);
    }
    bool back() {
      if (hist.size()>1) {
        delete con;
        fwd.push(hist.top());
        hist.pop();
        con = new Connection(hist.top());
        return true;
      }
      return false;
    }
    bool forward() {
      if (fwd.size()>0) {
        delete con;
        hist.push(fwd.top());
        fwd.pop();
        con = new Connection(hist.top());
        return true;
      }
      return false;
    }
};

#endif
