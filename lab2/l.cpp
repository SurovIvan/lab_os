#include <iostream>
#include <pthread.h>
#include <cstring>
#include <cstdlib> // для rand()
#include <chrono>
#include <unistd.h>

#define MAX_SIZE 100000000
#define LEN_WORD 8

pthread_mutex_t mutex;

typedef struct {
    char* text;
    char* word;
    int start;
    int end;
} thread_data;

void substring_search(char* text, char* word, int start, int end) {
    int i = start;
    int j = 0;
    //для теста
    /*for(int k = 0; k < 100000000; k++) {
        for (int l = 0; l < 1000000; l++) {
        int a = l;
        int b = k;
        b*=a;
        }

    }*/
    for (i; i <= end; ++i) {
        if (text[i] == word[j]) {
            ++j;
        } else if (text[i] != word[j]) {
            j = 0;
            i = i - j;
        }
        if (j == LEN_WORD - 1) {
            pthread_mutex_lock(&mutex);
            std::cout << "The substring: " << word << " was found on the index: " << i - j << std::endl;
            pthread_mutex_unlock(&mutex);
            j = 0;
        }
    }
}

void* pthread_sub_seacrh(void* args) {
    thread_data *data = (thread_data*)args;
    substring_search(data->text, data->word, data->start, data->end);
    return nullptr;
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Incorrect input, enter " << argv[0] << " number of threads" << std::endl;
        return 0;
    }
    char* text = new char[MAX_SIZE];
    char* word = new char[LEN_WORD];
    strcpy(word, "pattern");

    srand(static_cast<unsigned int>(time(0)));

    for(int i = 0; i < MAX_SIZE - 1; ++i) {
        text[i] = 'A' + (rand() % 26 );
    }
    text[MAX_SIZE - 1] = '\0';

    for (int i = 0; i < 5; ++i) {
        int insert_pos = rand() % (MAX_SIZE - LEN_WORD);
        strncpy(text + insert_pos, word, LEN_WORD - 1);
    }
    
    pthread_t tid[atoi(argv[1])];
    int segment_size = MAX_SIZE / atoi(argv[1]);
    thread_data data[atoi(argv[1])];
    pthread_mutex_init(&mutex, NULL);

    auto start = std::chrono::high_resolution_clock::now();
    //заполняем инфу и создаем поток
    for (int i = 0; i < atoi(argv[1]); ++i) {
        data[i].text = text;
        data[i].word = word;
        data[i].start = segment_size * i;
        data[i].end = (i == atoi(argv[1]) - 1) ? MAX_SIZE : (i + 1) * segment_size + LEN_WORD - 2;
        if (pthread_create(&tid[i], NULL, pthread_sub_seacrh, &data[i]) != 0) {
            std::cout << "error in creating a pthread" << std::endl;
            return 0;
        }
    }

    for (int i = 0; i < atoi(argv[1]); ++i) {
        if (pthread_join(tid[i], NULL) != 0) {
            std::cout << "error in waiting for completion a pthread" << std::endl;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Execution time: " << elapsed.count() << " seconds" << std::endl;

    delete[] text;
    delete[] word;
    return 0;
    
}