#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <complex>
#include <functional>
#include "BinaryTree.hpp"
#include "User.hpp"
#include "error.hpp"
#include <random>
#include <unordered_set>

BinaryTree<int> RandomTree(int nodes, int depth) {
    if (nodes > 1001)
        throw std::runtime_error("Too many nodes: can't generate more than 1001 unique keys in range [0, 1000]");

    BinaryTree<int> rand_tree;
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, 1000);
    std::unordered_set<int> unique_keys;

    while (static_cast<int>(unique_keys.size()) < nodes) {
        unique_keys.insert(dist(rng));
    }

    std::vector<int> keys(unique_keys.begin(), unique_keys.end());
    std::shuffle(keys.begin(), keys.end(), rng);

    for (int key : keys) {
        rand_tree.insert(key, key);
        if (rand_tree.GetDepth() > depth) {
            rand_tree.balance();
        }
    }

    return rand_tree;
}


void ClearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int GetInt(const std::string& prompt = "") {
    int v;
    if (!prompt.empty()) std::cout << prompt;
    if (!(std::cin >> v)) {
        ClearInput();
        throw Errors::InvalidArgument();
    }
    return v;
}

template<typename T>
T GetTyped(const std::string& prompt = "Enter value: ") {
    std::cout << prompt;
    T val;
    if (!(std::cin >> val)) {
        ClearInput();
        throw Errors::InvalidArgument();
    }
    return val;
}

template<>
std::complex<double> GetTyped<std::complex<double>>(const std::string& prompt) {
    std::cout << prompt << " (real and imaginary part): ";
    double re, im;
    std::cin >> re >> im;
    return { re, im };
}

template<>
Student GetTyped<Student>(const std::string& prompt) {
    Student s;
    std::cin >> s;
    return s;
}

template<>
Professor GetTyped<Professor>(const std::string& prompt) {
    Professor t;
    std::cin >> t;
    return t;
}

template<>
std::function<double(double)> GetTyped<std::function<double(double)>>(const std::string& prompt) {
    std::cout << prompt << " (select function)\n1. sin(x)\n2. x^2\n3. exp(x)\nChoice: ";
    int choice = GetInt();
    if (choice == 1) return [](double x) { return sin(x); };
    if (choice == 2) return [](double x) { return x * x; };
    if (choice == 3) return [](double x) { return exp(x); };
    throw Errors::InvalidArgument("Invalid function selection");
}

struct ITreeWrapper {
    virtual ~ITreeWrapper() = default;
    virtual std::string TypeName() const = 0;
    virtual void Menu(std::vector<ITreeWrapper*>&, std::vector<std::string>&) = 0;
};

template<typename T>
class TreeWrapper : public ITreeWrapper {
public:
    BinaryTree<T> tree;
    std::string typeName;

    TreeWrapper(std::string typeName_) : typeName(std::move(typeName_)) {}

    std::string TypeName() const override { return typeName; }

