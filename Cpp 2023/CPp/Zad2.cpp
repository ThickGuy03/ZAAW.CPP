#include <iostream>
#include <vector>

namespace cpplab {
    template <typename T>
    class vector {
    public:
        using value_type = T;

        vector() : data(nullptr), length(0), capacity(0) {}
        vector(size_t initial_length) : length(initial_length), capacity(initial_length) {
            data = new T[length];
        }
        ~vector() {delete[] data;}
        size_t get_length() const {return length;}
        size_t get_capacity() const {return capacity;}

        T& operator[](size_t index) {
            if (index < length) {
                return data[index];
            }
            else {
                throw std::out_of_range("Index out of range");
            }
        }

        void resize(size_t new_length) {
            if (new_length == length) {
                return;
            }
            else if (new_length < length) {
                length = new_length;
                if (new_length < capacity / 2) {
                    capacity = new_length;
                }
            }
            else {
                length = new_length;
                if (new_length > capacity) {
                    capacity = new_length;
                }
            }
            T* new_data = new T[capacity];
            for (size_t i = 0; i < length; ++i) {
                new_data[i] = data[i];
            }
            delete[] data;
            data = new_data;
        }

        T operator*(const vector<T>& other) const {
            if (length != other.length) {
                throw std::invalid_argument("Vector lengths must match for dot product.");
            }
            T result = 0;
            for (size_t i = 0; i < length; ++i) {
                result += data[i] * other.data[i];
            }
            return result;
        }

    private:
        T* data;
        size_t length;
        size_t capacity;
    };
}

int main() {
    cpplab::vector<int> v1(3);
    v1[0] = 1;
    v1[1] = 2;
    v1[2] = 3;

    cpplab::vector<int> v2(3);
    v2[0] = 4;
    v2[1] = 5;
    v2[2] = 6;

    std::cout << "Dot product: " << v1 * v2 << std::endl;

    return 0;
}