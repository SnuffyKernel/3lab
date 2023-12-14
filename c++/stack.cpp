#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <fstream>

template <typename T>
struct Node {
    T data;
    Node* next;

    Node(const T& value) : data(value), next(nullptr) {}
};

template <typename T>
class Stack {
private:
    Node<T>* top;

public:
    Stack() : top(nullptr) {}

    void push(const T& value) {
        Node<T>* newNode = new Node<T>(value);
        newNode->next = top;
        top = newNode;
    }

    void pop() {
        if (isEmpty()) {
            std::cerr << "The stack is empty. The pop() operation cannot be performed." << std::endl;
            return;
        }

        Node<T>* temp = top;
        top = top->next;
        delete temp;
    }

    T read() const {
        if (isEmpty()) {
            std::cerr << "The stack is empty. The peek() operation cannot be performed." << std::endl;
            return T();
        }

        return top->data;
    }

    bool isEmpty() const {
        return top == nullptr;
    }

    std::string serializeText() const {
        std::ostringstream oss;
        Node<T>* current = top;

        while (current != nullptr) {
            oss << current->data << " ";
            current = current->next;
        }

        return oss.str();
    }

    void deserializeText(const std::string& data) {

        std::istringstream iss(data);
        T value;

        while (iss >> value) {
            push(value);
        }
    }

    void serializeBinary(const std::string& filename) const {
        std::ofstream ofs(filename, std::ios::binary);

        if (ofs.is_open()) {
            Node<T>* current = top;

            while (current != nullptr) {
                ofs.write(reinterpret_cast<char*>(&current->data), sizeof(T));
                current = current->next;
            }

            ofs.close();
        }
        else {
            std::cerr << "Unable to open the file for binary serialization." << std::endl;
        }
    }

    void deserializeBinary(const std::string& filename) {

        std::ifstream ifs(filename, std::ios::binary);

        if (ifs.is_open()) {
            T value;

            while (ifs.read(reinterpret_cast<char*>(&value), sizeof(T))) {
                push(value);
            }

            ifs.close();
        }
        else {
            std::cerr << "Unable to open the file for binary deserialization." << std::endl;
        }
    }

    void print() const {
        Node<T>* current = top;
        while (current != nullptr) {
            std::cout << current->data << " ";
            current = current->next;
        }
        std::cout << std::endl;
    }
};
TEST(StackTest, PushAndPop) {
    Stack<int> myStack;

    ASSERT_TRUE(myStack.isEmpty());

    myStack.push(1);
    myStack.push(2);

    ASSERT_EQ(myStack.read(), 2);

    myStack.pop();

    ASSERT_EQ(myStack.read(), 1);
}

TEST(StackTest, PeekEmptyStack) {
    Stack<int> myStack;
    ASSERT_EQ(myStack.read(), 0);
}

TEST(StackTest, PopEmptyStack) {
    Stack<int> myStack;

    ASSERT_NO_THROW(myStack.pop());
}

static void BM_Push(benchmark::State& state) {
    Stack<int> myStack;

    for (auto _ : state) {
        myStack.push(42);
    }
}
BENCHMARK(BM_Push);

BENCHMARK_MAIN();

int main(int argc, char** argv) {
    Stack<int> myStack;
    myStack.push(1);
    myStack.push(2);
    myStack.push(3);

    std::string textData = myStack.serializeText();
    std::cout << "Text Serialization: " << textData << std::endl;

    myStack.deserializeText(textData);
    std::cout << "After Text Deserialization: ";
    myStack.print();


    myStack.serializeBinary("binary_data.bin");
    Stack<int> newStack;
    newStack.deserializeBinary("binary_data.bin");
    std::cout << "After Binary Deserialization: ";
    newStack.print();

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


