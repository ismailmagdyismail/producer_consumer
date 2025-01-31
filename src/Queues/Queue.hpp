#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <optional>

class Queue
{
public:
  //! Blocking pop() till an element is available
  MacFrame *pop()
  {
    std::unique_lock<std::mutex> lock{m_oQueueMutex};
    m_oQueueConditionVariable.wait(lock, [this]()
                                   { return m_oStopped || !m_oQueue.empty(); });

    if (m_oStopped && m_oQueue.empty())
      return nullptr; // Indicate shutdown

    auto frame = std::move(m_oQueue.front());
    m_oQueue.pop();
    return frame;
  }

  //! Push a new frame into the queue
  void push(MacFrame *in_pFrame)
  {
    {
      std::lock_guard<std::mutex> lock{m_oQueueMutex};
      if (m_oStopped)
        return; // Don't push if shutting down
      m_oQueue.push(std::move(in_pFrame));
    }
    m_oQueueConditionVariable.notify_one();
  }

  //! Check if queue is empty
  bool isEmpty()
  {
    std::lock_guard<std::mutex> lock{m_oQueueMutex};
    return m_oQueue.empty();
  }

  //! Get queue size
  size_t size()
  {
    std::lock_guard<std::mutex> lock{m_oQueueMutex};
    return m_oQueue.size();
  }

  //! Clear the queue safely
  void clear()
  {
    std::lock_guard<std::mutex> lock{m_oQueueMutex};
    while (!m_oQueue.empty())
    {
      delete m_oQueue.front();
      m_oQueue.pop();
    }
  }

  //! Stop queue processing
  void shutdown()
  {
    {
      std::lock_guard<std::mutex> lock{m_oQueueMutex};
      m_oStopped = true;
    }
    m_oQueueConditionVariable.notify_all(); // Wake up all waiting threads
  }

  ~Queue()
  {
    shutdown();
  }

private:
  std::mutex m_oQueueMutex;
  std::condition_variable m_oQueueConditionVariable;
  std::queue<MacFrame *> m_oQueue;
  std::atomic<bool> m_oStopped{false};
};
