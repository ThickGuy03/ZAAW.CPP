#include <iostream>
#include <string>

template<typename T1, typename T2>
auto add(T1 x,T2 y) -> decltype( x + y )
{
    return x + number;
}

int main()
{
    int a = 1;
    std::string name = "Lola";
    int number = std::stoi(name);
    std::cout << add(a, number) << std::endl;
}

