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

#include "pak.h"

FILE *pak;
int dirs = 0, files = 0;
int totalFiles = 0;
Bytef *buffer;
Bytef *output;

FileData *fileData = NULL;

void cleanup()
{
	if (buffer != NULL)
	{
		delete[] buffer;
	}
	
	if (output != NULL)
	{
		delete[] output;
	}
}

void countFiles(char *dirName)
{
	DIR *dirp, *dirp2;
	dirent *dfile;
	dirp = opendir(dirName);
	char filename[1024];

	while ((dfile = readdir(dirp)))
	{
		if (dfile->d_name[0] == '.')
		{
			continue;
		}

		sprintf(filename, "%s/%s", dirName, dfile->d_name);
		
		if (strlen(filename) > PAK_MAX_FILENAME - 1)
		{
			printf("\nERROR - '%s' exceeds maximum defined file length of %d\n", filename, PAK_MAX_FILENAME);
			exit(1);
		}

		dirp2 = opendir(filename);

		if (dirp2)
		{
			closedir(dirp2);
			countFiles(filename);
		}
		else
		{
			totalFiles++;
		}
	}
	
	fileData = new FileData[totalFiles];
}

void recurseDirectory(char *dirName)
{
	DIR *dirp, *dirp2;
	dirent *dfile;
	gzFile fp;
	FILE *infile;
	char filename[1024];
	const char *f;

	uLongf cSize = 0;
	uLongf fSize = 0;

	dirp = opendir(dirName);

	if (dirp == NULL)
	{
		printf("%s: Directory does not exist or is not accessable\n", dirName);
		return;
	}
	
	float percentage;
	long filesize;

	while ((dfile = readdir(dirp)))
	{
		if (dfile->d_name[0] == '.')
		{
			continue;
		}

		sprintf(filename, "%s/%s", dirName, dfile->d_name);

		dirp2 = opendir(filename);

		if (dirp2)
		{
			closedir(dirp2);
			recurseDirectory(filename);
		}
		else
		{
			infile = fopen(filename, "rb");
			if (!infile)
			{
				printf("Couldn't open %s for reading!\n", filename);
				closedir(dirp);
				gzclose(pak);
				exit(1);
			}
			
			fseek(infile, SEEK_SET, SEEK_END);
			
			filesize = ftell(infile);
			
			fclose(infile);
			
			if (buffer != NULL)
			{
				delete[] buffer;
				buffer = NULL;
			}
			
			buffer = new unsigned char[filesize];
			
			if (output != NULL)
			{
				delete[] output;
				output = NULL;
			}
			
			output = new unsigned char[(int)(filesize * 1.01) + 12];
			
			fp = gzopen(filename, "rb");

			if (!fp)
			{
				printf("Couldn't open %s for reading!\n", filename);
				closedir(dirp);
				gzclose(pak);
				exit(1);
			}
			else
			{
				fSize = gzread(fp, buffer, filesize);
				gzclose(fp);

				cSize = (uLongf)((fSize * 1.01) + 12);
				compress2(output, &cSize, buffer, fSize, 9);
				
				f = filename;
				
				fileData[files].set(f, fSize, cSize, ftell(pak));

				fwrite(output, 1, cSize, pak);

				files++;
				
				percentage = files;
				percentage /= totalFiles;
				percentage *= 100;

				printf("\b\b\b\b%3.0f%%", percentage);
				fflush(stdout);
			}
		}
	}

	closedir(dirp);

	dirs++;
}

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Usage   : pak <directory names> <outputname>\n");
		printf("Example : pak data music gfx sound data.pak\n");
		exit(1);
	}

	pak = fopen(argv[argc - 1], "wb");
	
	for (int i = 1 ; i < (argc - 1) ; i++)
	{
		countFiles(argv[i]);
	}
		
	printf("Paking...000%%");
	fflush(stdout);
	
	output = NULL;
	buffer = NULL;
	
	atexit(cleanup);

	for (int i = 1 ; i < (argc - 1) ; i++)
	{
		recurseDirectory(argv[i]);
	}

	unsigned int pos = ftell(pak);

	for (int i = 0 ; i < files ; i++)
	{
		if (fileData[i].fSize == 0)
		{
			break;
		}

		fwrite(&fileData[i], sizeof(FileData), 1, pak);
	}
	
	unsigned int numberOfFiles = totalFiles;
	
	pos = SDL_SwapLE32(pos);
	numberOfFiles = SDL_SwapLE32(numberOfFiles);

	fwrite(&pos, sizeof(unsigned int), 1, pak);
	fwrite(&numberOfFiles, sizeof(unsigned int), 1, pak);

	fclose(pak);

	printf("\nPak: All Done. Added %d files\n", numberOfFiles);

	return 0;
}
