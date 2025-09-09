#include "lib/termbox2.h"

#include <string>
#include <vector>

#ifndef HTML_H
#define HTML_H

namespace html {

  void render(int* scroll, std::string& dat);

  std::vector<std::string&> links(std::string& dat);

};

#endif
