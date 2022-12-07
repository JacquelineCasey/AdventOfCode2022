
#include <iostream>
#include <sstream>
#include <vector>


struct file {
public:
    std::string name {};
    int size {};

    file(std::string name, int size) : name {name}, size {size} {}
};

struct directory_tree {
public:
    std::string name {};
    directory_tree* parent = NULL;
    int total_size {}; // Computed explicitely after directory structure finalized.

    std::vector<directory_tree*> sub_dirs = {};
    std::vector<file> files = {};
};

struct command {
public:
    std::string program = {};
    std::string arg = {}; // Only one is needed

    std::vector<std::string> output_lines = {};
};


directory_tree* process_command(const command& command, directory_tree* dir) {
    if (command.program == "cd") {
        if (command.arg == "/") {
            return NULL; // Signals caller to set dir to root.
        }
        if (command.arg == "..") {
            if (dir->parent == NULL) {
                std::cout << "Tried to go to parent of root\n";
                return NULL;
            }
            return dir->parent;
        }

        for (directory_tree* child_dir : dir->sub_dirs) {
            if (child_dir->name == command.arg) {
                return child_dir;
            }
        }

        directory_tree* new_child_dir = new directory_tree {};

        new_child_dir->parent = dir;
        new_child_dir->name = command.arg;
        dir->sub_dirs.push_back(new_child_dir);

        return new_child_dir;
    }
    else if (command.program == "ls") {
        for (std::string line : command.output_lines) {
            std::stringstream sstream {line};
            std::string first;
            sstream >> first;

            // We currently ignore directories...
            if (first == "dir") {
                continue;
            }
        
            int size = std::stoi(first);
            std::string filename;
            sstream >> filename;
            
            bool found {false};
            for (const file& file: dir->files) {
                if (file.name == filename) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                dir->files.push_back({filename, size});
            }
        }

        return dir;
    }
    else {
        std::cout << "Unrecognized command...\n";
        return NULL;
    }
}

int compute_dir_sizes(directory_tree* dir) {
    int sum {0};

    for (directory_tree* sub_dir : dir->sub_dirs) {
        sum += compute_dir_sizes(sub_dir);
    }

    for (const file& file : dir->files) {
        sum += file.size;
    }

    dir->total_size = sum;
    return sum;
}

void sum_sizes_under_100000(directory_tree* dir, int& sum) {
    if (dir->total_size <= 100000) {
        sum += dir->total_size;
    }

    for (directory_tree* sub_dir : dir->sub_dirs) {
        sum_sizes_under_100000(sub_dir, sum);
    }
}


int main() {
    std::vector<std::string> lines {};
    
    std::string line {};
    while (std::getline(std::cin, line)) {
        lines.push_back(line);
    }

    std::vector<command> commands {};
    for (int i = 0; i < static_cast<int>(lines.size()); i++) {
        if (lines[i][0] == '$') {
            command curr_command {};
            std::stringstream sstream {lines[i]};
            char ch;
            sstream >> ch >> curr_command.program;
            if (curr_command.program == "cd") {
                sstream >> curr_command.arg;
            }
            else if (curr_command.program == "ls"){
                for (int j = i + 1; j < static_cast<int>(lines.size()); j++) {
                    if (lines[j][0] == '$') {
                        break;
                    }
                    curr_command.output_lines.push_back(lines[j]);
                }
            }
            else {
                std::cout << "Something went wrong\n";
                return -1;
            }

            commands.push_back(curr_command);
        }
    }

    directory_tree* root = new directory_tree {};
    root->name = "/";

    directory_tree* dir = root;
    for (const command& command : commands) {
        dir = process_command(command, dir);
        if (dir == NULL) { // This is a hack.
            dir = root;
        }
    } 

    compute_dir_sizes(root);

    int sum {0};
    sum_sizes_under_100000(root, sum);

    std::cout << sum << '\n';

    // Will not free directories.
}
