#pragma once

#include <string>
#include <mutex>
#include <iterator>
#include <map>
#include <utility>

namespace ndtech {

  template <typename ItemType>
  std::vector<ItemType> GetItemStore() {
    static std:vector<ItemType> m_items;

    return m_items;
  }

  template<typename ItemType>
  struct ItemProcessor {
    void Process(std::vector<ItemType>* itemStore) {

    }

  };

  template<typename ItemType>
  struct ItemStoreProcessor {

  };

}
