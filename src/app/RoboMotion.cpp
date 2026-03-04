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

/*NEXT TODO
Fix Initialization of robot shape
Proper Input error handling and messages
Robot stats

*/


/* TODO list
* Robot designer
* Robot settigns
* App settings 
* Program input - Piping
* Serial input
*/


/* Some not Inportant TODOs
One imgui context
Grid rerendering optimization

*/





