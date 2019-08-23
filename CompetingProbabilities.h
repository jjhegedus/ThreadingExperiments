#pragma once

#include "pch.h"
#include <vector>
#include <functional>
#include <utility>
#include <chrono>
#include <condition_variable>
#include <algorithm>
#include <thread>
#include <memory>
#include <mutex>

#include "TypeUtilities.h"


using namespace std::chrono;
using namespace std::chrono_literals;

namespace ndtech {

  template<typename ItemType>
	struct CompetingProbability {
    std::vector<ItemType> m_items;
    using ProbabilityProcessingFunctionType = std::function<ItemType(ItemType)>;
    CompetingProbability::ProbabilityProcessingFunctionType m_ProbabilityProcessingFunction;
    bool m_readyForNextProbability = false;

    CompetingProbability(std::function<ItemType(ItemType)> ProbabilityProcessingFunction) :
      m_ProbabilityProcessingFunction(ProbabilityProcessingFunction) {

    }

    void Process() {
      std::cout << "Process Probability" << this << std::endl;
      for (auto item : m_items) {
        std::cout << "item = " << item << std::endl;
        item.m_readyForNextProbability = true;
      }
    }

  };

  template<typename ItemType>
	struct CompetingProbabilitiesProcessing {

    CompetingProbabilitiesProcessing(std::vector<CompetingProbability<ItemType>> Probabilitys) :
      m_Probabilities(Probabilitys) {
      m_thread = std::thread{ &CompetingProbabilitiesProcessing::Run, this };
    }

    CompetingProbabilitiesProcessing(std::thread thread)
      :m_thread(std::move(thread)) {
    }

    void Run() {
      while (!m_done) {

        for (CompetingProbability<ItemType> Probability : m_Probabilities) {

          { // to scope the lock
            std::lock_guard<std::mutex> guard(m_itemsMutex);
            Probability.Process();
          }

        }

      }

    }

    void AddItem(ItemType item, std::size_t ProbabilityNumber) {

      std::lock_guard<std::mutex> guard(m_itemsMutex);
      m_Probabilities[ProbabilityNumber].m_items.push_back(item);

    }

    void Join() {
      this->m_done = true;
      m_thread.join();
    }

  private:
    std::thread                                                                         m_thread;
    std::vector<ndtech::CompetingProbability<ItemType>>                           m_Probabilities;
    std::mutex                                                                          m_itemsMutex;
    bool                                                                                m_done = false;
  };

  template <typename ItemType>
  CompetingProbabilitiesProcessing<ItemType> CreateCompetingProbabilitiesProcessing(std::vector<CompetingProbability<ItemType>> Probabilitys) {
    return CompetingProbabilitiesProcessing(Probabilitys);
  }

}