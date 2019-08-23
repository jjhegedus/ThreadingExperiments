#pragma once

#include <thread>
#include <algorithm>
#include <map>
#include <string>
#include <mutex>
#include <iostream>

namespace ndtech {

  template<typename ItemType>
	struct ItemProcessor {
    enum class WrappedTypeState {
			Created,
			Processing,
			Processed
		};
    template<typename WrappedType, WrappedTypeState state>
		struct ItemWrapper {
      ItemType m_item;
      ItemProcessor::WrappedTypeState m_wrappedTypeState;

      ItemWrapper(ItemType item) :
        m_item(std::move(item))
      {

      }

    };

    std::map<std::string, ItemType> m_itemsMap;
    std::function<bool(ItemType&)> m_processingFunction;
    std::thread m_processingThread;
    bool m_shutdown = false;
    std::mutex m_itemsMutex;

    ItemProcessor(std::function<bool(ItemType&)> processingFunction) :
      m_processingFunction(processingFunction),
      m_processingThread(ItemProcessor::Run, this)
    {

    }

    void Run() {
      while (!m_shutdown) {

        std::for_each(m_itemsMap.begin(), m_itemsMap.end(), [](ItemType& item) {

          if (m_processingFunction(item)) {
            m_itemsMap.erase(
              std::remove_if(
                m_itemsMap.begin(), 
                m_itemsMap.end(), 
                [](std::pair<std::string, ItemType> pair) {
                  std::cout << pair.second << std::endl;
                }));
          }

          });

      }
    }

    void AddItem(ItemType item) {
      std::lock_guard lockGuard(m_itemsMutex);
      //m_itemsMutex.
    }

    ItemType GetItem() {

    }

  };

}