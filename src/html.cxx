// HTML Rendering
#include "html.h"
#include <stack>
#include <sstream>
#include <iostream>

namespace html {

  void display(int y, std::string text, std::stack<std::string> tags, std::stack<std::string> attrs) {
    if (y < 0) return;
    
    tb_print(0, y, TB_BOLD, TB_BLUE, text.c_str());
    
  }

  void render(int* scroll, std::string& dat) {

    int y = 0-*scroll;
    std::stack<std::string> tags;
    tags.push("base");
    std::stack<std::string> attrs;
    attrs.push("");
    size_t index = 0;
    char c;
    bool tag = false;
    bool attr = false;
    bool space = false;
    bool text = false;
    std::ostringstream buffer;
    while (index < dat.length()) {
      c = dat[index++];
      if (tag && !attr) {
        if (c == ' ' || c == '>') {
          attr = true;
          tags.push(buffer.str());
          buffer.clear();
        }
        else
          buffer << c;
      }
      if (tag && attr) {
        if (c == '>') {
          attrs.push(buffer.str());
          buffer.clear();
          tag = false;
          attr = false;
          text = false;
          space = false;
          if (tags.top().length() > 0 && (tags.top()[0] == '/'|| tags.top()[0] == '!')) {
            tags.pop();
            attrs.pop();
            if (tags.top()[0] == '/') {}
              tags.pop();
              attrs.pop();
            }
          }
        }
      }
      if (!tag && tags.top() == "br") {
        y++;
        tags.pop();
        attrs.pop();
      }
      if (!tag) {
        // Open Tags
        if (c == '<') {
          tag = true;
          display(y++, buffer.str(), tags, attrs);
          buffer.clear();
        }
        // HTML Entities
        else if (c == '&') {
          std::ostringstream entity;
          while (index < dat.length() && (c = dat[index++]) != ';')
            entity.put(c);
          if (entity.str() == "nbsp")
            buffer.put(' ');
          else if (entity.str() == "lt")
            buffer.put('<');
          else if (entity.str() == "gt")
            buffer.put('>');
          else if (entity.str() == "amp")
            buffer.put('&');
          else if (entity.str() == "quot")
            buffer.put('"');
          else if (entity.str() == "apos")
            buffer.put('\'');
          else
            buffer.put('?');
        }
        else {
          if (tags.top() == "pre") {
            if (c == '\n') {
              display(y++, buffer.str(), tags, attrs);
              buffer.clear();
            }
            else
              buffer.put(c);
          }
          else {
            if (c == ' ' || c == '\t' || c == '\n') {
              space = true;
            }
            else {
              if (space) {
                space = false;
                if (text)
                  buffer.put(' ');
              }
              text = true;
              buffer.put(c);
            }
          }
        }
      }
      if (buffer.str().length() != 0) {
        display(0, buffer.str(), tags, attrs);
      }
    }

};
