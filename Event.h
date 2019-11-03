#pragma once

#include "pch.h"

#include <atomic>
#include <functional>
#include <any>

namespace ndtech {

  static std::atomic<size_t> nextEventId = 0;

  enum class EventCategory {
    Synchronous,
    AnyThreadImmediate,
    SynchronizeOnMainThread,
    AsyncRequest,
    AsyncCompletion
  };

  enum class EventRepeatCategory {
    OnlyApplyLatest,
    Repeat,
    DoNotProcessDuplicatesByPredicate
  };

  enum class EventProcessingStage {
    Input,
    Logic,
    Physics,
    Animation,
    Visibility,
    Drawing
  };

  struct Event {
    size_t EventTypeId;
    std::string typeName;
    std::any payload;
    EventCategory EventCategory = EventCategory::Synchronous;
    EventRepeatCategory EventRepeatCategory = EventRepeatCategory::Repeat;
    std::function<bool(Event, Event)> predicate = nullptr;
    size_t originatingEventId = (size_t)-1;
    size_t entityId = (size_t)-1;
    size_t EventId = nextEventId.fetch_add(1);
  };


}