#pragma once

#include <vector>
#include <random>
#include <optional>
#include <memory>
#include <string>

template <typename Key, typename Value>
class SkipList {
public:
    static constexpr size_t kMaxHeight = 16;
    static constexpr double kBranching = 0.5; 

    struct Node {
        Key key;
        Value value;
        // forward[i] — pointer to the next node on i`th level
        std::vector<Node*> forward;

        Node(Key k, Value v, size_t height)
                : key(std::move(k)), value(std::move(v)), forward(height, nullptr) {}
    };

    SkipList()
            : head_(new Node(Key{}, Value{}, kMaxHeight)),
              max_height_(1),
              dist_(0.0, 1.0) {}

    ~SkipList() {
        Node* current = head_;
        while (current != nullptr) {
            Node* next = current->forward[0];
            delete current;
            current = next;
        }
    }

    SkipList(const SkipList&) = delete;
    SkipList& operator=(const SkipList&) = delete;

    // Insert/Update by key
    void Put(const Key& key, const Value& value) {
        std::vector<Node*> update(kMaxHeight, nullptr);
        Node* current = head_;

        for (int i = static_cast<int>(max_height_) - 1; i >= 0; --i) {
            while (current->forward[i] != nullptr && current->forward[i]->key < key) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];

        // Update if key exists
        if (current != nullptr && current->key == key) {
            current->value = value;
            return;
        }

        // Insert otherwise
        size_t node_height = RandomHeight();
        if (node_height > max_height_) {
            for (size_t i = max_height_; i < node_height; ++i) {
                update[i] = head_;
            }
            max_height_ = node_height;
        }

        Node* new_node = new Node(key, value, node_height);
        for (size_t i = 0; i < node_height; ++i) {
            new_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = new_node;
        }
    }

    std::optional<Value> Get(const Key& key) const {
        Node* current = head_;
        for (int i = static_cast<int>(max_height_) - 1; i >= 0; --i) {
            while (current->forward[i] != nullptr && current->forward[i]->key < key) {
                current = current->forward[i];
            }
        }

        current = current->forward[0];
        if (current != nullptr && current->key == key) {
            return current->value;
        }
        return std::nullopt;
    }

    bool Contains(const Key& key) const {
        return Get(key).has_value();
    }

    class Iterator {
    public:
        explicit Iterator(Node* node) : current_(node) {}

        const Key& key() const { return current_->key; }
        const Value& value() const { return current_->value; }

        Iterator& operator++() {
            if (current_ != nullptr) {
                current_ = current_->forward[0];
            }
            return *this;
        }

        bool operator==(const Iterator& other) const { return current_ == other.current_; }
        bool operator!=(const Iterator& other) const { return current_ != other.current_; }

        [[nodiscard]] bool Valid() const { return current_ != nullptr; }

    private:
        Node* current_;
    };

    Iterator begin() const {
        return Iterator(head_->forward[0]);
    }

    Iterator end() const {
        return Iterator(nullptr);
    }


    // Return iterator with key >= target (like std::map::lower_bound)
    Iterator lower_bound(const Key& target) const {
        Node* current = head_;
        for (int i = static_cast<int>(max_height_) - 1; i >= 0; --i) {
            while (current->forward[i] != nullptr && current->forward[i]->key < target) {
                current = current->forward[i];
            }
        }
        return Iterator(current->forward[0]);
    }

    // Like LevelDB
    Iterator FindGreaterOrEqual(const Key& target) const {
        return lower_bound(target);
    }


private:
    size_t RandomHeight() {
        size_t height = 1;
        while (height < kMaxHeight && dist_(rng_) < kBranching) {
            height++;
        }
        return height;
    }

    Node* head_;
    size_t max_height_;
    std::mt19937 rng_{std::random_device{}()};
    std::uniform_real_distribution<double> dist_;
};