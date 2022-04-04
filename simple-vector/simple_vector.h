#pragma once

#include <cassert>
#include <initializer_list>
#include <iterator>
#include <algorithm>

#include "array_ptr.h"

// ����� ������ ��� ����������� ������������ ����������� SimpleVector(Reserve(X));
class ReserveProxyObj {
public:
    ReserveProxyObj(size_t capacity)
        : capacity_(capacity) {        
    }

    size_t Get_capacity() {
        return capacity_;
    }

private:
    size_t capacity_;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;      

    // ����������� �� ���������
    SimpleVector() noexcept = default;

    // ������ ������ �� size ���������, ������������������ ��������� �� ���������
    explicit SimpleVector(size_t size) {
        size_ = size;
        capacity_ = size;

        ArrayPtr<Type> temp_vector(capacity_);        
        //std::fill(temp_vector.Get(), temp_vector.Get() + size_, Type());
        std::generate(temp_vector.Get(), temp_vector.Get() + size_, []() { return Type(); });
        items_.swap(temp_vector);
    }

    // ������ ������ �� size ���������, ������������������ ��������� value
    SimpleVector(size_t size, const Type& value) {
        size_ = size;
        capacity_ = size;

        SimpleVector temp_vector(capacity_);
        std::fill(temp_vector.begin(), temp_vector.end(), value);
        items_.swap(temp_vector.items_);        
    } 

    // ������ ������ �� std::initializer_list
    SimpleVector(std::initializer_list<Type> init) {
        size_ = init.size();
        capacity_ = init.size();

        SimpleVector temp_vector(capacity_);
        std::copy(init.begin(), init.end(), temp_vector.begin());

        items_.swap(temp_vector.items_);
    }

    // �����������
    SimpleVector(const SimpleVector& other) {
        // ������ ��������� ������
        SimpleVector temp_vector(other.capacity_);
        temp_vector.Resize(other.size_);
        // �������� � ���� �������� �� other
        std::copy(other.begin(), other.end(), temp_vector.begin());

        // ������ ������� ��������� ������ � ������ ������
        swap(temp_vector);
    }

    // �������� ����������� (�����������?)
    SimpleVector(SimpleVector&& other) noexcept
        : items_(std::move(other.items_)), size_(std::move(other.size_)), capacity_(std::move(other.capacity_)) {
        other.size_ = 0;
    }
        
