Running the Script:
=========================== 

1.	Navigate to the project root directory:
    •	Open a terminal and use the "cd" command to go to the "assign1" directory.
    •	Verify your location with the "ls" command.
2.	Clean old compiled files:
    •	Type "make clean" to remove any previously compiled .o files.
3.	Compile all project files:
    •	Type "make" to compile the project, including test_assign1_1.c.
4.	Run the first test file:
    •	Type "make run_test1" to execute test_assign1_1.c.
5.	Compile the custom test file (optional):
    •	Type "make test2" to compile test_assign1_2.c.
6.	Run the custom test file (optional):
    •	Type "make run_test2" to execute test_assign1_2.c.


Solution Description:
===========================

    •	Makefile: Created using a tutorial from http://mrbook.org/blog/tutorials/make/.
    •	Memory Management: Proper memory management is ensured by closing file streams and freeing reserved space when needed.


File-Related Functions:
===========================

    •	initStorageManager(): Initializes the storage manager.
    •	createPageFile(): Creates a new page file with the specified name.
    •	openPageFile(): Opens an existing page file for reading.
    •	closePageFile(): Closes the current page file.
    •	destroyPageFile(): Removes a page file from memory.


Read-Related Functions:
===========================

    •	readBlock(): Reads a block of data from a specified page number.
    •	getBlockPos(): Returns the current page position.
    •	readFirstBlock(), readPreviousBlock(), readCurrentBlock(), readNextBlock(), readLastBlock(): Read blocks relative to the current position.


Write-Related Functions:
===========================

    •	writeBlock(): Writes a block of data to a specified page number.
    •	writeCurrentBlock(): Writes to the current page position.
    •	appendEmptyBlock(): Adds a new empty block to the end of the file.
    •	ensureCapacity(): Ensures the file has enough pages for upcoming writes.


Test Cases 2:
===========================

    •	Additional test cases are located in test_assign_2.c.
    •	Instructions for running these tests are in the README file.

