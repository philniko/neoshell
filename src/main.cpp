#include <filesystem>
#include <iostream>
#include <optional>
#include <sstream>
#include <unistd.h>
#include <unordered_set>
#include <vector>
#include <wait.h>

namespace fs = std::filesystem;

std::unordered_set<std::string> builtins = {"exit", "echo", "type", "pwd"};

// split command into whitespace-separated tokens
std::vector<std::string> tokenize(const std::string& input) {
    std::istringstream iss(input);
    std::vector<std::string> args;
    std::string arg;

    while (iss >> arg) {
        args.push_back(arg);
    }

    return args;
}

int execute_command(const std::string& path, std::vector<std::string>& args) {
    if (args.empty()) {
        return -1;
    }

    // execv needs a null-terminated char* array
    std::vector<char*> argv;

    for (auto& s : args) {
        argv.push_back(s.data());
    }
    argv.push_back(nullptr);

    pid_t pid = fork();

    if (pid == 0) {
        // child
        execv(path.c_str(), argv.data());

        // only reached if execv fails
        perror("execv");
        _exit(127);
    } else if (pid > 0) {
        // parent
        int status;
        waitpid(pid, &status, 0);
        // 128 + signal_number is bash convention for signal deaths in $
        return WIFEXITED(status) ? WEXITSTATUS(status) : 128 + WTERMSIG(status);
    } else {
        perror("fork");
        return -1;
    }
}

std::optional<std::string> find_executable(const std::string& command) {
    const char* path_env = std::getenv("PATH");

    if (!path_env) {
        return std::nullopt;
    }

    std::stringstream ss(path_env);
    std::string dir;

    while (std::getline(ss, dir, ':')) {
        std::string full_path = dir + "/" + command;

        if (access(full_path.c_str(), X_OK) == 0) {
            return full_path;
        }
    }

    return std::nullopt;
}

int main() {
    // flush after every output operation
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    std::string input;
    int last_status = 0;

    while (true) {
        std::cout << "$ ";

        // read input
        if (!std::getline(std::cin, input)) {
            break; // EOF (Ctrl-D)
        }

        std::vector<std::string> args = tokenize(input);
        if (args.empty()) {
            continue;
        }

        const std::string& command = args[0];

        if (command == "exit") {
            return args.size() > 1 ? std::stoi(args[1]) : 0;
        } else if (command == "echo") {
            for (size_t i = 1; i < args.size(); ++i) {
                std::cout << args[i] << (i + 1 < args.size() ? " " : "\n");
            }
            if (args.size() == 1) {
                std::cout << "\n";
            }
        } else if (command == "type") {
            const std::string& name = args.size() > 1 ? args[1] : command;

            if (builtins.contains(name)) {
                std::cout << name << " is a shell builtin\n";
            } else if (auto path = find_executable(name)) {
                std::cout << name << " is " << *path << "\n";
            } else {
                std::cout << "type: " << name << ": not found\n";
            }
        } else if (command == "pwd") {
            fs::path cwd = fs::current_path();
            std::cout << cwd.string() << "\n";
        } else if (auto path = find_executable(command)) {
            last_status = execute_command(*path, args);
        } else {
            std::cout << command << ": command not found\n";
            last_status = 127;
        }
    }

    return last_status;
}
