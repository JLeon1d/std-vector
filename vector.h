#pragma once

#include <initializer_list>
#include <algorithm>
#include <memory>
#include <type_traits>
#include <iterator>

template <class T>
class Vector {
public:
    typedef T value_type;
    typedef size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;

    class Iterator {
    public:
        Iterator() = default;

        Iterator(T* elem) : elem_(elem) {
        }

        Iterator(const Iterator& other) : elem_(other.elem_) {
        }

        Iterator(Iterator&& other) : elem_(std::move(other.elem_)) {
            other.elem_ = nullptr;
        }

        Iterator& operator=(const Iterator& other) {
            elem_ = other.elem_;
            return *this;
        }

        Iterator& operator=(Iterator&& other) {
            if (this == &other) {
                return *this;
            }

            elem_ = other.elem_;
            other.elem_ = nullptr;
            return *this;
        }

        Iterator& operator++() {
            ++elem_;
            return *this;
        }

        Iterator operator++(int) {
            auto old = *this;
            ++elem_;
            return old;
        }

        Iterator& operator--() {
            --elem_;
            return *this;
        }

        Iterator operator--(int) {
            auto old = *this;
            --elem_;
            return old;
        }

        Iterator& operator+=(size_type num) {
            elem_ += num;
            return *this;
        }

        Iterator& operator-=(size_type num) {
            elem_ -= num;
            return *this;
        }

        T& operator*() {
            return *elem_;
        }

        T operator*() const {
            return *elem_;
        }

        T* GetPointer() const {
            return elem_;
        }

        friend Iterator operator+(const Iterator& left, size_type right) {
            return Iterator(left.elem_ + right);
        }

        friend Iterator operator+(size_type left, const Iterator& right) {
            return Iterator(left + right.elem_);
        }

        friend std::ptrdiff_t operator-(const Iterator& left, const Iterator& right) {
            if (left.elem_ < right.elem_) {
                return right.elem_ - left.elem_;
            } else {
                return left.elem_ - right.elem_;
            }
        }

        friend bool operator==(const Iterator& left, const Iterator& right) {
            return left.elem_ == right.elem_;
        }

        friend bool operator!=(const Iterator& left, const Iterator& right) {
            return left.elem_ != right.elem_;
        }
    private:
        T* elem_;
    };

    typedef Iterator iterator;
    typedef const Iterator const_iterator;

    // constructors
    Vector() = default;

    explicit Vector(size_type size) : size_(size), cap_(size) {
        data_ = std::make_unique<T[]>(size);
    }

    explicit Vector(size_type size, const T& value) : size_(size), cap_(size) {
        data_ = std::make_unique<T[]>(size);
        std::fill(data_.get(), data_.get() + size, value);
    }

    Vector(std::initializer_list<T> list) : size_(list.size()), cap_(list.size()) {
        data_ = std::make_unique<T[]>(list.size());
        std::copy(list.begin(), list.end(), data_.get());
    }

    template <class InputIt>
        requires(std::input_iterator<InputIt>)
    Vector(InputIt first, InputIt last) {
        size_ = cap_ = std::distance(first, last);
        data_ = std::make_unique<T[]>(size_);
        std::copy(first, last, data_.get());
    }

    Vector(const Vector& other) : size_(other.size_), cap_(other.cap_) {
        if (other.data_ == nullptr) {
            data_ = nullptr;
            return;
        }
        data_ = std::make_unique<T[]>(cap_);
        std::copy(other.data_.get(), other.data_.get() + other.size_, data_.get());
    }

    Vector(Vector&& other) : size_(other.size_), cap_(other.cap_) {
        data_ = std::move(other.data_);
        other.size_ = other.cap_ = 0;
        other.data_ = nullptr;
    }

    // operator=
    Vector& operator=(const Vector& other) {
        if (this == &other) {
            return *this;
        }

        if (other.data_ == nullptr) {
            size_ = cap_ = 0;
            data_ = nullptr;
            return *this;
        }

        size_ = other.size_;
        cap_ = other.cap_;

        data_ = std::make_unique<T[]>(cap_);
        std::copy(other.data_.get(), other.data_.get() + other.size_, data_.get());
        return *this;
    }

