#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>

int main(int argc, char *argv[]) {
    if (argc != 2){
        perror("Incorrect number of arguments");
        exit(-1);
    }

    std::ofstream result;
    result.open(argv[1]);
    
    std::cout<<"helelel";
    if (!result) {
        perror("File opening error");
        exit(-1);
    }
    
    std::string numbers;
    std::vector<char> buffer(1024);
    int bytes;

    while ((bytes = read(STDIN_FILENO, buffer.data(), buffer.size())) > 0) {
        numbers.append(buffer.data(), bytes);
    }

    if (bytes == -1){
        perror("Read error");
        exit(-1);
    }

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
            write(STDOUT_FILENO, err.c_str(), err.length());
            result.close();
            return 0;
        }
        res_numb /= ans[i];
    }
    
    result << res_numb;
    result.close();
    return 0;
}