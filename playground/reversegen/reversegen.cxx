#include <iostream>

int main(int argc, char* argv[]) {

	for(size_t i = 0; i < 4096; i++) {
		std::cout << i << "," << i << "," << i << std::endl;
	}
}