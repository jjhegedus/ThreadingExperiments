#pragma once

#include <thread>
#include <mutex>
#include <vector>

namespace ndtech {

  template <typename ItemType>
  struct EventProcessor {

    std::mutex m_itemsMutex;
    std::thread m_thread;
    bool m_done = false;
    std::vector< std::function<ItemType(ItemType, Action)>>

    void Start() {
      m_thread = std::thread{ &EventProcessor::Run, this };
    }

    void Run() {
      while (!m_done) {
        Process();
      }

    }

    void Process() {
      std::lock_guard lockGuard(m_itemsMutex);
      for (auto metadata = m_itemsMetadataMap.begin(); metadata != m_itemsMetadataMap.end(); metadata++) {
        if (!metadata->second.m_ready) {
          auto item = m_itemsMap.find(metadata->first);
          auto retVal = m_itemFunction(*item);
          metadata->second.m_ready = true;
          metadata->second.m_conditionVariable->notify_all();
        }
      }
    };

    void Join() {
      m_done = true;
      m_thread.join();
    }

  };
}