    void Menu(std::vector<ITreeWrapper*>& globalTrees, std::vector<std::string>& typeRegistry) override {
        while (true) {
            std::cout << "\n--- Tree Menu (" << typeName << ") ---\n"
                << "1. Insert\n2. Search\n3. Min\n4. Max\n5. Remove\n"
                << "6. Traverse (KLP)\n7. Merge with another\n8. Extract Subtree\n"
                << "9. Balance Tree\n10. Print tree\n11. Serialize\n12. Back\n"
                << "Choose: ";
            try {
                int ch = GetInt();
                switch (ch) {
                case 1: {
                    int key;
                    T val = GetTyped<T>("Value: ");
                    if constexpr (std::is_same_v<T, int>) {
                        key = val;
                    }
                    else {
                        key = GetInt("Key: ");
                    }
                    tree.insert(key, val);
                    break;
                }
                case 2: { 
                    int key = GetInt("Key: ");
                    T* found = tree.search(key);
                    if (found) {
                        if constexpr (std::is_same_v<T, std::function<double(double)>>)
                            std::cout << "Found: f(1.0) = " << (*found)(1.0) << "\n";
                        else std::cout << "Found: " << *found << "\n";
                    }
                    else std::cout << "Not found\n";
                    break;
                }
                case 3: { 
                    if constexpr (std::is_same_v<T, std::function<double(double)>>)
                        std::cout << "Min: f(1.0) = " << tree.getMin()(1.0) << "\n";
                    else std::cout << "Min: " << tree.getMin() << "\n";
                    break;
                }
                case 4: { 
                    if constexpr (std::is_same_v<T, std::function<double(double)>>)
                        std::cout << "Max: f(1.0) = " << tree.getMax()(1.0) << "\n";
                    else std::cout << "Max: " << tree.getMax() << "\n";
                    break;
                }
                case 5: { 
                    int key = GetInt("Key: ");
                    std::cout << (tree.remove(key) ? "Removed.\n" : "Key not found.\n");
                    break;
                }
                case 6: { 
                    std::cout << "KLP traversal:\n";
                    if constexpr (std::is_same_v<T, std::function<double(double)>>)
                        tree.traverseKLP([](const T& f) { std::cout << "f(1.0)=" << f(1.0) << " "; });
                    else tree.traverseKLP([](const T& x) { std::cout << x << " "; });
                    std::cout << "\n";
                    break;
                }
                case 7: { 
                    std::cout << "Available tree indices: ";
                    for (size_t i = 0; i < globalTrees.size(); ++i)
                        std::cout << i << " ";
                    std::cout << "\n";
                    int idx = GetInt("Index of tree to merge with: ");
                    if (idx < 0 || static_cast<size_t>(idx) >= globalTrees.size()) throw Errors::IndexOutOfRange();
                    auto* other = dynamic_cast<TreeWrapper<T>*>(globalTrees[idx]);
                    if (!other) throw Errors::ConcatTypeMismatchError();
                    auto* result = new TreeWrapper<T>("merged_" + typeName);
                    result->tree = this->tree.merge(other->tree);
                    globalTrees.push_back(result);
                    typeRegistry.push_back(typeName);
                    std::cout << "Merged tree added as index " << globalTrees.size() - 1 << "\n";
                    break;
                }
                case 8: { 
                    int key = GetInt("Key for subtree root: ");
                    auto* subtree = new TreeWrapper<T>("subtree_" + typeName);
                    subtree->tree = this->tree.extractSubtree(key);
                    globalTrees.push_back(subtree);
                    typeRegistry.push_back(typeName);
                    std::cout << "Subtree added as index " << globalTrees.size() - 1 << "\n";
                    break;
                }
                case 9: {
                    tree.balance();
                    std::cout << "Tree balanced.\n";
                    break;
                }
                case 10: { 
                    if constexpr (std::is_same_v<T, std::function<double(double)>>)
                        tree.traverseKLP([](const T& f) { std::cout << "f(1.0)=" << f(1.0) << " "; });
                    else { tree.PrintTree(); }
                    break;
                }
                case 11: { 
                    try {
                        std::string serialized = tree.toString();
                        std::cout << "Serialized tree: " << serialized << "\n";
                    }
                    catch (const std::exception& e) {
                        std::cout << "Serialization error: " << e.what() << "\n";
                    }
                    break;
                }
                case 12: return;

                default: std::cout << "Invalid option.\n";
                }
            }
            catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << "\n";
            }
        }
    }
};

void ShowTypeMenu() {
    std::cout << "Choose data type:\n"
        << "1. int\n2. double\n3. string\n4. complex\n"
        << "5. student\n6. professor\nChoice: ";
}

