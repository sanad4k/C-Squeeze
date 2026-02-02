C-Huffman-CLI: Lossless File Compression & Decompression

A lightweight, high-performance command-line tool written in raw C that implements Huffman Coding for lossless file compression. It supports encoding and decoding of UTF-8 text files (and arbitrary binary data) by reducing redundancy at the bit level.

🚀 What This Does

This program takes a file and "squeezes" it into a smaller size without losing a single bit of data. It operates in two distinct modes:

Compress: Reads your file, analyzes character frequency, builds an optimal Huffman Tree, and packs the bits into a binary output file.

Decompress: Reads the compressed binary file, reconstructs the Huffman Tree from the header metadata, and restores the original content exactly.

It features a dynamic header optimization (internally nicknamed the "BigGirl" flag) which intelligently switches between 32-bit and 64-bit frequency counters. This saves space on smaller files while preventing integer overflow on massive files (>4GB).

🎓 Why This Project? (The Learning Journey)

This project was built as a deep-dive learning experience into low-level systems programming. Implementing Huffman Coding from scratch is arguably one of the best ways to master C because it forces you to touch almost every critical concept in the language simultaneously:

Memory Management: Heavy use of malloc, free, and avoiding memory leaks while managing dynamic arrays and recursive tree structures.

Data Structures: Building Min-Heaps (Priority Queues) and Binary Trees manually without external libraries.

Bitwise Operations: Writing logic to shift bits (<<, >>) and mask bytes (&) to pack 8 boolean codes into a single byte of physical memory.

File I/O: Precise control over binary reading/writing (fread, fwrite, fseek) and handling file cursors.

Pointer Arithmetic: Managing complex data types and void pointers for optimization.

If you can build this, you understand how computers handle data at the fundamental level.

⚙️ How It Works (Program Flow)

The program is structured into a single monolithic C file for portability, using distinct phases for encoding and decoding.

Compression Flow

Frequency Analysis: Scans the entire file to count how many times every byte (0-255) appears.

Heap Construction: Creates a Min-Heap where the least frequent characters are prioritized at the top.

Tree Building: Repeatedly extracts the two smallest nodes and combines them until a single Huffman Tree remains.

Header Generation: Writes a "Dictionary" to the file header so the decoder knows how to interpret the data.

Optimization: Checks file size. If <4GB, writes 4-byte integers to save space. If >4GB, writes 8-byte longs to ensure safety.

Bit Packing: Re-reads the input file, translates characters into binary codes (e.g., 'A' -> 110), buffers them, and writes packed bytes to the output.

Decompression Flow

Header Parsing: Reads the first byte (Flag) to determine if the file uses 32-bit or 64-bit counters.

Tree Reconstruction: Reads the frequency table and rebuilds the exact same Huffman Tree in RAM.

Bit Walking: Reads the compressed body bit-by-bit.

Reads a 0 -> Move Left in Tree.

Reads a 1 -> Move Right in Tree.

Hits a Leaf -> Writes the character to disk and resets to the root.

🛠️ Usage

Compilation

You need a C compiler (like GCC or Clang).

gcc main.c -o huffman


Running the Tool

The program requires 3 arguments: Input File, Output File, and Mode.

To Compress a file (Mode 0):
```
./huffman input.txt compressed.huff 0
```


To Decompress a file (Mode 1):
```
./huffman compressed.huff restored.txt 1
```


📚 Resources & Citations

This project wouldn't be possible without the detailed explanations found in:

Wikipedia: For the theory behind Huffman Coding.

GeeksforGeeks: For reference implementations of Min-Heaps and Tree Traversals.

Data Compression Community: For insights on bit-manipulation techniques and header structure.

Written as a personal project to master the C language.
