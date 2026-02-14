#pragma once
#include <Voxel/pch.h>

// Forward declare for handle
template <typename T> class Subject;

template <typename T> class ObserverHandle {
  public:
    ObserverHandle() : subject(nullptr), id(0) {}
    ObserverHandle(Subject<T>* subject, size_t id) : subject(subject), id(id) {}

    void Unsubscribe() {
        if (subject)
            subject->RemoveObserver(id);
        subject = nullptr;
    }

  private:
    Subject<T>* subject;
    size_t id;
};

template <typename T> class Subject {
  public:
    using Callback = std::function<void(const T&)>;

    // Add an observer, returns a handle that can unsubscribe
    ObserverHandle<T> AddObserver(Callback cb) {
        size_t observerId = nextId++;
        observers.emplace_back(observerId, std::move(cb));
        return ObserverHandle<T>(this, observerId);
    }

    // Notify all observers
    void Notify(const T& data) {
        for (auto& [id, cb] : observers) {
            cb(data);
        }
    }

  private:
    friend class ObserverHandle<T>;
    void RemoveObserver(size_t id) {
        observers.erase(std::remove_if(observers.begin(), observers.end(),
                                       [id](auto& pair) { return pair.first == id; }),
                        observers.end());
    }

    std::vector<std::pair<size_t, Callback>> observers;
    size_t nextId = 1;
};
