#pragma once

#include <string>
#include <mutex>
#include <iterator>
#include <map>
#include <utility>

namespace ndtech {

  template <typename ItemType>
  struct NamedItemStore {

    struct ItemMetadata {
      bool m_ready = false;
      std::shared_ptr<std::mutex> m_mutex;
      std::shared_ptr<std::condition_variable> m_conditionVariable;
    };

    std::function<std::pair<std::string, ItemType>(std::pair<std::string, ItemType>)> m_itemFunction;
    std::map<std::string, ItemType> m_itemsMap;
    std::map<std::string, ItemMetadata> m_itemsMetadataMap;
    std::mutex m_itemsMutex;
    std::thread m_thread;
    bool m_done = false;

    NamedItemStore(std::function<std::pair<std::string, ItemType>(std::pair<std::string, ItemType>)> itemFunction) :
      m_itemFunction(itemFunction)
    {

    }

    ItemType AddItem(std::string name, ItemType value) {

      {
        std::lock_guard lock(m_itemsMutex);
        auto foundItemMetadata = m_itemsMetadataMap.find(name);

        if (foundItemMetadata == m_itemsMetadataMap.end()) {
          // not found insert metadata and item
          ItemMetadata metadata;
          metadata.m_mutex = std::make_shared<std::mutex>();
          metadata.m_conditionVariable = std::make_shared <std::condition_variable>();
          m_itemsMetadataMap[name] = metadata;
          return m_itemsMap[name] = value;
        }
        else {
          
          auto item = m_itemsMap.find(name);
          item->second = value;
          foundItemMetadata->second.m_ready = true;
          foundItemMetadata->second.m_conditionVariable->notify_all();
          return item->second;
        }

      }

    }

    ItemType GetItem(std::string name) {

      {
        std::lock_guard lock(m_itemsMutex);
        auto foundItemMetadata = m_itemsMetadataMap.find(name);

        if (foundItemMetadata == m_itemsMetadataMap.end()) {
          // not found insert metadata and item
          ItemMetadata metadata;
          //m_itemsMetadataMap[name] = metadata;

          //*metadataInMap.m_mutex = std::make_shared<std::mutex>();

          metadata.m_mutex = std::make_shared<std::mutex>();
          metadata.m_conditionVariable = std::make_shared <std::condition_variable>();
          m_itemsMetadataMap[name] = metadata;
          //return m_itemsMap[name] = value;
          ItemMetadata& metadataInMap = m_itemsMetadataMap[name];



          std::unique_lock uniqueLock(*metadataInMap.m_mutex);
          ItemType& item = m_itemsMap[name] = ItemType();
          metadataInMap.m_conditionVariable->wait(uniqueLock, [&metadataInMap]() { return metadataInMap.m_ready; });
          
          return m_itemsMap[name];
        }
        else {
          if (foundItemMetadata->second.m_ready) {
            return m_itemsMap[name];
          }
          std::unique_lock uniqueLock(*foundItemMetadata->second.m_mutex);
          foundItemMetadata->second.m_conditionVariable->wait(uniqueLock, [&foundItemMetadata]() { return foundItemMetadata->second.m_ready; });
          return m_itemsMap[name];
        }


      }

    }

    void Start() {
      m_thread = std::thread{ &NamedItemStore::Run, this };
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

  template <typename ItemType>
  NamedItemStore<ItemType>& GetItemStore() {
    static NamedItemStore<ItemType> sm_itemStore;
    return sm_itemStore;
  }

}
