#pragma once

#include "TypeUtilities.h"

namespace ndtech {

  template<typename... ProcessorTypes>
	struct ProcessorGroup {

    ~ProcessorGroup() {
      std::cout << "~ProcessorGroup" << std::endl;
    }

    ProcessorGroup(ProcessorTypes&... processors) :
      m_processors{ processors... }
    {
    }

    void Start() {
      m_thread = std::thread{ &ProcessorGroup::Run, this };
    }

    void Run() {
      std::cout << "ProcessorGroup threadId = " << std::this_thread::get_id() << std::endl;
      while (!m_done) {
        Process();
      }

    }

    void Process() {

      TypeUtilities::ForTuple(
        [](auto& processor) {

          processor.Process();

        },
        m_processors);
    }

    void Join() {
      this->m_done = true;
      m_thread.join();
    }

  private:
    std::thread                                                                         m_thread;
    std::tuple<ProcessorTypes&...>                                                       m_processors;
    bool                                                                                m_done = false;
  };

}