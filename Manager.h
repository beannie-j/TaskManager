#pragma once
#include <iostream>
#include <vector>
#include "Task.h"
#include "User.h"
#include <unordered_map>
using namespace TaskStuff;
using namespace UserStuff;

class Manager {
    using TaskList = std::vector<std::shared_ptr<TaskStuff::Task>>;
public:
    UserStuff::User GetUserByID(uint32_t id);
    const std::shared_ptr<TaskStuff::Task> GetTaskByID(uint32_t id);
    void AddOwnerToTask(std::shared_ptr<TaskStuff::Task> t_ptr, UserStuff::User user);
    const TaskList& GetVectorOfTasks();
    void AddTask(const std::shared_ptr<TaskStuff::Task>& t_ptr);
    std::shared_ptr<Task> CreateTask(const std::string& name);

    void AddAllocations(std::shared_ptr<TaskStuff::Task> t_ptr, UserStuff::User* user);
    const std::unordered_map<uint32_t, std::shared_ptr<TaskStuff::Task>> GetTaskMap();
    const std::unordered_map<uint32_t, User*> GetUserMap();
    const std::unordered_map<uint32_t, std::pair<std::shared_ptr<TaskStuff::Task>, User*>> GetAllocationsMap();
    void QueryPendingTasks(bool isComplete);
    User* CreateUser(const std::string& name);
    void QueryByUser(User* user);
    void PrintMap(std::unordered_map<uint32_t, std::pair<std::shared_ptr<TaskStuff::Task>, User*>> map);
    void AddDueDateTime(std::unordered_map<uint32_t, std::pair<std::shared_ptr<TaskStuff::Task>, User*>> map, int ID, int year, int month, int dd);
    int CheckDueTasks(const TaskStuff::Task::Date& dt);
    void RemindUsers(std::unordered_map<uint32_t, std::pair<std::shared_ptr<TaskStuff::Task>, User*>> map);


    Manager();
    ~Manager();

private:
    TaskList m_vectorOfTasks;
    std::unordered_map<uint32_t, std::shared_ptr<TaskStuff::Task>> m_Tasks;
    std::unordered_map<uint32_t, std::pair<std::shared_ptr<TaskStuff::Task>, User*>> m_Allocations;
    std::unordered_map<uint32_t, User*> m_Users;
};

