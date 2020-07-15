#include "User.h"
#include "Task.h"
#include "Manager.h"
#include <iostream>
#include <vector>
#include <sqlite3/sqlite3.h>
#include <functional>
#include <bitset>
#include <iomanip>
#include <ctime>
#include <string>
#pragma warning(disable : 4996)

using namespace UserStuff;
using namespace TaskStuff;

static std::string s_password = "null";
static uint32_t max_id = 0;

class Log {
public:
    void Print(const char* message) {
        std::cout << message << std::endl;
    }
};


std::string Encrypt(std::string str)
{
    // how does this code work? std::hash<std::string>{}(str);
    // from tutorial https://en.cppreference.com/w/cpp/utility/hash
    std::size_t str_hash = std::hash<std::string>{}(str);
    std::cout << "hash(" << std::quoted(str) << ") = " << str_hash << '\n';
    return std::to_string(str_hash);
}

int open_db(const char* filename, sqlite3* db) {
    int conn = sqlite3_open(filename, &db);
    if (conn) {
        std::cerr << "Error opening DB" << sqlite3_errmsg(db) << std::endl;
        return -1;
    }
    else
        std::cout << "Opened database successfully!" << std::endl;
    return 0;
}


struct Data
{
    int* Array; 

    Data()
    {
        Array = new int[1024];
    }

    Data(const Data& d)
    {
        Array = new int[1024];
        memcpy(Array, d.Array, 1024);
    }
};

int Select_callback(void* unused, int count, char** data, char** columns)
{
    std::cout << "There are " << count << " columns" << std::endl;
    for (int i = 0; i < count; i++) {
        std::cout << "The data in column " << columns[i] << " is : " << data[i] << std::endl;
    }
    return 0;
}

int MaxIDCallback(void* unused, int count, char** data, char** columns)
{
    for (int i = 0; i < count; i++) {
        max_id = std::stoi(data[i]);
    }
    return 0;
}

int GetMaxID(sqlite3* db)
{
    char* error_msg;
    std::string data = "CALLBACK FUNCTION";
    std::string sql = "Select max(user_id) from users;";
    int result_code = sqlite3_exec(db, sql.c_str(), MaxIDCallback, (void*)data.c_str(), &error_msg);

    if (result_code != SQLITE_OK) {
        std::cerr << "Error SELECT max id, result code - " << result_code << " : error msg " << &error_msg << std::endl;
        sqlite3_free(error_msg);
    }
    else {
        std::cout << "Operation OK" << std::endl;
    }
    return result_code;
}

int PrintTable(sqlite3* db)
{
    char* error_msg;
    std::string data = "CALLBACK FUNCTION";
    std::string sql = "SELECT * FROM users;";
    int result_code = sqlite3_exec(db, sql.c_str(), Select_callback, (void*)data.c_str(), &error_msg);
    if (result_code != SQLITE_OK) {
        std::cerr << "Error print table:" << result_code << " : " << &error_msg << std::endl;
        sqlite3_free(error_msg);
    }
    else {
        std::cout << "Operation OK" << std::endl;
    }
    return result_code;
}

int PasswordCallback(void* userdata, int count, char** data, char** columns)
{

    // Data d; // Array // 7650
    // d.Array[0];

    // Data a = d;  // Array // 7650
    // a.Array[0] = 5;

    // uint64_t d = *(uint64_t*)data;
    // // std::cout << d << std::endl;

    std::string& result = *(std::string*)userdata;
    if (!data[0])
        std::cout << "Failed to retrieve password!\n";

    result = data[0];
    printf("\n");
    return 0;
}

std::string GetPassword(std::string username, sqlite3* db)
{
    char* error_msg;
    std::string sql = "SELECT password FROM users where username = '" + username + "';";

    std::string result;
    int result_code = sqlite3_exec(db, sql.c_str(), PasswordCallback, &result, &error_msg);
    if (result_code != SQLITE_OK) {
        std::cerr << "Error get password :" << result_code << " : " << &error_msg << std::endl;
        sqlite3_free(error_msg);
    }
    else {
        std::cout << "Operation OK" << std::endl;
    }
    std::cout << "result " << result << std::endl;
    return result;
}

void close_db(sqlite3* db)
{
    sqlite3_close(db);
}

int create_user_table(sqlite3* db)
{
    std::string create_sql = "CREATE TABLE IF NOT EXISTS users (user_id integer PRIMARY KEY, username text NOT NULL UNIQUE, password text NOT NULL);";
    char* error_msg;
    int create = sqlite3_exec(db, create_sql.c_str(), NULL, 0, &error_msg);

    if (create != SQLITE_OK) {
        std::cerr << "Error Create Table " << error_msg << std::endl;
        sqlite3_free(error_msg);
    }
    else
        std::cout << "Table created Successfully" << std::endl;
    return create;
}

