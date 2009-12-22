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

class Pak {

	private:
	
		Pak();
		
		static Pak instance;

		unsigned int numberOfFiles;
		unsigned int uncompressedSize;
		Bytef *input;

		FileData *file;

		String validFileName;
		String filename;

		unsigned long lastReadDataSize;

	void initPakFile();
	void showPakErrorAndExit() GCC_NORETURN;

	public:

		String tmpFilename;
		SDL_RWops *sdlrw;

		unsigned char *binaryBuffer; // used for unzipping
		unsigned char *dataBuffer; // used for unzipping

	~Pak();
	
	static Pak *getInstance();

	bool unpack(const char *filename, PAK::TYPE type);
	void writeToTempFile();
	bool exists(const char *filename);
	long getLastReadDataSize();
	
	void freeAll();

};
