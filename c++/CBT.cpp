#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <queue>


template <typename T>
struct TreeNode {
    T data;
    TreeNode* left;
    TreeNode* right;

    TreeNode(const T& value) : data(value), left(nullptr), right(nullptr) {}
};

template <typename T>
class CompleteBinaryTree {
private:
    TreeNode<T>* root;

public:
    CompleteBinaryTree() : root(nullptr) {}

    void insert(const T& value) {
        TreeNode<T>* newNode = new TreeNode<T>(value);

        if (!root) {
            root = newNode;
            return;
        }

        std::queue<TreeNode<T>*> nodesQueue;
        nodesQueue.push(root);

        while (!nodesQueue.empty()) {
            TreeNode<T>* current = nodesQueue.front();
            nodesQueue.pop();

            if (!current->left) {
                current->left = newNode;
                return;
            }
            else {
                nodesQueue.push(current->left);
            }

            if (!current->right) {
                current->right = newNode;
                return;
            }
            else {
                nodesQueue.push(current->right);
            }
        }
    }

    void breadthFirstTraversal() const {
        if (!root) {
            std::cout << "The tree is empty." << std::endl;
            return;
        }

        std::queue<TreeNode<T>*> nodesQueue;
        nodesQueue.push(root);

        while (!nodesQueue.empty()) {
            TreeNode<T>* current = nodesQueue.front();
            nodesQueue.pop();

            std::cout << current->data << " ";

            if (current->left) {
                nodesQueue.push(current->left);
            }

            if (current->right) {
                nodesQueue.push(current->right);
            }
        }

        std::cout << std::endl;
    }

    std::string serializeText() const {
        std::ostringstream oss;

        serializeTextHelper(root, oss);

        return oss.str();
    }

    void serializeTextHelper(TreeNode<T>* node, std::ostringstream& oss) const {
        if (!node) {
            oss << "null ";
            return;
        }

        oss << node->data << " ";
        serializeTextHelper(node->left, oss);
        serializeTextHelper(node->right, oss);
    }

    void deserializeText(const std::string& data) {
        std::istringstream iss(data);
        root = deserializeTextHelper(iss);
    }

    TreeNode<T>* deserializeTextHelper(std::istringstream& iss) {
        std::string token;
        iss >> token;

        if (token == "null") {
            return nullptr;
        }

        T value;
        std::istringstream(token) >> value;

        TreeNode<T>* node = new TreeNode<T>(value);
        node->left = deserializeTextHelper(iss);
        node->right = deserializeTextHelper(iss);

        return node;
    }

    void serializeBinary(const std::string& filename) const {
        std::ofstream ofs(filename, std::ios::binary);

        if (ofs.is_open()) {
            serializeBinaryHelper(root, ofs);
            ofs.close();
        }
        else {
            std::cerr << "Unable to open the file for binary serialization." << std::endl;
        }
    }

    void serializeBinaryHelper(TreeNode<T>* node, std::ofstream& ofs) const {
        if (!node) {
            return;
        }

        ofs.write(reinterpret_cast<char*>(&node->data), sizeof(T));
        serializeBinaryHelper(node->left, ofs);
        serializeBinaryHelper(node->right, ofs);
    }

    void deserializeBinary(const std::string& filename) {
        std::ifstream ifs(filename, std::ios::binary);

        if (ifs.is_open()) {
            root = deserializeBinaryHelper(ifs);
            ifs.close();
        }
        else {
            std::cerr << "Unable to open the file for binary deserialization." << std::endl;
        }
    }

    TreeNode<T>* deserializeBinaryHelper(std::ifstream& ifs) {
        T value;
        ifs.read(reinterpret_cast<char*>(&value), sizeof(T));

        if (!ifs) {
            return nullptr;
        }

        TreeNode<T>* node = new TreeNode<T>(value);
        node->left = deserializeBinaryHelper(ifs);
        node->right = deserializeBinaryHelper(ifs);

        return node;
    }

};

TEST(CompleteBinaryTreeTest, InsertAndBreadthFirstTraversal) {
    CompleteBinaryTree<int> myTree;

    myTree.insert(1);
    myTree.insert(2);
    myTree.insert(3);
    myTree.insert(4);
    myTree.insert(5);

    testing::internal::CaptureStdout();
    myTree.breadthFirstTraversal();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output, "1 2 3 4 5 \n");
}

TEST(CompleteBinaryTreeTest, EmptyTreeTraversal) {
    CompleteBinaryTree<int> emptyTree;

    testing::internal::CaptureStdout();
    emptyTree.breadthFirstTraversal();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output, "The tree is empty.\n");
}

static void BM_Insert(benchmark::State& state) {
    CompleteBinaryTree<int> myTree;

    for (auto _ : state) {
        myTree.insert(42);
    }
}
BENCHMARK(BM_Insert);

BENCHMARK_MAIN();

int main(int argc, char** argv) {
    CompleteBinaryTree<int> myTree;
    myTree.insert(1);
    myTree.insert(2);
    myTree.insert(3);
    myTree.insert(4);
    myTree.insert(5);

    std::cout << "Breadth First Traversal: ";
    myTree.breadthFirstTraversal();

    std::string textData = myTree.serializeText();
    std::cout << "Text Serialization: " << textData << std::endl;

    myTree.deserializeText(textData);

    std::cout << "After Text Deserialization: ";
    myTree.breadthFirstTraversal();

    myTree.serializeBinary("binary_tree_data.bin");

    CompleteBinaryTree<int> newTree;

    newTree.deserializeBinary("binary_tree_data.bin");

    std::cout << "After Binary Deserialization: ";
    newTree.breadthFirstTraversal();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}