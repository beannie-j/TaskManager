#include "MyFrame.h"
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <sstream>
#include <iostream>
#include <sqlite3/sqlite3.h>


wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT, OnMenuExit)
    EVT_MENU(wxID_ABOUT, OnMenuAbout)
    EVT_MENU(wxID_VIEW_LIST, OnMenuSearch)
    EVT_MENU(wxID_NEW, OnMenuNew)
wxEND_EVENT_TABLE()

bool MyFrame::UserLoginStatus = false;

MyFrame::MyFrame(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size)
    :wxFrame(parent, wxID_ANY, title, pos, size)
{  
    UsernameTextBox = new wxTextCtrl(this, wxID_ANY, "", wxPoint(100, 50), wxSize(100, 25),
        wxTE_RICH, wxDefaultValidator, wxTextCtrlNameStr);
    PasswordEditBox = new wxTextCtrl(this, wxID_ANY, "", wxPoint(100, 100), wxSize(100, 25),
        wxTE_PASSWORD, wxDefaultValidator, wxTextCtrlNameStr);
 
    LoginButton = new wxButton(this, BUTTON_Login, _T("Login"), wxPoint(80, 160), wxDefaultSize, 0);    
    SignUpButton = new wxButton(this, BUTTON_SignUp, _T("Sign Up"), wxPoint(180, 160), wxDefaultSize, 0);
    LogBox = new wxListBox(this, wxID_ANY, wxPoint(78, 200), wxSize(200, 250));
    LogBox->Append("logs..");
    UsernameLabel = new wxStaticText(this, wxID_ANY, "Username: ", wxPoint(40, 52), wxDefaultSize, 0);
    PasswordLabel = new wxStaticText(this, wxID_ANY, "Password: ", wxPoint(40, 102), wxDefaultSize, 0);

    MenuBar = new wxMenuBar();
    FileMenu = new wxMenu();
    FileMenu->Append(wxID_NEW, _T("&New"));
    FileMenu->Append(wxID_VIEW_LIST, _T("&Search"));
    FileMenu->AppendSeparator();
    FileMenu->Append(wxID_EXIT, _T("&Quit"));
    MenuBar->Append(FileMenu, _T("&File"));

    HelpMenu = new wxMenu();
    HelpMenu->Append(wxID_ABOUT, _T("&About"));
    MenuBar->Append(HelpMenu, _T("&Help"));

    SetMenuBar(MenuBar);

    SignUpButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MyFrame::OnSignUpButton, this);
    LoginButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MyFrame::OnLoginButton, this);
    
    manager = std::make_unique<Manager>();

    //CleanDB();

}

MyFrame::~MyFrame()
{
}


std::string MyFrame::gui_Encrypt(std::string str)
{
    std::size_t str_hash = std::hash<std::string>{}(str);
    return std::to_string(str_hash);
}

void MyFrame::OnMenuNew(wxCommandEvent &event)
{
    NewTaskDialog = new wxDialog(this, wxID_NEW, _T("Create New Task"), wxPoint(50, 50), wxSize(400, 500), wxDEFAULT_DIALOG_STYLE);
    wxStaticText* TaskNameLabel = new wxStaticText(NewTaskDialog, wxID_ANY, _T("Enter Task Name:"), wxPoint(100, 34), wxDefaultSize);
    TaskNameTextBox = new wxTextCtrl(NewTaskDialog, wxID_ANY, "", wxPoint(100, 50), wxSize(100, 40),
        wxTE_RICH, wxDefaultValidator, wxTextCtrlNameStr);
    ComboBox = new wxComboBox(NewTaskDialog, COMBOBOX, _T("Asignee"), wxPoint(100, 100), wxDefaultSize, 0);
    Dialog_CreateNewTaskButton = new wxButton(NewTaskDialog, BUTTON_Dialog_Create_Task, _T("Create New Task"), wxPoint(100, 350), wxDefaultSize, 0);
    Dialog_CreateNewTaskButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MyFrame::OnCreateNewTaskButton, this);

    wxStaticText* SelectDateLabel = new wxStaticText(NewTaskDialog, wxID_ANY, _T("Select Task Due Date:"), wxPoint(100, 160), wxDefaultSize);
    Calendar = new wxCalendarCtrl(NewTaskDialog, wxID_ANY, wxDefaultDateTime, wxPoint(100, 170), wxDefaultSize);
    Calendar->EnableHolidayDisplay(true);

    GetUserNamesFromDB();

    for (auto const& x : USERS)
    {
        ComboBox->Append(x);
    }
    NewTaskDialog->Show(true);  

    if (!MyFrame::UserLoginStatus)
    {
        wxMessageBox(wxT("Please login first"), wxT("Login"), wxICON_ERROR);
        NewTaskDialog->Close();
    }
}

