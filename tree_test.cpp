#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

#include "B_tree.hpp"

int main(){
    B_tree<int, 3> t;

    std::vector<int> a;

    a.resize(50);
    std::mt19937 gen(std::random_device{}());
    std::generate(a.begin(), a.end(), [&](){
        return gen()%100;
    });

    for(auto x: a){
        t.insert(x);
    }

    std::sort(a.begin(), a.end());
    a.erase(std::unique(a.begin(), a.end()), a.end());
    for(auto x: a)
        std::cout << x << " ";
    std::cout << std::endl;

    for(int i = 0; i < 100; ++i)
        if(t.find(i))
            std::cout << i << " ";
    std::cout << std::endl;

    t.print_tree(std::cout);

    std::shuffle(a.begin(), a.end(), gen);
    size_t min_n = a.size()/2;
    for(size_t i = a.size()-1; i > min_n; --i){
        t.remove(a[i]);
        a.pop_back();
    }


    std::sort(a.begin(), a.end());
    for(auto x: a)
        std::cout << x << " ";
    std::cout << std::endl;

    for(int i = 0; i < 100; ++i)
        if(t.find(i))
            std::cout << i << " ";
    std::cout << std::endl;

    t.print_tree(std::cout);
}