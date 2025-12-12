#include "io.h"

#include <util/log.h>
#include <stdio.h>
#include <stdlib.h>

char* rt_read_file(const char* file_path) 
{
	/* Open file */
	FILE* file = fopen(file_path, "rb");	
	if (file == NULL) { 
		rt_log(error, "Failed to open file '%s'!", file_path);
		return NULL;
	}

	/* Obtain file size */
	fseek(file, 0L, SEEK_END);
	unsigned long fileSize = (unsigned long)ftell(file);
	rewind(file);

	/* Allocate memory buffer (with +1 for null terminator) */
	char* buffer = (char*)malloc(fileSize + 1);
	if (buffer == NULL) {
		rt_log(error, "Failed to allocate %zu bytes of memory for file '%s'!\n", fileSize + 1, file_path);
		fclose(file);
		return NULL;
	}

	/* Copy file into memory buffer */
	size_t resultSize = fread(buffer, 1, fileSize, file);
	if (resultSize != fileSize) {
		rt_log(error, "Failed to read file!\nExpected %zu, found %zu\n", fileSize, resultSize);
		free(buffer);
		fclose(file);
		return NULL;
	}

	/* Null-terminate the buffer */
	buffer[fileSize] = '\0';

	/* Terminate */
	fclose(file);
	return buffer;
}
