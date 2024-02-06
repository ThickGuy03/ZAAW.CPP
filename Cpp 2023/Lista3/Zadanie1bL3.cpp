#include <iostream>
#include <algorithm>

namespace cpplab {

    template <typename T>
    class vector {
    private:
        T* data;
        std::size_t size;
        std::size_t capacity;

    public:
        vector() : data(nullptr), size(0), capacity(0) {}

        vector(const vector& other) : size(other.size), capacity(other.size) {
            data = new T[capacity];
            std::copy(other.data, other.data + size, data);
        }

        vector& operator=(const vector& other) {
            if (this != &other) {
                delete[] data;
                size = other.size;
                capacity = other.size;
                data = new T[capacity];
                std::copy(other.data, other.data + size, data);
            }
            return *this;
        }

        vector(vector&& other) noexcept : data(nullptr), size(0), capacity(0) {
            swap(*this, other);
        }

        vector& operator=(vector&& other) noexcept {
            if (this != &other) {
                delete[] data;
                data = nullptr;
                size = 0;
                capacity = 0;
                swap(*this, other);
            }
            return *this;
        }

        ~vector() {
            delete[] data;
        }

        void push_back(const T& value) {
            if (size == capacity) {
                reserve(capacity == 0 ? 1 : capacity * 2);
            }
            data[size++] = value;
        }

        void reserve(std::size_t new_capacity) {
            if (new_capacity > capacity) {
                T* new_data = new T[new_capacity];
                std::copy(data, data + size, new_data);
                delete[] data;
                data = new_data;
                capacity = new_capacity;
            }
        }

        void print() const {
            for (std::size_t i = 0; i < size; ++i) {
                std::cout << data[i] << " ";
            }
            std::cout << std::endl;
        }

        friend void swap(vector& first, vector& second) noexcept {
            using std::swap;
            swap(first.data, second.data);
            swap(first.size, second.size);
            swap(first.capacity, second.capacity);
        }
    };

} 

int main() {
    // Testowanie działania konstruktorów i funkcji push_back
    cpplab::vector<int> v1;
    v1.push_back(1);
    v1.push_back(2);
    v1.push_back(3);

    cpplab::vector<int> v2 = v1;  // konstruktor kopiujący
    v2.print();

    cpplab::vector<int> v3;
    v3 = v1;  // operator przypisania kopiujący
    v3.print();

    cpplab::vector<int> v4 = std::move(v1);  // konstruktor przenoszący
    v4.print();

    cpplab::vector<int> v5;
    v5 = std::move(v2);  // operator przypisania przenoszący
    v5.print();

    return 0;
}