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

String::String()
{
	text = NULL;
	capacity = 0;
	reallocate(64);
}

String::~String()
{
	if (text != NULL)
	{
		delete[] text;
	}
	
	text = NULL;
}

void String::reallocate(int size)
{
	if (size < capacity)
	{
		return;
	}
	
	if (text != NULL)
	{
		delete[] text;
	}
	
	capacity = size + 1;
	text = new char[capacity];
	memset(text, 0, capacity);
}

void String::setText(const char *in, ...)
{
	char tmpString[4096];
	strcpy(tmpString, "");
	
	va_list argp;
	va_start(argp, in);
	vsnprintf(tmpString, sizeof(tmpString), in, argp);
	va_end(argp);
	
	reallocate(strlen(tmpString));
	
	strcpy(text, tmpString);
}

void String::operator= (const char *in)
{
	if(text == in)
		return;

	if (!in)
		abort();

	reallocate(strlen(in));
	
	strcpy(text, in);
}

void String::operator= (String s)
{
	abort();
}

void String::operator= (String *s)
{
	abort();
}

bool String::operator== (const char *in)
{
	return (strcasecmp(text, in) == 0);
}

bool String::operator!= (const char *in)
{
	return (strcasecmp(text, in) != 0);
}

const char *String::getText()
{
	return text;
}

void String::trim()
{
	int c = 0;
	int start = 0;
	int end = strlen(text) - 1;

	if(end < 0)
		return;

	char *newText = new char[capacity];
	
	memset(newText, 0, capacity);
	strcpy(newText, text);
	
	while (start <= end && isspace(text[start]))
	{
		start++;
	}
	
	while (end >= 0 && isspace(text[end]))
	{
		end--;
	}
	
	memset(text, 0, capacity);
	
	for (int i = start ; i < end + 1 ; i++)
	{
		text[c++] = newText[i];
	}
	
	delete[] newText;
}

void String::toLowerCase()
{
	for (unsigned int i = 0 ; i < strlen(text) ; i++)
	{
		text[i] = tolower(text[i]);
	}
}

void String::captialise()
{
	bool upper = true;
	
	for (unsigned int i = 0 ; i < strlen(text) ; i++)
	{
		if (upper)
		{
			text[i] = toupper(text[i]);
			upper = false;
		}
		
		if (text[i] == ' ')
		{
			upper = true;
		}
	}
}
