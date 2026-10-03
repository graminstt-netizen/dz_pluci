#include <cstdlib> // malloc, free, realloc
#include <cstddef>  // size_t
#include <iostream>
#include <new> // std::bad_alloc

class DynamicArray {
private:
    int* data_; // блок памяти в куче
    std::size_t size_; // сколько элементов реально лежит
    std::size_t capacity_;  // сколько элементов влезает без перевыделения

public:
    // Конструктор по умолчанию пустой массив
    DynamicArray() noexcept: data_(nullptr), size_(0), capacity_(0) {} // noecept - функция не будет бросать исключения (для оптимизации крч)

    // Конструктор с преаллокацией
    explicit DynamicArray(std::size_t initial_capacity) // explicit - запрет неявных преобразований (длля безопасности)
    : data_(nullptr), size_(0), capacity_(initial_capacity)
    {
        if (initial_capacity > 0) {
            data_ = static_cast<int*>(std::malloc(initial_capacity * sizeof(int)));
            if (!data_) {
                capacity_ = 0;
                throw std::bad_alloc();
            }
        }
    }

    // Запрет копирования
    DynamicArray(const DynamicArray&) = delete;
    DynamicArray& operator=(const DynamicArray&) = delete;

    //  Перемещение 
    DynamicArray(DynamicArray&& other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_)
    {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this != &other) {
            std::free(data_);
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    // Деструктор освобождение памяти 
    ~DynamicArray() {
        std::free(data_);
    }

    // operator доступ по индексу
    int& operator[](std::size_t index) {
        return data_[index];
    }

    //для const-объектов
    const int& operator[](std::size_t index) const {
        return data_[index];
    }

    void Add(int value) {
        if (size_ >= capacity_) {
            std::size_t new_capacity = capacity_ + 10; // перевыделение на +10
            int* new_data = static_cast<int*>(
                std::realloc(data_, new_capacity * sizeof(int))
            );
            if (!new_data) {
                // не удалось -старый блок цел, ничего не меняем 
                return;
            }
            data_ = new_data;
            capacity_ = new_capacity;
        }
        data_[size_++] = value;
    }

    std::size_t Size() const noexcept { return size_; }
    std::size_t Capacity() const noexcept { return capacity_; } //методы чтоб просто доставать сайз и капасити
};

int main() {
    DynamicArray arr;   // пустой
    std::cout << "size=" << arr.Size()
              << " cap=" << arr.Capacity() << "\n";
    // заполняем массив
    for (int i = 0; i < 25; ++i) {
        arr.Add(i * i);
    }

    std::cout << "size=" << arr.Size()
              << " cap=" << arr.Capacity() << "\n";

    for (std::size_t i = 0; i < arr.Size(); ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << "\n";

    // Массив с преаллокацией
    DynamicArray arr2(5);
    std::cout << "arr2 cap=" << arr2.Capacity() << "\n";
    arr2.Add(42);
    std::cout << "arr2[0]=" << arr2[0] << "\n";

    //     Проверка перемещения, владение переходит к arr3, arr2 обнуляется
    DynamicArray arr3 = std::move(arr2);
    std::cout << "arr3.size=" << arr3.Size()
              << " arr2.size=" << arr2.Size() << "\n";

    return 0;
}