int insert_user_table(sqlite3* db, std::string username, std::string password)
{
    char* error_msg;
    GetMaxID(db);
    password = Encrypt(password);
    std::string insert_sql = "INSERT INTO users VALUES(" + std::to_string(max_id + 1) + ", '" + username + "', '" + password + "');";
    std::cout << insert_sql << std::endl;

    int insert_operation = sqlite3_exec(db, insert_sql.c_str(), NULL, 0, &error_msg);
    if (insert_operation != SQLITE_OK) {
        std::cerr << "Error Insert" << error_msg << std::endl;
        sqlite3_free(error_msg);
    }
    else
        std::cout << "Records created Successfully!" << std::endl;
    return insert_operation;
}


void AskSignUp(Log log, sqlite3* db)
{
    log.Print("Sign up: Please enter your username");
    std::string username = "";
    std::string password = "";
    std::cin >> username;
    std::cin.get();
    log.Print("Sign up: Please enter your password");
    std::cin >> password;
    std::cin.get();
    insert_user_table(db, username, password);
}

bool AskLogin(Log log, sqlite3* db)
{
    log.Print("Login: Please enter your username");
    std::string username = "";
    std::string password = "";
    std::cin >> username;
    std::cin.get();
    if (username == "2") { AskSignUp(log, db); }
    else if (username == "3") { log.Print("Exit"); return -1; }
    log.Print("Login: Please enter your password");
    std::cin >> password;
    std::cin.get();
    std::string passwordString = GetPassword(username, db);
    if (Encrypt(password) == passwordString) { std::cout << "true - " << Encrypt(password) << " , " << passwordString << std::endl; return true; }
    else { std::cout << "false" << std::endl; return false; }
}

tm GetLocalTime() {
    time_t curr_time = time(NULL);
    tm* tm_local = localtime(&curr_time);
    // convert now to string form
    char* dt = ctime(&curr_time);
    std::cout << dt << std::endl;
    std::cout << "Current local time : "
        //  << tm_local->tm_zone <<" - " 
        << 1900 + tm_local->tm_year << "/"
        << tm_local->tm_mon + 1 << "/"
        << tm_local->tm_mday << " "
        << tm_local->tm_hour << ":"
        << tm_local->tm_min << ":"
        << tm_local->tm_sec << std::endl;

    return *tm_local;
}

/*
int main() {

    GetLocalTime();

    sqlite3* DB;
    const char* filename = "test.db";

    int exit = sqlite3_open(filename, &DB);
    if (exit) {
        std::cerr << "Error open DB " << sqlite3_errmsg(DB) << std::endl;
        return -1;
    }
    else std::cout << "Opened Database Successfully!" << std::endl;


    Log log;

    log.Print("Welcome to Chores Calendar");
    log.Print("---------------------------");
    log.Print("Options");
    log.Print("1. Login");
    log.Print("2. Sign up");
    log.Print("3. Exit");

    PrintTable(DB);

    int choice = 0;
    std::cin >> choice;
    std::cin.get();

    bool login_choice = choice == 1;
    bool sign_up_choice = choice == 2;
    bool exit_choice = choice == 3;

    
    create_user_table(DB);
    int itr = 3;
    if (login_choice) {

        while (true) {
            if (AskLogin(log, DB))
                break;

            log.Print("Wrong password, please try again. Forgot your password?");
            --itr;
            std::cout << "You have " << itr << " attempts left." << std::endl;
            log.Print("2. Sign up");
            log.Print("3. Exit");
            if (itr <= 0) {
                log.Print("You have exceeded 3 attempts, exiting program.");
                break;
            }
        }
    }

    else if (sign_up_choice) {
        AskSignUp(log, DB);
    }

    else if (exit_choice)
        return -1;
  

    auto manager = std::make_unique<Manager>();

    User* user = manager->CreateUser("Jeannie");
    User* flatmate = manager->CreateUser("Flatmate");
    std::shared_ptr<TaskStuff::Task> task_ptr = manager->CreateTask("Do the dishes");
    std::shared_ptr<TaskStuff::Task> task_ptr2 = manager->CreateTask("Clean bathroom");

    task_ptr2->SetCompletionStatus(true);

    for (auto const& x : manager->GetTaskMap())
    {
        std::cout << x.first  // string (key)
            << ':'
            << x.second->GetTaskName() // string's value
            << std::endl;
    }

    manager->AddAllocations(task_ptr, user);
    manager->AddAllocations(task_ptr2, user);
    manager->AddAllocations(task_ptr2, flatmate);
    manager->QueryByUser(user);
    manager->QueryByUser(flatmate);
    manager->QueryPendingTasks(true);
    manager->QueryPendingTasks(false);

    // this needs to be corrected....
    manager->AddDueDateTime(manager->GetAllocationsMap(), 1, 2020, 5, 31);

    std::cout << manager->GetTaskByID(2)->GetTaskName() << std::endl;

    manager->PrintMap(manager->GetAllocationsMap());

    task_ptr2->date = { 2020, 6, 30 };
    task_ptr->date = { 2019, 11, 20 };

    manager->RemindUsers(manager->GetAllocationsMap());

    close_db(DB);

    return 0;
}*/
