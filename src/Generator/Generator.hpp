#pragma once

#include <mutex>

#include "Configurations/DummyConfigurations.hpp"
#include "Frames/MacFrame.hpp"

class Generator
{
public:
  Generator() = delete;
  Generator(DummyConfigurations &&configurations);
  bool getPacket(MacFrame &frame);
  bool isDone();

private:
  bool consumePacket();
  bool lockFreeIsDone() const;

  DummyConfigurations m_oConfigurations;
  std::mutex m_oFramesMutex;
  uint64_t m_ui64ProducedFrames{0};
};
