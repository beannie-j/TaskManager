#pragma once
#include <iostream> 
#include <ctime>
#include <vector>

namespace TaskStuff {
    class Task {
    public:
        struct Date {
            int year;
            int month;
            int day;
        } date;

        enum Category { Laundry, Dishes, Hoover, Trash, Cleaning };
        enum Location { Kitchen, Bedroom, Bathroom };

        /* Member functions - getters and setters
        */
        std::string GetTaskName();
        std::vector<std::string> GetOwners();
        void SetOwners(std::vector <std::string> vec);
        void SetYear(int year);
        void SetMonth(int month);
        void SetDay(int day);
        void SetHour(int hour);
        void SetMinute(int minute);
        void SetDaysLeft(int days_left);
        void SetCompletionStatus(bool completed);
        void SetTime(tm* time);

        bool GetCompletionStatus();
        int GetYear();
        int GetMonth();
        int GetDay();
        int GetHour();
        int GetMinute();
        int GetDaysLeft();
        tm* GetTime();
        void Data();

        uint32_t GetID() const { return m_ID; }

        // Default constructor
        Task();
        // Initialise a Task with customisation.
        Task(std::string name, Category category, Location location, int yy, int month, int dd, int hh, int mm, int duration, bool completed);
        // Simple Task constructor - just with name
        Task(std::string name);
        //Destructor
        ~Task();

    private:
        uint32_t m_ID = 0;
        // If I use wstring here I get an error why
        std::string m_name{ "Task 1" };
        enum Category m_category;
        enum Location m_location;
        int m_yy{ 0 }, m_month{ 0 }, m_dd{ 0 }, m_hh{ 0 }, m_mm{ 0 };
        int m_duration{ 0 };
        int m_days_left{ 0 };
        bool m_completed = false;
        tm* m_time;
        // why can't I not do type Users here
        // std::vector<User> owners;
        std::vector<std::string> m_owners;

        static uint32_t s_NextID;
    };

    //non member functions here.
    void AssignTask(std::shared_ptr<Task> t_ptr, std::string name);
}
