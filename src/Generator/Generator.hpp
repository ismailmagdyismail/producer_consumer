#pragma once

#include <mutex>

#include "Configurations/DummyConfigurations.hpp"
#include "Frames/MacFrame.hpp"

class Generator
{
public:
  Generator() = default;

  void configure(DummyConfigurations &&DummyConfigurations);
  bool getPacket(MacFrame &frame);
  bool isDone();

private:
  bool consumePacket();
  bool lockFreeIsDone() const;
  bool isConfigured();

  //! frames
  std::mutex m_oFramesMutex;
  uint64_t m_ui64ProducedFrames{0};

  //! configurations
  std::mutex m_oConfigurationMutex;
  bool m_bIsConfigured{false};
  DummyConfigurations m_oConfigurations;
};
