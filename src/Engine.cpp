#include "Engine.hpp"

// Generator oGenerator(DummyConfigurations(ui32FramesCount, pDestAddress, pSrcAddress));

void Engine::configure(DummyConfigurations &&configurations)
{
  m_oGenerator.configure(std::move(configurations));
}

void Engine::start()
{
  do
  {
    MacFrame frame;
    m_oGenerator.getPacket(frame);
    std::cout << "Frame:" << frame.m_pSrcAddress << '\n';
  } while (!m_oGenerator.isDone());
}