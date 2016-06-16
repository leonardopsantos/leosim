/*
 * linked.cc
 *
 *  Created on: 16 de jun de 2016
 *      Author: lsantos
 */


/*
 * linked.cpp
 *
 *  Created on: 22 de mar de 2016
 *      Author: lsantos
 */

#include <iostream>
#include <stdlib.h>     /* atoi */

#include "sim.hh"
#include "sim_features.hh"
#include "linked.hh"

using namespace std;

int listElement::get_data() {
	 return this->data;
}

void listElement::set_data(int d) {
	 this->data = d;
}

listElement* listElement::get_next() {
	 return this->next;
}

void listElement::set_next(class listElement* el) {
	 this->next = el;
}

listElement* listElement::get_prev() {
	 return this->prev;
}

void listElement::set_prev(class listElement* el) {
	 this->prev = el;
}

listElement::listElement() {
	 this->data = 0;
	 this->next = NULL;
	 this->prev = NULL;
}

listElement::listElement(listElement* next) {
	 this->data = 0;
	 this->next = next;
	 this->prev = NULL;
}

listElement::listElement(listElement* next, int data) {
	 this->data = data;
	 this->next = next;
	 this->prev = NULL;
}

listElement::listElement(listElement* next,
		listElement* prev) {
	 this->data = 0;
	 this->next = next;
	 this->prev = prev;
}

listElement::listElement(listElement* next,
		listElement* prev, int data) {
	 this->data = data;
	 this->next = next;
	 this->prev = prev;
}

listElement* List::get_head() {
	 return this->head;
}

ostream& operator<<(ostream& os, const listElement& d)
{
	os << d.prev << "->[ " << d.data << " ]->" << d.next << "\n";
    return os;
}

ostream& operator<<(ostream& os, const listElement* d)
{
	void *p = d->prev;
	void *n = d->next;
	const void *k = d;
	if( d != NULL )
		os << p << "->["<< k << ": " << d->data << " ]->" << n << "\n";
    return os;
}

void List::set_head(class listElement* el) {
	 this->head = el;
}

listElement* List::get_tail() {
	 return this->tail;
}

void List::set_tail(listElement* el) {
	this->tail = el;
}

List::List() {
	this->head = NULL;
	this->tail = NULL;
	this->dynamic_alloc = false;
}

List::List(int num) {

	int i;
	listElement* el, *next;

	el = new listElement(0);
	this->head = el;

	for(i = 1; i < num; i++) {
		next = new listElement((listElement*) NULL, el, i);
		el->set_next(next);
		el = next;
	}
	this->tail = next;
	this->dynamic_alloc = true;
}

List::List(class listElement* head, class listElement* tail) {
	this->head = head;
	this->tail = tail;
	this->dynamic_alloc = false;
}

List::~List() {
	if( this->dynamic_alloc != true )
		return;

	listElement *n;
	listElement *h = this->head;
	while (h != this->tail) {
		n = h->next;
		delete h;
		h = n;
	}
}

listElement* List::get_element_by_data(int data) {
	listElement *el = this->head;
	while (el != this->tail) {
		if( el->data == data )
			return el;
		el = el->get_next();
	}
	if( el->data == data )
		return el;

	return NULL;
}

ostream& operator<<(ostream& os, const List& d)
{
	cout << "List:" << endl;
	cout << "    Head = " << (void*) d.head << ", Tail: " << (void*) d.tail << endl;
	listElement *el = d.head;
	while (el != d.tail) {
		cout << "    " << el;
		el = el->get_next();
	}
	cout << el;
    return os;
}

void linked_main(int size)
{
	ifstream infile("apps/linked.S");

	if( infile.is_open() == false ) {
		cout << "Whoa!! Can't open file " << "apps/linked.S" <<
				", cowardly giving up...\n";
	}

	simulator leosim;
	if( leosim.setup(infile) < 0 ) {
		cout << "Whoa!! Can't setup simulator! cowardly giving up...\n";
	}

	return;

	List list = List(size);

	cout << list;

	listElement *el = list.get_element_by_data(size-1);

	leosim.system.cpu.register_bank[0] = (unsigned long int) list.get_head();
	leosim.system.cpu.register_bank[2] = (unsigned long int) list.get_tail();
	leosim.system.cpu.register_bank[1] = (long int) el->get_data();

	leosim.run();

	return;
}
