#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <cstring>
#include <compare>

enum class ValueType : uint8_t {
    kTypeDeletion = 0x00,
    kTypeValue = 0x01
};

using SequenceNumber = uint64_t;

class InternalKey {
public:
    InternalKey() : sequence_number_(0), type_(ValueType::kTypeValue) {}

    InternalKey(std::string_view user_key,
                SequenceNumber seq,
                ValueType type) :
                user_key_(user_key),
                sequence_number_(seq),
                type_(type) {}

    [[nodiscard]] std::string_view user_key() const noexcept { return user_key_; }
    [[nodiscard]] SequenceNumber sequence_number() const noexcept { return sequence_number_; }
    [[nodiscard]] ValueType type() const noexcept { return type_; }

    // Comparator for SkipList:
    // 1. user_key in ascending order
    // 2. sequence_number in descending order (more recent entries come first)
    bool operator<(const InternalKey& other) const {
        if (user_key_ != other.user_key_) {
            return user_key_ < other.user_key_;
        }
        return sequence_number_ > other.sequence_number_;
    }

    bool operator==(const InternalKey& other) const {
        return user_key_ == other.user_key_ &&
               sequence_number_ == other.sequence_number_ &&
               type_ == other.type_;
    }

private:
    std::string user_key_;
    SequenceNumber sequence_number_;
    ValueType type_;
};