    Vector& operator=(Vector&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        data_ = std::move(other.data_);
        size_ = other.size_;
        cap_ = other.cap_;

        other.data_ = nullptr;
        other.size_ = other.cap_ = 0;
        return *this;
    }

    Vector& operator=(std::initializer_list<T> list) {
        size_ = cap_ = list.size();
        data_ = std::make_unique<T[]>(cap_);
        std::copy(list.begin(), list.end(), data_.get());
        return *this;
    }

    // assign
    void assign(size_t count, const T& value) {
        size_ = cap_ = count;
        data_ = std::make_unique<T[]>(size_);
        std::fill(data_.get(), data_.get() + count, value);
    }

    template <class InputIt>
        requires(std::input_iterator<InputIt>)
    void assign(InputIt first, InputIt last) {
        cap_ = size_ = std::distance(first, last);
        data_ = std::make_unique<T[]>(size_);
        std::copy(first, last, data_.get());
    }

    void assign(std::initializer_list<T> ilist) {
        size_ = cap_ = ilist.size();
        data_ = std::make_unique<T[]>(size_);
        std::copy(ilist.begin(), ilist.end(), data_.get());
    }

    // element access
    reference at(size_t pos) {
        if (pos >= size_) {
            throw std::out_of_range("Vector::at out of range");
        }

        return data_[pos];
    }

    const_reference at(size_t pos) const {
        if (pos >= size_) {
            throw std::out_of_range("Vector::at out of range");
        }

        return data_[pos];
    }

    reference operator[](size_t pos) {
        return data_[pos];
    }

    const_reference operator[](size_t pos) const {
        return data_[pos];
    }  

    reference front() {
        return data_[0];
    }

    const_reference front() const {
        return data_[0];
    }

    reference back() {
        return data_[size_ - 1];
    }

    const_reference back() const {
        return data_[size_ - 1];
    }

    T* data() {
        return data_.get();
    }

    const T* data() const {
        return data_.get();
    }

    // iterators
    iterator begin() noexcept {
        return (data_ == nullptr or size_ == 0 ? nullptr : Iterator(&data_[0]));
    }

    const_iterator begin() const noexcept {
        return (data_ == nullptr or size_ == 0 ? nullptr : Iterator(&data_[0]));
    }

    const_iterator cbegin() const noexcept {
        return (data_ == nullptr or size_ == 0 ? nullptr: Iterator(&data_[0]));
    }

    iterator end() noexcept {
        if (data_ == nullptr or size_ == 0) {
            return nullptr;
        }

        auto p = &data_[size_ - 1];
        ++p;
        return Iterator(p);
    }

    const_iterator end() const noexcept {
        if (data_ == nullptr or size_ == 0) {
            return nullptr;
        }

        auto p = &data_[size_ - 1];
        ++p;
        return Iterator(p);
    }

    const_iterator cend() const noexcept {
        if (data_ == nullptr or size_ == 0) {  // UB was here
            return nullptr;
        }

        auto p = &data_[size_ - 1];
        ++p;
        return Iterator(p);
    }

    // capacity
    bool empty() const {
        return size_ == 0;
    }

    size_type size() const {
        return size_;
    }

    size_type max_size() const {
        return std::numeric_limits<size_type>::max()/sizeof(T) - static_cast<size_type>(1); 
    }

    void reserve(size_t new_cap) {
        if (new_cap <= cap_) {
            return;
        } else if (new_cap > max_size()) {
            throw std::length_error("Vector::reserve: new_cap is too large");
        }

        std::unique_ptr<T[]> new_data = std::make_unique<T[]>(new_cap);
        move_or_copy(data_.get(), new_data.get(), size_);

        data_ = std::move(new_data);
        cap_ = new_cap;
    }

    size_type capacity() const {
        return cap_;
    }

