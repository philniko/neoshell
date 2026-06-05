#include <iostream>
#include <sstream>
#include <unordered_set>

std::unordered_set<std::string> builtins = {"exit", "echo", "type"};

int main() {
    // flush after every output operation
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    std::string input;

    while (true) {
        std::cout << "$ ";

        // read input
        std::getline(std::cin, input);
        std::istringstream iss(input);
        std::string command;
        iss >> command;

        if (command == "exit") {
            return 0;
        } else if (command == "echo") {
            std::string rest;
            std::getline(iss, rest);
            if (!rest.empty() && rest[0] == ' ') {
                rest.erase(0, 1);
                std::cout << rest << "\n";
            }
        } else if (command == "type") {
            std::string arg;
            iss >> arg;
            
            if (builtins.contains(arg)) {
                std::cout << arg << " is a shell builtin\n";
            } else {
                std::cout << "type: " << arg << ": not found\n";
            }
        } else {
            std::cout << input << ": command not found..." << "\n";
        }
    }

    return 0;
}
