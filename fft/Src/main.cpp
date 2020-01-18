#include<iostream>
#include<vector>
#include<variant>
#include<numeric>

#include "AppState.h"

int main(int argc, char* args[])
{
	auto application = APP_STATE::AppState::instance();
	application->init(1024, 768); // window size

	//set to run mode
	application->run();

	//main program loop
	application->working_loop();

	application->shut();

	return 0;
}

