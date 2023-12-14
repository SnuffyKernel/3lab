#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>


template <typename Key, typename Value>
class HashTable {
private:
    static const size_t TABLE_SIZE = 10;

    struct HashNode {
        Key key;
        Value value;
        bool occupied;

        HashNode() : occupied(false) {}
    };

    std::vector<HashNode> table;

    size_t hashFunction(const Key& key) const {
        return std::hash<Key>{}(key) % TABLE_SIZE;
    }

public:
    HashTable() : table(TABLE_SIZE) {}

    void insert(const Key& key, const Value& value) {
        size_t index = hashFunction(key);

        while (table[index].occupied && table[index].key != key) {
            index = (index + 1) % TABLE_SIZE;
        }

        table[index].key = key;
        table[index].value = value;
        table[index].occupied = true;
    }

    void remove(const Key& key) {
        size_t index = hashFunction(key);

        while (table[index].occupied) {
            if (table[index].key == key) {
                table[index].occupied = false;
                return;
            }

            index = (index + 1) % TABLE_SIZE;
        }

        std::cerr << "An element with a key " << key << " not found." << std::endl;
    }

    Value get(const Key& key) const {
        size_t index = hashFunction(key);

        while (table[index].occupied) {
            if (table[index].key == key) {
                return table[index].value;
            }

            index = (index + 1) % TABLE_SIZE;
        }

        std::cerr << "An element with a key " << key << " not found." << std::endl;
        return Value();
    }

    std::string serializeText() const {
        std::ostringstream oss;

        for (const auto& node : table) {
            if (node.occupied) {
                oss << node.key << ":" << node.value << " ";
            }
        }

        return oss.str();
    }

    void deserializeText(const std::string& data) {

        std::istringstream iss(data);
        std::string keyValue;

        while (iss >> keyValue) {
            size_t delimiterPos = keyValue.find(':');
            if (delimiterPos != std::string::npos) {
                Key key = keyValue.substr(0, delimiterPos);
                Value value = std::stoi(keyValue.substr(delimiterPos + 1));
                insert(key, value);
            }
        }
    }

    void serializeBinary(const std::string& filename) const {
        std::ofstream ofs(filename, std::ios::binary);

        if (ofs.is_open()) {
            for (const auto& node : table) {
                if (node.occupied) {
                    ofs.write(reinterpret_cast<const char*>(&node.key), sizeof(Key));
                    ofs.write(reinterpret_cast<const char*>(&node.value), sizeof(Value));
                }
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
            while (ifs) {
                Key key;
                Value value;

                ifs.read(reinterpret_cast<char*>(&key), sizeof(Key));
                ifs.read(reinterpret_cast<char*>(&value), sizeof(Value));

                if (ifs) {
                    insert(key, value);
                }
            }

            ifs.close();
        }
        else {
            std::cerr << "Unable to open the file for binary deserialization." << std::endl;
        }
    }

};

TEST(HashTableTest, InsertAndRetrieve) {
    HashTable<std::string, int> myHashTable;

    myHashTable.insert("one", 1);
    myHashTable.insert("two", 2);

    EXPECT_EQ(myHashTable.get("one"), 1);
    EXPECT_EQ(myHashTable.get("two"), 2);
}

TEST(HashTableTest, Remove) {
    HashTable<std::string, int> myHashTable;

    myHashTable.insert("one", 1);
    myHashTable.insert("two", 2);

    myHashTable.remove("one");

    EXPECT_EQ(myHashTable.get("one"), 0);
    EXPECT_EQ(myHashTable.get("two"), 2);
}

TEST(HashTableTest, GetNonExistentKey) {
    HashTable<std::string, int> myHashTable;

    EXPECT_EQ(myHashTable.get("nonexistent"), 0);
}

static void BM_Insert(benchmark::State& state) {
    HashTable<std::string, int> myHashTable;

    for (auto _ : state) {
        myHashTable.insert("key", 42); 
    }
}
BENCHMARK(BM_Insert);

static void BM_Get(benchmark::State& state) {
    HashTable<std::string, int> myHashTable;
    myHashTable.insert("key", 42);

    for (auto _ : state) {
        int value = myHashTable.get("key");
        benchmark::DoNotOptimize(value);
    }
}
BENCHMARK(BM_Get);

BENCHMARK_MAIN();

int main(int argc, char** argv) {
    HashTable<std::string, int> myHashTable;

    myHashTable.insert("one", 1);
    myHashTable.insert("two", 2);

    std::string textData = myHashTable.serializeText();
    std::cout << "Text Serialization: " << textData << std::endl;

    myHashTable.deserializeText(textData);

    std::string binaryFilename = "binary_file.bin";
    myHashTable.serializeBinary(binaryFilename);

    HashTable<std::string, int> newHashTable;
    newHashTable.deserializeBinary(binaryFilename);

    std::cout << "After Deserialization:" << std::endl;
    std::cout << "one: " << newHashTable.get("one") << std::endl;
    std::cout << "two: " << newHashTable.get("two") << std::endl;

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}