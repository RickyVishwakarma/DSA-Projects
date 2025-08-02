#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm> // For std::swap

// Using the standard namespace to avoid prefixing with "std::"
using namespace std;

// --- Data Structures ---

// Node for the Huffman Tree using raw pointers
struct HuffmanNode {
    char data;
    int frequency;
    HuffmanNode *left;
    HuffmanNode *right;
};

// Custom Min-Heap structure, similar to the user's provided code
struct MinHeap {
    int size;
    int capacity;
    HuffmanNode** array;
};

// --- Custom Min-Heap Functions ---

/**
 * @brief Creates a new Huffman tree node.
 * @param data The character for the node.
 * @param frequency The frequency of the character.
 * @return A pointer to the new node.
 */
HuffmanNode* newNode(char data, int frequency) {
    HuffmanNode* temp = new HuffmanNode();
    temp->left = temp->right = nullptr;
    temp->data = data;
    temp->frequency = frequency;
    return temp;
}

/**
 * @brief Creates a Min-Heap of a given capacity.
 * @param capacity The maximum capacity of the heap.
 * @return A pointer to the new MinHeap.
 */
MinHeap* createMinHeap(int capacity) {
    MinHeap* minHeap = new MinHeap();
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = new HuffmanNode*[minHeap->capacity];
    return minHeap;
}

/**
 * @brief Swaps two HuffmanNode pointers.
 * @param a Pointer to the first node pointer.
 * @param b Pointer to the second node pointer.
 */
void swapHuffmanNode(HuffmanNode** a, HuffmanNode** b) {
    HuffmanNode* t = *a;
    *a = *b;
    *b = t;
}

/**
 * @brief The standard min-heapify function.
 * @param minHeap The heap to heapify.
 * @param idx The index to start heapifying from.
 */
void minHeapify(MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->frequency < minHeap->array[smallest]->frequency)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->frequency < minHeap->array[smallest]->frequency)
        smallest = right;

    if (smallest != idx) {
        swapHuffmanNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

/**
 * @brief Checks if the heap has only one node.
 * @param minHeap The heap to check.
 * @return True if size is 1, false otherwise.
 */
bool isSizeOne(MinHeap* minHeap) {
    return (minHeap->size == 1);
}

/**
 * @brief Extracts the minimum value node from the heap.
 * @param minHeap The heap to extract from.
 * @return A pointer to the minimum node.
 */
HuffmanNode* extractMin(MinHeap* minHeap) {
    HuffmanNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

/**
 * @brief Inserts a new node into the min-heap.
 * @param minHeap The heap to insert into.
 * @param huffmanNode The node to insert.
 */
void insertMinHeap(MinHeap* minHeap, HuffmanNode* huffmanNode) {
    ++minHeap->size;
    int i = minHeap->size - 1;
    while (i && huffmanNode->frequency < minHeap->array[(i - 1) / 2]->frequency) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = huffmanNode;
}

/**
 * @brief Builds the min-heap.
 * @param minHeap The heap to build.
 */
void buildMinHeap(MinHeap* minHeap) {
    int n = minHeap->size - 1;
    for (int i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}

/**
 * @brief Creates and builds a min-heap from character and frequency data.
 * @param data Array of characters.
 * @param freq Array of frequencies.
 * @param size The number of unique characters.
 * @return A pointer to the fully built MinHeap.
 */
MinHeap* createAndBuildMinHeap(const unordered_map<char, int>& frequencies) {
    MinHeap* minHeap = createMinHeap(frequencies.size());
    int i = 0;
    for(const auto& pair : frequencies) {
        minHeap->array[i] = newNode(pair.first, pair.second);
        i++;
    }
    minHeap->size = frequencies.size();
    buildMinHeap(minHeap);
    return minHeap;
}


// --- Tree and Code Generation ---

/**
 * @brief Builds the Huffman Tree using the min-heap.
 * @param frequencies The frequency map of characters.
 * @return The root node of the Huffman Tree.
 */
HuffmanNode* buildHuffmanTree(const unordered_map<char, int>& frequencies) {
    HuffmanNode *left, *right, *top;
    MinHeap* minHeap = createAndBuildMinHeap(frequencies);

    while (!isSizeOne(minHeap)) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);
        
        top = newNode('\0', left->frequency + right->frequency);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }
    
    HuffmanNode* root = extractMin(minHeap);
    delete minHeap->array; // Clean up the heap's internal array
    delete minHeap; // Clean up the heap struct
    return root;
}

/**
 * @brief Deletes the Huffman tree to prevent memory leaks.
 * @param root The root of the Huffman tree to delete.
 */
void deleteTree(HuffmanNode* root) {
    if (root == nullptr) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

/**
 * @brief Traverses the Huffman tree to generate binary codes for each character.
 * @param root The root of the Huffman tree.
 * @param code The current binary code being built.
 * @param huffmanCodes A map to store the generated codes.
 */
void generateCodes(const HuffmanNode* root, const string& code, unordered_map<char, string>& huffmanCodes) {
    if (!root) return;

    if (!root->left && !root->right) {
        huffmanCodes[root->data] = code;
        return;
    }

    generateCodes(root->left, code + "0", huffmanCodes);
    generateCodes(root->right, code + "1", huffmanCodes);
}


// --- Core Compression/Decompression Logic ---

void compressFile(const string& inputFilePath, const string& outputFilePath) {
    ifstream inputFile(inputFilePath, ios::binary);
    if (!inputFile) {
        cerr << "Error: Cannot open input file: " << inputFilePath << endl;
        return;
    }

    unordered_map<char, int> frequencies;
    char c;
    while (inputFile.get(c)) frequencies[c]++;
    
    inputFile.clear();
    inputFile.seekg(0);

    if (frequencies.empty()) {
        cout << "Input file is empty. Nothing to compress." << endl;
        ofstream(outputFilePath, ios::binary).close();
        return;
    }

    HuffmanNode* root = buildHuffmanTree(frequencies);
    unordered_map<char, string> huffmanCodes;
    generateCodes(root, "", huffmanCodes);

    ofstream outputFile(outputFilePath, ios::binary);
    if (!outputFile) {
        cerr << "Error: Cannot create output file: " << outputFilePath << endl;
        deleteTree(root);
        return;
    }

    uint32_t mapSize = frequencies.size();
    outputFile.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));
    for (const auto& pair : frequencies) {
        outputFile.write(&pair.first, sizeof(pair.first));
        outputFile.write(reinterpret_cast<const char*>(&pair.second), sizeof(pair.second));
    }

    unsigned char buffer = 0;
    int bitsWritten = 0;
    while (inputFile.get(c)) {
        string code = huffmanCodes[c];
        for (char bit : code) {
            buffer = (buffer << 1) | (bit - '0');
            bitsWritten++;
            if (bitsWritten == 8) {
                outputFile.write(reinterpret_cast<const char*>(&buffer), sizeof(buffer));
                buffer = 0;
                bitsWritten = 0;
            }
        }
    }

    if (bitsWritten > 0) {
        buffer <<= (8 - bitsWritten);
        outputFile.write(reinterpret_cast<const char*>(&buffer), sizeof(buffer));
    }
    
    deleteTree(root);
    cout << "File compressed successfully." << endl;
}

