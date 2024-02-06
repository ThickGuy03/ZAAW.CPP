#include <iostream>
#include <type_traits>

template <std::size_t Dim>
class HyperCube {
    double side;

public:
    constexpr HyperCube(double side) : side(side) {}

    template <std::size_t D = Dim>
    constexpr std::enable_if_t<D == 1, double> get_volume() const {
        return side;
    }

    template <std::size_t D = Dim>
    constexpr std::enable_if_t<D != 1, double> get_volume() const {
        return side * HyperCube<D - 1>(side).get_volume();
    }
};

int main() {
    constexpr HyperCube<3> hyperCube(5.0);
    static_assert(hyperCube.get_volume() == 125, "dupa");
    std::cout << "Objetosc hiperkostki o boku 5.0 w 3 wymiarach wynosi: " << hyperCube.get_volume() << std::endl;

    constexpr HyperCube<4> hyperCube4D(3.0);
    static_assert(hyperCube4D.get_volume() == 81,"zzz");
    std::cout << "Objetosc hiperkostki o boku 3.0 w 4 wymiarach wynosi: " << hyperCube4D.get_volume() << std::endl;
   
}