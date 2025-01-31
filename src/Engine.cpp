#include "Engine.hpp"

void Engine::configure(DummyConfigurations &&configurations)
{
  std::shared_ptr<DummyConfigurations> pConfigurations = std::make_shared<DummyConfigurations>(std::move(configurations));
  m_oGenerator.configure(pConfigurations);
  m_oAnalyzer.configure(pConfigurations);
}

void Engine::start()
{
  m_oGenerator.start();
  m_oAnalyzer.start();
  while (true)
  {
    MacFrame *pFrame = m_oGenerator.getPacket();
    bool bGeneratorDone = (pFrame == nullptr);
    bool bCanAnalyze = m_oAnalyzer.analyzeFrame(pFrame);
    if (bGeneratorDone)
    {
      break;
    }
  }
  m_oAnalyzer.finalize();
}