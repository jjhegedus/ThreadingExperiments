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
	struct PrioritizedProcessingStage {
    using StageProcessingFunctionType = std::function<ItemType(ItemType)>;

    std::vector<ItemType>       m_items;
    PrioritizedProcessingStage::StageProcessingFunctionType m_stageProcessingFunction;
    std::string                 m_name;

    ~PrioritizedProcessingStage() {
      std::cout << "~PrioritizedProcessingStage" << std::endl;
    }

    PrioritizedProcessingStage(std::function<ItemType(ItemType)> stageProcessingFunction) :
      m_stageProcessingFunction(stageProcessingFunction) {

    }

    PrioritizedProcessingStage(std::string name, std::function<ItemType(ItemType)> stageProcessingFunction) :
      m_stageProcessingFunction(stageProcessingFunction),
      m_name(name)
    {

    }

    void Process() {
      for (auto item : m_items) {
        m_stageProcessingFunction(item);
        //std::cout << "item = " << item << std::endl;
      }
    }

  };

  template<typename ItemType>
	struct PrioritizedStagedProcessor {

    ~PrioritizedStagedProcessor() {
      std::cout << "~PrioritizedStagedProcessor" << std::endl;
    }

    PrioritizedStagedProcessor(std::vector<PrioritizedProcessingStage<ItemType>> stages) :
      m_stages(stages) {
      //m_thread = std::thread{ &PrioritizedStagedProcessor::Run, this };
    }

    PrioritizedStagedProcessor(std::thread thread)
      :m_thread(std::move(thread)) {
    }

    void Start() {
      m_thread = std::thread{ &PrioritizedStagedProcessor::Run, this };
    }

    void Run() {
      while (!m_done) {
        Process();
      }

    }

    void Process() {

      { // to scope the lock
        std::lock_guard<std::mutex> guard(m_stagesMutex);

        size_t stageNumber = 0;
        bool isFirstStage = true;

        for (auto it = m_stages.begin(); it != m_stages.end(); it++) {

          if (stageNumber == 0) {
            // The first stage
            it->Process();
          }
          else if (stageNumber == m_stages.size() - 1) {
            // The last stage
            auto previousStageIterator = --it;
            it++;
            it->m_items.insert(
              it->m_items.end(),
              std::make_move_iterator(previousStageIterator->m_items.begin()),
              std::make_move_iterator(previousStageIterator->m_items.end()));

            previousStageIterator->m_items.clear();

            it->Process();

            // Since this is the last stage, we can clear the items when finished
            it->m_items.clear();
          }
          else {
            // This is a middle stage
            auto previousStageIterator = --it;
            it++;
            it->m_items.insert(
              it->m_items.end(),
              std::make_move_iterator(previousStageIterator->m_items.begin()),
              std::make_move_iterator(previousStageIterator->m_items.end()));

            previousStageIterator->m_items.clear();

            it->Process();

          }

          isFirstStage = false;
          stageNumber++;

        }

      }
    }

    void AddItem(ItemType item, std::size_t stageNumber) {

        std::lock_guard<std::mutex> guard(m_stagesMutex);
        m_stages[stageNumber].m_items.push_back(item);

    }

    void AddItem(ItemType item) {
      AddItem(item, 0);
    }

    void Join() {
      this->m_done = true;
      m_thread.join();
    }

  private:
    std::thread                                                                         m_thread;
    std::vector<ndtech::PrioritizedProcessingStage<ItemType>>                           m_stages;
    std::mutex                                                                          m_stagesMutex;
    bool                                                                                m_done = false;
  };

  template <typename ItemType>
  PrioritizedStagedProcessor<ItemType> CreatePrioritizedStagedProcessor(std::vector<PrioritizedProcessingStage<ItemType>> stages) {
    return PrioritizedStagedProcessor(stages);
  }

}