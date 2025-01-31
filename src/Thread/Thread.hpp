#pragma once

#include <thread>

/*
simple Actor/Active Object pattern to handle each resposnibility in thread
NOTE: a more efficient approach is thread pools to avoid having thread per object
*/

class Thread
{
public:
  explicit Thread(std::function<void(void)> p_fWorkerThreadHandler)
  {
    m_fWorkerThreadHandler = p_fWorkerThreadHandler;
  }
  void start()
  {
    std::lock_guard<std::mutex> lock{m_oThreadStateMutex};
    if (m_bThreadStarted)
    {
      return;
    }
    m_bThreadStarted = true;
    std::function<void(void)> oWorker = [&]()
    {
      m_fWorkerThreadHandler();
    };
    m_oThread = std::thread{oWorker};
    m_oThread.detach();
  }
  void stop()
  {
    std::lock_guard<std::mutex> lock{m_oThreadStateMutex};
    if (!m_bThreadStarted || !m_oThread.joinable())
    {
      return;
    }
    m_bThreadStarted = false;
    m_oThread.join();
  }

  ~Thread()
  {
    stop();
  }

private:
  std::mutex m_oThreadStateMutex;
  bool m_bThreadStarted{false};
  std::thread m_oThread;
  std::function<void(void)> m_fWorkerThreadHandler;
};