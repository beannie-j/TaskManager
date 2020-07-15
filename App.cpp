#include "App.h"
#include "MyFrame.h"

IMPLEMENT_APP(App); // Main


App::App()
{
	
}

App::~App()
{

}


bool App::OnInit()
{
	if (!wxApp::OnInit())
		return false;

	myFrame = new MyFrame(nullptr, "Task Manager", wxPoint(50, 50), wxSize(450, 600));
	myFrame->Show(true);

	return true;
}

DECLARE_APP(App); // wxGetApp();

