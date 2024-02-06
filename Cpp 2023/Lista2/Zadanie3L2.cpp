#include <iostream>
#include <string>

// This helper function returns the sum of its arguments
template<typename... Args>
constexpr auto sum(Args... args)
{
    return (... + args);
}

// This function returns the total number of arguments it received
template<typename... Args>
constexpr std::size_t total_args(Args... args)
{
    return sizeof...(args);
}

// This is the main function that uses the helper functions
template<typename... Args>
constexpr auto add_total(Args... args)
{
    // If the sum of the arguments is zero, we return 0.
    // Otherwise, we return the sum plus the total number of arguments.
    if constexpr (sum(args...) == 0)
    {
        return 0;
    }
    else
    {
        return sum(args...) + total_args(args...);
    }
}

int main()
{
    // Example usage
    std::cout << add_total(1, 1.0, 1.0f) << std::endl; // Outputs: 6
    std::cout << add_total(0, 0, 0) << std::endl;       // Outputs: 0
    std::cout << add_total(-1, -1.0, -1.0f) << std::endl;// Outputs: -6
    return 0;
}