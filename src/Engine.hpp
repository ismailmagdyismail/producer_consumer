#pragma once

#include "Generator/Generator.hpp"

class Engine
{
public:
  Engine() = default;

  void configure(DummyConfigurations &&configurations);
  void start();

  ~Engine() = default;

private:
  Generator m_oGenerator;
};