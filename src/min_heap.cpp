#include "min_heap.hpp"
#include "file_manager.hpp"
#include "task.hpp"
#include <iostream>

void MinHeap::heapifyUp(int index) {
    while (index > 0) {
        int parentIndex = getParentIndex(index);
        if (heap[parentIndex] > heap[index]) {
            std::swap(heap[index], heap[parentIndex]);
            index = parentIndex;
        } else {
            break;
        }
    }
}

void MinHeap::heapifyDown(int index) {
    int smallestIndex = index;
    int leftChild = getLeftChildIndex(index);
    int rightChild = getRightChildIndex(index);
    
    if (leftChild < heap.size() && heap[leftChild].getPriority() < heap[smallestIndex].getPriority()) {
        smallestIndex = leftChild;
    }
    
    if (rightChild < heap.size() && heap[rightChild].getPriority() < heap[smallestIndex].getPriority()) {
        smallestIndex = rightChild;
    }
    
    if (smallestIndex != index) {
        std::swap(heap[index], heap[smallestIndex]);
        heapifyDown(smallestIndex);
    }
}

void MinHeap::addTask(const Task& task) {
    // Check if task ID already exists
    if (isTaskIdExists(task.getId())) {
        throw std::runtime_error("Task ID already exists. Please use a unique ID.");
    }
    
    heap.push_back(task);
    existingTaskIds.insert(task.getId());  // Add to tracking set
    heapifyUp(heap.size() - 1);
}

Task MinHeap::removeHighestPriorityTask() {
    if (heap.empty()) {
        fileManager.logAction("Attempted to remove task from empty heap");
        throw std::runtime_error("Heap is empty");
    }
    
    // Store the highest priority task
    Task highestPriorityTask = heap[0];
    
    // Remove the task ID from tracking set
    existingTaskIds.erase(highestPriorityTask.getId());
    
    // Replace root with last element and remove last element
    heap[0] = heap.back();
    heap.pop_back();
    
    // Rebalance the heap if not empty
    if (!heap.empty()) {
        heapifyDown(0);
    }
    
    // Add to completed tasks history
    completedTasks.push_back(highestPriorityTask);
    
    // Keep only the last 10 completed tasks
    if (completedTasks.size() > 10) {
        completedTasks.erase(completedTasks.begin());
    }
    
    // Log the task execution
    fileManager.logAction("Executed task", highestPriorityTask);
    
    // Save the current state to file
    saveToFile();
    
    // Create automatic backup after every 5 tasks are completed
    static int completedCount = 0;
    completedCount++;
    if (completedCount % 5 == 0) {
        fileManager.createBackup(heap);
        fileManager.logAction("Created automatic backup after 5 task completions");
    }
    
    return highestPriorityTask;
}


int MinHeap::findTaskIndex(int taskId) const {
    for (size_t i = 0; i < heap.size(); ++i) {
        if (heap[i].getId() == taskId) {
            return i;
        }
    }
    return -1;
}

void MinHeap::updateTaskPriority(int taskId, int newPriority) {
    int index = findTaskIndex(taskId);
    if (index == -1) {
        throw std::runtime_error("Task not found");
    }
    
    int oldPriority = heap[index].getPriority();
    heap[index].setPriority(newPriority);
    
    if (newPriority < oldPriority) {
        heapifyUp(index);
    } else {
        heapifyDown(index);
    }
}

void MinHeap::displayTasks() const {
    if (heap.empty()) {
        std::cout << "No tasks in the scheduler.\n";
        return;
    }
    
    std::cout << "\nCurrent Tasks in Scheduler:\n";
    std::cout << "ID\tPriority\tDescription\n";
    std::cout << "--------------------------------\n";
    
    for (const auto& task : heap) {
        std::cout << task.getId() << "\t" 
                  << task.getPriority() << "\t\t"
                  << task.getDescription() << "\n";
    }
    std::cout << "--------------------------------\n";
}

void MinHeap::loadFromFile() {
    auto tasks = fileManager.loadTasks();
    for (const auto& task : tasks) {
        addTask(task);
    }
    fileManager.logAction("Loaded tasks from file");
}

void MinHeap::saveToFile() {
    fileManager.saveTasks(heap);
    fileManager.logAction("Saved tasks to file");
}

void MinHeap::createBackup() {
    fileManager.createBackup(heap);
}

void MinHeap::generateReport() {
    fileManager.generateReport(heap, completedTasks);
}

bool MinHeap::restoreFromLatestBackup() {
    std::string latestBackup = fileManager.getLatestBackupFile();
    if (latestBackup.empty()) {
        return false;
    }

    std::vector<Task> restoredTasks;
    if (!fileManager.restoreFromBackup(latestBackup, restoredTasks)) {
        return false;
    }

    // Clear current heap and existing task IDs
    heap.clear();
    existingTaskIds.clear();

    // Add restored tasks
    for (const auto& task : restoredTasks) {
        try {
            addTask(task);
        } catch (const std::exception& e) {
            fileManager.logAction("Error restoring task: " + std::string(e.what()));
            return false;
        }
    }

    fileManager.logAction("Restored from backup: " + latestBackup);
    return true;
}

// // Optional: Add a method to get restore points for user selection
// public:
//     std::vector<std::string> getRestorePoints() {
//         return fileManager.getBackupFiles();
//     }