void MyFrame::OnMenuAbout(wxCommandEvent& event)
{
    LogBox->Append("About clicked");
}
void MyFrame::OnMenuExit(wxCommandEvent& event)
{
    Close();
    event.Skip();
}

void MyFrame::OnSearchButton(wxCommandEvent& event)
{
    int i = 0;
    std::string SearchTaskName = std::string(SearchTaskBox->GetValue());
    std::string SearchUsername = std::string(SearchUserBox->GetValue());
    sqlite3* db;
    sqlite3_open("test.db", &db);
    if (!SearchTaskName.empty() || !SearchUsername.empty())
    {
        SearchResultBox->Clear();
        char* error_msg = NULL;
        sqlite3_stmt* stmt = NULL;
        const char* taskName, *taskDate, *userName = NULL;
        const char* SQL = "select a.id, t.taskname, t.date, u.username from tasks t inner join allocations a on a.task_id = t.task_id inner join users u on u.user_id = a.user_id where t.taskname = ?";
        sqlite3_prepare_v2(db, SQL, -1, &stmt, 0);
        int rc = sqlite3_bind_text(stmt, 1, SearchTaskName.c_str(), SearchTaskName.length(), SQLITE_STATIC);
        if (rc != SQLITE_OK)
        {
            LogBox->Append(error_msg);
            sqlite3_free(error_msg);
        }
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            taskName = (const char*)sqlite3_column_text(stmt, 1);
            taskDate = (const char*)sqlite3_column_text(stmt, 2);
            userName = (const char*)sqlite3_column_text(stmt, 3);
            SearchResultBox->Append(taskName);  
            SearchResultBox->Append("Due date:");
            SearchResultBox->Append(taskDate);
            SearchResultBox->Append("Asignee: ");
            SearchResultBox->Append(userName);
        }
    }
    sqlite3_close(db);
}

void MyFrame::OnMenuSearch(wxCommandEvent &event)
{
    SearchDialog = new wxDialog(this, wxID_NEW, _T("Search Tasks"), wxPoint(50, 50), wxSize(500, 500), wxDEFAULT_DIALOG_STYLE);
    wxStaticText* SearchByTaskNameLabel = new wxStaticText(SearchDialog, wxID_ANY, _T("Search by Task Name:"), wxPoint(100, 34), wxDefaultSize);
    wxStaticText* SearchByUserNameLabel = new wxStaticText(SearchDialog, wxID_ANY, _T("Search by User Name:"), wxPoint(100, 85), wxDefaultSize);
    SearchTaskBox = new wxComboBox(SearchDialog, wxID_ANY, _T("Search with Name"), wxPoint(100, 50), wxDefaultSize, 0);
    SearchUserBox = new wxComboBox(SearchDialog, COMBOBOX, _T("Search with User"), wxPoint(100, 100), wxDefaultSize, 0);
    wxButton* SearchButton = new wxButton(SearchDialog, BUTTON_Dialog_Search_Task, _T("Search"), wxPoint(100, 125), wxDefaultSize, 0);
    wxStaticText *SearchResult = new wxStaticText(SearchDialog, wxID_ANY, _T("Results"), wxPoint(100, 155), wxDefaultSize, 0);
    SearchResultBox = new wxListBox(SearchDialog, wxID_ANY, wxPoint(100, 170), wxSize(250, 200));
    SearchButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MyFrame::OnSearchButton, this);

    GetUserNamesFromDB();
    GetTaskNamesFromDB();

    for (auto const& x : USERS)
    {
        SearchUserBox->Append(x);
    }
    SearchDialog->Show(true);

    for (auto const& x : TASKS)
    {
        SearchTaskBox->Append(x);
    }
    SearchDialog->Show(true);


    if (!MyFrame::UserLoginStatus)
    {
        wxMessageBox(wxT("Please login first"), wxT("Login"), wxICON_ERROR);
        SearchDialog->Close();
    }
}


