#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <iostream>

#include "instance.h"

int main() {
    // Connection
    Instance site("https://bit-turtle.github.io/loader.html");
    
    std::cout << site.con->res->value().body;

    return 0;
}
