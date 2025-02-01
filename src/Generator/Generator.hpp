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

  void configure(std::shared_ptr<DummyConfigurations> DummyConfigurations);
  void start();
  void stop();
  MacFrame *getPacket();

private:
  //! producing / generating packets
  void generatePackets();
  MacFrame *generatePacket(int32_t &p_si32PacketNumber);
  int32_t producePacket();
  void autoStop();
  void reportPacket(MacFrame *p_pPacket, int32_t p_si32PacketNumber);
  bool lockFreeIsDone() const;

  //! reading packets
  bool consumePacket();
  bool lockFreeIsAllConsumed() const;

  //! frames
  std::mutex m_oFramesMutex;
  uint32_t m_ui32ProducedFrames{0};

  //! state
  std::mutex m_oStateMutex;
  GeneratorState m_eState{GeneratorState::STOPPED};

  //! configurations
  std::shared_ptr<DummyConfigurations> m_oConfigurations;

  //! producer thread, buffers
  Thread m_oThread{std::bind(&Generator::generatePackets, this)};
  Queue m_oGeneratedPacketsQueue;

  //! consumed packets
  uint32_t m_ui32ConsumedCount{0};
  std::mutex m_oConsumedCountMutex;

  std::ofstream m_oTxReportFile{"Tx_report.txt"};
};
