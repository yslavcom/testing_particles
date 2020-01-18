#include<iostream>
#include<vector>
#include<variant>
#include<numeric>

#include "fft.h"
#include "audio.h"
#include "screen.h"
#include "BasicShapesDraw.h"
#include "BasicShapes.h"
#include "Widget.h"
#include "DebugLog.h"
#include "Effect.h"
#include "Grid.h"
#include "Axis.h"
#include "Line.h"
#include "Events.h"
#include "AppState.h"

using namespace TEST_SCREEN;
using namespace BASIC_SHAPES_2D;


int main(int argc, char* args[])
{
	auto application = APP_STATE::AppState::instance();
	application->init(1024, 768);

	//set to run mode
	application->run();

	//main program loop
	application->working_loop();

	application->shut();

	return 0;
}