    SimpleVector(ReserveProxyObj reserve_obj) {
        SimpleVector temp_vector(reserve_obj.Get_capacity());
        items_.swap(temp_vector.items_);

        size_ = 0;
        capacity_ = reserve_obj.Get_capacity();
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            SimpleVector temp_vector(new_capacity);

            std::copy(begin(), end(), temp_vector.begin());
            temp_vector.Resize(size_);

            swap(temp_vector);
        }
    }    

    // ���������� ���������� ��������� � �������
    size_t GetSize() const noexcept {        
        return size_;
    }

    // ���������� ����������� �������
    size_t GetCapacity() const noexcept {        
        return capacity_;
    }

    // ��������, ������ �� ������
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // ���������� ������ �� ������� � �������� index
    Type& operator[](size_t index) noexcept {
        // ��������� ��� ������ �� ������� �� ������� �������
        assert(index < size_);

        return items_[index];
    }

    // ���������� ����������� ������ �� ������� � �������� index
    const Type& operator[](size_t index) const noexcept {
        // ��������� ��� ������ �� ������� �� ������� �������
        assert(index < size_);

        return items_[index];
    }

    // ���������� ����������� ������ �� ������� � �������� index
    // ����������� ���������� std::out_of_range, ���� index >= size
    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Out of range");
        }
        return items_[index];
    }

    // ���������� ����������� ������ �� ������� � �������� index
    // ����������� ���������� std::out_of_range, ���� index >= size
    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Out of range");
        }
        return items_[index];
    }

    // �������� ������ �������, �� ������� ��� �����������
    void Clear() noexcept {
        size_ = 0;
    }

    // �������� ������ �������.
    // ��� ���������� ������� ����� �������� �������� �������� �� ��������� ��� ���� Type
    void Resize(size_t new_size) {
        // ������ ���� ����� ������ ��������� ������ ������
        if (new_size > size_) {
            if (new_size > capacity_) {
                // ����� ������ ������ ������������ �����������
                // ����������� ����� �����������
                size_t new_capacity = new_size*2;

                SimpleVector<Type> new_vector(new_capacity);
                std::generate(new_vector.begin(), new_vector.end(), []() { return Type(); });;
                std::copy(std::make_move_iterator(begin()), std::make_move_iterator(end()), new_vector.begin());

                items_.swap(new_vector.items_);                
                capacity_ = new_capacity;
            }
            else {
                // ����� ������ ������ �������, �� ������ �����������
                // ���������� ��������� ����� �������� ��������� �� ��������� ��� Type
                size_t old_size = size_;                
                std::generate((begin() + old_size), end(), []() { return Type(); });
            }
        }
        // ���� �� ���, �� ������ ������ ������
        size_ = new_size;        
    }    

    // ���������� �������� �� ������ �������
    // ��� ������� ������� ����� ���� ����� (��� �� �����) nullptr
    Iterator begin() noexcept {
        return items_.Get();
    }

    // ���������� �������� �� �������, ��������� �� ���������
    // ��� ������� ������� ����� ���� ����� (��� �� �����) nullptr
    Iterator end() noexcept {
        return items_.Get() + size_;
    }

    // ���������� ����������� �������� �� ������ �������
    // ��� ������� ������� ����� ���� ����� (��� �� �����) nullptr
    ConstIterator begin() const noexcept {
        return items_.Get();
    }

    // ���������� �������� �� �������, ��������� �� ���������
    // ��� ������� ������� ����� ���� ����� (��� �� �����) nullptr
    ConstIterator end() const noexcept {
        return items_.Get() + size_;
    }

    // ���������� ����������� �������� �� ������ �������
    // ��� ������� ������� ����� ���� ����� (��� �� �����) nullptr
    ConstIterator cbegin() const noexcept {
        return items_.Get();
    }

    // ���������� �������� �� �������, ��������� �� ���������
    // ��� ������� ������� ����� ���� ����� (��� �� �����) nullptr
    ConstIterator cend() const noexcept {
        return items_.Get() + size_;
    }    

    // �������� ������������
    SimpleVector& operator=(const SimpleVector& rhs) {
        if (rhs == *this) return *this;
        SimpleVector temp_vector(rhs);
        swap(temp_vector);

        return *this;
    }

    // �������� �����������
    SimpleVector& operator=(SimpleVector&& rhs) noexcept {
        if (rhs == *this) return *this;

        items_ = std::move(rhs.items_);
        size_ = std::move(rhs.size_);
        capacity_ = std::move(rhs.capacity_);

        rhs.size_ = 0;

        return *this;
    }

    // ��������� ������� � ����� �������
    // ��� �������� ����� ����������� ����� ����������� �������
    void PushBack(const Type& item) {
        // ���� ������ �� �������� ���������, ��������� ������� �� ��������� � ����������� ������
        if (size_ < capacity_) {
            items_[size_++] = item;            
        }
        else {
            // ���� ������ �������� ��������� ������ ����� ������ � ��������� ������������
            SimpleVector new_vector(2 * capacity_);
            new_vector.Resize(size_ + 1);
            // �������� � ���� ������ ��������
            std::copy(begin(), end(), new_vector.begin());
            new_vector[new_vector.size_ - 1] = item;

            swap(new_vector);
        }
    }

    // ��������� ������� � ����� �������, ��������� ���
    // ��� �������� ����� ����������� ����� ����������� �������
    void PushBack(Type&& item) {
        // ���� ������ �� �������� ���������, ��������� ������� �� ��������� � ����������� ������
        if (size_ < capacity_) {
            items_[size_++] = std::move(item);
        }
        else {
            // ���� ������ �������� ��������� ������ ����� ������ � ��������� ������������
            SimpleVector new_vector(capacity_ * 2);
            new_vector.Resize(size_ + 1);
            // �������� � ���� ������ ��������
            std::copy(std::make_move_iterator(begin()), std::make_move_iterator(end()), new_vector.begin());
            new_vector[new_vector.size_ - 1] = std::move(item);

            swap(new_vector);
        }
    }
    // ��������� �������� value � ������� pos.
    // ���������� �������� �� ����������� ��������
    // ���� ����� �������� �������� ������ ��� �������� ���������,
    // ����������� ������� ������ ����������� �����, � ��� ������� ������������ 0 ����� ������ 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        Iterator pos_to_insert = const_cast<Iterator>(pos);
        // ��������� ��� pos ��������� � ��������� �� begin() �� end()
        assert( (begin() <= pos_to_insert) && (pos_to_insert <= end()) );

        if (size_ < capacity_) {
            // ������ ����� �� ���� �������� ����������� �������
            Resize(++size_);
            std::copy_backward(pos_to_insert, end() - 1, end());
            *pos_to_insert = value;
            return pos_to_insert;
        }
        else {
            // ������ ����� ���� �������� �����������
            SimpleVector new_vector(2 * capacity_);
            new_vector.Resize(size_ + 1);

            auto pos_in_new_vector = std::copy(begin(), pos_to_insert, new_vector.begin());            
            *pos_in_new_vector = value;            
            std::copy(pos_to_insert, end(), pos_in_new_vector + 1);
            
            swap(new_vector);

            return pos_in_new_vector;
        }
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        Iterator pos_to_insert = const_cast<Iterator>(pos);
        // ��������� ��� pos ��������� � ��������� �� begin() �� end()
        assert((begin() <= pos_to_insert) && (pos_to_insert <= end()));

        if (size_ < capacity_) {
            // ������ ����� �� ���� �������� ����������� �������
            Resize(++size_);
            std::copy_backward(std::make_move_iterator(pos_to_insert), std::make_move_iterator(end() - 1), end());
            *pos_to_insert = std::move(value);
            return pos_to_insert;
        }
        else {
            // ������ ����� ���� �������� �����������
            SimpleVector new_vector(2 * capacity_);
            new_vector.Resize(size_ + 1);

            auto pos_in_new_vector = std::copy(std::make_move_iterator(begin()), std::make_move_iterator(pos_to_insert), new_vector.begin());
            *pos_in_new_vector = std::move(value);
            std::copy(std::make_move_iterator(pos_to_insert), std::make_move_iterator(end()), pos_in_new_vector + 1);

            swap(new_vector);

            return pos_in_new_vector;
        }
    }

    // "�������" ��������� ������� �������. ������ �� ������ ���� ������
    void PopBack() noexcept {
        assert(!IsEmpty());
        // ���� ������ �� ���� ��������� ��� ������ �� 1
        --size_;        
    }

    // ������� ������� ������� � ��������� �������
    Iterator Erase(ConstIterator pos) {    
        assert(!IsEmpty());
        Iterator pos_to_delete = const_cast<Iterator>(pos);
        // ��������� ��� pos ��������� � ��������� �� begin() �� end()
        assert((begin() <= pos_to_delete) && (pos_to_delete <= end()));

        std::copy(std::make_move_iterator(pos_to_delete + 1), std::make_move_iterator(end()), pos_to_delete);
        --size_;
        return pos_to_delete;
    }

    // ���������� �������� � ������ ��������
    void swap(SimpleVector& other) noexcept {
        items_.swap(other.items_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

private:
    ArrayPtr<Type> items_;

    size_t size_ = 0; // ������ �������
    size_t capacity_ = 0; // ������������ �����������
};

template<typename Type>
void PrintSimpleVector(const SimpleVector<Type>& values) {
    for (auto value : values) {
        std::cout << value << " ";
    }
    std::cout << std::endl;
}

template<typename Type>
void TestPrintSimpleVector(const SimpleVector<Type>& values) {
    std::cout << "Elements: ";
    for (auto value : values) {
        std::cout << value << " ";
    }
    std::cout << std::endl;
    std::cout << "Size: " << values.GetSize() << ". Capacity: " << values.GetCapacity() << std::endl;
}


template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {    
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());;
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {    
    return !(rhs == lhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {    
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {    
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {    
    return !(lhs < rhs);;
}
