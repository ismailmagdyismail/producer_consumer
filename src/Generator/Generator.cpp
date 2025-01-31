#include "Generator/Generator.hpp"
#include <random>

#define FAULTY_PACKET 2
#define FIELD_SIZE 9

void Generator::generatePacket()
{
  m_oStateMutex.lock();
  while (GeneratorState::STARTED == m_eState)
  {
    m_oStateMutex.unlock();
    bool bNewPacketConsumed = consumePacket();
    if (!bNewPacketConsumed)
    {
      //! all packets generated so stop producer thread
      m_oTxReportFile << "Auto stop"
                      << std::endl;
      autoStop();
      return;
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, 2);
    int randomNumber = distrib(gen);

    MacFrame *pPacket = new MacFrame;
    if (FAULTY_PACKET != randomNumber)
    {
      //! read src , mac address from  configurations
      pPacket->m_pDestAdress = new char[FIELD_SIZE];
      pPacket->m_pSrcAddress = new char[FIELD_SIZE];

      std::strncpy(pPacket->m_pDestAdress, m_oConfigurations.m_pDestAdress, FIELD_SIZE);
      std::strncpy(pPacket->m_pSrcAddress, m_oConfigurations.m_pSrcAddress, FIELD_SIZE);
    }
    else
    {
      //! simulate faulty packets
      pPacket->m_pDestAdress = new char[FIELD_SIZE]{"xxxxxxxx"};
      pPacket->m_pSrcAddress = new char[FIELD_SIZE]{"xxxxxxxx"};
    }
    pPacket->m_pPayload = new char[FIELD_SIZE]{"ABCDEF12"};
    pPacket->m_pFCS = new char[FIELD_SIZE]{"FFFFFFFF"};
    m_oGeneratedPacketsQueue.push(pPacket);

    const char *pHEX = pPacket->toHEX();
    m_oTxReportFile << "Frame " << m_ui64ProducedFrames << " " << pHEX << std::endl;
    delete[] pHEX;

    m_oStateMutex.lock();
  }
}

void Generator::start()
{
  {
    std::lock_guard<std::mutex> lock{m_oStateMutex};
    if (GeneratorState::CONFIGURED != m_eState)
    {
      return;
    }
    m_eState = GeneratorState::STARTED;
  }
  m_oThread.start();
}

void Generator::autoStop()
{
  std::lock_guard<std::mutex> lock{m_oStateMutex};
  m_eState = GeneratorState::STOPPED;
  // m_oThread.stop(); CANNOT JOIN THREAD WITHIN ITSELF
}

void Generator::stop()
{
  //! NOTE if lock is held for whole duration (throught call to m_oThread.stop()) deadlock will arise between stop() , autoStop()
  {
    std::lock_guard<std::mutex> lock{m_oStateMutex};
    m_eState = GeneratorState::STOPPED;
  }
  m_oThread.stop();
}

MacFrame *Generator::getPacket()
{
  std::lock_guard<std::mutex> lock{m_oConsumedCountMutex};
  if (lockFreeIsAllConsumed())
  {
    return nullptr;
  }
  MacFrame *pMacFrame = m_oGeneratedPacketsQueue.pop();
  m_ui32ConsumedCount++;
  return pMacFrame;
}

// NOTE similar issue/problem as the inhert thread unsafty to stack interface
bool Generator::consumePacket()
{
  std::lock_guard<std::mutex> lock(m_oFramesMutex);
  bool bIsDone = lockFreeIsDone();
  if (!bIsDone)
  {
    m_ui64ProducedFrames++;
    return true;
  }
  return false;
}

bool Generator::isDone()
{
  std::lock_guard<std::mutex> lock(m_oFramesMutex);
  return lockFreeIsDone();
}

bool Generator::lockFreeIsDone() const
{
  return (m_oConfigurations.m_ui32framesCount == m_ui64ProducedFrames);
}

bool Generator::lockFreeIsAllConsumed() const
{
  return (m_ui32ConsumedCount == m_oConfigurations.m_ui32framesCount);
}

void Generator::configure(DummyConfigurations &&p_oConfigurations)
{
  std::lock_guard<std::mutex> lock{m_oStateMutex};
  if (GeneratorState::STARTED == m_eState)
  {
    return;
  }
  m_oConfigurations = std::move(p_oConfigurations);
  m_eState = GeneratorState::CONFIGURED;
}

Generator::~Generator()
{
  stop();
  m_oGeneratedPacketsQueue.clear();
}
