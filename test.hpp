#pragma once

#include "BinaryTree.hpp"
#include "User.hpp"
#include "error.hpp"

#include <iostream>
#include <complex>
#include <fstream>
#include <chrono>
#include <cmath>
#include <random>
#include <numeric>
#include <assert.h>



void TreeBaseOperationsTest() {
    std::cout << "Binary tree base operations tests: ";

    BinaryTree<int> tree0;

    for (int i = 10; i >= 1; --i) tree0.insert(i, i);
    for (int i = 1; i <= 10; ++i) assert(*tree0.search(i) == i);



    BinaryTree<int> tree1;

    tree1.insert(10, 100);
    tree1.insert(5, 50);
    tree1.insert(15, 150);

    assert(*tree1.search(10) == 100);
    assert(*tree1.search(5) == 50);
    assert(*tree1.search(15) == 150);
    
    assert(tree1.search(42) == nullptr);
    
    assert(tree1.getMin() == 50);
    assert(tree1.getMax() == 150);

    assert(tree1.remove(5));
    assert(tree1.search(5) == nullptr);
    assert(!tree1.remove(5));



    BinaryTree<std::string> tree2;

    tree2.insert(5, "five");
    tree2.insert(3, "three");
    tree2.insert(7, "seven");

    assert(*tree2.search(3) == "three");
    assert(*tree2.search(5) == "five");
    assert(*tree2.search(7) == "seven");
    assert(tree2.search(9) == nullptr);



    BinaryTree<Student> tree3;

    Student s1{ "Peter", 24, 234, "H98-101", true};
    Student s2{ "Nikolai", 16, 376, "G56-903", true};

    tree3.insert(1, s1);
    tree3.insert(2, s2);

    assert(*tree3.search(1) == s1);
    assert(*tree3.search(2) == s2);
    assert(tree3.search(3) == nullptr);



    BinaryTree<int> tree4;

    tree4.insert(20, 20);
    tree4.insert(10, 10);
    tree4.insert(30, 30);
    assert(tree4.remove(20));
    assert(tree4.search(20) == nullptr);
    assert(*tree4.search(10) == 10);
    assert(*tree4.search(30) == 30);



    BinaryTree<int> tree5;

    for (int i = 1; i <= 7; ++i) tree5.insert(i, i * 10);

    int depthBefore = tree5.GetDepth();
    assert(depthBefore == 7);

    tree5.balance();

    int depthAfter = tree5.GetDepth();
    assert(depthAfter < depthBefore);
    assert(depthAfter <= 3);

    for (int i = 1; i <= 7; ++i)    assert(*tree5.search(i) == i * 10);



    BinaryTree<Professor> tree6;

    Professor p1{ "Sokol Petrovich", 42, 340, "Camistry", true };
    Professor p2{ "Afon Pagasyanovic", 63, 450, "Math Analys", true };

    tree6.insert(1, p1);
    tree6.insert(2, p2);

    assert(*tree6.search(1) == p1);
    assert(*tree6.search(2) == p2);
    assert(tree6.search(3) == nullptr);



    BinaryTree<int> tree7;

    for (int i = 1; i <= 10; ++i) tree7.insert(i, i);
    for (int i = 1; i <= 10; ++i) assert(*tree7.search(i) == i);



    BinaryTree<int> tree8;

    const int N = 1000;
    for (int i = 0; i < N; ++i) tree8.insert(i, i * 2);

    for (int i = 0; i < N; i += 50)
        assert(*tree8.search(i) == i * 2);

    for (int i = 0; i < N; i += 100)
        assert(tree8.remove(i));

    for (int i = 0; i < N; i += 100)
        assert(tree8.search(i) == nullptr);

    std::cout << "Binary tree base operations tests completed successfully\n";
}

void TreeDiffOperationsTest() {
    std::cout << "Binary tree different operations tests: ";

    BinaryTree<int> tree1, tree2;

    tree1.insert(1, 10);
    tree1.insert(2, 20);
    tree2.insert(3, 30);

    BinaryTree<int> tree3 = tree1.merge(tree2);

    assert(*tree3.search(1) == 10);
    assert(*tree3.search(2) == 20);
    assert(*tree3.search(3) == 30);



    BinaryTree<int> tree4;
    tree4.insert(10, 100);
    tree4.insert(5, 50);
    tree4.insert(15, 150);
    tree4.insert(12, 120);

    BinaryTree<int> sub = tree4.extractSubtree(15);
    assert(sub.search(15));
    assert(sub.search(12));
    assert(!sub.search(10));
    assert(!sub.search(5));



    BinaryTree<int> tree6;
    std::vector<int> values;
    tree6.traverseKLP([&](const int& val) { values.push_back(val); });
    assert(values.empty());



    BinaryTree<int> tree7, tree8;
    for (int i = 0; i < 5; ++i)
        tree7.insert(i, i * 10);
    for (int i = 5; i < 10; ++i)
        tree8.insert(i, i * 10);

    BinaryTree<int> merged = tree7.merge(tree8);
    for (int i = 0; i < 10; ++i)
        assert(*merged.search(i) == i * 10);

    BinaryTree<int> subtree = merged.extractSubtree(5);
    assert(subtree.search(5));
    assert(subtree.search(6));



    BinaryTree<int> tree9;
    tree9.insert(1, 100);
    tree9.insert(1, 200);
    assert(*tree9.search(1) == 200);



    BinaryTree<int> tree10;
    tree10.insert(20, 20);
    tree10.insert(10, 10);
    tree10.insert(30, 30);

    std::string serialized = tree10.toString();


    assert(tree10.isValidTreeString(serialized));


    BinaryTree<int> new_tree = BinaryTree<int>::fromString(serialized);
    assert(new_tree == tree10);


    std::string invalid_bst = "((()25:25())20:20(()30:30()))";
    BinaryTree<int> tmp_tree;
    assert(!tmp_tree.isValidTreeString(invalid_bst));

    std::cout << "Binary tree different operations tests completed successfully\n";
}

void StressTest(const std::string& filename) {
    std::cout << "Binary tree stress test: ";

    std::ofstream file(filename);
    file << "N,InsertTimeMs,SearchTimeMs\n";

    for (int exp = 1; exp <= 7; ++exp) {
        size_t N = static_cast<size_t>(std::pow(10, exp));

        BinaryTree<int> tree;

        std::vector<int> keys(N);
        std::iota(keys.begin(), keys.end(), 0);
        std::shuffle(keys.begin(), keys.end(), std::mt19937{ std::random_device{}() });

        auto t1 = std::chrono::high_resolution_clock::now();
        for (int key : keys) {
            tree.insert(key, key);
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        double insert_time = std::chrono::duration<double, std::milli>(t2 - t1).count();

        t1 = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < std::min(N, size_t(1000)); ++i) {
            tree.search(keys[i]);
        }
        t2 = std::chrono::high_resolution_clock::now();
        double search_time = std::chrono::duration<double, std::milli>(t2 - t1).count();

        file << N << "," << insert_time << "," << search_time << "\n";
    }

    file.close();

    std::cout << "Binary tree stress test completed successfully\n";
}