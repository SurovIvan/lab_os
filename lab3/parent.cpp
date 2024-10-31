#include <iostream>
#include <unistd.h>  // Для системных вызовов, таких как fork(), pipe(), read(), write(), и dup2()
#include <sys/wait.h>  // Для wait()
#include <fcntl.h>
#include <fstream> // Для работы с файловыми потоками (ifstream, ofstream)
#include <string>
#include <cstring>
#include <vector>
#include <sys/mman.h> //mmap
#include <signal.h> //raise


#define FILE_SIZE 4096
#define FILE_NAME "file.txt"

int create_process() {
    pid_t pid = fork();
    if (pid == -1) {
        perror("Fork failed");
        exit(-1);
    }
    return pid;
}

int main() {
    std::string numbers;

    int file = open(FILE_NAME, O_CREAT | O_RDWR, 0777);
    if (file == -1) {
        perror("Error opening file");
    }

    if (ftruncate(file, FILE_SIZE) == -1) {
        perror("File resizing failed");
        close(file);
        return 1;
    }

    char* buffer = (char*)mmap(NULL, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, file, 0);
    if (buffer == MAP_FAILED) {
        perror("Memory mapping failed");
        close(file);
        return 1;
    }
    
    close(file);

    pid_t pid = create_process();

    if (pid == 0) {
        raise(SIGSTOP);
        if (execl("./child", "./child", FILE_NAME, nullptr) == -1) {
            perror("execl error");
            exit(-1);
        }
    }

    else {
        waitpid(pid, nullptr, WUNTRACED);
        std::cout << "Введите числа через пробел: ";
        std::getline(std::cin, numbers);
        strncpy(buffer, numbers.c_str(), FILE_SIZE - 1);

        kill(pid, SIGCONT);

        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            std::cout << buffer << std::endl;
        }
        munmap(buffer, FILE_SIZE);
    }

    return 0;
}