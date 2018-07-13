#ifndef index_H
#define index_H

typedef struct LinkedList
{
	char* fileData;
	int fileCount;
	struct LinkedList* next;
}FileLinkedList;

typedef struct Tree
{
	char* wordData;
	FileLinkedList* llTokens;
	struct Tree* rightChild;
	struct Tree* leftChild;
}BSTTree;

BSTTree* root;

BSTTree* insert(char* word,char* fileName,BSTTree* rootPtr);
int compareStrings(char* stringOne,char* stringTwo);
void inOrderTraversal(BSTTree* traversePtr,FILE* outputFile);
void bubbleSort(FileLinkedList* fileInput);
void directoryTraverse(char* name);
void freeMemoryTraversal(BSTTree* rootPtr);
void fileRead(char* pathName, struct stat FileData, char* fileName);
#endif
