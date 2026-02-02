#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
  char chara;
  long freq;
  struct Node *Left;
  struct Node *Right;
} Node;
typedef struct minHeap {
  unsigned int capacity;
  unsigned int maxCapacity;
  Node **list;
} minHeap;
typedef struct freqholder {
  char chara;
  long frequency;
} __attribute__((packed)) freqholder;

typedef struct freqholdershort {
  char chara;
  int frequency;
} __attribute__((packed)) freqholdershort;

FILE *openFile(char *filename) {
  FILE *file = fopen(filename, "rb");
  if (file == NULL) {
    perror("error opening the file check the file name");
    exit(1);
  } else {
    return file;
  }
}
// this just prints the frequency array  NOTE: this is very early in the flow
// and is just raw frequency data
void arrayPrinter(long *array) {
  for (int i = 0; i < 256; i++) {
    if (isprint(i)) {
      printf("%c (%d ): %ld times \n", (char)i, i, array[i]);
    } else {
      printf("0x%02x (%d): %ld times \n", i, i, array[i]);
    }
  }
}
// here fgetc gets a byte from the file regardless if its open as text or binary
// and returns the corresponding integer value ie within 0 to 255
long *Counter(FILE *file, long frequency[256]) {
  int sign;
  while ((sign = fgetc(file)) != EOF) {
    frequency[sign] += 1;
  }
}

// here this function creates nodes based on the frequency array and returns its
// address in the node** array ie address
void NodeWrapper(long array[256], Node *address[256]) {

  for (int i = 0; i < 256; i++) {
    if (array[i] > 0) {
      Node *newNode = (Node *)malloc(sizeof(Node));
      newNode->chara = (char)i;
      newNode->freq = array[i];
      newNode->Left = NULL;
      newNode->Right = NULL;
      address[i] = newNode;
    } else {
      address[i] = NULL;
    }
  }
}

void swapNode(Node **a, Node **b) {
  Node *temp = *a;
  *a = *b;
  *b = temp;
}

// this just malloc a heap based on the struct that i defined  dense dense
void HeapCreator(minHeap *heapy, Node *address[256]) {
  heapy->capacity = 0;
  heapy->maxCapacity = 256;
  heapy->list = (Node **)malloc(heapy->maxCapacity * sizeof(Node *));
  for (int i = 0; i < 256; i++) {
    if (address[i] != NULL) {
      heapy->list[heapy->capacity] = address[i];
      heapy->capacity += 1;
    }
  }
}
// this is the logic which is a recursion and helps in building the minheap
// using minHeapBuilder function
void minHeapifier(minHeap *heapy, int pos) {
  int smallest = pos;
  int left = 2 * pos + 1;
  int right = 2 * pos + 2;
  if (left < heapy->capacity &&
      heapy->list[left]->freq < heapy->list[smallest]->freq) {
    smallest = left;
  }
  if (right < heapy->capacity &&
      heapy->list[right]->freq < heapy->list[smallest]->freq) {
    smallest = right;
  }
  if (smallest != pos) {
    swapNode(&heapy->list[smallest], &heapy->list[pos]);
    minHeapifier(heapy, smallest);
  }
}
// this will create the minheap from the data
void minHeapBuilder(minHeap *heapy) {
  int n = heapy->capacity - 1;
  for (int i = (n - 1) / 2; i >= 0; --i) {
    minHeapifier(heapy, i);
  }
}

Node *extractMin(minHeap *heapy) {
  Node *temp = heapy->list[0];
  heapy->list[0] = heapy->list[heapy->capacity - 1];
  --heapy->capacity;
  minHeapifier(heapy, 0);
  return temp;
}
void insertNode(minHeap *heapy, Node *insertNode) {
  int i = heapy->capacity;
  ++heapy->capacity;
  while (i && insertNode->freq < heapy->list[(i - 1) / 2]->freq) {
    heapy->list[i] = heapy->list[(i - 1) / 2];
    i = (i - 1) / 2;
  }
  heapy->list[i] = insertNode;
}

