#ifndef SEMINAR_TESTING_STACK_HPP
#define SEMINAR_TESTING_STACK_HPP

#include "StdAfx.hpp"

template <typename _Tp, typename _Alloc = std::allocator<_Tp>>
class Stack {
private:
  const Stack *const headPoint_;

  size_t bufferSize_;
  size_t size_;

  size_t bufferChecksum_;
  size_t totalChecksum_;

  _Tp *buffer_;

  _Alloc allocator_;

  const Stack *const tailPoint_;

  /*
   * Constants
   */

  static constexpr uint32_t STACK_INITIAL_SIZE = 1;
  static constexpr uint32_t STACK_GROWTH_RATE = 2;
  static constexpr uint32_t STACK_MAX_SIZE = 1 << 30;

  static constexpr uint8_t STACK_POISON = 0b11000011;
  static constexpr uint8_t STACK_CHECKSUM_OFFSET = 0b11100111;
  static constexpr uint64_t STACK_POINTER_POISON = 0xABCDEF123456789F;

  enum class StackStatus {
    Good,
    InvalidSize,
    InvalidBufferSize,
    NullptrBuffer,
    InvalidHeadPoint,
    InvalidTailPoint,
    InvalidBufferChecksum,
    InvalidTotalChecksum,
    InvalidInitialization
  };

  template <typename _T1>
  static void AttachToChecksum(const _T1 &item, uint8_t *leftSum, uint8_t *rightSum) {
    auto temp = reinterpret_cast <const uint8_t *> (&item);
    for (size_t i = 0; i < sizeof(_T1); ++i) {
      *leftSum += temp[i];
      *rightSum += *leftSum;
    }
  }

  uint16_t GetBufferChecksum() const {
    uint8_t leftSum = STACK_CHECKSUM_OFFSET;
    uint8_t rightSum = STACK_CHECKSUM_OFFSET;

    for (int i = 0; i < bufferSize_; ++i)
      AttachToChecksum(buffer_[i], &leftSum, &rightSum);

    return (static_cast <uint16_t> (leftSum) << 8) | rightSum;
  }

  uint16_t GetTotalChecksum() const {
    uint8_t leftSum = STACK_CHECKSUM_OFFSET;
    uint8_t rightSum = STACK_CHECKSUM_OFFSET;

    AttachToChecksum(bufferSize_, &leftSum, &rightSum);
    AttachToChecksum(size_, &leftSum, &rightSum);
    AttachToChecksum(headPoint_, &leftSum, &rightSum);
    AttachToChecksum(tailPoint_, &leftSum, &rightSum);
    AttachToChecksum(buffer_, &leftSum, &rightSum);
    AttachToChecksum(bufferChecksum_, &leftSum, &rightSum);
    AttachToChecksum(allocator_, &leftSum, &rightSum);

    return (static_cast <uint16_t> (leftSum) << 8) | rightSum;
  }

  void ReinitializeChecksum() {
    bufferChecksum_ = GetBufferChecksum();
    totalChecksum_ = GetTotalChecksum();
  }

  void DropInfo(StackStatus status) const {
    std::cerr << "Stack status: " << static_cast<int>(status) << "\n";
    std::cerr << "Items type: " << typeid(_Tp).name() << "\n";
    std::cerr << "Items size: " << sizeof(_Tp) << "\n";

    if (StackStatus::Good != status) {
      std::cerr << "ERROR: ";
    }

    switch (status) {
      case StackStatus::Good:
        std::cerr << "No problems detected\n";
        break;
      case StackStatus::InvalidSize:
        std::cerr << "Invalid current size\n";
        break;
      case StackStatus::InvalidBufferSize:
        std::cerr << "Invalid buffer size\n";
        break;
      case StackStatus::NullptrBuffer:
        std::cerr << "Null pointer buffer detected\n";
        break;
      case StackStatus::InvalidHeadPoint:
        std::cerr << "Invalid heading pointer: got " << headPoint_ << ", expected: " << this << "\n";
        break;
      case StackStatus::InvalidTailPoint:
        std::cerr << "Invalid tail pointer: got " << tailPoint_ << ", expected: " << this << "\n";
        break;
      case StackStatus::InvalidBufferChecksum:
        std::cerr << "Invalid buffer checksum : got " << GetBufferChecksum() << ", expected: " << bufferChecksum_ << "\n";
        break;
      case StackStatus::InvalidTotalChecksum:
        std::cerr << "Invalid checksum: got " << GetTotalChecksum() << ", expected: " << totalChecksum_ << "\n";
        break;
      case StackStatus::InvalidInitialization:
        std::cerr << "Buffer contains unpoisoned items\n";
        break;
    }

    std::cerr << "Items count: " << size_ << "\n";
    std::cerr << "Buffer size: " << bufferSize_ << "\n";

    std::cerr << "Head point: " << headPoint_ << "\n";
    std::cerr << "Tail point: " << tailPoint_ << "\n";
    std::cerr << "Buffer: " << buffer_ << "\n";
    std::cerr << "Buffer checksum: " << bufferChecksum_ << "\n";
    std::cerr << "Total checksum: " << totalChecksum_ << "\n";

    if (buffer_) {
      for (size_t i = 0; i < bufferSize_; ++i) {
        std::cerr << (i < size_ ?
                      "<exist> " :
                      "    ");

        auto *temp = reinterpret_cast <const uint8_t *> (buffer_ + i);
        bool poisoned = true;

        for (size_t j = 0; j < sizeof(_Tp); ++j) {
          std::cerr << std::setw(2) << std::setfill('0') <<
                    std::hex << static_cast<int>(temp[j]) << " ";
          poisoned &= temp[j] == STACK_POISON;
        }

        std::cerr << (poisoned ? " <poisoned>" : "") << "\n";
      }

      std::cerr << "\n";
    }
  }

