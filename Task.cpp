#include "Task.h"
#include <iostream>
#include <ctime>
using namespace TaskStuff;

uint32_t Task::s_NextID = 1;
/* Users have tasks to complete but they dont own it.
Task name
Task category - use enum here
Task time - is there a time class I can use?
Task duration
Task points - you get points for doing tasks
*/

// Default constructor
Task::Task()
    : m_ID(s_NextID++)
{
}

// Initialise a Task with customisation.
Task::Task(std::string name, Category category, Location location, int yy, int month, int dd, int hh, int mm, int duration, bool completed)
    : m_ID(s_NextID++), m_name(name), m_category(category), m_location(location), m_yy(yy), m_month(month), m_dd(dd), m_hh(hh), m_mm(mm), m_duration(duration), m_completed(completed)
{
}

Task::Task(std::string name)
    : m_ID(s_NextID++), m_name(name)
{
    std::cout << "Creating Task [" << m_ID << "] " << name << std::endl;
}

Task::~Task()
{
    std::cout << "Destroyed Task" << std::endl;
}

void Task::Data() {
    std::cout << "task name : " << GetTaskName() << std::endl;
}

std::string Task::GetTaskName() {
    return m_name;
}

bool Task::GetCompletionStatus() {
    return m_completed;
}

std::vector<std::string> Task::GetOwners() {
    return m_owners;
}

// #Question
void Task::SetTime(tm* time) {
    m_time = time;
}

tm* Task::GetTime() {
    return m_time;
}

int Task::GetDay() {
    return m_dd;
}

int Task::GetYear() {
    return m_yy;
}

int Task::GetMonth() {
    return m_mm;
}

int Task::GetDaysLeft() {
    return m_days_left;
}

void Task::SetOwners(std::vector <std::string> vec) {
    m_owners = vec;
}

void Task::SetCompletionStatus(bool completed) {
    m_completed = completed;
}

void Task::SetYear(int yy) {
    m_yy = yy;
}

void Task::SetMonth(int mm) {
    m_mm = mm;
}

void Task::SetDay(int dd) {
    m_dd = dd;
}

// how can I use User here?
void TaskStuff::AssignTask(std::shared_ptr<Task> t_ptr, std::string name) {
    std::vector <std::string> v = t_ptr->GetOwners();
    v.push_back(name);
    t_ptr->SetOwners(v);
}



