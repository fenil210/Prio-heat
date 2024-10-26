#include <iostream>
#include <limits>
#include <filesystem>
#include "min_heap.hpp"
#include "file_manager.hpp"

namespace fs = std::filesystem;

void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void displayMenu() {
    std::cout << "\nPriority-Based Task Scheduler\n"
              << "1. Add New Task\n"
              << "2. Execute Highest Priority Task\n"
              << "3. Update Task Priority\n"
              << "4. Display All Tasks\n"
              << "5. Create Backup\n"
              << "6. Generate HTML Report\n"
              << "7. Restore from Latest Backup\n"
              << "8. Exit\n"
              << "Enter your choice: ";
}

bool isValidPriority(int priority) {
    return priority >= 1 && priority <= 100;  // Assuming valid priority range is 1-100
}

int main() {
    MinHeap taskScheduler;
    int choice, taskId, priority;
    std::string description;
    
    while (true) {
        displayMenu();
        
        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number.\n";
            clearInputBuffer();
            continue;
        }
        
        try {
            switch (choice) {
                case 1: {
                    std::cout << "Enter Task ID (positive integer): ";
                    if (!(std::cin >> taskId) || taskId <= 0) {
                        throw std::runtime_error("Invalid Task ID. Please enter a positive integer.");
                    }
                    
                    // Check if task ID already exists before asking for other details
                    if (taskScheduler.isTaskIdExists(taskId)) {
                        throw std::runtime_error("Task ID already exists. Please use a unique ID.");
                    }
                    
                    std::cout << "Enter Priority (1-100, lower number = higher priority): ";
                    if (!(std::cin >> priority) || !isValidPriority(priority)) {
                        throw std::runtime_error("Invalid priority. Please enter a number between 1 and 100.");
                    }
                    
                    clearInputBuffer();
                    std::cout << "Enter Task Description: ";
                    std::getline(std::cin, description);
                    
                    if (description.empty()) {
                        throw std::runtime_error("Task description cannot be empty.");
                    }
                    
                    Task newTask(taskId, description, priority);
                    taskScheduler.addTask(newTask);
                    std::cout << "Task added successfully!\n";
                    break;
                }
                
                case 2: {
                    if (!taskScheduler.isEmpty()) {
                        Task executedTask = taskScheduler.removeHighestPriorityTask();
                        std::cout << "\nExecuted Task:\n";
                        std::cout << "ID: " << executedTask.getId() << "\n";
                        std::cout << "Description: " << executedTask.getDescription() << "\n";
                        std::cout << "Priority: " << executedTask.getPriority() << "\n";
                    } else {
                        std::cout << "No tasks to execute!\n";
                    }
                    break;
                }
                
                case 3: {
                    std::cout << "Enter Task ID to update: ";
                    if (!(std::cin >> taskId)) {
                        throw std::runtime_error("Invalid Task ID format.");
                    }
                    
                    if (!taskScheduler.isTaskIdExists(taskId)) {
                        throw std::runtime_error("Task ID does not exist.");
                    }
                    
                    std::cout << "Enter new priority (1-100): ";
                    if (!(std::cin >> priority) || !isValidPriority(priority)) {
                        throw std::runtime_error("Invalid priority. Please enter a number between 1 and 100.");
                    }
                    
                    taskScheduler.updateTaskPriority(taskId, priority);
                    std::cout << "Task priority updated successfully!\n";
                    break;
                }
                
                case 4:
                    taskScheduler.displayTasks();
                    break;
            

                case 5: {
                    taskScheduler.createBackup();
                    std::cout << "Backup created successfully!\n";
                    break;
                }

                case 6: {
                    taskScheduler.generateReport();
                    std::cout << "HTML report generated successfully!\n";
                    break;
                }

                                case 7: {
                    auto backups = taskScheduler.getRestorePoints();
                    if (backups.empty()) {
                        std::cout << "No backup files found!\n";
                        break;
                    }

                    std::cout << "\nAvailable restore points:\n";
                    for (size_t i = 0; i < backups.size(); ++i) {
                        // Extract just the filename from the path
                        std::string filename = fs::path(backups[i]).filename().string();
                        std::cout << i + 1 << ". " << filename << "\n";
                    }

                    if (taskScheduler.restoreFromLatestBackup()) {
                        std::cout << "Successfully restored from latest backup!\n";
                    } else {
                        std::cout << "Restore failed. Please check the log file for details.\n";
                    }
                    break;
                }

                case 8:
                    taskScheduler.saveToFile();
                    std::cout << "Saving tasks and exiting...\n";
                    return 0;
                
                default:
                    std::cout << "Invalid choice! Please enter a number between 1 and 5.\n";
            }
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
            clearInputBuffer();
        }
    }
    
    return 0;
}