// the actual building fuction which uses the helper function to build proper
// huffman tree structure is this actualBuilder function
Node *treeBuilder(minHeap *heapy) {
  Node *left, *right, *top;
  minHeapBuilder(heapy);
  while (heapy->capacity > 1) {
    left = extractMin(heapy);
    right = extractMin(heapy);

    top = malloc(sizeof(Node));
    top->chara = '$';
    top->Left = left;
    top->Right = right;
    top->freq = left->freq + right->freq;
    insertNode(heapy, top);
  }
  return extractMin(heapy);
}

// This function builds the code for each character and make a look up table for
// it NOTE:lookup Table , character code , here nodey is the root node
void LookUpArray(char *charCode[], int top, Node *nodey, char buffer[]) {
  if (nodey->Left == NULL && nodey->Right == NULL) {
    buffer[top] = '\0';
    unsigned char index = (unsigned char)nodey->chara;
    charCode[index] = (char *)malloc((top + 1) * sizeof(char));
    for (int i = 0; i < top; i++) {
      charCode[index][i] = buffer[i];
    }
    charCode[index][top] = '\0';
    // printf("Character: %c  | Code: %s\n", nodey->chara, charCode[index]);
  }
  if (nodey->Left) {
    buffer[top] = '0';
    LookUpArray(charCode, top + 1, nodey->Left, buffer);
  }
  if (nodey->Right) {
    buffer[top] = '1';
    LookUpArray(charCode, top + 1, nodey->Right, buffer);
  }
}
// TODO: header writer
freqholder *headerbuilderlong(long frequency[], int *count) {
  for (int i = 0; i < 256; i++) {
    if (frequency[i] != 0) {
      (*count)++;
    }
  }
  freqholder *dataholder = malloc((*count) * sizeof(freqholder));
  int index = 0;
  for (int i = 0; i < 256; i++) {
    if (frequency[i] != 0) {
      dataholder[index].chara = i;
      dataholder[index].frequency = frequency[i];
      index++;
    }
  }
  return dataholder;
}
freqholdershort *headerbuildershort(long frequency[], int *count) {
  for (int i = 0; i < 256; i++) {
    if (frequency[i] != 0) {
      (*count)++;
    }
  }
  freqholdershort *dataholder = malloc((*count) * sizeof(freqholdershort));
  int index = 0;
  for (int i = 0; i < 256; i++) {
    if (frequency[i] != 0) {
      dataholder[index].chara = i;
      dataholder[index].frequency = frequency[i];
      index++;
    }
  }
  return dataholder;
}

// NOTE: this will tell me how big the file size is
long getFileSize(char *filename) {
  FILE *fp = fopen(filename, "rb");
  if (fp == NULL)
    return -1;

  fseek(fp, 0, SEEK_END); // Jump to the end
  long size = ftell(fp);  // Check byte position
  fclose(fp);

  return size;
}

// TODO:  here the overhead of the frequency table is large and inefficient for
// smaller file so need to opitimze for that
void CompressedFile(char *inputFile, char *outputFile, long frequency[],
                    char *charCode[]) {
  long fileSize = getFileSize(inputFile);
  unsigned char isBigGirl = (fileSize > 4294967295L) ? 1 : 0;

  FILE *in = fopen(inputFile, "rb");
  FILE *out = fopen(outputFile, "wb");
  fwrite(&isBigGirl, sizeof(unsigned char), 1, out);
  unsigned int count = 0;
  if (isBigGirl) {
    // MODE A: Big File (Longs)
    freqholder *header = headerbuilderlong(frequency, &count);
    fwrite(&count, sizeof(unsigned int), 1, out);   // Write Count
    fwrite(header, sizeof(freqholder), count, out); // ONE SHOT WRITE
    free(header);
  } else {
    // MODE B: Small File (Ints)
    freqholdershort *header = headerbuildershort(frequency, &count);
    fwrite(&count, sizeof(unsigned int), 1, out);        // Write Count
    fwrite(header, sizeof(freqholdershort), count, out); // ONE SHOT WRITE
    free(header);
  }

  unsigned char buffer = 0;
  int bitprogress = 0;
  int ch;
  while ((ch = fgetc(in)) != EOF) {
    char *code = charCode[ch];
    for (int i = 0; code[i] != '\0'; i++) {
      if (code[i] == '1') {
        buffer = buffer | (1 << (7 - bitprogress));
      }
      bitprogress++;
      if (bitprogress == 8) {
        fputc(buffer, out);
        bitprogress = 0;
        buffer = 0;
      }
    }
  }
  if (bitprogress > 0) {
    fputc(buffer, out);
  }
  printf("File compressed and writen to %s\n", outputFile);
  fclose(in);
  fclose(out);
}

