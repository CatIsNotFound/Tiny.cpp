

#ifndef TINY_CPP_EVENTS_HPP
#define TINY_CPP_EVENTS_HPP

#include <string>
#include <functional>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <thread>
#include <cstdint>

namespace Tiny {
    class Event {
    public:
        virtual ~Event() = default;
        void setID(uint32_t id);
        void setName(const std::string& name);
        void setEvent(const std::function<void()>& callback);
        [[nodiscard]] uint32_t eventID() const;
        [[nodiscard]] std::string_view eventName() const;
    private:
        uint32_t _id{};
        std::string _name{};
        std::function<void()> _event{};
    };

    class EventBus {
    public:
        using constEventIter = std::unordered_map<uint32_t, Event>::const_iterator;
        using eventIter = std::unordered_map<uint32_t, Event>::const_iterator;
        static EventBus& global();
        virtual ~EventBus() = default;

        bool pushEvent(const Event& event);
        bool removeEvent(uint32_t event_id);
        eventIter begin();
        [[nodiscard]] constEventIter cbegin() const;
        eventIter end();
        [[nodiscard]] constEventIter cend() const;
        [[nodiscard]] Event& at(uint32_t event_id) const;
        [[nodiscard]] size_t size() const;

        EventBus(const EventBus&) = delete;
        EventBus(EventBus&&) = delete;
        EventBus& operator=(const EventBus&) = delete;
        EventBus& operator=(EventBus&&) = delete;
    protected:
        explicit EventBus();
    private:
        std::unordered_map<uint32_t, Event> _events_bus;

        static EventBus _global;
    };
}


#endif //TINY_CPP_EVENTS_HPP