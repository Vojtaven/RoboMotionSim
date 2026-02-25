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
FIX text
Fix Initialization of robot shape


*/


/* TODO list
* Add proper input handling 
* Controller
* Robot designer
* Robot settigns
* App settings 
* Program input
* Serial intpu* 
*/





