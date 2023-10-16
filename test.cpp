#include <string>
#include <iostream>

int main(void) {
	std::string str = "HI THIS IS BRAIN";
	std::cout << "before: " << str << std::endl;
	str.clear();
	std::cout << "after: " << str << std::endl;
	return 0;
}
