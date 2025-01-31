#pragma once

#include <mutex>
#include <fstream>

#include "Configurations/DummyConfigurations.hpp"
#include "Frames/MacFrame.hpp"
#include "Thread/Thread.hpp"
#include "Queues/Queue.hpp"

enum class GeneratorState : uint8_t
{
  STARTED,
  STOPPED,
  CONFIGURED,
};

class Generator
{
public:
  Generator() = default;
  ~Generator();

  void configure(DummyConfigurations &&DummyConfigurations);
  void start();
  bool isDone();
  MacFrame *getPacket();
  void stop();

private:
  void generatePacket();
  bool consumePacket();
  bool lockFreeIsDone() const;
  bool lockFreeIsAllConsumed() const;
  void autoStop();

  //! frames
  std::mutex m_oFramesMutex;
  uint64_t m_ui64ProducedFrames{0};

  //! state
  std::mutex m_oStateMutex;
  GeneratorState m_eState{GeneratorState::STOPPED};

  //! configurations
  DummyConfigurations m_oConfigurations;

  //! producer thread, buffers
  Thread m_oThread{std::bind(&Generator::generatePacket, this)};
  Queue m_oGeneratedPacketsQueue;

  //! consumed packets
  uint32_t m_ui32ConsumedCount{0};
  std::mutex m_oConsumedCountMutex;

  std::ofstream m_oTxReportFile{"Tx_report.txt"};
};
