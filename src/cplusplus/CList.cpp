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

List::List()
{
	linkHead.previous = &linkHead;

	linkTail = &linkHead;

	name = "Unnamed List";
	
	size = 0;
}

List::~List()
{
	clear();
}

void List::setName(const char *name)
{
	this->name = name;
}

void List::add(Linkable *link)
{
	linkTail->next = link;
	link->previous = linkTail;
	linkTail = link;
	
	linkTail->next = NULL;
	
	size++;
}

void List::remove(Linkable *link)
{
	if (!link)
	{
		printf("Attempt to remove NULL link\n");
		abort();
	}

	if (linkTail == link)
	{
		linkTail = link->previous;
	}

	link->previous->next = link->next;

	if (link->next != NULL)
	{
		link->next->previous = link->previous;
	}
	
	link->next = link->previous = NULL;
	linkTail->next = NULL;
	
	size--;
	
	delete link;
}

void List::clear()
{
	//debug(("%s::clear()\n", name.getText()));
	
	int count = 0;

	Linkable *link, *link2;

	for (link = linkHead.next ; link != NULL ; link = link2)
	{
		link2 = link->next;
		delete link;
		count++;
	}

	linkHead.next = NULL;
	linkTail = &linkHead;
	
	size = 0;
	
	//debug(("%s::clear() - Done\n", name.getText()));
}

Linkable *List::getFirstElement()
{
	return linkHead.next;
}

int List::getSize()
{
	return size;
}

Linkable *List::getElement(int i)
{
	if (i >= size)
	{
		printf("WARNING - List::getItem %d exceeds max element %d! Returning last!\n", i, size - 1);
		i = size - 1;
	}
	
	int count = 0;
	
	for (Linkable *link = linkHead.next ; link != NULL ; link = link->next)
	{
		if (count == i)
		{
			return link;
		}
		
		count++;
	}
	
	printf("WARNING - List::getItem returning NULL!\n");
	
	return NULL;
}

Linkable *List::getHead()
{
	return &linkHead;
}

Linkable *List::getTail()
{
	return linkTail;
}
