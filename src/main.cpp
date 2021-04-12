

#include "unit_test.h"
#include "aoi/Scene.h"
#include "aoi/screen_index.h"
 
using namespace std;
using namespace aoi;


int main(int argc, char* argv[])
{
	UnitTestMgr::Ins().Start();
}

UNITTEST(tt)
{
	ScreenIndexBase::Ins().initScreenIndex();
	ScreenIndexBase::Ins().printAllDirectScreen(450);
	UNIT_INFO("ddddddddddddddddddd");
	ScreenIndexBase::Ins().printAllReverseDirectScreen(450);
}