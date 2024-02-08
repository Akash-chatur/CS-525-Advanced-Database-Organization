	#include<stdio.h>
	#include<stdlib.h>
	#include<sys/stat.h>
	#include<sys/types.h>
	#include<unistd.h>
	#include<string.h>
	#include<math.h>

	#include "storage_mgr.h"

	FILE *filePtr; // Global file pointer

	// Function to initialize the storage manager
	extern void initStorageManager (void) {
		filePtr = NULL; // Set the global file pointer to NULL
	}

	// Function to create a new page file
	extern RC createPageFile (char *fileName) {
		filePtr = fopen(fileName, "w+"); // Open a new file for reading and writing

		if(filePtr == NULL) { // If the file pointer is still NULL, the file was not found
			return RC_FILE_NOT_FOUND;
		} else {
			SM_PageHandle newPage = (SM_PageHandle)calloc(PAGE_SIZE, sizeof(char)); // Allocate memory for a new page
			
			// Write the new page to the file
			if(fwrite(newPage, sizeof(char), PAGE_SIZE, filePtr) < PAGE_SIZE)
				printf("Failed to write \n");
			else
				printf("Write successful \n");
			
			fclose(filePtr); // Close the file
			
			free(newPage); // Free the memory allocated for the new page
			
			return RC_OK;
		}
	}

	// Function to open an existing page file
	extern RC openPageFile (char *fileName, SM_FileHandle *fHandle) {
		filePtr = fopen(fileName, "r"); // Open the file for reading

		if(filePtr == NULL) { // If the file pointer is still NULL, the file was not found
			return RC_FILE_NOT_FOUND;
		} else { 
			fHandle->fileName = fileName; // Update the file handle's filename
			fHandle->curPagePos = 0; // Set the current position to the start of the page

			struct stat fileInfo; // Structure to hold file information
			if(fstat(fileno(filePtr), &fileInfo) < 0) // Get the file information    
				return RC_ERROR;
			fHandle->totalNumPages = fileInfo.st_size/ PAGE_SIZE; // Calculate the total number of pages

			fclose(filePtr); // Close the file
			return RC_OK;
		}
	}

	// Function to close an opened page file
	extern RC closePageFile (SM_FileHandle *fHandle) {
		if(filePtr != NULL) // If the file pointer is not NULL, set it to NULL
			filePtr = NULL;	

		return RC_OK; 
	}

	// Function to delete a page file
	extern RC destroyPageFile(char * fileName) {
		filePtr = fopen(fileName, "r"); // Open the file for reading

		if (filePtr == NULL) // If the file pointer is still NULL, the file was not found
			return RC_FILE_NOT_FOUND;

		// Close the file pointer before deleting (important for resource management and proper deletion)
		if (fclose(filePtr) != 0) {
			printf("Error closing file pointer: %s\n", strerror(errno));
			return RC_ERROR;
		}

		// Attempt to delete the file
		if (remove(fileName) == 0) {
			printf("File removed successfully\n");
		} else {
			printf("Error removing file: %s\n", strerror(errno));
			return RC_ERROR;
		}

		return RC_OK;
	}

	// Function to read a specific block from a page file
	extern RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
		if (pageNum > fHandle->totalNumPages || pageNum < 0) // If the page number is out of range, return an error
				return RC_READ_NON_EXISTING_PAGE;

		filePtr = fopen(fHandle->fileName, "r"); // Open the file for reading

		if(filePtr == NULL) // If the file pointer is still NULL, the file was not found
			return RC_FILE_NOT_FOUND;
		
		int isSeekSuccess = fseek(filePtr, (pageNum * PAGE_SIZE), SEEK_SET); // Seek to the specific page
		if(isSeekSuccess == 0) { // If the seek was successful, read the page
			fread(memPage, sizeof(char), PAGE_SIZE, filePtr);
		} else {
			return RC_READ_NON_EXISTING_PAGE; 
		}
			
		fHandle->curPagePos = ftell(filePtr); // Update the current page position
		
		fclose(filePtr); // Close the file
		
			return RC_OK;
	}

	// Function to get the current block position
	extern int getBlockPos (SM_FileHandle *fHandle) {
		return fHandle->curPagePos; // Return the current page position
	}

	// Function to read the first block from a page file
	extern RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
		return readBlock(0, fHandle, memPage); // Call readBlock with page number 0
	}

	// Function to read the previous block from a page file
	extern RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
		int currentPageNumber = fHandle->curPagePos / PAGE_SIZE; // Calculate the current page number

		return readBlock(currentPageNumber - 1, fHandle, memPage); // Call readBlock with the previous page number
	}

	// Function to read the current block from a page file
	extern RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
		int currentPageNumber = fHandle->curPagePos / PAGE_SIZE; // Calculate the current page number
		
		return readBlock(currentPageNumber, fHandle, memPage); // Call readBlock with the current page number
	}

	// Function to read the next block from a page file
	extern RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
		int currentPageNumber = fHandle->curPagePos / PAGE_SIZE; // Calculate the current page number
		
		return readBlock(currentPageNumber + 1, fHandle, memPage); // Call readBlock with the next page number
	}

	// Function to read the last block from a page file
	extern RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
		return readBlock(fHandle->totalNumPages - 1, fHandle, memPage); // Call readBlock with the last page number
	}

	// Function to write a specific block to a page file
	extern RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
		if (pageNum > fHandle->totalNumPages || pageNum < 0) // If the page number is out of range, return an error
				return RC_WRITE_FAILED;
		
		filePtr = fopen(fHandle->fileName, "r+"); // Open the file for reading and writing
		
		if(filePtr == NULL) // If the file pointer is still NULL, the file was not found
			return RC_FILE_NOT_FOUND;

		int isSeekSuccess = fseek(filePtr, (pageNum * PAGE_SIZE), SEEK_SET); // Seek to the specific page
		if(isSeekSuccess == 0) { // If the seek was successful, write the page
			fwrite(memPage, sizeof(char), strlen(memPage), filePtr);

			fHandle->curPagePos = ftell(filePtr); // Update the current page position
			
			fclose(filePtr); // Close the file
		} else {
			return RC_WRITE_FAILED;
		}	
		
		return RC_OK;
	}

	// Function to write the current block to a page file
	extern RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
		int currentPageNumber = fHandle->curPagePos / PAGE_SIZE; // Calculate the current page number
		
		fHandle->totalNumPages++; // Increment the total number of pages
		return writeBlock(currentPageNumber, fHandle, memPage); // Call writeBlock with the current page number
	}

	// Function to add an empty block to a page file
	extern RC appendEmptyBlock (SM_FileHandle *fHandle) {
		SM_PageHandle emptyBlock = (SM_PageHandle)calloc(PAGE_SIZE, sizeof(char)); // Allocate memory for an empty block
		
		int isSeekSuccess = fseek(filePtr, 0, SEEK_END); // Seek to the end of the file
		
		if( isSeekSuccess == 0 ) { // If the seek was successful, write the empty block
			fwrite(emptyBlock, sizeof(char), PAGE_SIZE, filePtr);
		} else {
			free(emptyBlock); // Free the memory allocated for the empty block
			return RC_WRITE_FAILED;
		}
		
		free(emptyBlock); // Free the memory allocated for the empty block
		
		fHandle->totalNumPages++; // Increment the total number of pages
		return RC_OK;
	}

	// Function to ensure the capacity of a page file
	extern RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle) {
		filePtr = fopen(fHandle->fileName, "a"); // Open the file for appending
		
		if(filePtr == NULL) // If the file pointer is still NULL, the file was not found
			return RC_FILE_NOT_FOUND;
		
		while(numberOfPages > fHandle->totalNumPages) // While the number of pages is greater than the total number of pages
			appendEmptyBlock(fHandle); // Call appendEmptyBlock
		
		fclose(filePtr); // Close the file
		return RC_OK;
	}
