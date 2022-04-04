#pragma once

#include <cstdlib>
#include <iostream>

template <typename Type>
class ArrayPtr {
public:
    // �������������� ArrayPtr ������� ����������
    ArrayPtr() = default;

    // ������ � ���� ������ �� size ��������� ���� Type.
    // ���� size == 0, ���� raw_ptr_ ������ ���� ����� nullptr
    // ������������� �������� ������������� �� ��������� ��� ���� Type
    explicit ArrayPtr(size_t size) {
        if (size == 0) {
            raw_ptr_ = nullptr;
        }
        else {
            Type* new_ptr = new Type[size * sizeof(Type)];
            raw_ptr_ = new_ptr;
        }
    }

    // ����������� �� ������ ���������, ��������� ����� ������� � ���� ���� nullptr
    explicit ArrayPtr(Type* raw_ptr) noexcept
        : raw_ptr_(raw_ptr) {
    }

    // ��������� �����������
    ArrayPtr(const ArrayPtr&) = delete;    

    // ����������� �����������
    ArrayPtr(ArrayPtr&& other) noexcept {
        raw_ptr_ = other.raw_ptr_;
        other.raw_ptr_ = nullptr;
    }

    ~ArrayPtr() {
        delete[] raw_ptr_;
    }

    // ��������� ������������
    ArrayPtr& operator=(const ArrayPtr&) = delete;

    ArrayPtr& operator=(ArrayPtr&& other) noexcept {
        raw_ptr_ = other.raw_ptr_;
        other.raw_ptr_ = nullptr;

        return *this;
    }

    // ���������� ��������� �������� � ������, ���������� �������� ������ �������
    // ����� ������ ������ ��������� �� ������ ������ ����������
    [[nodiscard]] Type* Release() noexcept {
        Type* temp_ptr = raw_ptr_;
        raw_ptr_ = nullptr;
        return temp_ptr;
    }

    // ���������� ������ �� ������� ������� � �������� index
    Type& operator[](size_t index) noexcept {
        return *(raw_ptr_ + index);
    }

    // ���������� ����������� ������ �� ������� ������� � �������� index
    const Type& operator[](size_t index) const noexcept {
        return *(raw_ptr_ + index);
    }

    // ���������� true, ���� ��������� ���������, � false � ��������� ������
    explicit operator bool() const {
        return (raw_ptr_ != nullptr);
    }

    // ���������� �������� ������ ���������, ��������� ����� ������ �������
    Type* Get() const noexcept {
        return raw_ptr_;
    }

    // ������������ ��������� ��������� �� ������ � �������� other
    void swap(ArrayPtr& other) noexcept {
        std::swap(this->raw_ptr_, other.raw_ptr_);
    }

private:
    Type* raw_ptr_ = nullptr;
};