    void shrink_to_fit() {
        if (size_ == cap_) {
            return;
        }

        std::unique_ptr<T[]> new_data = std::make_unique<T[]>(size_);
        move_or_copy(data_.get(), new_data.get(), size_);

        data_ = std::move(new_data);
        cap_ = size_;
    }

    void clear() {
        size_ = 0;
    }

    iterator insert(const_iterator pos, const T& value) { 
        size_t id = static_cast<size_t>(pos - begin());
        if (size_ == cap_) {
            relocate();
        }

        if (id != size_) {
            std::copy(data_.get() + id, data_.get() + size_, data_.get() + id + 1);
        }
        data_[id] = value;
        ++size_;
        return Iterator(data_.get() + id);
    }

    iterator insert(const_iterator pos, T&& value) {
        size_t id = static_cast<size_t>(pos - begin());
        if (size_ == cap_) {
            relocate();
        }

        if (id != size_) {
            std::copy(data_.get() + id, data_.get() + size_, data_.get() + id + 1);
        }
        data_[id] = std::move(value);
        ++size_;
        return Iterator(data_.get() + id);
    }

    // iterator emplace(const_iterator pos, Args&&... args); in this realisation will be the same insert(

    iterator erase(const_iterator pos) {
        size_t id = static_cast<size_t>(pos - begin());

        std::copy(data_.get() + id + 1, data_.get() + size_, data_.get() + id);
        --size_;
        return Iterator(data_.get() + id);
    }

    void push_back(const T& value) {
        if (size_ == cap_) {
            relocate();
        }

        try {
            data_[size_] = value;
        } catch (...) {
            throw;
        }

        ++size_;
    }

    void push_back(T&& value) {
        if (size_ == cap_) {
            relocate();
        }

        try {
            data_[size_] = std::move(value);
        } catch (...) {
            throw;
        }

        ++size_;
    }

    template <class... Args>
    void emplace_back(Args&&... args) {
        if (size_ == cap_) {
            relocate();
        }

        try {
            data_[size_] = T(std::forward<Args>(args)...);
        } catch (...) {
            throw;
        }

        ++size_;
    }

    void pop_back() {
        --size_;
    }

    void resize(size_type new_size) {  // no strong exception safety guarantee
        if (new_size == size_) {
            return;
        } else if (new_size > size_) {
            reserve(new_size);
            std::fill(data_.get() + size_, data_.get() + new_size, value_type());
        } else {
            std::unique_ptr<T[]> new_data = std::make_unique<T[]>(new_size);
            move_or_copy(data_.get(), new_data.get(), new_size);
            cap_ = size_ = new_size;
            data_ = std::move(new_data);
        }

        size_ = new_size;
    }

    void resize(size_type new_size, const value_type& value) {  // no strong exception safety guarantee
        if (new_size > size_) {
            reserve(new_size);
            std::fill(data_.get() + size_, data_.get() + new_size, value);
            size_ = new_size;
        } else {
            resize(new_size);
        }
    }

    void swap(Vector& other) noexcept {
        std::swap(size_, other.size_);
        std::swap(cap_, other.cap_);
        std::swap(data_, other.data_);
    }

    // destructor
    ~Vector() {
        data_.reset();
    }

    friend bool operator==(const Vector& left, const Vector& right) {
        if (left.size_ != right.size_) {
            return false;
        }

        for (size_t id = 0; id < left.size_; ++id) {
            if (left[id] != right[id]){
                return false;
            }
        }

        return true;
    }

    friend bool operator!=(const Vector& left, const Vector& right) {
        return !(left == right);
    }

private:
    size_t size_ = 0u;
    size_t cap_ = 0u;
    std::unique_ptr<T[]> data_ = nullptr;

    void relocate() {
        if (size_ == 0) {
            reserve(1);
        } else {
            reserve(2 * size_);
        }
    }

    void move_or_copy(T* src, T* dst, size_t count) {
        if constexpr (std::is_move_constructible_v<T> and std::is_move_assignable_v<T>) {
            std::move(src, src + count, dst);
        } else {
            std::copy(src, src + count, dst);
        }
    }
};