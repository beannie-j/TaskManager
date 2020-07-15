#pragma once
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/log.h>
#include <wx/statline.h>
#include <wx/event.h>
#include "MyFrame.h"
#include <sqlite3/sqlite3.h>
#include "Manager.h"


class App : public wxApp
{
public:
	App();
	bool OnInit();

private:
	MyFrame *myFrame;

	~App();

};
