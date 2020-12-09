#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <set>
#include <fstream>

#include "B_tree.hpp"

namespace chr = std::chrono;

struct timer{
    using clock = chr::high_resolution_clock;
    chr::time_point<clock> mem;

    void start(){
        mem = clock::now();
    }
    double get(){
        chr::duration<double, std::milli> d = clock::now() - mem;
        return d.count();
    }
};

std::vector<int> random_vec(const int sz){
    static std::mt19937 gen(std::random_device{}());

    std::vector<int> res(sz);
    std::generate(res.begin(), res.end(), gen);
    return res;
}

void shuffle_vec(std::vector<int> &v){
    static std::mt19937 gen(std::random_device{}());

    std::shuffle(v.begin(), v.end(), gen);
}

template <class Tree>
double run_insert_test(int n){
    auto vec = random_vec(n);
    timer tim;
    Tree t;

    tim.start();
    for (int i = 0; i < vec.size(); ++i)
        t.insert(vec[i]);

    return tim.get();
}

template <class Tree>
std::vector<double>
test_insert_speed(std::vector<int> cnt, std::vector<int> rep){
    std::vector<double> res;

    for (int i = 0; i < cnt.size(); ++i){
        std::cout << "testing insert speed " << i << "/" << cnt.size() << "\r";
        std::cout.flush();

        double total = 0;
        for (int j = 0; j < rep[i]; ++j)
            total += run_insert_test<Tree>(cnt[i]);

        res.push_back(total / rep[i] / cnt[i]);
    }
    std::cout << "testing insert speed " << cnt.size() << "/" << cnt.size() << std::endl;

    return res;
}

template <class Tree>
double run_delete_test(int n){
    auto vec = random_vec(n);
    timer tim;
    Tree t;

    for (int i = 0; i < vec.size(); ++i)
        t.insert(vec[i]);
    shuffle_vec(vec);

    tim.start();
    for (int i = 0; i < vec.size(); ++i)
        t.remove(vec[i]);

    return tim.get();
}

template <class Tree>
std::vector<double> test_delete_speed(std::vector<int> cnt, std::vector<int> rep){
    std::vector<double> res;

    for (int i = 0; i < cnt.size(); ++i){
        std::cout << "testing delete speed " << i << "/" << cnt.size() << "\r";
        std::cout.flush();

        double total = 0;
        for (int j = 0; j < rep[i]; ++j)
            total += run_delete_test<Tree>(cnt[i]);

        res.push_back(total / rep[i] / cnt[i]);
    }
    std::cout << "testing delete speed " << cnt.size() << "/" << cnt.size() << std::endl;

    return res;
}

const std::vector<int> test_cnt{
    25000, 50000, 100000, 150000, 200000, 250000, 300000, 450000, 400000, 450000, 500000};
const std::vector<int> test_rep{
    20, 15, 15, 10, 5, 5, 5, 5, 4, 4, 4};

template <class Tree>
void test_tree(std::string name, std::ofstream &os){
    std::cout << "testing " << name << std::endl;
    os << name << std::endl;

    auto vec = test_insert_speed<Tree>(test_cnt, test_rep);
    for (auto x : vec)
        os << x * 1000000 << " ";
    os << std::endl;

    vec = test_delete_speed<Tree>(test_cnt, test_rep);
    for (auto x : vec)
        os << x * 1000000 << " ";
    os << std::endl
       << std::endl;
}

int main(int argc, char **argv){
    if (argc < 2){
        std::cerr << "syntax: ./tree_test <output filename>" << std::endl;
        return 1;
    }

    try{
        std::ofstream os(argv[1]);
        for (auto x : test_cnt)
            os << x << " ";
        os << std::endl
           << std::endl;

        test_tree<B_tree<int, 10>>("t = 5", os);
        test_tree<B_tree<int, 50>>("t = 50", os);
        test_tree<B_tree<int, 100>>("t = 100", os);
        test_tree<B_tree<int, 500>>("t = 500", os);
    }
    catch (const std::exception &e){
        std::cerr << e.what() << std::endl;
        return 1;
    }
}