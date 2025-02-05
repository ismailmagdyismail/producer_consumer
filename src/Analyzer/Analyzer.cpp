#include "Analyzer/Analyzer.hpp"
#include <unistd.h>
#include <cassert>

#define ALL_CONSUMED -1

void Analyzer::configure(std::shared_ptr<DummyConfigurations> p_oConfigurations)
{
  std::lock_guard<std::mutex> lock{m_oStateMutex};
  if (AnalyzerState::STARTED == m_eState)
  {
    return;
  }
  m_eState = AnalyzerState::CONFIGURED;
  m_oConfigurations = p_oConfigurations;
}

void Analyzer::start()
{
  std::lock_guard<std::mutex> lock{m_oStateMutex};
  if (AnalyzerState::CONFIGURED != m_eState || AnalyzerState::STARTED == m_eState)
  {
    return;
  }
  m_eState = AnalyzerState::STARTED;
  m_oThread.start(); //! calls start while holding lock to be ATOMIC, state machine consitent with thread state
  m_oStateCv.notify_all();
}

void Analyzer::stop()
{
  std::lock_guard<std::mutex> lock{m_oStateMutex};
  if (AnalyzerState::STOPPED == m_eState)
  {
    return;
  }
  m_eState = AnalyzerState::STOPPED;
  m_oStateCv.notify_all();
  // m_oThread.stop(); //! calls start while holding lock to be ATOMIC, state machine consitent with thread state
}

bool Analyzer::analyzeFrame(MacFrame *frame)
{
  std::lock_guard<std::mutex> lock{m_oMessagesReceivedMutex};
  if (m_ui32MessagesReceived == m_oConfigurations->m_ui32framesCount)
  {
    return false;
  }
  m_ui32MessagesReceived++;
  m_oFramesQueue.push(frame);
  return true;
}

void Analyzer::processMessages()
{
  m_oStateMutex.lock();
  while (AnalyzerState::STARTED == m_eState)
  {
    m_oStateMutex.unlock();
    uint32_t ui32FrameNumber = consumeFrame();
    if (ALL_CONSUMED == ui32FrameNumber)
    {
      m_oRxReportFile << "Auto stop"
                      << std::endl;
      stop();
      return;
    }
    MacFrame *pMacFrame = m_oFramesQueue.pop();
    assert(nullptr != pMacFrame);
    processFrame(pMacFrame, ui32FrameNumber);
    m_oStateMutex.lock();
  }
}

void Analyzer::processFrame(MacFrame *p_pFrame, int64_t p_ui32FrameNumber)
{
  if (0 == strcmp(p_pFrame->m_pDestAdress, "xxxxxxxx"))
  {
    m_oRxReportFile << "Frame " << p_ui32FrameNumber << " Invalid " << std::endl;
  }
  else
  {
    m_oRxReportFile << "Frame " << p_ui32FrameNumber << " Valid" << std::endl;
  }
}

void Analyzer::finalize()
{
  std::unique_lock<std::mutex> lock{m_oStateMutex};
  m_oStateCv.wait(lock, [this]() -> bool
                  { return AnalyzerState::STOPPED == m_eState && m_ui32ConsumedMessagesCount == m_oConfigurations->m_ui32framesCount; });
}

int64_t Analyzer::consumeFrame()
{
  std::lock_guard<std::mutex> lock{m_oConumedMessagesMutex};
  if (m_ui32ConsumedMessagesCount == m_oConfigurations->m_ui32framesCount)
  {
    return ALL_CONSUMED;
  }
  m_ui32ConsumedMessagesCount++;
  return m_ui32ConsumedMessagesCount;
}

bool Analyzer::isDone()
{
  std::lock_guard<std::mutex> messagesLock{m_oConumedMessagesMutex};
  std::lock_guard<std::mutex> stateLock{m_oStateMutex};
  return (AnalyzerState::STOPPED == m_eState && m_ui32ConsumedMessagesCount == m_oConfigurations->m_ui32framesCount);
}

Analyzer::~Analyzer()
{
  //! stop processing
  {
    std::lock_guard<std::mutex> lock{m_oStateMutex};
    m_eState = AnalyzerState::STOPPED;
  }
  //! stop thread
  m_oThread.stop();

  //! delete
  m_oFramesQueue.clear();
}
