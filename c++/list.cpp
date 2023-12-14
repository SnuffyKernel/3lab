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
class List {
private:
    Node<T>* head;

public:
    List() : head(nullptr) {}

    void push(const T& value) {
        Node<T>* newNode = new Node<T>(value);

        if (head == nullptr) {
            head = newNode;
        }
        else {
            Node<T>* current = head;
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newNode;
        }
    }

    std::string serializeText() const {
        std::ostringstream oss;
        Node<T>* current = head;

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
            Node<T>* current = head;

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
        Node<T>* current = head;
        while (current != nullptr) {
            std::cout << current->data << " ";
            current = current->next;
        }
        std::cout << std::endl;
    }
};

TEST(ListTest, PushAndPrint) {
    List<int> myList;

    myList.push(1);
    myList.push(2);

    std::string textData = myList.serializeText();
    std::cout << "Text Serialization: " << textData << std::endl;

    myList.deserializeText(textData);
    std::cout << "After Text Deserialization: ";
    myList.print();

    myList.serializeBinary("binary_data_list.bin");

    List<int> newList;

    newList.deserializeBinary("binary_data_list.bin");

    std::cout << "After Binary Deserialization: ";
    newList.print();
}

TEST(ListTest, PrintEmptyList) {
    List<int> myList;
    std::cout << "Empty List: ";
    myList.print();
}

static void BM_Push(benchmark::State& state) {
    List<int> myList;

    for (auto _ : state) {
        myList.push(42); 
    }
}
BENCHMARK(BM_Push);

BENCHMARK_MAIN();

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}