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

Hashtable::Hashtable()
{
	name = "Unnamed Hashtable";
}

Hashtable::~Hashtable()
{
	clear();
}

void Hashtable::put(long hash, Linkable *data)
{
	int entryBucket = abs(hash % MAX_BUCKETS);
	
	List *list = &bucket[entryBucket];
	
	for (Entry *entry = (Entry*)list->getFirstElement() ; entry != NULL ; entry = (Entry*)entry->next)
	{
		if (entry->hash == hash)
		{
			printf("WARNING: Hashtable::put - Entry already added\n");
			#if DEV
			//exit(1);
			#endif
			return;
		}
	}

	Entry *entry = new Entry();
	entry->hash = hash;
	entry->data = data;
	
	list->add(entry);
}

void Hashtable::put(const char *key, Linkable *data)
{
	long hash = 0;

	for (unsigned int i = 0 ; i < strlen(key) ; i++)
	{
		hash = hash * MULTIPLIER + key[i];
	}

	put(hash, data);
}

void *Hashtable::get(long hash)
{
	int entryBucket = abs(hash % MAX_BUCKETS);
	
	List *list = &bucket[entryBucket];
	
	for (Entry *entry = (Entry*)list->getFirstElement() ; entry != NULL ; entry = (Entry*)entry->next)
	{
		if (entry->hash == hash)
		{
			return entry->data;
		}
	}

	return NULL;
}

void *Hashtable::get(const char *key)
{
	long hash = 0;

	for (unsigned int i = 0 ; i < strlen(key) ; i++)
	{
		hash = hash * MULTIPLIER + key[i];
	}

	return get(hash);
}

void Hashtable::remove(long hash)
{
	int entryBucket = abs(hash % MAX_BUCKETS);
	
	List *list = &bucket[entryBucket];
	
	for (Entry *entry = (Entry*)list->getFirstElement() ; entry != NULL ; entry = (Entry*)entry->next)
	{
		if (entry->hash == hash)
		{
			entry = (Entry*)entry->previous;
			list->remove(entry->next);
		}
	}
}

void Hashtable::remove(const char *key)
{
	long hash = 0;

	for (unsigned int i = 0 ; i < strlen(key) ; i++)
	{
		hash = hash * MULTIPLIER + key[i];
	}
	
	remove(hash);
}

int Hashtable::getSize()
{
	int count = 0;

	for (int i = 0 ; i < MAX_BUCKETS ; i++)
	{
		count += bucket[i].getSize();
	}

	return count;
}

List *Hashtable::toList()
{
	list.clear();
	List *l;

	Reference *ref;

	for (int i = 0 ; i < MAX_BUCKETS ; i++)
	{
		l = &bucket[i];
	
		for (Entry *entry = (Entry*)l->getFirstElement() ; entry != NULL ; entry = (Entry*)entry->next)
		{
			ref = new Reference();
			ref->object = entry->data;
			list.add(ref);
		}
	}

	return &list;
}

void Hashtable::clear()
{
	list.clear();

	for (int i = 0 ; i < MAX_BUCKETS ; i++)
	{
		bucket[i].clear();
	}
}
