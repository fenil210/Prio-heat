#ifndef MIN_HEAP_HPP
#define MIN_HEAP_HPP

#include <vector>
#include <stdexcept>
#include <unordered_set>  // Add this for tracking task IDs
#include "task.hpp"
#include "file_manager.hpp"

class MinHeap {
private:
    std::vector<Task> heap;
    std::unordered_set<int> existingTaskIds;  // Track existing task IDs
    FileManager fileManager;
    
    void heapifyUp(int index);
    void heapifyDown(int index);
    int getParentIndex(int index) const { return (index - 1) / 2; }
    int getLeftChildIndex(int index) const { return 2 * index + 1; }
    int getRightChildIndex(int index) const { return 2 * index + 2; }
    std::vector<Task> completedTasks;
    
public:
    void addTask(const Task& task);
    Task removeHighestPriorityTask();
    void updateTaskPriority(int taskId, int newPriority);
    bool isEmpty() const { return heap.empty(); }
    size_t size() const { return heap.size(); }
    void displayTasks() const;
    int findTaskIndex(int taskId) const;
    bool isTaskIdExists(int taskId) const { return existingTaskIds.count(taskId) > 0; }
    void loadFromFile();
    void saveToFile();
    void createBackup();
    void generateReport();
    bool restoreFromLatestBackup();
    std::vector<std::string> getRestorePoints() {
        return fileManager.getBackupFiles();
    }
};

#endif