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
class Queue {
private:
    Node<T>* front;
    Node<T>* rear;

public:
    Queue() : front(nullptr), rear(nullptr) {}


    void push(const T& value) {
        Node<T>* newNode = new Node<T>(value);

        if (isEmpty()) {
            front = rear = newNode;
        }
        else {
            rear->next = newNode;
            rear = newNode;
        }
    }


    void pop() {
        if (isEmpty()) {
            std::cerr << "The queue is empty. The dequeue() operation cannot be performed." << std::endl;
            return;
        }

        Node<T>* temp = front;
        front = front->next;
        delete temp;

        if (isEmpty()) {
            rear = nullptr;
        }
    }

    T read() const {
        if (isEmpty()) {
            std::cerr << "The queue is empty. The peek() operation cannot be performed." << std::endl;
            return T();
        }

        return front->data;
    }

    bool isEmpty() const {
        return front == nullptr;
    }
    std::string serializeText() const {
        std::ostringstream oss;
        Node<T>* current = front;

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
            Node<T>* current = front;

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
    void display() const {
        Node<T>* current = front;
        while (current != nullptr) {
            std::cout << current->data << " ";
            current = current->next;
        }
        std::cout << std::endl;
    }

};

TEST(StackTest, PushAndPop) {
    Queue<int> myQueue;

    ASSERT_TRUE(myQueue.isEmpty());

    myQueue.push(1);
    myQueue.push(2);

    ASSERT_EQ(myQueue.read(), 1);

    myQueue.pop();

    ASSERT_EQ(myQueue.read(), 2);
}

TEST(StackTest, PeekEmptyStack) {
    Queue<int> myQueue;
    ASSERT_EQ(myQueue.read(), 0);
}

TEST(StackTest, PopEmptyStack) {
    Queue<int> myQueue;

    ASSERT_NO_THROW(myQueue.pop());
}

static void BM_Push(benchmark::State& state) {
    Queue<int> myQueue;

    for (auto _ : state) {
        myQueue.push(42);
    }
}
BENCHMARK(BM_Push);

BENCHMARK_MAIN();

int main(int argc, char** argv) {
    Queue<int> myQueue;
    myQueue.push(1);
    myQueue.push(2);
    myQueue.push(3);

    std::string textData = myQueue.serializeText();
    std::cout << "Text Serialization: " << textData << std::endl;

    myQueue.deserializeText(textData);
    std::cout << "After Text Deserialization: ";
    myQueue.display();

    myQueue.serializeBinary("binary_data_queue.bin");

    Queue<int> newQueue;

    newQueue.deserializeBinary("binary_data_queue.bin");

    std::cout << "After Binary Deserialization: ";
    newQueue.display();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

