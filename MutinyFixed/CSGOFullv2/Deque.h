#pragma once


template <class T>
class Dequeue {
private:
	vector<T> elems;     // elements 

public:
	void push_front(T const&);  // push element 
	void pop_back();               // pop element 
	T top() const;            // return top element 
	bool empty() const {       // return true if empty.
		return elems.empty();
	}
};