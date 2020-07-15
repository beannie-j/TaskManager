#pragma once
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <sqlite3/sqlite3.h>
#include <string>
#include <wx/calctrl.h>
#include "User.h"
#include "Manager.h"
#include "Task.h"
#include <iostream>
#include <set>

using namespace UserStuff;
using namespace TaskStuff;


class MyFrame: public wxFrame
{
public:
	MyFrame(wxWindow *parent, const wxString &title, const wxPoint &pos, const wxSize &size);
	void OnLoginButton(wxCommandEvent &event);
	void OnSignUpButton(wxCommandEvent &event);

	std::string gui_Encrypt(std::string str);
	std::string gui_GetPassword(std::string username, sqlite3* db);
	int GetMaxUserID(sqlite3* db);
	void InsertToUsersTable(std::string username, std::string password);
	void OnCreateNewTaskButton(wxCommandEvent &event);
	void OnMenuExit(wxCommandEvent &event);
	void OnMenuAbout(wxCommandEvent &event);
	void OnMenuNew(wxCommandEvent &event);
	void OnMenuSearch(wxCommandEvent &event);
	void OnSearchButton(wxCommandEvent& event);
	void CreateTaskTable();
	void CreateAllocationsTable();
	std::string GetUserIDFromDB(std::string username, sqlite3* db);
	int GetMaxTaskID(sqlite3* db);
	int GetMaxAllocationsID(sqlite3* db);
	void CleanDB();
	std::set<std::string> GetUserNamesFromDB();
	std::set<std::string> GetTaskNamesFromDB();





private:
	wxDialog *NewTaskDialog;
	wxDialog *SearchDialog;
	wxTextCtrl *UsernameTextBox, * PasswordEditBox;
	wxTextCtrl* TaskNameTextBox, * TaskContentEditBox;
	wxListBox *LogBox;
	wxButton *LoginButton;
	wxButton *SignUpButton;
	wxButton* Dialog_CreateNewTaskButton;
	wxButton *CreateNewTaskButton;
	wxCalendarCtrl *Calendar;
	wxStaticText *UsernameLabel;
	wxStaticText *PasswordLabel;
	wxMenuBar *MenuBar;
	wxMenu *FileMenu;
	wxMenu *HelpMenu;
	wxComboBox* ComboBox;
	wxComboBox* SearchTaskBox;
	wxComboBox* SearchUserBox;
	wxListBox* SearchResultBox;

	std::set<std::string> USERS;
	std::set<std::string> TASKS;

	int nFieldWidth = 10;
	int nFieldHeight = 10;
	wxButton **button;
	uint32_t m_LoginAttemptsRemaining = 3;

	std::shared_ptr<Manager> manager;
	static bool UserLoginStatus;


	DECLARE_EVENT_TABLE()
	~MyFrame();
};

enum
{
	BUTTON_Login = wxID_HIGHEST + 1, // declares an id which will be used to call our button
	BUTTON_SignUp = wxID_HIGHEST + 2,
	BUTTON_CreateNewTask = wxID_HIGHEST + 3,
	MENU_BAR_CreateNewTask = wxID_HIGHEST + 4,
	MENU_BAR_Exit = wxID_HIGHEST + 5,
	BUTTON_Dialog_Create_Task = wxID_HIGHEST + 6,
	COMBOBOX = wxID_HIGHEST + 7,
	BUTTON_Dialog_Search_Task = wxID_HIGHEST + 8
};



