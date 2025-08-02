Huffman File Compressor
A command-line utility written in C++ for lossless file compression and decompression using the Huffman coding algorithm.

📜 Overview
This project is a classic implementation of Huffman's greedy algorithm to demonstrate a real-world application of data structures and algorithms. It can take any file, compress it into a smaller binary format (.huf), and then decompress it back to its original state with no data loss.

The core principle is to assign variable-length binary codes to characters based on their frequency of appearance in the source file. More frequent characters get shorter codes, and less frequent characters get longer codes, leading to an overall reduction in file size.

🧠 Core Concepts Used
This program is built upon several fundamental Data Structures and Algorithms concepts:

Huffman's Greedy Algorithm:

The algorithm builds an optimal prefix-free code tree (the Huffman Tree) by repeatedly selecting the two nodes (characters or sub-trees) with the lowest frequencies and merging them into a new parent node. This "greedy" choice at each step locally optimal, and leads to a globally optimal solution for generating the best compression codes.

Min-Heap (as a Priority Queue):

What it's used for: To efficiently find the two nodes with the minimum frequencies at every step of the tree-building process.

Implementation: The code uses a custom-built MinHeap structure that operates on an array of HuffmanNode pointers. Key functions like insertMinHeap, extractMin, and minHeapify are implemented manually to manage this data structure.

Binary Trees (The Huffman Tree):

What it's used for: The final data structure that represents the optimal character codes.

Structure: Leaf nodes of the tree represent the actual characters from the file. Internal nodes represent merged sub-trees. The path from the root to a leaf node defines the binary code for that character (a '0' for a left turn, a '1' for a right turn).

Hash Map (std::unordered_map):

What it's used for:

Frequency Counting: To efficiently store the frequency of each character in the input file.

Code Lookup: To store the generated Huffman code for each character, allowing for fast (O(1) average time) retrieval during the encoding process.

Bit Manipulation:

What it's used for: To pack the variable-length string codes (like "01101") into actual bytes (unsigned char). Instead of writing the characters '0' and '1' to the file, the program uses bitwise operations (<< left shift, | bitwise OR) to create bytes, which is the key to achieving actual compression.

🛠️ How to Compile and Run
This program must be compiled and run from a command-line terminal.

1. Compilation
You will need a C++ compiler like G++. Navigate to the directory containing huffman.cpp and run the following command:

g++ huffman.cpp -o huffman

This command compiles the source code and creates an executable file named huffman.

2. Usage
The program operates in two modes: compress (-c) and decompress (-d).

To Compress a File:
Use the following command structure:

./huffman -c <input_file> <output_file>

Example:
To compress a file named my_document.txt into compressed.huf:

./huffman -c my_document.txt compressed.huf

To Decompress a File:
Use the following command structure:

./huffman -d <compressed_file> <decompressed_file>

Example:
To decompress compressed.huf back into a file named original_document.txt:

./huffman -d compressed.huf original_document.txt

The resulting original_document.txt will be identical to the original my_document.txt.

⚙️ How It Works
Compression Process
Frequency Counting: The program reads the input file once to count the occurrences of every character, storing them in a hash map.

Build Huffman Tree: It uses the frequency map to build the Huffman tree with the custom Min-Heap, as described in the concepts above.

Generate Codes: It traverses the completed tree to generate a unique binary code for each character.

Write to File:

Header: It first writes a header to the output file containing the frequency map. This is essential for the decompressor to rebuild the exact same tree.

Data: It reads the input file a second time. For each character, it looks up its binary code and writes the bits to the output file using bitwise operations.

Decompression Process
Read Header: The program first reads the frequency map from the header of the compressed file.

Rebuild Tree: It uses this map to construct the identical Huffman tree that was used for compression.

Decode Data: It reads the rest of the file bit by bit. For each bit, it traverses the tree (left for '0', right for '1') starting from the root. When it reaches a leaf node, it has found a character. It writes this character to the output file and returns to the root to decode the next character. This continues until all characters are decoded.