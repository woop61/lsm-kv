#pragma once

#include "engine/skiplist.hpp"
#include "engine/internal_key.hpp"
#include <string>
#include <string_view>
#include <optional>
#include <cstdint>
#include <limits>

class MemTable {
public:
    MemTable() : approximate_memory_usage_(0), last_seq_(0) {}
    ~MemTable() = default;

    MemTable(const MemTable&) = delete;
    MemTable& operator=(const MemTable&) = delete;
    MemTable(MemTable&&) noexcept = default;
    MemTable& operator=(MemTable&&) noexcept = default;

    void Put(std::string_view key, std::string_view value) {
        InternalKey internal_key(key, ++last_seq_, ValueType::kTypeValue);
        TrackMemory(key, value);
        table_.Put(std::move(internal_key), std::string(value));
    }

    void Delete(std::string_view key) {
        InternalKey internal_key(key, ++last_seq_, ValueType::kTypeDeletion);
        TrackMemory(key, "");
        table_.Put(std::move(internal_key), "");
    }

    struct GetResult {
        bool found{false};
        bool is_deleted{false};
        std::string value{};
    };

    // Read only relevant version of key
    GetResult Get(std::string_view key) const {
        InternalKey lookup_key(key, std::numeric_limits<SequenceNumber>::max(), ValueType::kTypeValue);

        auto it = table_.lower_bound(lookup_key);
        if (it.Valid() && it.key().user_key() == key) {
            if (it.key().type() == ValueType::kTypeDeletion) {
                return GetResult{.found = true, .is_deleted = true, .value = ""};
            }
            return GetResult{.found = true, .is_deleted = false, .value = it.value()};
        }

        return GetResult{.found = false, .is_deleted = false, .value = ""};
    }

    [[nodiscard]] size_t ApproximateMemoryUsage() const noexcept {
        return approximate_memory_usage_;
    }

    [[nodiscard]] SequenceNumber GetLastSequenceNumber() const noexcept {
        return last_seq_;
    }

    auto begin() const { return table_.begin(); }
    auto end() const { return table_.end(); }

private:
    void TrackMemory(std::string_view key, std::string_view value) {
        constexpr size_t kAvgPointersPerNode = 2 * sizeof(void*);
        constexpr size_t kNodeBaseSize = sizeof(typename SkipList<InternalKey, std::string>::Node);

        approximate_memory_usage_ += key.size() + value.size() + sizeof(InternalKey) + kNodeBaseSize + kAvgPointersPerNode;
    }

    SkipList<InternalKey, std::string> table_;
    size_t approximate_memory_usage_;
    SequenceNumber last_seq_;
};