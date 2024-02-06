#include <iostream>
#include <type_traits>

template<typename T>
T Add(const T* a,const T* b)
{
	return *a * *b;
}

template<typename T,typename std::enable_if <std::is_same<T,const char*>::value,const char*>::type>
T Add(const char* a, const char* b) {
	return std::string(a) + std::string(b);
}


int main()
{
	int a = 12, b = 5;
	double c = 12.5, d = 5.3;
	const char* e = "ala";
	const char* f = "ela";
	std::cout << Add(&a, &b) << std::endl;
	std::cout << Add(&c, &d) << std::endl;
	std::cout << Add(e, f) << std::endl;

	return 0;
}