#include "io.h"

#include <util/log.h>
#include <stdio.h>
#include <stdlib.h>

char* ReadFileStr(const char* filePath) 
{
	// Open file
	FILE* file = fopen(filePath, "rb");	
	if (file == NULL) { 
		Log(error, "Failed to open file '%s'!", filePath);
		return NULL;
	}

	// Obtain file size
	fseek(file, 0L, SEEK_END);
	unsigned long fileSize = (unsigned long)ftell(file);
	rewind(file);

	// Allocate memory buffer (with +1 for null terminator)
	char* buffer = (char*)malloc(fileSize + 1);
	if (buffer == NULL) {
		Log(error, "Failed to allocate %zu bytes of memory for file '%s'!\n", fileSize + 1, filePath);
		fclose(file);
		return NULL;
	}

	// Copy file into memory buffer
	size_t resultSize = fread(buffer, 1, fileSize, file);
	if (resultSize != fileSize) {
		Log(error, "Failed to read file!\nExpected %zu, found %zu\n", fileSize, resultSize);
		free(buffer);
		fclose(file);
		return NULL;
	}

	// Null-terminate the buffer
	buffer[fileSize] = '\0';

	// Terminate
	fclose(file);
	return buffer;
}
