#ifndef FILE_MANAGER_HPP
#define FILE_MANAGER_HPP

#include <string>
#include <vector>
#include "task.hpp"
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>

class FileManager {
private:
    const std::string TASKS_FILE = "data/tasks.csv";
    const std::string LOG_FILE = "data/scheduler_log.txt";
    const std::string BACKUP_DIR = "data/backups/";
    const std::string REPORT_DIR = "data/reports/";
    
    void createDirectories();
    std::string getCurrentTimestamp() const;
    std::string formatDuration(double seconds) const;

public:
    FileManager();
    void saveTasks(const std::vector<Task>& tasks);
    std::vector<Task> loadTasks();
    void logAction(const std::string& action, const Task& task);
    void logAction(const std::string& action);
    void createBackup(const std::vector<Task>& tasks);
    void generateReport(const std::vector<Task>& tasks, 
                       const std::vector<Task>& completedTasks);
    std::vector<std::string> getBackupFiles() const;
    std::string getLatestBackupFile() const;
    bool restoreFromBackup(const std::string& backupFile, std::vector<Task>& tasks);
};

#endif