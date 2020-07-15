#include <iostream>
#include <vector>
#include "Manager.h"
#include "Task.h"
#include "User.h"
#include <ctime>
#include <string>
using namespace TaskStuff;
using namespace UserStuff;
#pragma warning(disable : 4996)

Manager::Manager()
{
    std::cout << "Created Manager" << std::endl;
}

Manager::~Manager()
{
    std::cout << "Destroyed Manager" << std::endl;
}

void Manager::AddTask(const std::shared_ptr<TaskStuff::Task>& t_ptr) {
    m_vectorOfTasks.push_back(t_ptr);
}

const Manager::TaskList& Manager::GetVectorOfTasks() {
    return m_vectorOfTasks;
}

const std::unordered_map<uint32_t, std::shared_ptr<TaskStuff::Task>> Manager::GetTaskMap() {
    return m_Tasks;
}

std::shared_ptr<Task> Manager::CreateTask(const std::string& name)
{
    std::shared_ptr<Task> task = std::make_shared<Task>(name);
    m_Tasks[task->GetID()] = task;
    return task;
}

User* Manager::CreateUser(const std::string& name)
{
    User* u_ptr = new User(name);
    m_Users[u_ptr->GetID()] = u_ptr;
    return u_ptr;
}

const std::shared_ptr<TaskStuff::Task> Manager::GetTaskByID(uint32_t id)
{
    return m_Tasks[id];
}

const std::unordered_map<uint32_t, User*> Manager::GetUserMap()
{
    return m_Users;
}

const std::unordered_map<uint32_t, std::pair<std::shared_ptr<TaskStuff::Task>, User*>> Manager::GetAllocationsMap() {
    return m_Allocations;
}

void Manager::QueryByUser(User* user) {
    uint32_t user_id = user->GetID();
    std::cout << "------ Tasks for user " << user->GetName() << "--------" << std::endl;
    for (auto const& [key, val] : m_Allocations) {
        if (val.second->GetID() == user_id) {
            std::cout << "[Key: " << key << "| Task: " << val.first->GetTaskName() << "| User: " << val.second->GetName() << "| " << val.first->GetCompletionStatus() << "] " << std::endl;
        }
    }
}

void Manager::QueryPendingTasks(bool isComplete)
{
    if (isComplete) {
        std::cout << "---------  printing completed tasks ----------" << std::endl;
    }
    else {
        std::cout << "---------  printing pending tasks ----------" << std::endl;
    }

    for (auto const& [key, val] : m_Allocations) {
        if (val.first->GetCompletionStatus() == isComplete) {
            std::cout << "[Key: " << key << "| Task: " << val.first->GetTaskName() << "| User: " << val.second->GetName() << "| " << val.first->GetCompletionStatus() << "] " << std::endl;
        }
    }
}

void Manager::PrintMap(std::unordered_map<uint32_t, std::pair<std::shared_ptr<TaskStuff::Task>, User*>> map) {
    std::cout << "---------  printing map ----------" << std::endl;
    for (auto const& [key, val] : map) {
        std::cout << "[Key: " << key << "| Task: " << val.first->GetTaskName() << "| User: " << val.second->GetName() << "] " << std::endl;
    }
}

void Manager::AddDueDateTime(std::unordered_map<uint32_t, std::pair<std::shared_ptr<TaskStuff::Task>, User*>> map, int ID, int year, int month, int day) {
    std::cout << "Adding time to task.." << std::endl;
    for (auto const& [key, val] : map) {
        if (key == ID) {
            val.first->SetYear(year);
            val.first->SetMonth(month);
            val.first->SetDay(day);
            std::cout << "[Key: " << key << "| Task: " << val.first->GetTaskName() << " due date : " << val.first->GetYear() << "/" << val.first->GetMonth() << "/" << val.first->GetDay() << "| User: " << val.second->GetName() << "] " << std::endl;
        }
    }
}

int Manager::CheckDueTasks(const TaskStuff::Task::Date& dt) {
    int days = 0;

    const int month_days[12] = { 31, 28, 31, 30, 31, 30,
                           31, 31, 30, 31, 30, 31 };

    time_t curr_time = time(NULL);
    tm* tm_local = localtime(&curr_time);
    int curr_year = 1900 + tm_local->tm_year;
    int curr_month = tm_local->tm_mon + 1;
    int curr_day = tm_local->tm_mday;

    int curr_number = curr_year * 365 + curr_day;
    for (int i = 0; i < curr_month - 1; i++) {
        curr_number += month_days[i];
    }
    int task_date = dt.year * 365 + dt.day;
    for (int i = 0; i < dt.month - 1; i++) {
        task_date += month_days[i];
    }

    if (curr_year > dt.year || curr_month > dt.month || curr_day > dt.day) {
        std::cout << "task is over due  " << curr_number - task_date << " days" << std::endl;
        return curr_number - task_date;

    }
    else {
        std::cout << "Task is due soon " << task_date - curr_number << " days" << std::endl;
        return task_date - curr_number;
    }
}

void Manager::RemindUsers(std::unordered_map<uint32_t, std::pair<std::shared_ptr<TaskStuff::Task>, User*>> map)
{
    for (auto const& [key, val] : map)
    {
        CheckDueTasks(val.first->date);
    }
}

void Manager::AddAllocations(std::shared_ptr<Task> t_ptr, UserStuff::User* user)
{
    if (m_Allocations.empty()) {
        m_Allocations.emplace(1, std::make_pair(t_ptr, user));
    }
    else {
        int size = m_Allocations.size();
        m_Allocations.emplace(size + 1, std::make_pair(t_ptr, user));
    }
}

void Manager::AddOwnerToTask(std::shared_ptr<TaskStuff::Task> t_ptr, UserStuff::User user) {
    std::vector <std::string> v = t_ptr->GetOwners();
    std::string name = user.GetName();
    v.push_back(name);
    t_ptr->SetOwners(v);
}
