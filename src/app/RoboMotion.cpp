#include "AppEngine.hpp"
#include <iostream>
#include <nfd.hpp>

int main() {
	// Native File Dialog initialization
	NFD_Init();

	AppEngine app;
	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	// Native File Dialog cleanup
	NFD_Quit();
}


/* Some not Inportant TODOs
One imgui context
Grid rerendering optimization
Solve the problem of lag when connecting controller 
*/


/*Interesting problems encountered
Grid Aligment
Lag when connecting controller 

*/




