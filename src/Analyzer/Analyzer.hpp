#pragma once

#include <fstream>

#include "Frames/MacFrame.hpp"
#include "Configurations/DummyConfigurations.hpp"
#include "Thread/Thread.hpp"
#include "Queues/Queue.hpp"

enum class AnalyzerState : uint8_t
{
  STOPPED = 0,
  CONFIGURED,
  STARTED,
};

class Analyzer
{
public:
  void configure(std::shared_ptr<DummyConfigurations>);
  void start();
  bool analyzeFrame(MacFrame *frame);
  bool isDone();
  void finalize();

  ~Analyzer();

private:
  void processMessages();
  void processFrame(MacFrame *p_pFrame, int64_t p_ui32FrameNumber);
  int64_t consumeFrame();
  void autoStop();
  void stop();

  std::shared_ptr<DummyConfigurations> m_oConfigurations;

  //! consumer thread
  Thread m_oThread{std::bind(&Analyzer::processMessages, this)};

  //! Message Queues
  Queue m_oFramesQueue;

  //! consumed messages
  std::mutex m_oConumedMessagesMutex;
  uint32_t m_ui32ConsumedMessagesCount{0};

  //! total messages
  std::mutex m_oMessagesReceivedMutex;
  uint32_t m_ui32MessagesReceived{0};

  //! analyzer state
  std::mutex m_oStateMutex;
  AnalyzerState m_eState{AnalyzerState::STOPPED};

  //! reporting file
  std::ofstream m_oRxReportFile{"Rx_report.txt"};
};