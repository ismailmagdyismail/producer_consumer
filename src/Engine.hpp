#pragma once

#include "Generator/Generator.hpp"

class Engine
{
public:
  Engine() = default;

  void configure(DummyConfigurations &&configurations);
  void start();

private:
  Generator m_oGenerator;
};