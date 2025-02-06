#include <vector>
#include <unordered_set>
#include <mutex>
#include <iostream>
#include <memory>

// NoMutex class for thread-unsafe implementation
class NoMutex {
public:
    void lock() {}
    void unlock() {}
};

// ReferenceTracker class template
template <typename T, typename MutexType = NoMutex>
class ReferenceTracker {
public:
    void addReference(T*& ref) {
        std::lock_guard<MutexType> lock(mutex);
        references.insert(&ref);
    }

    void removeReference(T*& ref) {
        std::lock_guard<MutexType> lock(mutex);
        references.erase(&ref);
    }

    void clearReferences() {
        std::lock_guard<MutexType> lock(mutex);
        for (T** ref : references) {
            if (ref) {
                *ref = nullptr;
            }
        }
        references.clear();
    }

private:
    std::unordered_set<T**> references;
    MutexType mutex;
};

// TrackedObject class template
template <typename MutexType = NoMutex>
class TrackedObject {
public:
    TrackedObject() : tracker(std::make_unique<ReferenceTracker<TrackedObject, MutexType>>()) {}

    ~TrackedObject() {
        std::cout << "TrackedObject destroyed\n";
        tracker->clearReferences(); // Clear all references before destruction
    }

    void addReference(TrackedObject*& ref) {
        tracker->addReference(ref);
    }

    void removeReference(TrackedObject*& ref) {
        tracker->removeReference(ref);
    }

private:
    std::unique_ptr<ReferenceTracker<TrackedObject, MutexType>> tracker;
};

// Demo for thread-unsafe version
void demoThreadUnsafe() {
    std::cout << "=== Thread-Unsafe Demo ===\n";
    TrackedObject<NoMutex>* obj = new TrackedObject<NoMutex>(); 
    TrackedObject<NoMutex>* ref1 = obj;
    TrackedObject<NoMutex>* ref2 = obj;

    obj->addReference(ref1);
    obj->addReference(ref2);

    delete obj;

    if (ref1 == nullptr && ref2 == nullptr) {
        std::cout << "References cleared successfully.\n";
    } else {
        std::cout << "References still pointing to old memory!\n";
    }
}

// Demo for thread-safe version
void demoThreadSafe() {
    std::cout << "=== Thread-Safe Demo ===\n";
    TrackedObject<std::mutex>* obj = new TrackedObject<std::mutex>();
    TrackedObject<std::mutex>* ref1 = obj;
    TrackedObject<std::mutex>* ref2 = obj;

    obj->addReference(ref1);
    obj->addReference(ref2);

    delete obj;

    if (ref1 == nullptr && ref2 == nullptr) {
        std::cout << "References cleared successfully.\n";
    } else {
        std::cout << "References still pointing to old memory!\n";
    }
}

int main() {
    
    demoThreadUnsafe();
    demoThreadSafe();

    return 0;
}
