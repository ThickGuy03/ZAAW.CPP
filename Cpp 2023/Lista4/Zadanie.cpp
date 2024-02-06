#include <iostream>
#include <vector>
#include <stdexcept>
#include <concepts>

namespace cpplab {
    template <typename T>
    concept Vector = requires(T v) {
        { v.size() } -> std::same_as<size_t>;
        { v[std::declval<size_t>()] } -> std::same_as<typename T::value_type&>;
    };

    template <typename T>
    class vector
    {
    public:
        using value_type = T;

        vector() : data(nullptr), length(0), capacity(0) {}
        vector(size_t initial_length) : length(initial_length), capacity(initial_length) {
            data = new T[length];
        }
        ~vector() { delete[] data; }
        size_t size() const { return length; }

        T& operator[](size_t index) {
            if (index < length) {
                return data[index];
            }
            else {
                throw std::out_of_range("Index out of range");
            }
        }

        const T& operator[](size_t index) const {
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
    private:
        T* data;
        size_t length;
        size_t capacity;
    };

    template <typename T, typename U>
        requires std::same_as<typename T::value_type, typename U::value_type>&&Vector<T>&& Vector<U>
        auto operator*(const T& vec1, const U& vec2) {
        if (vec1.size() != vec2.size()) {
            throw std::invalid_argument("Vector lengths must match for dot product.");
        }
        decltype(vec1[0] * vec2[0]) result = 0;
        for (size_t i = 0; i < vec1.size(); ++i) {
            result += vec1[i] * vec2[i];
        }
        return result;
    }
}


int main() {
    std::vector<int> int_vec1(3);
    int_vec1[0] = 1;
    int_vec1[1] = 3;
    int_vec1[2] = 9;

    cpplab::vector<int> int_vec2(3);
    int_vec2[0] = 2;
    int_vec2[1] = 0;
    int_vec2[2] = 12;

    std::cout << "Dot product (integers): " << int_vec1 * int_vec2 << std::endl;

    cpplab::vector<double> cpplab_vec1(3);
    cpplab_vec1[0] = 2.2;
    cpplab_vec1[1] = 1.5;
    cpplab_vec1[2] = 4.7;

    std::vector<double> cpplab_vec2(3);
    cpplab_vec2[0] = 1.1;
    cpplab_vec2[1] = 3.3;
    cpplab_vec2[2] = 5.5;

    std::cout << "Dot product (doubles): " << cpplab_vec1 * cpplab_vec2 << std::endl;

    return 0;
}
