//! system includes
#include <iostream>

#include "Engine.hpp"

int main()
{
    Engine engine;
    DummyConfigurations configurations = DummyConfigurations(32, new char[9]{"11111111"}, new char[9]{"FFFFFFFF"});
    engine.configure(std::move(configurations));
    engine.start();
}
