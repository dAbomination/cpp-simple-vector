#pragma once

#include <cassert>
#include <initializer_list>
#include <iterator>
#include <algorithm>

#include "array_ptr.h"

// Класс обёртка для возможности использовать конструкцию SimpleVector(Reserve(X));
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

    // Конструктор по умолчанию
    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) {
        size_ = size;
        capacity_ = size;

        ArrayPtr<Type> temp_vector(capacity_);        
        //std::fill(temp_vector.Get(), temp_vector.Get() + size_, Type());
        std::generate(temp_vector.Get(), temp_vector.Get() + size_, []() { return Type(); });
        items_.swap(temp_vector);
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) {
        size_ = size;
        capacity_ = size;

        SimpleVector temp_vector(capacity_);
        std::fill(temp_vector.begin(), temp_vector.end(), value);
        items_.swap(temp_vector.items_);        
    } 

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) {
        size_ = init.size();
        capacity_ = init.size();

        SimpleVector temp_vector(capacity_);
        std::copy(init.begin(), init.end(), temp_vector.begin());

        items_.swap(temp_vector.items_);
    }

    // Копирование
    SimpleVector(const SimpleVector& other) {
        // Создаём временный вектор
        SimpleVector temp_vector(other.capacity_);
        temp_vector.Resize(other.size_);
        // Копируем в него элементы из other
        std::copy(other.begin(), other.end(), temp_vector.begin());

        // Меняем местами временный вектор и данный вектор
        swap(temp_vector);
    }

    // Оператор копирования (перемещения?)
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

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {        
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {        
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        // Проверяем что индекс не выходит за пределы вектора
        assert(index < size_);

        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        // Проверяем что индекс не выходит за пределы вектора
        assert(index < size_);

        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Out of range");
        }
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Out of range");
        }
        return items_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        // Случай если новый размер превышает старый размер
        if (new_size > size_) {
            if (new_size > capacity_) {
                // Новый размер больше максимальной вместимости
                // расчитываем новую вместимость
                size_t new_capacity = new_size*2;

                SimpleVector<Type> new_vector(new_capacity);
                std::generate(new_vector.begin(), new_vector.end(), []() { return Type(); });;
                std::copy(std::make_move_iterator(begin()), std::make_move_iterator(end()), new_vector.begin());

                items_.swap(new_vector.items_);                
                capacity_ = new_capacity;
            }
            else {
                // Новый размер больше старого, но меньше вместимости
                // необходимо заполнить новые элементы значением по умолчанию для Type
                size_t old_size = size_;                
                std::generate((begin() + old_size), end(), []() { return Type(); });
            }
        }
        // Если же нет, то просто меняем размер
        size_ = new_size;        
    }    

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return items_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return items_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return items_.Get() + size_;
    }    

    // Оператор присваивания
    SimpleVector& operator=(const SimpleVector& rhs) {
        if (rhs == *this) return *this;
        SimpleVector temp_vector(rhs);
        swap(temp_vector);

        return *this;
    }

    // Оператор перемещения
    SimpleVector& operator=(SimpleVector&& rhs) noexcept {
        if (rhs == *this) return *this;

        items_ = std::move(rhs.items_);
        size_ = std::move(rhs.size_);
        capacity_ = std::move(rhs.capacity_);

        rhs.size_ = 0;

        return *this;
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
        // Если вектор не заполнен полностью, добавляем элемент за последним и увеличиваем размер
        if (size_ < capacity_) {
            items_[size_++] = item;            
        }
        else {
            // Если вектор заполнен полностью создаём новый вектор с удвоенной вместимостью
            SimpleVector new_vector(2 * capacity_);
            new_vector.Resize(size_ + 1);
            // Копируем в него старые элементы
            std::copy(begin(), end(), new_vector.begin());
            new_vector[new_vector.size_ - 1] = item;

            swap(new_vector);
        }
    }

    // Добавляет элемент в конец вектора, перемещая его
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(Type&& item) {
        // Если вектор не заполнен полностью, добавляем элемент за последним и увеличиваем размер
        if (size_ < capacity_) {
            items_[size_++] = std::move(item);
        }
        else {
            // Если вектор заполнен полностью создаём новый вектор с удвоенной вместимостью
            SimpleVector new_vector(capacity_ * 2);
            new_vector.Resize(size_ + 1);
            // Копируем в него старые элементы
            std::copy(std::make_move_iterator(begin()), std::make_move_iterator(end()), new_vector.begin());
            new_vector[new_vector.size_ - 1] = std::move(item);

            swap(new_vector);
        }
    }
    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        Iterator pos_to_insert = const_cast<Iterator>(pos);
        // Проверяем что pos находится в интервале от begin() до end()
        assert( (begin() <= pos_to_insert) && (pos_to_insert <= end()) );

        if (size_ < capacity_) {
            // Случай когда не надо изменять вместимость вектора
            Resize(++size_);
            std::copy_backward(pos_to_insert, end() - 1, end());
            *pos_to_insert = value;
            return pos_to_insert;
        }
        else {
            // Случай когда надо изменять вместимость
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
        // Проверяем что pos находится в интервале от begin() до end()
        assert((begin() <= pos_to_insert) && (pos_to_insert <= end()));

        if (size_ < capacity_) {
            // Случай когда не надо изменять вместимость вектора
            Resize(++size_);
            std::copy_backward(std::make_move_iterator(pos_to_insert), std::make_move_iterator(end() - 1), end());
            *pos_to_insert = std::move(value);
            return pos_to_insert;
        }
        else {
            // Случай когда надо изменять вместимость
            SimpleVector new_vector(2 * capacity_);
            new_vector.Resize(size_ + 1);

            auto pos_in_new_vector = std::copy(std::make_move_iterator(begin()), std::make_move_iterator(pos_to_insert), new_vector.begin());
            *pos_in_new_vector = std::move(value);
            std::copy(std::make_move_iterator(pos_to_insert), std::make_move_iterator(end()), pos_in_new_vector + 1);

            swap(new_vector);

            return pos_in_new_vector;
        }
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        assert(!IsEmpty());
        // Если вектор не пуст уменьшаем его размер на 1
        --size_;        
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {    
        assert(!IsEmpty());
        Iterator pos_to_delete = const_cast<Iterator>(pos);
        // Проверяем что pos находится в интервале от begin() до end()
        assert((begin() <= pos_to_delete) && (pos_to_delete <= end()));

        std::copy(std::make_move_iterator(pos_to_delete + 1), std::make_move_iterator(end()), pos_to_delete);
        --size_;
        return pos_to_delete;
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        items_.swap(other.items_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

private:
    ArrayPtr<Type> items_;

    size_t size_ = 0; // Размер вектора
    size_t capacity_ = 0; // Максимальная вместимость
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