  void Poison(_Tp *item) {
    auto *data = reinterpret_cast <uint8_t *> (item);
    for (size_t i = 0; i < sizeof(_Tp); ++i) {
      data[i] = STACK_POISON;
    }
  }

  void* PoisonPointer(const void* const item) const {
    return (Stack *) (((uint64_t) item) ^ STACK_POINTER_POISON);
  }

  _Tp *ConstructBuffer(size_t size) {
    _Tp *buffer = std::allocator_traits <_Alloc>::allocate(allocator_, size);
    for (auto *pointer = buffer; pointer < buffer + size; ++pointer) {
      Poison(pointer);
    }

    return buffer;
  }

  void DestroyBuffer(_Tp **buffer, size_t bufferSize, size_t size) {
    for (size_t i = 0; i < size; ++i) {
      std::allocator_traits <_Alloc>::destroy(allocator_, *buffer + i);
    }

    std::allocator_traits <_Alloc>::deallocate(allocator_, *buffer, size);
    *buffer = reinterpret_cast<_Tp *>(PoisonPointer(*buffer));
  }

  void ConstructItem(size_t position, const _Tp &item) {
    std::allocator_traits <_Alloc>::construct(allocator_, buffer_ + position, item);
  }

  void Reallocate(size_t newSize) {
    _Tp *copy = buffer_;
    buffer_ = ConstructBuffer(newSize);

    for (size_t i = 0; i < size_; ++i)
      ConstructItem(i, copy[i]);

    DestroyBuffer(&copy, bufferSize_, size_);
    bufferSize_ = newSize;
  }

  StackStatus GetStackStatus() const {
    if ((size_ > bufferSize_) || (size_ > STACK_MAX_SIZE))
      return StackStatus::InvalidSize;
    if (bufferSize_ > STACK_MAX_SIZE)
      return StackStatus::InvalidBufferSize;
    if (nullptr == buffer_)
      return StackStatus::NullptrBuffer;
    if (headPoint_ != PoisonPointer(this))
      return StackStatus::InvalidHeadPoint;
    if (tailPoint_ != PoisonPointer(this))
      return StackStatus::InvalidTailPoint;
    if (GetBufferChecksum() != bufferChecksum_)
      return StackStatus::InvalidBufferChecksum;
    if (GetBufferChecksum() != bufferChecksum_)
      return StackStatus::InvalidBufferChecksum;
    if (GetTotalChecksum() != totalChecksum_)
      return StackStatus::InvalidTotalChecksum;

    auto *temp = reinterpret_cast <uint8_t *> (buffer_);
    for (size_t i = size_ * sizeof(_Tp); i < bufferSize_ * sizeof(_Tp); ++i)
      if (temp[i] != STACK_POISON)
        return StackStatus::InvalidInitialization;


    return StackStatus::Good;
  }

#ifndef TESTING
#define TRY_CORRUPT()                  \
do {                                   \
    auto status = GetStackStatus();    \
    if (status != StackStatus::Good) { \
        DropInfo(status);              \
        assert(false);                 \
    }                                  \
} while(0)                             \

#else
  #define TRY_CORRUPT()                  \
do {                                   \
    auto status = GetStackStatus();    \
    if (status != StackStatus::Good) { \
        DropInfo(status);              \
        throw std::runtime_error(      \
            "Stack exception occured, "\
            "see dump");               \
    }                                  \
} while(0)                             \

#endif

public:
  Stack(_Alloc alloc = _Alloc{})
      : headPoint_(reinterpret_cast<Stack *>(PoisonPointer(this))),
        tailPoint_(reinterpret_cast<Stack *>(PoisonPointer(this))),
        bufferSize_(STACK_INITIAL_SIZE), size_(0),
        buffer_(ConstructBuffer(bufferSize_)), allocator_{alloc} {
    ReinitializeChecksum();
  }

  ~Stack() {
    DestroyBuffer(&buffer_, bufferSize_, size_);
  }

  size_t Size() const {
    TRY_CORRUPT();
    return size_;
  }

  bool Empty() const {
    TRY_CORRUPT();
    return 0 == Size();
  }

  void Push(const _Tp &value) {
    TRY_CORRUPT();

    if (bufferSize_ <= size_)
      Reallocate(bufferSize_ * STACK_GROWTH_RATE);


    ConstructItem(size_++, value);

    ReinitializeChecksum();

    TRY_CORRUPT();
  }

  _Tp Pop() {
    TRY_CORRUPT();
    if (size_ <= 0)
      throw std::runtime_error("Can't pop: no elements in stack");

    auto item = buffer_[--size_];

    Poison(&buffer_[size_]);

    ReinitializeChecksum();
    TRY_CORRUPT();
    return item;
  }


#undef TRY_CORRUPT
};

#endif //SEMINAR_TESTING_STACK_HPP
