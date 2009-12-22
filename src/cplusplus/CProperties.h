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

class Properties : public Linkable {

	private:

		Hashtable table;

	Data *findProperty(const char *key);

	public:

		String name;

	Properties();
	~Properties();

	void operator +=(Properties *properties);

	void setName(const char *name);
	const char *getName();

	int getSize();
	
	bool hasProperty(const char *key);

	void setProperty(const char *key, const char *value);
	void setProperty(const char *key, int value);
	void setProperty(const char *key, long value);
	void setProperty(const char *key, unsigned int value);
	void setProperty(const char *key, float value);
	void setProperty(const char *key, double value);
	void setProperty(const char *key, Vector v);
	void setProperty(const char *key, GLColor c);
	
	void alterProperty(const char *key, double value);

	const char *getString(const char *key, const char *defaultValue);
	float getFloat(const char *key, float defaultValue);
	int getInt(const char *key, int defaultValue);
	Vector getVector(const char *key);
	Vector getVector(const char *key, Vector defaultValue);
	GLColor getColor(const char *key);
	
	void removeProperty(const char *key);
	void removeProperties(const char *key);
	void clear();

	Hashtable *getTable();

	void save(FILE *fp);

	void listProperties();
};
