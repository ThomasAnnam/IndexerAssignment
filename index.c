#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<ctype.h>
#include<libgen.h>
#include"index.h"
//firstInstance is used to check whether or not we include a token with numbers in it i.e a123 or b234
static int firstInstance=0;
int main(int argc, char*argv[])
{
	//Error Check One: If Arguments is less than 3, then we know either the inverted file index(output file--> argv[1]) is passed into the arguments or the file or directory(argv[2]) is
	//not passed into the arguments, or both
	if(argc!=3)
	{
		printf("Arguments must be equal to 3!\n");
		return 1;
	}
	else
	{
		//Use the access struct to check whether or not the file(argv[1] exists or not) if it doesn't equal -1, then we know the file already exists and we ask the user whether or not
		//they wish to overwrite the file
		//If Yes--> Program will continue as usual
		//If No-->Program will return
		//int invertedFileIndexfd=0;
		if(access(argv[1],F_OK)!=-1)
		{
			//file already exists
			//printf("OVERWRITE!!!!\n");
			int overWriteFd=0;
			overWriteFd=open(argv[1],O_RDWR);
			if(overWriteFd==-1)
			{
				if(errno==EACCES)
				{
					printf("%s\n",strerror(errno));
					close(overWriteFd);
					return -1;
				}
			}
			printf("File already exists!\n");
			printf("Would you like to overwrite the file? For Yes, enter y. For No,enter n\n");
			char response=getchar();
			while(!(tolower(response)=='y'|| tolower(response)=='n'))
			{
				//clear the input buffer with the while loop
				char clear;
				while ((clear = getchar()) != '\n' && clear != EOF) { }
				printf("Invalid response!\n");
				printf("Please enter y for yes to overwrite or n for no to overwrite\n");
				response=getchar();
			}
			if(tolower(response=='n'))
			{
				printf("User does not wish to overwrite the file. Exiting the program.\n");
				return -1;
			}
			else
			{
				//argv[1]=strdup(argv[1]);
				int overWriteFd=remove(argv[1]);
				if(overWriteFd==0)
				{
					//printf("Succes!!!!!\n");
				}
			}
		}
		//fd represets the file file descriptor for argv[2] and we have read and write mode on the file
		int fd=open(argv[2],O_RDWR);
		//printf("What is the file descritpor......%d\n",fd);
		//Suppose that the fd is -1
		if(errno==EISDIR)
		{
			//We check what errno is equivalent to errno==EISDIR, if so that we know that argv[2] is a directory so we pass this into the directoryTraverse method
			//and use FILE* called outputPtr, which creates the file argv[1], and then we perform an inorderTraversal on the root(this is a Binary Search Tree) and output
			//the content into outputPtr and return
			
				printf("%s\n",strerror(errno));
				directoryTraverse(argv[2]);
				//printf("\n");
				//inOrderTraversal(root);
				FILE* outputPtr;
				outputPtr=fopen(argv[1],"w");
				fprintf(outputPtr,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
				fprintf(outputPtr,"<fileIndex>\n");
				//printf("HERE BEFORE IN ORDER TRAVERSAL!!!!!!!!!!!\n");
				inOrderTraversal(root,outputPtr);
				freeMemoryTraversal(root);
				fprintf(outputPtr,"</fileIndex>\n");
				close(fd);
				return 0;
			
		}
		else
		{
			
			//If we get to here, then we know that argv[2] is a file and we can directly open the file
			struct stat FileData; //the struct stat is desgined to get the # of bytes from argv[2] and store it into the address of FileData
			fileRead(argv[2], FileData, basename(argv[2]));
			//Output the content into the outputPtr which is a FILE*
			//we first write the string <?xml version=\"1.0\" encoding=\"UTF-8\"?>
			//then we write the strig <fileIndex"
			//lastly we do an inOrderTraversal on the BST root and output the content into the outputPtr and return
			FILE* outputPtr;
			outputPtr=fopen(argv[1],"w");
			fprintf(outputPtr,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
			fprintf(outputPtr,"<fileIndex>\n");
			
			//printf("HERE BEFORE IN ORDER TRAVERSAL!!!!!!!!!!!\n");
 			inOrderTraversal(root,outputPtr);
			fprintf(outputPtr,"</fileIndex>\n");
			//lastly free fileArraySize
			//////////////NEW CANCEL OUT *********free(fileArraySize);
			freeMemoryTraversal(root);
			//free(root);
			close(fd);
			return 0;
		}
	}
	return 0;
}

void directoryTraverse(char* name)
{
	//declare a DIR* pointer for the directoryTraversal
	//declare a struct stat used to get the length of the file
	//and a dirent struct to traverse directories as well as subdirectories in a Depth First Search(DFS) Matter
  	DIR* dirFD;
	struct stat FileData;
	struct dirent *fileDirectory=NULL;
 	dirFD=opendir(name);
	if(dirFD==NULL&&errno==EACCES)
	{
		//printf("%s\n",strerror(errno));
		return;
	}
	//Since we know we have a directory that does exist! We call opendir on the char* name
	do
	{
		fileDirectory=readdir(dirFD);
		//Read the directory from the DIR* called dirFD
		if(fileDirectory!=NULL)
		{
			//create a char* pathName to a crate a path with the size beign length of the fileDirectory->d_name, the length of the directory, and one for the null termianting character
			//and one for the / charactr
      			char* pathName=(char*)malloc(strlen(fileDirectory->d_name)+strlen(name)+2);
      			pathName[0]='\0'; //first Initialize all the values in the pathName with the null terminating character
			//Concatenate the pathName with the name and then concatenate the new pathname with / and then concatenate the new pathname with the fileDirectory->d_name
      			strcat(pathName,name);
      			strcat(pathName,"/");
      			strcat(pathName,fileDirectory->d_name);
			//checks whether the fileDirectory is a regular file using the memeber d_type
			if(fileDirectory->d_type==DT_REG)
			{
			
        			fileRead(pathName, FileData, fileDirectory->d_name);
        			free(pathName);
      			}
			//If the fileDirectory->d_type is a directory as oppposed to a regular file
			else if(fileDirectory->d_type==DT_DIR)
			{
				//printf("Found a directory!\n");
				//the . and .. are in every directory, and we don't want to recurse on the . and .. so we skip them and continue
				if((strcmp(fileDirectory->d_name,".")==0)||(strcmp(fileDirectory->d_name,"..")==0))
        			{
					continue;
        			}
				else
				{
          				//printf("Here is the full pathName......%s\n",pathName);
          				//printf("Here is the directory Name: %s\n",fileDirectory->d_name);
          				//printf("RECURSIVE CALL INITIATE!!!!!!!!!!!!!\n");
          				//get the pathName and call directoryTraverse again with the new pathName
          				directoryTraverse(pathName);
					//after we are done recursing on the directory we can free the path name
          				free(pathName);
				}
			}
		}
	}while(fileDirectory!=NULL);
	//after we are done looping through all the directories and subdirectories we can close the directory with the closedir function
	closedir(dirFD);
}

void fileRead(char* pathName, struct stat FileData, char* fileName)
{
	//printf("Here is the full pathName......%s\n",pathName);
        			//printf("Here is the directory Name: %s\n",fileDirectory->d_name);

        			char* fileArraySize = NULL;
        			int fileSize =0;
				int fd=open(pathName,O_RDONLY);
				//open the file of the pathName with the fd
				//check whether or not the file descriptor returns -1
				//if -1
				//check errno
				if(fd==-1)
				{
					//If errno is EACCES, then we know we don't have permission to read the given file, so print out the strerror(errno) and we continue the program
					if(errno==EACCES)
					{
						//printf("%s\n",strerror(errno));
						close(fd);
						return;
					}
					//If errno is ENOENT, then we know that no file or dirctory exists and print out of the strerror(errno)) and we continue the program
					else if(errno==ENOENT)
					{
						//printf("%s\n",strerror(errno));
						close(fd);
						return;
					}
				}
				else
				{
					//If so get the length of the file using the struct stat and store into the address of FileData
        			stat(pathName,&FileData);
        			fileSize=(int)(FileData.st_size);
				
			
        				//printf("Here is the size of the file: %d\n",fileSize);
					//if fileSize is equal to 0 then we have no content in the file and continue
					if(fileSize==0)
					{
						//printf("Empty content in the file!\n");
						close(fd);
						return;
					}
					//if the fileSize is NOT 0 then we create a char* with with size of fileSize
					fileArraySize=(char*)malloc(fileSize);
					//bzero the fileArraySize with the fileSize
					bzero(fileArraySize,fileSize);
					int lengthOfFile=0;
					lengthOfFile=fileSize-1;
					//use the read() function to get the content of the file and store into the fileArraySize
					read(fd,fileArraySize,lengthOfFile);
					//give the index at fileArraySize[fileSize]= to be the null terminating character
					fileArraySize[fileSize]='\0';
					close(fd);
				}
				int indexer=0;
				//Loop through the fileArraySize and determine the tokens to store into the Binary Search Tree
				while(fileArraySize[indexer]!='\0')
				{
					//printf("Here are the values associated with each file........%c\n",fileArraySize[indexer]);
					//Check whether or not the character of the fileArraySize[indexer] is an alphabetic character or not
					//If not alphabetic character meaning isalpha returns 0 then we know that it could potentially be a digit
					if(isalpha(fileArraySize[indexer])==0)
					{
						//int randomValue=isdigit(fileArraySize[indexer]);
						//printf("What is the randomvalue associated with each thing......%d\n",randomValue);
						//Check whether or not the following character is a digit or not, so we can determine whether to include
						//a number into a valid token
						if(isdigit(fileArraySize[indexer]))
						{
							//first create a char array with the digit and then pass into the atoi function
							char digitArray[1];
							digitArray[0]=fileArraySize[indexer];
							digitArray[1]='\0';
							//char* digitPtr=lol;
							int digit=0;
							//the atoi function converts a character into an integer value
							digit=atoi(digitArray);
							int previous=indexer-1;
							//printf("What is the value of fileArraySize[previous] %c\n",fileArraySize[previous]);
							//check whether the digit is between 0 and 9(inclusive)
							if((digit>=0&&digit<10))
							{
								//printf("Enter here if you pass the proper conditions\n");
								//First we know that the current character fileArraySize[indexer] is a digit
								//check whether or not the previous character was an alphabetic character

								//if isalpha(fileArraySize[previous] returns return we can include the following number into our token and set firstInstance to 1
								if((isalpha(fileArraySize[previous])))
								{
										//printf("Proper conditions have been met in order for you to include the digits!\n");
										firstInstance=1;
								}
								//Suppose we have a12,we know we can include 1 since the previous character 'a' is an alphabet, in the next iteration since 2 is a digit
								//and since firstInstance is already equal to 1, we can also include 2
								else if(firstInstance==1)
								{
									//printf("Boolean conditon has been met......\n");
								}
								//if the previous character is NOT an alphabetic character, then we cannot include the number as part of our valid tokens
								//so we set firstInstance=0 and replace the following digit with fileArraySize[indexer]=' ';
								else
								{
									//printf("Sorry can't include digits just need to replace as usual lol\n");
									firstInstance=0;
									fileArraySize[indexer]=' ';
								}
							}
						}
						//Suppose the following character is not an alphabetic character and is not a digit as well
						else
						{
							//printf("CURRENT VALUES OF IS ALPHA if you dont pass......%c\n",fileArraySize[indexer]);
							//char slash[2]="\\";
							//char* ptr=slash;
							char c=fileArraySize[indexer];
							//check whether or not the following character is a backslash ('\') the purpose for the backslah is to check for escape sequence characters
							if(c=='\\')
							{
									//printf("BAD TOKEN DETECTED\n");
									//Check whether or not the next character is not the null terminating character if its not check whether the next character is
									//equal to a,b,f,n,r,t, or v since these are all escape sequence characters in C
									//If so then we replace the current character fileArraySize[indexer] with ' ' and fileArraySize[indexer+1] with ' '
									if(fileArraySize[indexer+1]!='\0'&&(fileArraySize[indexer+1]=='a'||fileArraySize[indexer+1]=='b'||fileArraySize[indexer+1]=='f'
									||fileArraySize[indexer+1]=='n'||fileArraySize[indexer+1]=='r'||fileArraySize[indexer+1]=='t'||fileArraySize[indexer+1]=='v'))
									{
										fileArraySize[indexer]=' ';
										fileArraySize[indexer+1]=' ';
									}
							}
						        //if its NOT a backslah, we can simpley just replace the character at fileArraySize[indexer] with ' '
						        //since its neither an alphabetic character nor
							//a digit and set firstInstance to 0
							fileArraySize[indexer]=' ';
							firstInstance=0;
						}
						//printf("TOKEN DETECTED!!!!!!!!!!!!!!!!!!!!\n");
						//fileArraySize[indexer]=' ';
					}
					//increment indexer
					indexer++;
				}
				//isDigitIncluded=0;
				indexer=0;
				/*
				printf("After we tokenize............\n");
				while(fileArraySize[indexer]!='\0')
				{
					printf("Here are values after tokenize.........%c\n",fileArraySize[indexer]);
					indexer++;
				}
				indexer=0;
				*/
				//create a tokenArray
				char* tokenArray=NULL;
				//At this point, we colllected all the valid tokens and, since we replaced all non-alphabetic characters and digit(if they were not included) with ' ', and we will use
				//" " as our delimeter for the strtok function
				//Check whether the tokenArray is NULL
				//If NULL then we know that the there is no content in the file, so we just continue
				tokenArray=strtok(fileArraySize," ");
				if(tokenArray==NULL)
				{
					//printf("No content in file!\n");
					
					free(fileArraySize);
					close(fd);
					return;
				}
				else
				{
					//Else loop through the tokenArray and insert each valid token into the insert function and store it into the BSTTree* root with the token (tokenArray) and
					//the file name with fileDirectory->d_name, and root, which is the BSTTree*
					while(tokenArray!=NULL)
					{
						//printf("Here is the current value of each token after strtok......%s\n",tokenArray);
						//printf("FILE ASSOCIATED WITH EACH TOKEN!!!!!!!!!!%s\n",fileDirectory->d_name);
						//char* wordInsert=tokenArray;
						//printf("INSERTING WORD!\n");
						root=insert(tokenArray,fileName,root);
						//printf("AFTER INSERTING WORD!\n");
						tokenArray=strtok(NULL," ");
					}
				}
				//printf("**************************************************************************************\n");

				//free(fileArraySize);
				//reset firstInstance and fileSize back to 0
				fileSize=0;
				firstInstance=0;
				//close the file descriptor
				//close(fd);
	
}

BSTTree* insert(char* word,char* fileName,BSTTree* rootPtr)
{

	//printf("HERE IS THE CURRENT WORD %s\n",word);
	BSTTree* currentPtr=NULL;
	BSTTree* tempPtr=NULL;
	//Both the wordInput and fileInput are desgined to keep track of the fileName and word
	char* wordInput=NULL;
	char* fileInput=NULL;
	wordInput=(char*)malloc(strlen(word)+1);
	//bzero(wordInput,strlen(word)+1);
	fileInput=(char*)malloc(strlen(fileName)+1);
	//bzero(fileInput,strlen(fileName)+1);
	//wordInput[strlen(word)+1]='\0';
	//fileInput[strlen(fileName)+1]='\0';
	int index=0;
	//printf("Converting to lowercase.....\n");
	//First convert all words to lowercase
	while(word[index]!='\0')
	{
		//check whether the following character is a digit
		//if its not a digit, then we know its an alphebatic character, so we convert that letter to a lowercase
		if(isdigit(word[index])==0)
		{
			wordInput[index]=tolower(word[index]);
		}
		else
		{
			//if it is a digit then we simply just store that digit into wordInput
			wordInput[index]=word[index];
		}
		//increment the index
		index++;
	}
	//set the last index as the null terminating character
	wordInput[index]='\0';
	//printf("Finished converting to lowercase........%s\n",wordInput);
	index=0;
	//create a duplicate copy of word and store into word
	word=strdup(wordInput);
	//store the contents of the fileName into the fileInput
	while(fileName[index]!='\0')
        {
		if(isdigit(fileName[index])==0)
		{
			fileInput[index]=tolower(fileName[index]);
		}
		else
		{
			fileInput[index]=fileName[index];
		}
		index++;
	}
	//ser the last index as the null terminating character
	fileInput[index]='\0';
	//printf("FINISHED CONVERTING TO LOWERCASE........%s\n",fileInput);
	//store the contents of the fileInput into fileName
	fileName=strdup(fileInput);
        index=0;
	//check whether the rootPtr (BSTTree*) is NULL, if it is NULL then this is the first time the insert function is called
	if(rootPtr==NULL)
	{
		//printf("WE HAVE A NULL ROOT!\n");
		//Malloc a new BSTTree* node called headPtr with the sizeof(BSTTree))
		BSTTree* headPtr=NULL;
		headPtr=(BSTTree*)malloc(sizeof(BSTTree));
		//check whether malloc returns NULL or not
		//If NULL then we have no more memory
		//if so we then we return NULL;
		if(headPtr==NULL)
		{
			printf("Out of memory!\n");
			return NULL;
		}
		//if it's not NULL then we set headPtr->wordData=word;
		//and then headPtr's leftChild and rightChild both equal to NULL
		else
		{
			headPtr->wordData=word;
			headPtr->rightChild=NULL;
			headPtr->leftChild=NULL;
		}
		//FileLinkedList* is a Linked List data structure and each linked list is associated to a node within the tree
		//See this is the first time insert is called since we have a NULL root
		FileLinkedList* rootLinkedList=NULL;
		//Malloc a new Linked List and cast it into a FileLinkedList* with the sizeof(FileLinkedList)
		rootLinkedList=(FileLinkedList*)malloc(sizeof(FileLinkedList));
		//Check whether the rootLinkedList is NULL or not when returned from malloc
		//If rootLinkedList is NULL, then we are out of memory and return NULL;
		if(rootLinkedList==NULL)
		{
			printf("Out of memory!\n");
			return NULL;
		}
		else
		{
			//If rootLinkedList is NOT NULL
			//then we set rootLinkedList->fileData=fileName
			//set the inital count of rootLinkedList->fileCount to NULL
			//and set rootLinkedList
			rootLinkedList->fileData=fileName;
			rootLinkedList->fileCount=1;
			rootLinkedList->next=NULL;
		}

		headPtr->llTokens=rootLinkedList;
		free(wordInput);
		free(fileInput);
		rootPtr=headPtr;
		return rootPtr;
	}
	else
	{
		//printf("NOT NULLLLLLLL!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		//Create a new node called insertNode to put into the Binary Search Tree rootPtr
		BSTTree* insertNode=NULL;
		insertNode=(BSTTree*)malloc(sizeof(BSTTree));
		//Check the return value of malloc and see whether or not it was NULL
		//If NULL, then we print "Out of memory" and return rootPtr
		if(insertNode==NULL)
		{
			printf("Out of memory!\n");
			return rootPtr;
		}
		else
		{
			//initialize all the fields within the insertNode
			//set insertNode->wordData=wordInput;
			//and set insertNode leftChild and rightChild to be NULL
			insertNode->wordData=word;
			insertNode->rightChild=NULL;
			insertNode->leftChild=NULL;
		}
		//set currentPtr to rootPtr
		currentPtr=rootPtr;
		while(currentPtr!=NULL)
		{
			tempPtr=currentPtr;
			//Compare the strings with currentPtr->wordData(word already within the LinkedList) to wordInput
			//(word we are trying to insert into the LinkedList)
			//If the result is -1 we know that the currentPtr->wordData is less than wordInput(word we are trying to insert)
			//so we set currentPtr=currentPtr->rightChild
			if(compareStrings(currentPtr->wordData,wordInput)==-1)
			{
				//printf("Case one must be a right child\n");
				currentPtr=currentPtr->rightChild;
			}
			//Compare the strings with currentPtr->wordData(word already within the LinkedList) to wordInput
			//(word we are trying to insert into the LinkedList)
			//If the result is 1 we know that the currentPtr->wordData is greater than wordInput(word we are trying to insert)
			//so we set currentPtr=currentPtr->leftChild
			else if(compareStrings(currentPtr->wordData,wordInput)==1)
			{
				//printf("Case one must be a left child\n");
				currentPtr=currentPtr->leftChild;
			}
			else
			{
				//If we get to this point, then we know our currentPtr->wordData is equal to wordInput, thus we have a duplicate
				//set curretPtrLinkedList to insertNode->llTokens
				//FileLinkedList* currentPtrLinkedList=insertNode->llTokens;
				//create a new node for the Linked List called insertNewToken and case that into a FileLinkedList*
				FileLinkedList* insertNewToken=NULL;
				insertNewToken=(FileLinkedList*)malloc(sizeof(FileLinkedList));
				//Check the return value from malloc and see whether or not it was NULL
				//If NULL then we print "Out of memory!" and return rootPtr
				if(insertNewToken==NULL)
				{
					printf("Out of memory!\n");
					return rootPtr;
				}
				else
				{
					//intitialize the fields for insertNewToken
					//Set insertNewToken->fileData=fileInput
					//and set insertNewToken->next=NULL;
					insertNewToken->fileData=fileInput;
					insertNewToken->next=NULL;
					//Set currentNodeLinedListPtr to tempPtr->llTokens
					//and set previousLinkedListPtr to NULL;
					FileLinkedList* currentNodeLinkedListPtr=tempPtr->llTokens;
					FileLinkedList* previousLinkedListPtr=NULL;
					//checks to see if the the fileData (fileName) is already within the linkedList
					int isDuplicate=0;
					isDuplicate=0;
					//Traverse the Linked List of the current node
					while(currentNodeLinkedListPtr!=NULL)
					{
						//Compare the currentNodeLinkedListPtr->fileData(file name associated with the current node in the linked list)
						//to the insertNewToken->fileData(the file data we are trying to insert into the linked list)
						//If compareStrings returns 0, then we know it is a duplicate
						if(compareStrings(currentNodeLinkedListPtr->fileData,insertNewToken->fileData)==0)
						{
							//Implements the currentNodeLinkedListPtr->fileCount by 1
							//Set isDuplicate=1, and break
							int fileCounting=0;
							fileCounting=currentNodeLinkedListPtr->fileCount;
							fileCounting=fileCounting+1;
							currentNodeLinkedListPtr->fileCount=fileCounting;
							isDuplicate=1;
							break;
						}
						else
						{
							//Have not found a duplicate yet
							isDuplicate=0;
						}
						//Set previousLinkedListPtr=currentNodeLinkedListPtr
						//and set currentNodeLinkedListPtr=currentNodeLinkedListPtr->next
						previousLinkedListPtr=currentNodeLinkedListPtr;
						currentNodeLinkedListPtr=currentNodeLinkedListPtr->next;
					}
					//if isDuplicate=0,then we know that the fileData is not within the linked list
					if(isDuplicate==0)
					{
						//set the insertNewToken->fileCount=1;
						//and set previousLinkedListPtr(points to the last element of the linked list) to insertNewToken
						insertNewToken->fileCount=1;
						previousLinkedListPtr->next=insertNewToken;
						//set insertNode->llTokens to tempPtr->llTokens
						insertNode->llTokens=tempPtr->llTokens;
					}
					else
					{
						//Guaranteed that the node we are trying to insert into the currentNodeLinedListPtr is a duplicate
						//so we free anything that was previously malloced
						free(insertNewToken);
						free(insertNode);
						free(fileInput);
						free(wordInput);
					
					}
				}
				//isDuplicate=0;
				return rootPtr;
			}
		}
		//compare the strings with word we are trying to input with tempPtr->wordData
		//If 1 then we know that the word (word we are trying to insert into the tree) is greater than tempPtr->wordData
		//(word already within the tree) so we set this to tempPtr->rightChild=insertNode;
		if(compareStrings(word,tempPtr->wordData)==1)
		{
			//printf("Setting as rightChild.....\n");
			//Create a new Linked List called newLinkedListFile
      			FileLinkedList* newLinkedListFile=NULL;
      			newLinkedListFile=(FileLinkedList*)malloc(sizeof(FileLinkedList));
			//Check the return value of malloc and see whether NULL was returned or not
			//if newLinkedListFile returns NULL, then we print "Out of memory!" and return rootPtr
      			if(newLinkedListFile==NULL)
      			{
       				 printf("Out of memory!\n");
        			 return rootPtr;
     			}
      			else
      			{
				//if it does not return null then we initialize the fields of the newLinkedListFile
				//set newLinkedListFile->fileData=fileInput
				//newLinkedListFile->fileCount=1;
				//and lastly newLinkedListFile->next=NULL;
        			newLinkedListFile->fileData=fileName;
				newLinkedListFile->fileCount=1;
        			newLinkedListFile->next=NULL;
      			}
			//lastly set insertNode->llTokens=newLinkedListFile
			insertNode->llTokens=newLinkedListFile;
			//Set tempPtr->rightChild=insertNode
			tempPtr->rightChild=insertNode;
		}
		//compare the strings with word we are trying to input with tempPtr->wordData
		//If -1 then we know that the word (word we are trying to insert into the tree) is less than tempPtr->wordData
		//(word already within the tree) so we set this to tempPtr->leftChild=insertNode;
		else if(compareStrings(word,tempPtr->wordData)==-1)
		{
			//printf("Setting as leftChild.....\n");
			//Create a new Linked List called newLinkedListFile
			FileLinkedList* newLinkedListFile=NULL;
			newLinkedListFile=(FileLinkedList*)malloc(sizeof(FileLinkedList));
			//Check the return value of malloc and see whether NULL was returned or not
			//if newLinkedListFile returns NULL, then we print "Out of memory!" and return rootPtr
			if(newLinkedListFile==NULL)
			{
				printf("Out of memory!\n");
				return rootPtr;
			}
			else
			{
				//if it does not return null then we initialize the fields of the newLinkedListFile
				//set newLinkedListFile->fileData=fileInput
				//newLinkedListFile->fileCount=1;
				//and lastly newLinkedListFile->next=NULL;
				newLinkedListFile->fileData=fileName;
				newLinkedListFile->fileCount=1;
				newLinkedListFile->next=NULL;
			}
			//lastly set insertNode->llTokens=newLinkedListFile
			insertNode->llTokens=newLinkedListFile;
			//Set tempPtr->rightChild=insertNode
			tempPtr->leftChild=insertNode;
		}
		return rootPtr;
	}
	return rootPtr;
}

int compareStrings(char* stringOne,char* stringTwo)
{
  //printf("HERE IS STRINGONE! %s\n",stringOne);
  //printf("HERE IS STRINGTWO! %s\n",stringTwo);
  //int index=0;
  int lengthOfStringOne=strlen(stringOne); //get the length of stringOne
  //printf("What is the lengthOfStringOne %d\n",lengthOfStringOne);
  int lengthOfStringTwo=strlen(stringTwo); //get the length of stringTwo
  //printf("What is the lengthOfStringTwo %d\n",lengthOfStringTwo);
  char alphabetOne;
  char alphabetTwo;
  //traverse both stringOne and stringTwo
  while((alphabetOne=*stringOne)!='\0'&&(alphabetTwo=*stringTwo)!='\0')
  {
	//printf("What is the value of alphabetOne %c\n",alphabetOne);
	//printf("What is the value of alphabetTwo %c\n",alphabetTwo);
	//checks to see whether both alphabetOne and alphabetTwo is an alphabet by using the isalpha function
        if((isalpha(alphabetOne)&&isalpha(alphabetTwo)))
	{
		//printf("BOTH ARE alphabetical characters\n");
		//if both alphabetOne and alphabetTwo are equal then we increment stringOne and stringTwo
		if(alphabetOne==alphabetTwo)
		{
			//printf("EQUAL!\n");
			stringOne++;
			stringTwo++;
		}
		//if alphabetOne is less than alphabetTwo, then we return -1
		else if(alphabetOne<alphabetTwo)
		{
			//printf("CASE ONE\n");
			return -1;
		}
		//if alphabetOne is greater than alphabetTwo, then we return 1
		else if(alphabetOne>alphabetTwo)
		{
			//printf("CASE TWO\n");
			return 1;
		}
	}
	//if alphabetOne is a digit and alphabetTwo is an alphabet, then we return 1, since digits are greater than letters
	else if(isdigit(alphabetOne)&&isalpha(alphabetTwo))
	{
		//printf("CASE THREE\n");
		return 1;
	}
	//if alphabetOne is a alphabet and alphabetTwo is a digit, then we return -1, since letters are less than digits
	else if(isalpha(alphabetOne)&&isdigit(alphabetTwo))
	{
		//printf("CASE FOUR\n");
		return -1;
	}
	//if alphabetOne is not an alphabet and alphabetTwo is not an alphabet, then we increment stringOne and stringTwo
	else if(isalpha(alphabetOne)==0&&isalpha(alphabetTwo)==0)
	{
		//printf("Some delimiter has occured such as .!\n");
		if(isdigit(alphabetOne)&&isdigit(alphabetTwo))
        	{
			//printf("WHAT is alphabetOne now! %c\n",alphabetOne);
			//printf("WHAT is alphabetTwo now! %c\n", alphabetTwo);
			if(alphabetOne==alphabetTwo)
			{
				stringOne++;
				stringTwo++;
			}
			else if(alphabetOne>alphabetTwo)
			{
				return 1;
			}
			else if(alphabetOne<alphabetTwo)
			{
				return -1;
			}
                }
		else if(isalpha(alphabetOne)&&(isalpha(alphabetTwo)==0))
		{
			//printf("CASE FIVEEEEE!!!!!\n");
			return -1;
		}
		else if(isalpha(alphabetOne)==0&&(isalpha(alphabetTwo)))
		{
			//printf("CASE SIX!!!!!!!!!\n");
			return 1;
		}
		else if(isalpha(alphabetOne)==0&&(isdigit(alphabetTwo)))
		{
			//printf("FIRST IS ALPHA AND SECOND IS DIGIT!!\n");
			return 1;
		}
		else if(isdigit(alphabetOne)&&(isalpha(alphabetTwo)==0))
		{
			//printf("FIRST IS DIGIT AND SECOND IS ALPHA!!!!!!!n");
			return -1;
		}
		else
		{
			stringOne++;
			stringTwo++;
		}	
	}
	else if(isalpha(alphabetOne)&&(isalpha(alphabetTwo)==0))
	{
                return -1;
        }
        else if(isalpha(alphabetOne)==0&&(isalpha(alphabetTwo)))
        {
               return 1;
        }
	else
	{
		stringOne++;
		stringTwo++;
	}
   }
	//Equal up to this point
	//If both lengths are equal we return 0, signifying its a duplicate
	//printf("What are stringOne....%s\n",stringOne);
	//printf("What are stringTwo.....%s\n", stringTwo);
   	if(lengthOfStringOne==lengthOfStringTwo)
   	{
		//printf("CASE FIVE\n");
		return 0;
   	}
	//If lengthOfStringOne>lengthOfStringTwo, then we return 1, since stringOne is greater than stringTwo
 	else if(lengthOfStringOne>lengthOfStringTwo)
   	{
		//printf("CASE SIX\n");
		return 1;
   	}
   	else
   	{
		//If lengthOfStringOne<lengthOfStringTwo, then we return -1, since stringTwo is greater than stringOne
		//printf("CASE SEVEN\n");
		return -1;
   	}
}
//This function traverses the Binary Search Tree, in an inorder traversal manner and outputs the content into the outputFile
void inOrderTraversal(BSTTree* traversePtr,FILE* outputFile)
{
	//checks if current node is NULL or not
	if(traversePtr == NULL)
	{
		return;
	}
	//Checks whether the traversePtr BSTTree's leftchild is NULL or not 
	//If not NULL then we called inOrderTraversal again passing in traversePtr->leftChild and outputFile
	//and then we recurse
	if(traversePtr->leftChild!=NULL)
	{
		inOrderTraversal(traversePtr->leftChild,outputFile);
	}
	//Once the method call returns back we output the data into the outputFile by first printing out the word(traversePtr->wordData)
	//printf("THE DATA RIGHT NOW IS: %s\n",traversePtr->wordData);
	fprintf(outputFile, "\t<word text=\"%s\">\n", traversePtr->wordData);
	//set currentLL(currentLinkedList) to traversePtr->llTokens
	FileLinkedList* currentLL=traversePtr->llTokens;
	//bubbleSort on the current Linked List to get the count and the file names in the proper order
	bubbleSort(currentLL);
	//Traverse the currentLL and output both the fileData(also known as the fileName) and the fileCount into the outputFile
	while(currentLL!=NULL)
	{
		fprintf(outputFile,"\t\t<file name=\"%s\">%d</file>\n",currentLL->fileData, currentLL->fileCount);

		//printf("File associated with some Linked List is %s\n",currentLL->fileData);
		//printf("Count associated with some Linked List is %d\n",currentLL->fileCount);
		currentLL=currentLL->next;
	}
	//after we traverse the Linked List output </word> into the out file 
	fprintf(outputFile,"\t</word>\n");
	//Like with the left child in the BST, we recurse on the right child as well as do the same approach as shown above and output it into outputFile
	if(traversePtr->rightChild!=NULL)
	{
		inOrderTraversal(traversePtr->rightChild,outputFile);
	}
}
//the bubbleSort function is desgined to sort the LinkedList for each node into the Linked List before outputting into a file
void bubbleSort(FileLinkedList* fileInput)
{
	FileLinkedList* currentPtr=NULL;
	FileLinkedList* previousPtr=NULL;
	//isTrue checks to see whether any values were swapped in the linkedList 
	int isTrue=0;
	do
	{
		isTrue=0;
		//set currentPtr to the head of the FileLinkedList* which is fileInput
		currentPtr=fileInput;
		while(currentPtr->next!=previousPtr)
		{
			//if the currentPtr->fileCount is less than the currentPtr->next->fileCount such as 1<2 
			//then we swap both the currentPtr->next's fileCount and fileData with currentPtr's fileCOunt and fileData
			//and set isTrue=1
			if(currentPtr->fileCount<currentPtr->next->fileCount)
			{
				int tempFileCount=currentPtr->next->fileCount;
				char* tempFileData=currentPtr->next->fileData;
				currentPtr->next->fileCount=currentPtr->fileCount;
				currentPtr->next->fileData=currentPtr->fileData;
				currentPtr->fileData=tempFileData;
				currentPtr->fileCount=tempFileCount;
				isTrue=1;
			}
			//Suppose currentPtr->fileCount==currentPtr->next->fileCount
			else if(currentPtr->fileCount==currentPtr->next->fileCount)
			{
				//In this situation, we sort alphanumerically by the path name
				//we compare the currentPtr->fileData to currentPtr->next->fileData 
				//If 1 is returned then we know that the currentPtr->fileData is greater than currentPtr-next->fileData 
				//so we swap currentPtr->next->fileData with currentPtr->fileData 
				//and set currentPtr->fileData=tempFileData 
				//and set isTrue=1
				//printf("FOR FILES FOR FILES FOR FILES FOR FILES\n");
				if(compareStrings(currentPtr->fileData,currentPtr->next->fileData)==1)
				{
					char* tempFileData=currentPtr->next->fileData;
					currentPtr->next->fileData=currentPtr->fileData;
					currentPtr->fileData=tempFileData;
					isTrue=1;
				}
				/*
				else if(compareStrings(currentPtr->fileData,currentPtr->next->fileData)==-1)
				{
					char* tempFileData=currentPtr->next->fileData;
					currentPtr->next->fileData=currentPtr->fileData;
					currentPtr->fileData=tempFileData;
					isTrue=1;	
				}
				*/
			}
			currentPtr=currentPtr->next;
		}
		//set previousPtr to currentPtr
		previousPtr=currentPtr;
	}while(isTrue);//loop until isTrue=0
}
//This function is used to free all the memory from both the BST and the LinkedList
//This is done through an inorder traversal of the Binary Search Tree
void freeMemoryTraversal(BSTTree* rootPtr)
{
	if(rootPtr==NULL)
	{
		return;
	}
	//printf("ERROR OCCURS HERE!!!!\n");
	//recurse on the leftChild of the BST
	if(rootPtr->leftChild!=NULL)
	{
		freeMemoryTraversal(rootPtr->leftChild);
	}
	// recurse on the rightChild of the BST
	if(rootPtr->rightChild!=NULL)
	{
		freeMemoryTraversal(rootPtr->rightChild);
	}
	
	FileLinkedList* currentLL=rootPtr->llTokens;
	
	//free all nodes within in the LinkedList for each node in the BST
	while(currentLL!=NULL)
	{
		FileLinkedList* temp=currentLL;
		currentLL=currentLL->next;
		free(temp);
		
	}
	//free the node itself
	free(rootPtr);
	
}
