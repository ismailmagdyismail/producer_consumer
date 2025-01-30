#include "Generator/Generator.hpp"
#include <random>

#define FAULTY_PACKET 2
#define FIELD_SIZE 9

Generator::Generator(DummyConfigurations &&p_oConfigurations) : m_oConfigurations{std::move(p_oConfigurations)}
{
}

bool Generator::getPacket(MacFrame &p_oPacket)
{
  if (consumePacket())
  {
    return false;
  }

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(1, 2);
  int randomNumber = distrib(gen);

  if (FAULTY_PACKET != randomNumber)
  {
    //! read src , mac address from  configurations
    p_oPacket.m_pDestAdress = new char[FIELD_SIZE];
    p_oPacket.m_pSrcAddress = new char[FIELD_SIZE];

    std::strncpy(p_oPacket.m_pDestAdress, m_oConfigurations.m_pDestAdress, FIELD_SIZE);
    std::strncpy(p_oPacket.m_pSrcAddress, m_oConfigurations.m_pSrcAddress, FIELD_SIZE);
  }
  else
  {
    //! simulate faulty packets
    p_oPacket.m_pDestAdress = new char[FIELD_SIZE]{"xxxxxxxx"};
    p_oPacket.m_pSrcAddress = new char[FIELD_SIZE]{"xxxxxxxx"};
  }
  p_oPacket.m_pPayload = new char[FIELD_SIZE]{"ABCDEF12"};
  p_oPacket.m_pFCS = new char[FIELD_SIZE]{"FFFFFFFF"};
  return true;
}

// NOTE similar issue/problem as the inhert thread unsafty to stack interface
bool Generator::consumePacket()
{
  std::lock_guard<std::mutex> lock(m_oFramesMutex);
  bool bIsDone = lockFreeIsDone();
  if (!bIsDone)
  {
    m_ui64ProducedFrames++;
  }
  return bIsDone;
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