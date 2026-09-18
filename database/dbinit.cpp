#include <bits/stdc++.h>
#include <chrono>
int main() {
    std::fstream filein("valid-wordle-words.txt", std::ios::in);
    std::vector<std::vector<int>>matrix(10,std::vector<int>(30,0));
    std::vector<std::string> words;
    std::string s;
    while (filein >> s)
        words.push_back(s);
    auto start = std::chrono::high_resolution_clock::now();

    for (const auto &w : words)
        for (int i = 0; i < 5; i++)
            matrix[i][w[i] - 'a']++;

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout<<duration.count()<<" us\n";
    for (int i=0;i<5;i++) {
        for (int j=0;j<26;j++) {
            if (j)std::cout<<" ";
            std::cout<<matrix[i][j];
        }
        std::cout<<std::endl;
    }
    // auto end = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    // std::cout<<duration.count()<<" us\n";
    return 0;
}