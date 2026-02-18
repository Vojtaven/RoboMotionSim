#include "AppEngine.hpp"
#include <iostream>


int main() {
	AppEngine app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}