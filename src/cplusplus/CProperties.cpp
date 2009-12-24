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

Properties::Properties()
{
	table.name = "<Anonymous Properies> List";
}

Properties::~Properties()
{
}

void Properties::operator+= (Properties *properties)
{
	List *list = properties->getTable()->toList();
	
	Data *d;
	
	for (Reference *ref = (Reference*)list->getFirstElement() ; ref != NULL ; ref = (Reference*)ref->next)
	{
		d = (Data*)ref->object;
		setProperty(d->getKey(), d->getValue());
	}
}

void Properties::setName(const char *name)
{
	this->name = name;
	
	table.name = name;
}

const char *Properties::getName()
{
	return name.getText();
}

int Properties::getSize()
{
	return table.getSize();
}

bool Properties::hasProperty(const char *key)
{
	return (table.get(key) != NULL);
}

void Properties::setProperty(const char *key_, const char *value_)
{
	String key, value;
	
	key = key_;
	value = value_;

	key.trim();
	value.trim();
	
	Data *data = new Data(key.getText(), value.getText());
	
	table.remove(key.getText());
	table.put(key.getText(), data);
}

void Properties::setProperty(const char *key, int value)
{
	String s;
	s.setText("%d", value);

	setProperty(key, s.getText());
}

void Properties::setProperty(const char *key, long value)
{
	String s;
	s.setText("%d", (int)value);

	setProperty(key, s.getText());
}

void Properties::setProperty(const char *key, unsigned int value)
{
	String s;
	s.setText("%d", value);

	setProperty(key, s.getText());
}

void Properties::setProperty(const char *key, float value)
{
	String s;
	s.setText("%.2f", value);

	setProperty(key, s.getText());
}

void Properties::setProperty(const char *key, double value)
{
	String s;
	s.setText("%.2f", value);

	setProperty(key, s.getText());
}

void Properties::setProperty(const char *key, Vector v)
{
	String s;
	s.setText("%f %f %f", v.x, v.y, v.z);
	
	setProperty(key, s.getText());
}

void Properties::setProperty(const char *key, GLColor c)
{
	String s;
	s.setText("%f %f %f %f", c.color[0], c.color[1], c.color[2], c.color[3]);
	
	setProperty(key, s.getText());
}

void Properties::alterProperty(const char *key, double amount)
{
	double value = getFloat(key, 0);
	value += amount;
	setProperty(key, value);
}

Data *Properties::findProperty(const char *key)
{
	return (Data*)table.get(key);
}

const char *Properties::getString(const char *key, const char *defaultValue)
{
	Data *data = (Data*)table.get(key);
	
	if (data == NULL)
	{
		return defaultValue;
	}
	
	return data->getValue();
}

float Properties::getFloat(const char *key, float defaultValue)
{
	Data *data = (Data*)table.get(key);
	
	if (data == NULL)
	{
		return defaultValue;
	}
	
	return atof(data->getValue());
}

int Properties::getInt(const char *key, int defaultValue)
{
	Data *data = (Data*)table.get(key);
	
	if (data == NULL)
	{
		return defaultValue;
	}
	
	return atoi(data->getValue());
}

Vector Properties::getVector(const char *key, Vector defaultValue)
{
	Vector v;
	
	Data *data = (Data*)table.get(key);
	
	if (data == NULL)
	{
		return defaultValue;
	}
	
	v.set(data->getValue());
	
	return v;
}

Vector Properties::getVector(const char *key)
{
	Vector v;
	
	return getVector(key, v);
}

GLColor Properties::getColor(const char *key)
{
	GLColor c;
	
	Data *data = (Data*)table.get(key);
	
	if (data != NULL)
	{
		c.set(data->getValue());
	}
	
	return c;
}

void Properties::removeProperty(const char *key)
{
	table.remove(key);
}

void Properties::removeProperties(const char *key)
{
	List *list = table.toList();
	
	Data *d;
	
	for (Reference *ref = (Reference*)list->getFirstElement() ; ref != NULL ; ref = (Reference*)ref->next)
	{
		d = (Data*)ref->object;
		
		if (strstr(d->key.getText(), key))
		{
			table.remove(d->key.getText());
		}
	}
}

void Properties::clear()
{
	table.clear();
}

Hashtable *Properties::getTable()
{
	return &table;
}

void Properties::listProperties()
{
	List *list = table.toList();
	
	Data *d;
	
	for (Reference *ref = (Reference*)list->getFirstElement() ; ref != NULL ; ref = (Reference*)ref->next)
	{
		d = (Data*)ref->object;
		
		printf("%s = %s\n", d->getKey(), d->getValue());
	}
}

void Properties::save(FILE *fp)
{
	List *list = table.toList();
	
	if ((name.getText() == NULL) || (name == ""))
	{
		printf("WARNING - Refusing to save unnamed properties!\n");
		return;
	}

	fprintf(fp, "%s {\n", name.getText());
	
	Data *d;
	
	for (Reference *ref = (Reference*)list->getFirstElement() ; ref != NULL ; ref = (Reference*)ref->next)
	{
		d = (Data*)ref->object;
		
		fprintf(fp, "\t%s = %s;\n", d->getKey(), d->getValue());
	}

	fprintf(fp, "};\n\n");
}
