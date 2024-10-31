#include <iostream>
#include <unistd.h>  // Для системных вызовов, таких как fork(), pipe(), read(), write(), и dup2()
#include <sys/wait.h>  // Для wait()
#include <fcntl.h>
#include <fstream> // Для работы с файловыми потоками (ifstream, ofstream)
#include <string>
#include <cstring>
#include <vector>
#include <sys/mman.h> //mmap
#include <signal.h> //

#define FILE_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc != 2){
        perror("Incorrect number of arguments");
        exit(-1);
    }

    int file = open(argv[1], O_CREAT | O_RDWR, 0777);
    if (file == -1) {
        perror("Error opening file");
    }
    
    char* buffer = (char*)mmap(NULL, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, file, 0);
    if (buffer == MAP_FAILED) {
        perror("Memory mapping failed in child");
        close(file);
        return 1;
    }
    close(file);
    
    std::string numbers(buffer);

    std::vector<float> ans;
    int16_t start = 0, end = 0;

    while ((end = numbers.find(' ', start)) != std::string::npos) {
        std::string substring = numbers.substr(start, end - start);
    
        ans.push_back(std::stof(substring));
        start = end + 1;
    }

    if (start < numbers.size()) {
        ans.push_back(std::stof(numbers.substr(start)));
    }

    float res_numb = ans[0];

    for(int16_t i = 1; i < ans.size(); ++i) {
        if (ans[i] == 0) {
            std::string err = "Erorr: divison by zero!";
            strncpy(buffer, err.c_str(), FILE_SIZE - 1);
            return 0;
        }
        res_numb /= ans[i];
    }
    std::string result = std::to_string(res_numb);
    strncpy(buffer, result.c_str(), FILE_SIZE - 1);
    munmap(buffer, 0);
    return 0;
}