void decodeFile(char *filename, char *outFile) {
  printf("now running decoding \n");
  FILE *inputFile = fopen(filename, "rb");
  FILE *out = fopen(outFile, "wb");
  unsigned char isBig = fgetc(inputFile);
  unsigned int count;
  fread(&count, sizeof(unsigned int), 1, inputFile);
  long frequency[256] = {0};
  if (isBig) {
    freqholder *freqdata = malloc(count * sizeof(freqholder));
    fread(freqdata, sizeof(freqholder), count, inputFile);
    for (int i = 0; i < count; i++) {
      unsigned char index = freqdata[i].chara;
      frequency[index] = freqdata[i].frequency;
    }
  } else {
    freqholdershort *freqdata = malloc(count * sizeof(freqholdershort));
    fread(freqdata, sizeof(freqholdershort), count, inputFile);
    for (int i = 0; i < count; i++) {
      unsigned char index = freqdata[i].chara;
      frequency[index] = (long)freqdata[i].frequency;
    }
  }
  Node *address[256];
  NodeWrapper(frequency, address);
  minHeap *heapy = malloc(sizeof(minHeap));
  HeapCreator(heapy, address);
  Node *root = treeBuilder(heapy);
  Node *current = root;
  unsigned char buffer = 0;
  int totalchara = root->freq;
  unsigned char ch;
  while ((ch = fgetc(inputFile)) != EOF) {
    buffer = ch;
    for (int i = 0; i < 8; i++) {
      int bit = (buffer >> (7 - i)) & 1;
      if (bit == 1) {
        current = current->Right;
      } else {
        current = current->Left;
      }
      if (current->Left == NULL && current->Right == NULL) {
        fputc(current->chara, out);
        current = root;
        totalchara--;
      }
      if (totalchara == 0) {
        return;
      }
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    printf("please provide the file name no other parameter is considered \n");
    printf("inputfile_Name outputfile_name mode\n");
    printf("mode = 1 for compression mode = 0 for decodefile\n");
    exit(1);
  }
  char *filename = argv[1];
  char *outfile = argv[2];
  char *mode = argv[3];

  long frequency[256] = {0};
  Node *address[256] = {0};
  FILE *file = openFile(filename);
  Counter(file, frequency);
  printf("the ascii frequency detection completed ");
  // arrayPrinter(frequency);
  // TODO: this logic here can be optimized it works but a bit chaos
  if (strcmp(mode, "0")) {
    NodeWrapper(frequency, address);
    minHeap *heapy = malloc(sizeof(minHeap));
    HeapCreator(heapy, address);
    Node *root = treeBuilder(heapy);
    printf("Huffman Tree Built. Root Frequency: %d\n", root->freq);
    char *charCode[256] = {0};
    char buffer[100];
    //  NOTE: this makes the lookup table from the huffman tree which is then
    //  used to compress the file
    LookUpArray(charCode, 0, root, buffer);
    printf("%d \n", heapy->capacity);
    //  printf("freqholder size: %zu\n", sizeof(freqholder));
    CompressedFile(filename, outfile, frequency, charCode);
    return 0;
  }
  if (strcmp(mode, "1")) {
    decodeFile(filename, outfile);
    return 0;
  } else {
    printf("enter a valid mode");
    return 0;
  }
  return 0;
}
