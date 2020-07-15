#pragma once
#include <iostream>

namespace UserStuff {
    class User {
    private:
        static uint32_t s_NextID;
        std::string m_name;
        std::string m_password;
        uint32_t m_ID = 0;
    public:
        // Default Constructor
        User();
        ~User();
        User(std::string name, std::string password);
        User(std::string name);
        std::string GetName();
        std::string GetPassword();
        void SetName(std::string name);
        void SetPassword(std::string password);
        void Data();
        uint32_t GetID() const { return m_ID; }

    };
}

