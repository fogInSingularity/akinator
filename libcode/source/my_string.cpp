#include "../include/my_string.h"

//public-----------------------------------------------------------------------

StringError String::Ctor(const wchar_t* origin, const size_t min_alloc) {
  ASSERT(origin != nullptr);

  size_ = wcslen(origin) + 1; // \0 included
  cap_ = MAX(size_ * kStrMultiplier, min_alloc);

  data_ = (wchar_t*)calloc(cap_, sizeof(wchar_t));
  if (data_ == nullptr) { return StringError::kCantAlloc; }

  memcpy(data_, origin, size_);

  return StringError::kSuccess;
}

void String::Dtor() {
  free(data_);

  data_ = nullptr;
  size_ = 0;
  cap_  = 0;
}

bool String::IsEmpty() {
  return !(Size());
}

size_t String::Size() {
  return size_ - 1;
}

size_t String::Length() {
  return Size();
}

size_t String::Capacity() {
  return cap_ - 1;
}

StringError String::Resize(size_t new_cap) {
  size_t new_size = (new_cap <= size_ - 1) ? (new_cap + 1) : size_;

  wchar_t* hold = data_;
  data_ = (wchar_t*)Recalloc(data_,
                             (new_cap + 1) * sizeof(wchar_t),
                             new_size * sizeof(wchar_t));
  if (data_ == nullptr) {
    data_ = hold;

    return StringError::kCantAlloc;
  }

  data_[new_cap] = L'\0';
  size_ = new_size;
  cap_ = new_cap + 1;

  return StringError::kSuccess;
}

StringError String::Reserve(size_t new_cap) {
  if (new_cap <= cap_ - 1) {
    return StringError::kSuccess;
  }

  return Resize(new_cap);
}

StringError String::Clear() {
  memset(data_, 0, cap_ * sizeof(wchar_t));
  size_ = 1;
  return Reserve(kStrMinAlloc - 1);
}

StringError String::ShrinkToFit() {
  return Reserve(size_ - 1);
}

wchar_t* String::At(Index ind) {
  if (ind >= size_ - 1) { return nullptr; }
  return data_ + ind;
}

const wchar_t* String::CStr() {
  return Data();
}

const wchar_t* String::Data() {
  return data_;
}

StringError String::Append(String* add_str) {
  ASSERT(add_str != nullptr);
  ASSERT(add_str->Data() != nullptr);

  return Append(add_str->Size(), add_str->Data());
}

StringError String::Append(const wchar_t* add_str) {
  ASSERT(add_str != nullptr);

  return Append(wcslen(add_str), add_str);
}

StringError String::PushBack(wchar_t ch) {
  StringError error = Reserve(size_ + 1);
  if (error != StringError::kSuccess) { return error; }

  data_[size_ - 1] = ch;
  data_[size_] = L'\0';

  size_++;

  return StringError::kSuccess;
}

wchar_t String::PopBack() {
  size_--;

  wchar_t ret = data_[size_ - 1];
  data_[size_ - 1] = L'\0';
  return ret;
}

StringError String::Assign(String* str) {
  ASSERT(str != nullptr);
  ASSERT(str->Data() != nullptr);

  Dtor();
  return Ctor(str->Data());
}

StringError String::Assign(const wchar_t* str) {
  ASSERT(str != nullptr);

  Dtor();
  return Ctor(str);
}

void StringView::Ctor(const wchar_t* origin = L"") {
  data_ = origin;
  size_ = wcslen(origin) + 1; //with \0
}

void StringView::Ctor(const wchar_t* origin, size_t len) {
  data_ = origin;
  size_ = len;
}

void StringView::Dtor() {
  data_ = L"";
  size_ = 1;
}

bool StringView::IsEmpty() {
  return !(Size());
}

size_t StringView::Size() {
  return size_ - 1;
}

size_t StringView::Length() {
  return Size();
}

const wchar_t* StringView::At(Index ind) {
  if (ind >= size_ - 1) { return nullptr; }
  return data_ + ind;
}

const wchar_t* StringView::Data() {
  return data_;
}

// view += size;

//private----------------------------------------------------------------------

StringError String::Append(size_t add_size, const wchar_t* add_str) {
  StringError error = Reserve(size_ + add_size);
  if (error != StringError::kSuccess) { return error; }

  memcpy(data_ + size_ - 1, add_str, add_size * sizeof(wchar_t));
  data_[size_ + add_size - 1] = L'\0';

  size_ += add_size;

  return StringError::kSuccess;
}
