#include <random>
#include <cassert>

#include "Generator/Generator.hpp"

#define ALL_PRODUCED -1
#define FAULTY_PACKET 2
#define FIELD_SIZE 9

void Generator::configure(std::shared_ptr<DummyConfigurations> p_oConfigurations)
{
  std::lock_guard<std::mutex> lock{m_oStateMutex};
  if (GeneratorState::STOPPED != m_eState)
  {
    return;
  }
  m_eState = GeneratorState::CONFIGURED;
  m_oConfigurations = p_oConfigurations;
}

void Generator::start()
{
  std::lock_guard<std::mutex> lock{m_oStateMutex};
  if (GeneratorState::CONFIGURED != m_eState)
  {
    return;
  }
  m_eState = GeneratorState::STARTED;
  m_oThread.start();
  // NOTICE: lock is held for the whole duration of the method , so that state updates are "Atomic"
  // Similar to the problem with the stack interface , Too much granularity == NotThread safe interface
}

void Generator::stop()
{
  std::lock_guard<std::mutex> lock{m_oStateMutex};
  if (GeneratorState::STOPPED == m_eState)
  {
    return;
  }
  m_eState = GeneratorState::STOPPED;
  m_oThread.stop();
  // NOTICE: lock is held for the whole duration of the method , so that state updates are "Atomic"
  // Similar to the problem with the stack interface , Too much granularity == NotThread safe interface
}

void Generator::generatePackets()
{
  m_oStateMutex.lock();
  while (GeneratorState::STARTED == m_eState)
  {
    m_oStateMutex.unlock();
    int32_t si32PacketNumber{0};
    MacFrame *pPacket = generatePacket(si32PacketNumber);
    if (nullptr == pPacket)
    {
      autoStop();
      m_oTxReportFile << "Auto Stop" << std::endl;
      return;
    }
    m_oGeneratedPacketsQueue.push(pPacket);
    reportPacket(pPacket, si32PacketNumber);
    m_oStateMutex.lock();
  }
}

void Generator::autoStop()
{
  std::lock_guard<std::mutex> lock{m_oStateMutex};
  m_eState = GeneratorState::STOPPED;
}

MacFrame *Generator::generatePacket(int32_t &p_si32PacketNumber)
{
  int32_t s32PacketNumber = producePacket();
  if (ALL_PRODUCED == s32PacketNumber)
  {
    return nullptr;
  }
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(1, 2);
  int randomNumber = distrib(gen);

  MacFrame *pPacket = new MacFrame;
  p_si32PacketNumber = s32PacketNumber;
  if (FAULTY_PACKET != randomNumber)
  {
    //! read src , mac address from  configurations
    pPacket->m_pDestAdress = new char[FIELD_SIZE];
    pPacket->m_pSrcAddress = new char[FIELD_SIZE];

    std::strncpy(pPacket->m_pDestAdress, m_oConfigurations->m_pDestAdress, FIELD_SIZE);
    std::strncpy(pPacket->m_pSrcAddress, m_oConfigurations->m_pSrcAddress, FIELD_SIZE);
  }
  else
  {
    //! simulate faulty packets
    pPacket->m_pDestAdress = new char[FIELD_SIZE]{"xxxxxxxx"};
    pPacket->m_pSrcAddress = new char[FIELD_SIZE]{"xxxxxxxx"};
  }
  pPacket->m_pPayload = new char[FIELD_SIZE]{"ABCDEF12"};
  pPacket->m_pFCS = new char[FIELD_SIZE]{"FFFFFFFF"};

  return pPacket;
}

void Generator::reportPacket(MacFrame *p_pPacket, int32_t p_si32PacketNumber)
{
  const char *pFrameHEX = p_pPacket->toHEX();
  m_oTxReportFile << "Frame " << p_si32PacketNumber << " " << pFrameHEX << std::endl;
  delete[] pFrameHEX;
}

int32_t Generator::producePacket()
{
  std::lock_guard<std::mutex> lock{m_oFramesMutex};
  if (lockFreeIsDone())
  {
    return ALL_PRODUCED;
  }
  m_ui32ProducedFrames++;
  return m_ui32ProducedFrames;
}

bool Generator::lockFreeIsDone() const
{
  return m_ui32ProducedFrames == m_oConfigurations->m_ui32framesCount;
}

MacFrame *Generator::getPacket()
{
  bool bPacketConsumed = consumePacket();
  if (!bPacketConsumed)
  {
    return nullptr;
  }
  MacFrame *pPacket = m_oGeneratedPacketsQueue.pop();
  assert(nullptr != pPacket);
  return pPacket;
}

bool Generator::consumePacket()
{
  std::lock_guard<std::mutex> lock{m_oFramesMutex};
  if (lockFreeIsAllConsumed())
  {
    return false;
  }
  m_ui32ConsumedCount++;
  return true;
}

bool Generator::lockFreeIsAllConsumed() const
{
  return m_ui32ConsumedCount == m_oConfigurations->m_ui32framesCount;
}

Generator::~Generator()
{
  {
    std::lock_guard<std::mutex> lock{m_oStateMutex};
    m_eState = GeneratorState::STOPPED;
  }
  m_oThread.stop();
  m_oGeneratedPacketsQueue.clear();
}