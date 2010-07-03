/*
Copyright (C) 2006 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "../headers.h"

Pak::Pak()
{
	dataBuffer = NULL;
	binaryBuffer = NULL;
	input = NULL;
	file = NULL;

	numberOfFiles = 0;
	uncompressedSize = 0;

	lastReadDataSize = 0;

	initPakFile();
}

Pak::~Pak()
{
	if (dataBuffer != NULL)
	{
		delete[] dataBuffer;
	}

	if (binaryBuffer != NULL)
	{
		delete[] binaryBuffer;
	}

	if (input != NULL)
	{
		delete[] input;
	}
	
	if (file != NULL)
	{
		delete[] file;
	}
}

Pak *Pak::getInstance()
{
	return &instance;
}

void Pak::showPakErrorAndExit()
{
	printf("\nFatal Error: The Blob Wars : Blob And Conquer PAK file was either not found or was not accessable.\n");
	printf("The file was expected to be found within,\n\n");
	printf(PAKFULLPATH"\n\n");
	printf("Please try running the game again. If problems persist either reinstall the game or check,\n\n");
	printf("http://www.parallelrealities.co.uk/blobAndConquer.php\n\n");
	printf("for updates.\n\n");
	exit(1);
}

void Pak::initPakFile()
{
	uLongf listPos;

	debug(("Pak : Filename set to %s\n", PAKFULLPATH));

	FILE *pak = fopen(PAKFULLPATH, "rb");

	if (!pak)
	{
		return;
	}

	fseek(pak, (-sizeof(Uint32)) * 2, SEEK_END);
	fread(&listPos, sizeof(Uint32), 1, pak);
	fread(&numberOfFiles, sizeof(Uint32), 1, pak);

	listPos = SDL_SwapLE32(listPos);
	numberOfFiles = SDL_SwapLE32(numberOfFiles);

	debug(("Pak : File list resides at %d\n", (int)listPos));
	debug(("Pak : Number of files are %d\n", (int)numberOfFiles));

	fseek(pak, listPos, SEEK_SET);

	file = new FileData[numberOfFiles];

	FileData fd;

	for (unsigned int i = 0 ; i < numberOfFiles ; i++)
	{
		if (!fread(&fd, sizeof(FileData), 1, pak))
		{
			printf("Pak::setPakFile - UNEXPECTED END OF FILE DATA ENCOUNTERED!\n");
			exit(1);
			break;
		}

		file[i] = fd;
		
		debug(("initPakFile() - %.3d: %s [%ld / %ld]\n", i, fd.name, fd.cSize, fd.fSize));
	}

	fclose(pak);
}

bool Pak::unpack(const char *filename, PAK::TYPE fileType)
{
	unsigned char **buffer = (fileType == PAK::TEXT) ? &dataBuffer : &binaryBuffer;
	
	if (*buffer != NULL)
	{
		delete[] *buffer;
	}
	
	*buffer = NULL;
	
	FileData *fileData = NULL;
	
	for (unsigned int i = 0 ; i < numberOfFiles ; i++)
	{
		if (strcmp(file[i].name, filename) == 0)
		{
			fileData = &file[i];
			break;
		}
	}
	
	if (fileData != NULL)
	{
		debug(("unpack() : Unpacking %s (%ld)\n", filename, lastReadDataSize));
	
		FILE *pak = fopen(PAKFULLPATH, "rb");
	
		if (!pak)
		{
			showPakErrorAndExit();
		}
	
		fseek(pak, fileData->location, SEEK_SET);
	
		if (input != NULL)
		{
			delete[] input;
		}
	
		input = NULL;
	
		input = new unsigned char[(int)(fileData->cSize * 1.01) + 12];
	
		*buffer = new unsigned char[fileData->fSize];
	
		fread(input, 1, fileData->cSize, pak);
	
		fclose(pak);
	
		uncompress(*buffer, &fileData->fSize, input, fileData->cSize);
	
		if (input != NULL)
		{
			delete[] input;
		}
	
		input = NULL;
		
		lastReadDataSize = fileData->fSize;
	}
	else
	{
		FILE *fp = fopen(filename, "rb");

		if (!fp)
		{
			debug(("unpack() : %s does not exist\n", filename));
			return false;
		}

		fseek(fp, 0, SEEK_END);
		lastReadDataSize = ftell(fp);
		
		debug(("unpack() : Reading %s (%ld)\n", filename, lastReadDataSize));
		
		*buffer = new unsigned char[lastReadDataSize + 1];
		rewind(fp);
		fread(*buffer, 1, lastReadDataSize, fp);
		(*buffer)[lastReadDataSize] = 0;
		fclose(fp);
	}

	if (fileType == PAK::BINARY)
	{
		sdlrw = SDL_RWFromConstMem(*buffer, lastReadDataSize);

		if (!sdlrw)
		{
			printf("Fatal Error: SDL_RWops allocation failed\n");
			exit(1);
		}
		
		debug(("unpack() - RWops : %p\n", (void *)sdlrw));
	}
	
	else
	{
		if (strlen((char *)*buffer) > lastReadDataSize)
		{
			((char *)*buffer)[lastReadDataSize - 1] = '\0';
		}
	}
	
	debug(("unpack() : Loaded %s (%ld)\n", filename, lastReadDataSize));

	return true;
}

void Pak::writeToTempFile()
{
	remove(tmpFilename.getText());
	SDL_Delay(50);

	FILE *fp = fopen(tmpFilename.getText(), "wb");
	
	if (!fp)
	{
		printf("ERROR: Pak::writeToTempFile() - Unable to write to '%s'\n", tmpFilename.getText());
		abort();
	}
	
	fwrite(binaryBuffer, 1, lastReadDataSize, fp);
	fclose(fp);
}

bool Pak::exists(const char *filename)
{
	for (unsigned int i = 0 ; i < numberOfFiles ; i++)
	{
		if (strcmp(file[i].name, filename) == 0)
		{
			return true;
		}
	}

	FILE *fp = fopen(filename, "rb");

	if (!fp)
	{
		return false;
	}

	fclose(fp);

	return true;
}

long Pak::getLastReadDataSize()
{
	return lastReadDataSize;
}

void Pak::freeAll()
{
	if (dataBuffer != NULL)
	{
		delete[] dataBuffer;
	}

	if (binaryBuffer != NULL)
	{
		delete[] binaryBuffer;
	}
	
	if (input != NULL)
	{
		delete[] input;
	}
	
	dataBuffer = NULL;
	binaryBuffer = NULL;
	input = NULL;
}

Pak Pak::instance;
