#include <libmod/app.hh>
#include <iostream>

bool mod::init()
{
    std::cout << "Hello World" << std::endl;
    return true;
}

void mod::exit() {}

libmod_entrypoint()
