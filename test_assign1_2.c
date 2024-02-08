#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "storage_mgr.h"
#include "dberror.h"
#include "test_helper.h"

// Define the test name
char *testName;

// Define the test output file
#define TESTPF "test_pagefile.bin"

// Prototype for test function
static void testAdditionalCases(void);

// Main function that runs all tests
int main (void)
{
	testName = "";
  
	// Initialize the storage manager
	initStorageManager();
	
	// Run the additional test cases
	testAdditionalCases();

	return 0;
}

// Function to test additional cases
void testAdditionalCases(void)
{
	SM_FileHandle fh;
	SM_PageHandle ph;
	int i;

	// Set the test name
	testName = "test single page content";

	ph = (SM_PageHandle) malloc(PAGE_SIZE);

	// Create a new page file and open it
	TEST_CHECK(createPageFile (TESTPF));
	TEST_CHECK(openPageFile (TESTPF, &fh));
	printf("Successfully created and opened the file.\n");
  
	// Read the first page into the handle
	TEST_CHECK(readFirstBlock (&fh, ph));
	// The first page should be empty (zero bytes)
  
	// Check if the first page is empty
	for (i=0; i < PAGE_SIZE; i++)		
		ASSERT_TRUE((ph[i] == 0), "Expected zero byte in the first page of the freshly initialized page");

	printf("Verified that the first block is empty.\n");

	// Change ph to be a string and write it to the disk
	for (i=0; i < PAGE_SIZE; i++)
		ph[i] = (i % 10) + '0';
	
	// Write the string ph to the first page of the file
	TEST_CHECK(writeBlock(0, &fh, ph));
	printf("Successfully wrote to the first block.\n");
  
  	// Write the string ph to the current position of the file
  	TEST_CHECK(writeCurrentBlock(&fh, ph));
  	printf("Successfully wrote to the current block.\n");
  
  	// Write the string ph to the second page of the file
  	TEST_CHECK(writeBlock(1, &fh, ph));
  	printf("Successfully wrote to the second block.\n");

 	// Write the string ph to the current position of the file
  	TEST_CHECK(writeCurrentBlock(&fh, ph));
  	printf("Successfully wrote to the current block.\n");

	// Write the string ph to the fourth page of the file
	TEST_CHECK(writeBlock(3, &fh, ph))
  	printf("Successfully wrote to the fourth block.\n");

  	// Read the content of the first page and check if it is correct
  	TEST_CHECK(readFirstBlock (&fh, ph));
  	for (i=0; i < PAGE_SIZE; i++)
    		ASSERT_TRUE((ph[i] == (i % 10) + '0'), "The character in the page read from the disk is as expected.");
  	printf("Successfully read the first block.\n");

  	// Read the previous block from the file
  	TEST_CHECK(readPreviousBlock (&fh, ph));
  	for (i=0; i < PAGE_SIZE; i++)
    		ASSERT_TRUE((ph[i] == (i % 10) + '0'), "The character in the page read from the disk is as expected.");
  	printf("Successfully read the previous block.\n");

	// Read the next block from the file
  	TEST_CHECK(readNextBlock (&fh, ph));
  	for (i=0; i < PAGE_SIZE; i++)
    		ASSERT_TRUE((ph[i] == (i % 10) + '0'), "The character in the page read from the disk is as expected.");
  	printf("Successfully read the next block.\n");
  
  	// Read the current block from the file
  	TEST_CHECK(readCurrentBlock (&fh, ph));
  	for (i=0; i < PAGE_SIZE; i++)
    		ASSERT_TRUE((ph[i] == (i % 10) + '0'), "The character in the page read from the disk is as expected.");
  	printf("Successfully read the current block.\n");
  
  	// Read the specific block (2nd block in this case) from the file
  	TEST_CHECK(readBlock(2,&fh, ph));
  	for (i=0; i < PAGE_SIZE; i++)
    		ASSERT_TRUE((ph[i] == (i % 10) + '0'), "The character in the page read from the disk is as expected.");

  	// Read the last block from the file
  	TEST_CHECK(readLastBlock (&fh, ph));
  	for (i=0; i < PAGE_SIZE; i++)
    		ASSERT_TRUE((ph[i] == (i % 10) + '0'), "The character in the page read from the disk is as expected.");
 
  	// Test the ensureCapacity function
  	TEST_CHECK(ensureCapacity(6,&fh));
  
  	// Destroy the new page file
  	TEST_CHECK(destroyPageFile (TESTPF));  
 
  	// Indicate that the test is done
  	TEST_DONE();
}
