#include "User.h"
#include "Task.h"
#include <iostream>
using namespace UserStuff;

uint32_t User::s_NextID = 1;

User::User()
    :m_ID(s_NextID++)
{
}


User::User(std::string name, std::string password) {}
User::User(std::string name)
    :m_ID(s_NextID++), m_name(name)
{
    std::cout << "Creating User :" << name << std::endl;
    std::cout << "m_ID is : " << m_ID << std::endl;
}

User::~User()
{
    std::cout << "Destroyed User" << std::endl;
}

void User::Data() {
    std::cout << "username : " << GetName() << std::endl;
}

std::string User::GetName() {
    return m_name;
}

std::string User::GetPassword() {
    return m_password;
}

void User::SetName(std::string name) {
    m_name = name;
}

void User::SetPassword(std::string password) {
    m_password = password;
}
