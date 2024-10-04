#include <iostream>
#include <unistd.h>  // Для системных вызовов, таких как fork(), pipe(), read(), write(), и dup2()
#include <sys/wait.h>  // Для wait()
#include <fstream> // Для работы с файловыми потоками (ifstream, ofstream)
#include <string>
#include <vector>

int create_process() {
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("Fork failed");
        exit(-1);
    }
    return pid;
}

int main() {
    std::string file_name;
    std::string numbers;
    int pipe_1[2], pipe_2[2];

    std::cout << "Введите имя файла для записи результата: ";
    getline(std::cin, file_name);

    std::cout << "Введите числа через пробел: ";
    getline(std::cin, numbers);

    if (pipe(pipe_1) == -1 || pipe(pipe_2) == -1) {
        perror("pipe error");
        exit(-1);
    }

    pid_t pid = create_process();

    if (pid == 0) {
        close(pipe_1[1]);
        close(pipe_2[0]);

        if (dup2(pipe_1[0], STDIN_FILENO) == -1) {
            perror("dup2 error1");
            exit(-1);
        }
        if (dup2(pipe_2[1], STDOUT_FILENO) == -1) {
            perror("dup2 error2");
            exit(-1);
        }
        if (execl("./child", "./child", file_name.c_str(), nullptr) == -1) {
            perror("execl error");
            exit(-1);
        }

        close(pipe_1[0]);
        close(pipe_2[1]);
    }

    else {
        close(pipe_1[0]);
        close(pipe_2[1]);

        write(pipe_1[1], numbers.c_str(), numbers.size());
        close(pipe_1[1]);

        char buffer[256];
        int bytes = read(pipe_2[0], buffer, sizeof(buffer));
        if (bytes > 0) {
            buffer[bytes] = '\0';
            std::string response(buffer);
            if (response == "Erorr: divison by zero!") {
                std::cout << "Ошибка: деление на ноль!" << std::endl;
                return 0;
            }
        }
        wait(nullptr);
        close(pipe_2[0]);
    }

    return 0;
}