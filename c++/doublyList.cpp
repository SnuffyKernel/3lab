#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <fstream>

template <typename T>
struct Node {
    T data;
    Node* prev;
    Node* next;

    Node(const T& value) : data(value), prev(nullptr), next(nullptr) {}
};

template <typename T>
class DoublyList {
private:
    Node<T>* head;

public:
    DoublyList() : head(nullptr) {}

    void append(const T& value) {
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
            newNode->prev = current;
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
            append(value);
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
                append(value);
            }

            ifs.close();
        }
        else {
            std::cerr << "Unable to open the file for binary deserialization." << std::endl;
        }
    }

    void display() const {
        Node<T>* current = head;
        while (current != nullptr) {
            std::cout << current->data << " ";
            current = current->next;
        }
        std::cout << std::endl;
    }
};


TEST(DoublyListTest, SerializeAndDeserializeText) {
    DoublyList<int> myList;

    myList.append(1);
    myList.append(2);
    myList.append(3);

    std::string serializedData = myList.serializeText();

    DoublyList<int> newList;
    newList.deserializeText(serializedData);

    testing::internal::CaptureStdout();
    newList.display();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output, "1 2 3 \n");
}

TEST(DoublyListTest, SerializeAndDeserializeBinary) {
    DoublyList<int> myList;

    myList.append(1);
    myList.append(2);
    myList.append(3);

    myList.serializeBinary("binary_data.bin");

    DoublyList<int> newList;
    newList.deserializeBinary("binary_data.bin");

    testing::internal::CaptureStdout();
    newList.display();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output, "1 2 3 \n");
}

static void BM_Append(benchmark::State& state) {
    DoublyList<int> myList;

    for (auto _ : state) {
        myList.append(42);
    }
}
BENCHMARK(BM_Append);

static void BM_SerializeText(benchmark::State& state) {
    DoublyList<int> myList;

    myList.append(1);
    myList.append(2);
    myList.append(3);

    for (auto _ : state) {
        std::string serializedData = myList.serializeText();
        benchmark::DoNotOptimize(serializedData);
    }
}
BENCHMARK(BM_SerializeText);

static void BM_DeserializeText(benchmark::State& state) {
    DoublyList<int> myList;

    myList.append(1);
    myList.append(2);
    myList.append(3);

    std::string serializedData = myList.serializeText();

    for (auto _ : state) {
        DoublyList<int> newList;
        newList.deserializeText(serializedData);
        benchmark::DoNotOptimize(newList);
    }
}
BENCHMARK(BM_DeserializeText);

static void BM_SerializeBinary(benchmark::State& state) {
    DoublyList<int> myList;

    myList.append(1);
    myList.append(2);
    myList.append(3);

    for (auto _ : state) {
        myList.serializeBinary("binary_data.bin");
    }
}
BENCHMARK(BM_SerializeBinary);

static void BM_DeserializeBinary(benchmark::State& state) {
    DoublyList<int> myList;

    myList.append(1);
    myList.append(2);
    myList.append(3);

    myList.serializeBinary("binary_data.bin");

    for (auto _ : state) {
        DoublyList<int> newList;
        newList.deserializeBinary("binary_data.bin");
        benchmark::DoNotOptimize(newList);
    }
}
BENCHMARK(BM_DeserializeBinary);

BENCHMARK_MAIN();

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}