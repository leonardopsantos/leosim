/*
 * linked.hh
 *
 *  Created on: 16 de jun de 2016
 *      Author: lsantos
 */

#ifndef SRC_LINKED_HH_
#define SRC_LINKED_HH_

#include <iostream>

using namespace std;

class listElement {
public:
	int get_data();
	void set_data(int);
	listElement *get_next();
	void set_next(class listElement *el);
	listElement *get_prev();
	void set_prev(class listElement *el);

	listElement();
	listElement(listElement *next);
	listElement(listElement *next, int data);
	listElement(listElement *next, listElement *prev);
	listElement(listElement *next, listElement *prev, int data);

	friend ostream& operator<<(ostream& os, const listElement& d);
	friend ostream& operator<<(ostream& os, const listElement* d);

protected:
	class listElement *next;
	class listElement *prev;
	int data;

	friend class List;
};

class List {
public:
	listElement *get_head();
	void set_head(listElement *el);
	listElement *get_tail();
	void set_tail(listElement *el);
	listElement *get_element_by_data(int data);

	List();
	List(int num);
	List(listElement *head, listElement *tail);
	~List();

	friend ostream& operator<<(ostream& os, const List& d);

protected:
	class listElement *head;
	class listElement *tail;
	bool dynamic_alloc;
};

void linked_main(int size);

#endif /* SRC_LINKED_HH_ */