void Run() {
    std::vector<ITreeWrapper*> trees;
    std::vector<std::string> treeTypes;

    while (true) {
        std::cout << "\n=== Main Menu ===\n"
            << "1. Add Tree\n2. List Trees\n3. Work With Tree\n4. Delete Tree\n5. Random tree\n6. String to tree\n7. Recovery tree by two traverses\n8. Exit\nChoose: ";
        try {
            int ch = GetInt();
            switch (ch) {
            case 1: { 
                ShowTypeMenu();
                int t = GetInt();
                switch (t) {
                case 1: trees.push_back(new TreeWrapper<int>("int")); break;
                case 2: trees.push_back(new TreeWrapper<double>("double")); break;
                case 3: trees.push_back(new TreeWrapper<std::string>("string")); break;
                case 4: trees.push_back(new TreeWrapper<std::complex<double>>("complex")); break;
                //case 5: trees.push_back(new TreeWrapper<std::function<double(double)>>("function")); break;
                case 5: trees.push_back(new TreeWrapper<Student>("Student")); break;
                case 6: trees.push_back(new TreeWrapper<Professor>("Professor")); break;
                default: throw Errors::InvalidArgument();
                }
                treeTypes.push_back(trees.back()->TypeName());
                std::cout << "Tree created. Index: " << trees.size() - 1 << "\n";
                break;
            }
            case 2: { 
                if (trees.empty()) {
                    std::cout << "No trees yet.\n";
                    break;
                }
                for (size_t i = 0; i < trees.size(); ++i)
                    std::cout << i << ": Tree<" << trees[i]->TypeName() << ">\n";
                break;
            }
            case 3: {
                if (trees.empty()) {
                    std::cout << "No trees yet.\n";
                    break;
                }
                std::cout << "Available tree indices: ";
                for (size_t i = 0; i < trees.size(); ++i) std::cout << i << " ";
                std::cout << "\n";
                int idx = GetInt("Tree index: ");
                if (idx < 0 || static_cast<size_t>(idx) >= trees.size()) throw Errors::IndexOutOfRange();
                trees[idx]->Menu(trees, treeTypes);
                break;
            }
            case 4: {
                if (trees.empty()) {
                    std::cout << "No trees yet.\n";
                    break;
                }
                std::cout << "Available tree indices: ";
                for (size_t i = 0; i < trees.size(); ++i) std::cout << i << " ";
                std::cout << "\n";
                int idx = GetInt("Index to remove: ");
                if (idx < 0 || static_cast<size_t>(idx) >= trees.size()) throw Errors::IndexOutOfRange();
                delete trees[idx];
                trees.erase(trees.begin() + idx);
                treeTypes.erase(treeTypes.begin() + idx);
                std::cout << "Tree deleted.\n";
                break;
            }
            case 5: {
                int nodes = GetInt("Number of nodes: ");
                int depth = GetInt("Max depth: ");
                if (nodes <= 0 || depth <= 0) throw Errors::InvalidArgument("Nodes and depth must be positive.");
                BinaryTree<int> rand_tree = RandomTree(nodes, depth);
                auto* wrapper = new TreeWrapper<int>("RandomTree");
                wrapper->tree = std::move(rand_tree);
                trees.push_back(wrapper);
                treeTypes.push_back("int");
                std::cout << "Random tree created with " << nodes << " nodes and max depth " << depth << ".\n";
                std::cout << "Random tree added as index " << trees.size() - 1 << "\n";
                break;
            }
            case 6: {
                ShowTypeMenu();
                int t = GetInt("Choose tree type: ");
                std::cout << "Enter serialized tree (format: (()key:value())): ";
                std::cin.ignore();
                std::string input;
                std::getline(std::cin, input);

                try {
                    switch (t) {
                    case 1: {
                        auto tree = BinaryTree<int>::fromString(input);
                        auto* wrapper = new TreeWrapper<int>("int");
                        wrapper->tree = std::move(tree);
                        trees.push_back(wrapper);
                        treeTypes.push_back("int");
                        break;
                    }
                    case 2: {
                        auto tree = BinaryTree<double>::fromString(input);
                        auto* wrapper = new TreeWrapper<double>("double");
                        wrapper->tree = std::move(tree);
                        trees.push_back(wrapper);
                        treeTypes.push_back("double");
                        break;
                    }
                    case 3: {
                        auto tree = BinaryTree<std::string>::fromString(input);
                        auto* wrapper = new TreeWrapper<std::string>("string");
                        wrapper->tree = std::move(tree);
                        trees.push_back(wrapper);
                        treeTypes.push_back("string");
                        break;
                    }
                    case 4: {
                        auto tree = BinaryTree<std::complex<double>>::fromString(input);
                        auto* wrapper = new TreeWrapper<std::complex<double>>("complex");
                        wrapper->tree = std::move(tree);
                        trees.push_back(wrapper);
                        treeTypes.push_back("complex");
                        break;
                    }
                    case 5: {
                        std::cout << "Deserialization of function trees is not supported.\n";
                        break;
                    }
                    case 6: {
                        auto tree = BinaryTree<Student>::fromString(input);
                        auto* wrapper = new TreeWrapper<Student>("Student");
                        wrapper->tree = std::move(tree);
                        trees.push_back(wrapper);
                        treeTypes.push_back("Student");
                        break;
                    }
                    case 7: {
                        auto tree = BinaryTree<Professor>::fromString(input);
                        auto* wrapper = new TreeWrapper<Professor>("Teacher");
                        wrapper->tree = std::move(tree);
                        trees.push_back(wrapper);
                        treeTypes.push_back("Teacher");
                        break;
                    }
                    default:
                        throw Errors::InvalidArgument("Unknown data type");
                    }
                    std::cout << "Tree successfully deserialized. Index: " << trees.size() - 1 << "\n";
                }
                catch (const std::exception& e) {
                    std::cout << "Deserialization error: " << e.what() << "\n";
                }
                break;
            }
            case 7: {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                std::cout << "Enter KLP traverse (format: value value value): ";
                std::string traverseKLP;
                std::getline(std::cin, traverseKLP);

                std::cout << "Enter LKP traverse (format: value value value): ";
                std::string traverseLKP;
                std::getline(std::cin, traverseLKP);

                try {
                    auto tree = BinaryTree<int>::recoveryTree(traverseKLP, traverseLKP);
                    auto* wrapper = new TreeWrapper<int>("int");
                    wrapper->tree = std::move(tree);
                    trees.push_back(wrapper);
                    treeTypes.push_back("int");
                }
                catch (const std::exception& e) {
                    std::cout << "Error: " << e.what() << "\n";
                }
                break;
            }
            case 8: {
                for (auto* ptr : trees) delete ptr;
                std::cout << "Exiting...\n";
                return;
            }
            default: std::cout << "Unknown option.\n";
            }
        }
        catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }
}