#include "Engine.hpp"

void Engine::configure(DummyConfigurations &&configurations)
{
  m_oGenerator.configure(std::move(configurations));
}

void Engine::start()
{
  m_oGenerator.start();
  while (true)
  {
    MacFrame *pFrame = m_oGenerator.getPacket();
    if (!pFrame)
    {
      break;
    }
  }
  // std::cout << "finished\n";
}