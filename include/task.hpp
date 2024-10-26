#ifndef TASK_HPP
#define TASK_HPP

#include <string>

class Task {
private:
    int taskId;
    std::string description;
    int priority;
    
public:
    Task(int id = 0, const std::string& desc = "", int prio = 0)
        : taskId(id), description(desc), priority(prio) {}
    
    int getId() const { return taskId; }
    std::string getDescription() const { return description; }
    int getPriority() const { return priority; }
    void setPriority(int newPriority) { priority = newPriority; }
    
    bool operator>(const Task& other) const {
        return priority > other.priority;
    }
};

#endif