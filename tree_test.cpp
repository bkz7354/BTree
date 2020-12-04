#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

#include "B_tree.hpp"


std::mt19937 gen(std::random_device{}());



template<typename T, size_t M>
void show_res(B_tree<T, M>& t, std::vector<int>& a){
    std::sort(a.begin(), a.end());
    a.erase(std::unique(a.begin(), a.end()), a.end());

    std::cout << std::endl;
    std::cout << "array: ";
    for(auto x: a)
        std::cout << x << " ";
    std::cout << std::endl;

    std::cout << "tree:  ";
    for(int i = 0; i < 100; ++i)
        if(t.find(i))
            std::cout << i << " ";
    std::cout << std::endl;

    t.print_tree(std::cout);
}

template<typename T, size_t M>
void delete_half(B_tree<T, M>& t, std::vector<int>& a){
    std::shuffle(a.begin(), a.end(), gen);
    
    int n_min = a.size()/2;
    for(size_t i = a.size()-1; i != n_min-1; --i){
        t.remove(a[i]);
        a.pop_back();
    }
}

int main(){
    B_tree<int, 3> t;

    std::vector<int> a;

    int samp_size = 50;
    a.resize(samp_size);
    std::generate(a.begin(), a.end(), [&](){
        return gen()%100;
    });

    for(auto x: a){
        t.insert(x);
    }

    show_res(t, a);

    while(a.size() > samp_size/4){
        delete_half(t, a);
        show_res(t, a);
    }
}