void MyFrame::OnCreateNewTaskButton(wxCommandEvent& event)
{
    wxString TaskName = TaskNameTextBox->GetValue();
    std::string AsigneeName = std::string(ComboBox->GetValue());

    if (TaskName.IsEmpty() || std::string(AsigneeName) == "Asignee") wxMessageBox(wxT("Task Name or Asignees cannot be empty"), wxT("Alert"), wxICON_INFORMATION);
    
    LogBox->Append(std::string(TaskName));
    std::string date = std::string(Calendar->GetDate().FormatISODate());
    std::string year = date.substr(0, 4);
    std::string month = date.substr(5, 2);
    std::string day = date.substr(8, 2);
    
    std::shared_ptr<TaskStuff::Task> taskptr = manager->CreateTask(std::string(TaskName));
 
    taskptr->date = { stoi(year), stoi(month), stoi(day) };

    sqlite3* db;
    sqlite3_open("test.db", &db);

    std::string assignee_id = GetUserIDFromDB(AsigneeName, db);

    char* error_msg;
    int task_id = GetMaxTaskID(db) + 1;
    std::string INSERT_TASK = "INSERT INTO tasks VALUES(" + std::to_string(task_id) + ", '" + taskptr->GetTaskName() + "', '" + date + "');";
    int INSERT_TASK_OPERATION = sqlite3_exec(db, INSERT_TASK.c_str(), NULL, 0, &error_msg);
    
    if (INSERT_TASK_OPERATION != SQLITE_OK) {
        LogBox->AppendString("INSERT_TASK_OPERATION ERROR: ");
        LogBox->AppendString(error_msg);
        sqlite3_free(error_msg);
    }
    int alloc_id = GetMaxAllocationsID(db) + 1;
    std::string INSERT_ALLOCATION = "INSERT INTO allocations values('" + std::to_string(alloc_id) +"', '" + std::to_string(task_id) + "','"  + assignee_id + "');";
    int INSERT_ALLOCATION_OPERATION = sqlite3_exec(db, INSERT_ALLOCATION.c_str(), NULL, 0, &error_msg);
    if (INSERT_ALLOCATION_OPERATION != SQLITE_OK) {
        LogBox->AppendString("INSERT_ALLOCATION_OPERATION ERROR:");
        LogBox->AppendString(error_msg);
        sqlite3_free(error_msg);
    }
        
    wxMessageBox(wxT("Task Created Successfully"), wxT("Confirmation"), wxICON_INFORMATION);
    sqlite3_close(db);
        
    int days = manager->CheckDueTasks(taskptr->date);
    LogBox->AppendString(std::to_string(days));

}

std::set<std::string> MyFrame::GetUserNamesFromDB()
{
    sqlite3* db;
    sqlite3_open("test.db", &db);
    char* error_msg = NULL;
    sqlite3_stmt* stmt = NULL;
    const char* data = NULL;
    std::string result = "";
    const char* sql = "SELECT username FROM users;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        data = (const char*)sqlite3_column_text(stmt, 0);
        result = data;
        USERS.insert(result);
    }
    sqlite3_close(db);
    return USERS;
}

std::set<std::string> MyFrame::GetTaskNamesFromDB()
{
    sqlite3* db;
    sqlite3_open("test.db", &db);
    char* error_msg = NULL;
    sqlite3_stmt* stmt = NULL;
    const char* data = NULL;
    std::string result = "";
    const char* sql = "SELECT taskname FROM tasks;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        data = (const char*)sqlite3_column_text(stmt, 0);
        result = data;
        TASKS.insert(result);
    }
    sqlite3_close(db);
    return TASKS;
}

std::string MyFrame::GetUserIDFromDB(std::string username, sqlite3* db)
{
    char* error_msg = NULL;
    sqlite3_stmt* stmt = NULL;
    const char* data = NULL;
    std::string result = "";
    const char* sql = "SELECT user_id FROM users where username = ?";
    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    int result_code = sqlite3_bind_text(stmt, 1, username.c_str(), username.length(), SQLITE_STATIC);
    if (result_code != SQLITE_OK)
    {
        sqlite3_free(error_msg);
    }
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        data = (const char*)sqlite3_column_text(stmt, 0);
        result = data;
    }
    return result;
}


std::string MyFrame::gui_GetPassword(std::string username, sqlite3 *db)
{
    char* error_msg = NULL;
    sqlite3_stmt* stmt = NULL;
    const char *data = NULL;
    std::string result = "";
    const char *sql = "SELECT password FROM users where username = ?";
    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    int result_code = sqlite3_bind_text(stmt, 1, username.c_str(), username.length(), SQLITE_STATIC);
    if (result_code != SQLITE_OK) 
    {
        sqlite3_free(error_msg);
    }
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        data = (const char*)sqlite3_column_text(stmt, 0);
        result = data;
    }
    return result;
}

void MyFrame::CreateTaskTable()
{
    sqlite3* db;
    sqlite3_open("test.db", &db);
    std::string create_sql = "CREATE TABLE IF NOT EXISTS tasks (task_id integer PRIMARY KEY, taskname text NOT NULL UNIQUE, date text NOT NULL);";
    char* error_msg;
    int create = sqlite3_exec(db, create_sql.c_str(), NULL, 0, &error_msg);

    if (create != SQLITE_OK) {
        LogBox->Append("Error" + std::string(error_msg));
        sqlite3_free(error_msg);
    }
}

