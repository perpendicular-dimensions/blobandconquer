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

#define MULTIPLIER 65599
#define MAX_BUCKETS 256

class Entry : public Linkable {

	public:
	
		long hash;
		
		Linkable *data;
	
	Entry()
	{
		hash = 0;
		data = NULL;
	}
	
	~Entry()
	{
		if (data != NULL)
		{
			delete data;
		}
		
		hash = 0;
		data = NULL;
	}
};

class Hashtable {

	private:

		List bucket[MAX_BUCKETS];

		List list;

	public:

		String name;

	Hashtable();
	~Hashtable();

	void put(const char *key, Linkable *data);
	void put(long hash, Linkable *data);

	void *get(const char *key);
	void *get(long hash);

	void remove(const char *key);
	void remove(long hash);

	List *toList();

	int getSize();

	void clear();

};
