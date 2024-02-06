#include <iostream>
#include <algorithm>
#include <utility>

namespace cpplab {

    template <typename T>
    class vector {
    private:
        T* data;
        std::size_t size;
        std::size_t capacity;

    public:
        // Konstruktor domyślny
        vector() : data(nullptr), size(0), capacity(0) {}

        // Konstruktor kopiujący
        vector(const vector& other) : size(other.size), capacity(other.size) {
            data = new T[capacity];
            std::copy(other.data, other.data + size, data);
        }

        // Operator przypisania kopiujący
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

        // Konstruktor przenoszący
        vector(vector&& other) noexcept : data(nullptr), size(0), capacity(0) {
            swap(*this, other);
        }

        // Operator przypisania przenoszący
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

        // Destruktor
        ~vector() {
            delete[] data;
        }

        // Swap dla konstruktorów przenoszących i operatorów przypisania przenoszących
        friend void swap(vector& first, vector& second) noexcept {
            using std::swap;
            swap(first.data, second.data);
            swap(first.size, second.size);
            swap(first.capacity, second.capacity);
        }

        // Funkcja dodająca element
        void push_back(const T& value) {
            if (size == capacity) {
                reserve(capacity == 0 ? 1 : capacity * 2);
            }
            data[size++] = value;
        }

        // Funkcja rezerwująca pamięć
        void reserve(std::size_t new_capacity) {
            if (new_capacity > capacity) {
                T* new_data = new T[new_capacity];  
                for (std::size_t i = 0; i < size; ++i) {
                    new_data[i] = data[i]; 
                }
                delete[] data;
                data = new_data;
                capacity = new_capacity;
            }
        }

        // Funkcja wypisująca zawartość vectora
        void print() const {
            for (std::size_t i = 0; i < size; ++i) {
                std::cout << data[i] << " ";
            }
            std::cout << std::endl;
        }

        // Funkcja dodająca element przy użyciu perfect forwarding i parameter pack
        template <typename... Args>
        T& emplace_back(Args&&... args) {
            if (size >= capacity) {
                reserve(capacity == 0 ? 1 : capacity * 2);
            }
            new (&data[size]) T(std::forward<Args>(args)...);
            return data[size++];
        }
    };

}  

class Pixel {
public:
    int r, g, b;

    Pixel() : r(0), g(0), b(0) {} 

    Pixel(int red, int green, int blue) : r(red), g(green), b(blue) {}
};


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

    // Testowanie działania emplace_back() na wektorze pixeli
    cpplab::vector<Pixel> v6;
    v6.emplace_back(3, 4, 6);



    return 0;
}