void MyFrame::CreateAllocationsTable()
{
    sqlite3* db;
    sqlite3_open("test.db", &db);
    std::string create_sql = "CREATE TABLE IF NOT EXISTS allocations (id integer PRIMARY KEY, task_id integer NOT NULL, user_id integer NOT NULL, FOREIGN KEY(task_id) references tasks(task_id), FOREIGN KEY(user_id) references users(user_id));";
    char* error_msg;
    int create = sqlite3_exec(db, create_sql.c_str(), NULL, 0, &error_msg);

    if (create != SQLITE_OK) {
        LogBox->Append("Error" + std::string(error_msg));
        sqlite3_free(error_msg);
    }
}

void MyFrame::CleanDB()
{
    sqlite3* db;
    sqlite3_open("test.db", &db);
    std::string create_sql = "delete from tasks where taskname like '%do%dishes%'";
    char* error_msg;
    int create = sqlite3_exec(db, create_sql.c_str(), NULL, 0, &error_msg);

    if (create != SQLITE_OK) {
        LogBox->Append("Error" + std::string(error_msg));
        sqlite3_free(error_msg);
    }
}

void MyFrame::OnLoginButton(wxCommandEvent& event)
{
    wxString username = UsernameTextBox->GetValue();
    wxString password = PasswordEditBox->GetValue();
    sqlite3* db;
    sqlite3_open("test.db", &db);
    std::string db_password = gui_GetPassword(std::string(username), db);
    std::string encrypted_password = gui_Encrypt(std::string(password));

    if (db_password == encrypted_password)
    {
        MyFrame::UserLoginStatus = true;
        User* user = manager->CreateUser(std::string(username));
        wxMessageBox(wxT("Login Successful"), wxT("Hello!"), wxICON_INFORMATION);
        wxStaticText* HelloUser = new wxStaticText(this, wxID_ANY, "Welcome back - username " + username, wxPoint(245, 10));
    }
    else
    {
        m_LoginAttemptsRemaining--;

        MyFrame::UserLoginStatus = false;
        wxString msg = "Incorrect username/password, please try again, you have " + std::to_string(m_LoginAttemptsRemaining) + " tries left";
        wxMessageBox(msg, wxT("Error"), wxICON_ERROR);

        if (m_LoginAttemptsRemaining == 0) {
            wxMessageBox(wxT("You have exceeded 3 attempts, exiting program"), wxT("Bye"), wxICON_ERROR);
            Close();
            sqlite3_close(db);
            exit(0);
        }
    }
    event.Skip();
    sqlite3_close(db);
}

void MyFrame::OnSignUpButton(wxCommandEvent &event)
{
    wxString username = UsernameTextBox->GetValue();
    wxString password = PasswordEditBox->GetValue();
    InsertToUsersTable(std::string(username), std::string(password));
    User* user = manager->CreateUser(std::string(username));
    event.Skip();
}

int MyFrame::GetMaxAllocationsID(sqlite3* db)
{
    char* error_msg = NULL;
    sqlite3_stmt* stmt = NULL;
    const char* data = NULL;
    int max = 0;
    const char* sql = "Select max(id) from allocations;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        max = sqlite3_column_int(stmt, 0);
    }
    return max;
}

int MyFrame::GetMaxTaskID(sqlite3* db)
{
    char* error_msg = NULL;
    sqlite3_stmt* stmt = NULL;
    const char* data = NULL;
    int max = 0;
    const char* sql = "Select max(task_id) from tasks;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        max = sqlite3_column_int(stmt, 0);
    }
    return max;
}

int MyFrame::GetMaxUserID(sqlite3 *db)
{
    char* error_msg = NULL;
    sqlite3_stmt* stmt = NULL;
    const char* data = NULL;
    int max = 0;
    const char* sql = "Select max(user_id) from users;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        max = sqlite3_column_int(stmt, 0);
    }
    return max;
}

void MyFrame::InsertToUsersTable(std::string username, std::string password)
{
    sqlite3* db;
    sqlite3_open("test.db", &db);
    char* error_msg;
    int max_id = GetMaxUserID(db);
    std::string encrypted_password = gui_Encrypt(password);
    std::string insert_sql = "INSERT INTO users VALUES(" + std::to_string(max_id + 1) + ", '" + username + "', '" + encrypted_password + "');";
    int insert_operation = sqlite3_exec(db, insert_sql.c_str(), NULL, 0, &error_msg);

    if (insert_operation != SQLITE_OK) {
        LogBox->AppendString(error_msg);
        sqlite3_free(error_msg);
    }
    else LogBox->AppendString("Records created Successfully!");
    sqlite3_close(db);
}