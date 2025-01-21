#include <vector>
#include <algorithm>
#include <iostream>

template <typename T>
class ReferenceTracker {
public:
    // Add a pointer to the reference list
    void addReference(T*& ref) {
        references.push_back(&ref);
    }

    // Remove a pointer from the reference list
    void removeReference(T*& ref) {
        auto it = std::find(references.begin(), references.end(), &ref);
        if (it != references.end()) {
            references.erase(it);
        }
    }

    // Set all tracked references to nullptr
    void clearReferences() {
        for (T** ref : references) {
            *ref = nullptr;
        }
        references.clear();
    }

    ~ReferenceTracker() {
        clearReferences();
    }

private:
    std::vector<T**> references;
};

class TrackedObject {
public:
    TrackedObject() : tracker(new ReferenceTracker<TrackedObject>()) {}

    ~TrackedObject() {
        std::cout << "TrackedObject destroyed\n";
        tracker->clearReferences(); // Clear all references before destruction
        delete tracker;
    }

    void addReference(TrackedObject*& ref) {
        tracker->addReference(ref);
    }

    void removeReference(TrackedObject*& ref) {
        tracker->removeReference(ref);
    }

private:
    ReferenceTracker<TrackedObject>* tracker;
};

int main() {
    TrackedObject* obj = new TrackedObject();
    TrackedObject* ref1 = obj;  // Another reference
    TrackedObject* ref2 = obj;  // Another reference

    // Track references
    obj->addReference(ref1);
    obj->addReference(ref2);

    // Destroy the object
    delete obj;

    // Now ref1 and ref2 are set to nullptr
    if (ref1 == nullptr && ref2 == nullptr) {
        std::cout << "References cleared successfully.\n";
    } else {
        std::cout << "References still pointing to old memory!\n";
    }

    return 0;
}