void decompressFile(const string& inputFilePath, const string& outputFilePath) {
    ifstream inputFile(inputFilePath, ios::binary);
    if (!inputFile) {
        cerr << "Error: Cannot open input file: " << inputFilePath << endl;
        return;
    }

    uint32_t mapSize;
    inputFile.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));

    if (inputFile.gcount() == 0) {
        cout << "Input file is empty or corrupt. Creating empty output." << endl;
        ofstream(outputFilePath, ios::binary).close();
        return;
    }

    unordered_map<char, int> frequencies;
    long long totalFreq = 0;
    for (uint32_t i = 0; i < mapSize; ++i) {
        char character;
        int frequency;
        inputFile.read(&character, sizeof(character));
        inputFile.read(reinterpret_cast<char*>(&frequency), sizeof(frequency));
        frequencies[character] = frequency;
        totalFreq += frequency;
    }

    HuffmanNode* root = buildHuffmanTree(frequencies);

    ofstream outputFile(outputFilePath, ios::binary);
    if (!outputFile) {
        cerr << "Error: Cannot create output file: " << outputFilePath << endl;
        deleteTree(root);
        return;
    }

    HuffmanNode* currentNode = root;
    unsigned char buffer;
    long long decodedChars = 0;

    while (inputFile.read(reinterpret_cast<char*>(&buffer), sizeof(buffer))) {
        for (int i = 7; i >= 0; --i) {
            if (decodedChars == totalFreq) break;

            int bit = (buffer >> i) & 1;
            currentNode = (bit == 0) ? currentNode->left : currentNode->right;

            if (!currentNode->left && !currentNode->right) {
                outputFile.put(currentNode->data);
                decodedChars++;
                currentNode = root;
            }
        }
    }

    deleteTree(root);
    cout << "File decompressed successfully." << endl;
}

void printUsage() {
    cout << "Usage:\n";
    cout << "  To compress:   huffman -c <input_file> <output_file>\n";
    cout << "  To decompress: huffman -d <input_file> <output_file>\n";
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printUsage();
        return 1;
    }

    string mode = argv[1];
    string inputFile = argv[2];
    string outputFile = argv[3];

    if (mode == "-c") {
        compressFile(inputFile, outputFile);
    } else if (mode == "-d") {
        decompressFile(inputFile, outputFile);
    } else {
        cerr << "Error: Invalid mode '" << mode << "'\n";
        printUsage();
        return 1;
    }

    return 0;
}
