#include "file_manager.hpp"
#include <filesystem>
#include <algorithm>
#include <chrono>

namespace fs = std::filesystem;

FileManager::FileManager() {
    createDirectories();
}

void FileManager::createDirectories() {
    fs::create_directories("data");
    fs::create_directories(BACKUP_DIR);
    fs::create_directories(REPORT_DIR);
}

std::string FileManager::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
    return ss.str();
}

void FileManager::saveTasks(const std::vector<Task>& tasks) {
    std::ofstream file(TASKS_FILE);
    file << "TaskID,Priority,Description\n";
    for (const auto& task : tasks) {
        file << task.getId() << ","
             << task.getPriority() << ","
             << task.getDescription() << "\n";
    }
}

std::vector<Task> FileManager::loadTasks() {
    std::vector<Task> tasks;
    std::ifstream file(TASKS_FILE);
    
    if (!file.is_open()) return tasks;
    
    std::string line, header;
    std::getline(file, header); // Skip header
    
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string idStr, priorityStr, description;
        
        std::getline(ss, idStr, ',');
        std::getline(ss, priorityStr, ',');
        std::getline(ss, description);
        
        tasks.emplace_back(std::stoi(idStr), description, std::stoi(priorityStr));
    }
    
    return tasks;
}

void FileManager::logAction(const std::string& action, const Task& task) {
    std::ofstream log(LOG_FILE, std::ios::app);
    log << getCurrentTimestamp() << " - " 
        << action << " - Task ID: " << task.getId()
        << ", Priority: " << task.getPriority()
        << ", Description: " << task.getDescription() << "\n";
}

void FileManager::logAction(const std::string& action) {
    std::ofstream log(LOG_FILE, std::ios::app);
    log << getCurrentTimestamp() << " - " << action << "\n";
}

void FileManager::createBackup(const std::vector<Task>& tasks) {
    std::string backupFile = BACKUP_DIR + "backup_" + getCurrentTimestamp() + ".csv";
    std::ofstream file(backupFile);
    
    file << "TaskID,Priority,Description\n";
    for (const auto& task : tasks) {
        file << task.getId() << ","
             << task.getPriority() << ","
             << task.getDescription() << "\n";
    }
    
    logAction("Created backup: " + backupFile);
}

void FileManager::generateReport(const std::vector<Task>& tasks,
                               const std::vector<Task>& completedTasks) {
    std::string reportFile = REPORT_DIR + "report_" + getCurrentTimestamp() + ".html";
    std::ofstream file(reportFile);
    
    file << "<!DOCTYPE html>\n<html>\n<head>\n"
         << "<style>\n"
         << "body { font-family: Arial, sans-serif; margin: 40px; }\n"
         << "table { border-collapse: collapse; width: 100%; }\n"
         << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n"
         << "th { background-color: #4CAF50; color: white; }\n"
         << "tr:nth-child(even) { background-color: #f2f2f2; }\n"
         << ".summary { margin-bottom: 20px; }\n"
         << "</style>\n"
         << "</head>\n<body>\n";
    
    // Summary Section
    file << "<div class='summary'>\n"
         << "<h2>Task Scheduler Report - " << getCurrentTimestamp() << "</h2>\n"
         << "<p>Total Active Tasks: " << tasks.size() << "</p>\n"
         << "<p>Total Completed Tasks: " << completedTasks.size() << "</p>\n"
         << "</div>\n";
    
    // Active Tasks Table
    file << "<h3>Active Tasks</h3>\n"
         << "<table>\n"
         << "<tr><th>ID</th><th>Priority</th><th>Description</th></tr>\n";
    
    for (const auto& task : tasks) {
        file << "<tr><td>" << task.getId() << "</td>"
             << "<td>" << task.getPriority() << "</td>"
             << "<td>" << task.getDescription() << "</td></tr>\n";
    }
    
    file << "</table>\n";
    
    // Completed Tasks Table
    file << "<h3>Recently Completed Tasks</h3>\n"
         << "<table>\n"
         << "<tr><th>ID</th><th>Priority</th><th>Description</th></tr>\n";
    
    for (const auto& task : completedTasks) {
        file << "<tr><td>" << task.getId() << "</td>"
             << "<td>" << task.getPriority() << "</td>"
             << "<td>" << task.getDescription() << "</td></tr>\n";
    }
    
    file << "</table>\n</body>\n</html>";
    
    logAction("Generated report: " + reportFile);
}

std::vector<std::string> FileManager::getBackupFiles() const {
    std::vector<std::string> backups;
    for (const auto& entry : fs::directory_iterator(BACKUP_DIR)) {
        if (entry.path().extension() == ".csv") {
            backups.push_back(entry.path().string());
        }
    }
    // Sort by filename (which contains timestamp) in descending order
    std::sort(backups.begin(), backups.end(), std::greater<std::string>());
    return backups;
}

std::string FileManager::getLatestBackupFile() const {
    auto backups = getBackupFiles();
    if (backups.empty()) {
        return "";
    }
    return backups[0]; // Return the most recent backup
}

bool FileManager::restoreFromBackup(const std::string& backupFile, std::vector<Task>& tasks) {
    if (!fs::exists(backupFile)) {
        return false;
    }

    std::ifstream file(backupFile);
    if (!file.is_open()) {
        return false;
    }

    tasks.clear(); // Clear existing tasks
    std::string line, header;
    std::getline(file, header); // Skip header

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string idStr, priorityStr, description;
        
        std::getline(ss, idStr, ',');
        std::getline(ss, priorityStr, ',');
        std::getline(ss, description);
        
        try {
            int id = std::stoi(idStr);
            int priority = std::stoi(priorityStr);
            tasks.emplace_back(id, description, priority);
        } catch (const std::exception& e) {
            logAction("Error parsing backup file: " + std::string(e.what()));
            return false;
        }
    }

    logAction("Successfully restored from backup: " + backupFile);
    return true;
}