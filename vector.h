#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <iostream>
#include <initializer_list>
#include <algorithm>
#include <functional>
#include <stdexcept>

using namespace std;

template<typename T>
class Vector{
	public:
		class ConstIterator;
		class Iterator;
		using value_type = T;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using reference = value_type&;
		using const_reference = const value_type&;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using iterator = Vector::Iterator;
		using const_iterator = Vector::ConstIterator;

	private:
		pointer values;
		size_type sz;
		size_type max_sz;
		static constexpr size_type min_sz{2};
	public:
	Vector():Vector(min_sz){};

	Vector(const Vector& other):Vector(){
		for (size_type i{0}; i < other.sz; i++)
			push_back(other.values[i]);
	}

	Vector(initializer_list<value_type> ilist):Vector(){
		for (const auto& element : ilist)
			push_back(element);
	}

	Vector operator=(Vector v){
		swap(sz, v.sz);
		swap(max_sz, v.max_sz);
		swap(values, v.values);
		return *this;
	}

	Vector(size_type max){
		sz = 0;
		max_sz = (max > min_sz) ? max : min_sz;
		values = new value_type[max_sz];
	}

	~Vector(){
		delete[] values;
	}

	size_type size() const{
		return sz;
	}

	bool empty() const{
		if(sz==0)
			return true;
		return false;
	}

	void pop_back(){
		if (empty())
			throw runtime_error("Vector ist leer");
		sz--;
		shrink_to_fit();
	}

   	void shrink_to_fit(){
		if (max_sz > sz)
			return;
		change_size(sz < min_sz ? min_sz : sz);
   	}

   	void push_back(value_type n){
		if (sz + 1 > max_sz)
			change_size(2*max_sz);
		values[sz] = n;
		sz++;
   	}

   	void reserve(size_type newsize){
		change_size(newsize);
	}

	void change_size(size_type newsize){
		value_type *buf = new value_type [newsize];
		for (size_type i = 0; i < sz; i++)
			buf[i] = values[i];
		delete[] values;
		values = buf;
		max_sz = newsize;
	}

	iterator insert (const_iterator pos, const_reference val) {
		auto diff = pos-begin();
		if(diff<0 || static_cast<size_type>(diff)>sz)
			throw runtime_error("Iterator out of bounds");
		size_type current{static_cast<size_type>(diff)};
		if(sz>=max_sz)
			reserve(max_sz*2);
		for (size_type i{sz}; i-->current; )
			values[i+1]=values[i];
		values[current]=val;
		++sz;
		return iterator {values+current};
	}

	iterator erase (const_iterator pos){
		auto diff = pos-begin();
		if(diff<0 || static_cast<size_type>(diff)>=sz)
			throw runtime_error("Iterator out of bounds");
		size_type current{static_cast<size_type>(diff)};

		for(size_type i{current}; i<sz-1; ++i)
			values[i]=values[i+1];
		--sz;
		return iterator{values+current};
	}

	void clear(){
		sz = 0;
		shrink_to_fit();
	}

	reference operator[](size_type val){
		if (val < 0 || val >= sz)
			throw runtime_error("Index ist ungueltig");
		return values[val];
	}

	const_reference operator[](size_type val) const{
		if (val <= 0 && val >= sz)
			throw runtime_error("Index ist ungueltig");
		return values[val];
	}

	iterator begin() { return iterator { values}; }
	
	iterator end() { return iterator { values+sz}; }

	iterator mid() { return iterator { values+(sz/2)}; }
	
	const_iterator begin() const { return const_iterator{values}; }
	
	const_iterator end() const { return const_iterator{values+sz}; }

	const_iterator mid() const { return const_iterator { values+(sz/2)}; }

   	ostream& print(ostream& os) const{
		cout << '[';
		bool first{true};
		for(const auto& elem : *this){
			if(first)
			  first = false;
			else
			  os << ", ";
			os << elem;
		}
		os << ']';
	  return os;
   	}

  class ConstIterator{
	public:
		using value_type = Vector::value_type;
		using difference_type = Vector::difference_type;
		using reference = Vector::reference;
		using const_reference = Vector::const_reference;
		using pointer = Vector::pointer;
		using iterator_category = std::forward_iterator_tag;
	private:
		const Vector* v;
		pointer ptr;
	public:
		ConstIterator() { this->ptr=nullptr; }
		
		ConstIterator(pointer ptr) { this->ptr=ptr; }
		
		ConstIterator& operator++() { ptr++; return *this; }
		
		ConstIterator operator++(int) { ConstIterator iter(*this); ++*this; return iter; }

		pointer operator->() { return ptr; }

		ConstIterator operator+(size_type n) {
    		ptr = ptr + n;
    		return *this;
    	//	    		lhs += rhs; // reuse compound assignment
    	//	return lhs; // return the result by value (uses move constructor)
  		}

		friend bool operator!= (const ConstIterator& lop, const ConstIterator& rop) { return (lop.ptr!=rop.ptr); }
		
		friend bool operator== (const ConstIterator& lop, const ConstIterator& rop) { return (lop.ptr==rop.ptr); }

		const_reference operator*() const{return *ptr;}

		friend Vector::difference_type operator-(const Vector::ConstIterator& lop, const Vector::ConstIterator& rop){
				return lop.ptr - rop.ptr;
		}
};

  class Iterator{
	public:
		using value_type = Vector::value_type;
		using difference_type = Vector::difference_type;
		using reference = Vector::reference;
		using const_reference = Vector::const_reference;
		using pointer = Vector::pointer;
		using iterator_category = std::forward_iterator_tag;
	private:
		const Vector* v;
		pointer ptr;
	public:
		Iterator() { this->ptr=nullptr; }
		
		Iterator(pointer ptr) { this->ptr=ptr; }
		
		Iterator& operator++() { ptr++; return *this; }
		
		Iterator operator++(int) { Iterator iter {*this}; ++*this; return iter; }
		
		friend bool operator!= (const Iterator& lop, const Iterator& rop) {
			return static_cast<ConstIterator>(lop)!=rop;
		}

		friend bool operator== (const Iterator& lop, const Iterator& rop) {
			return static_cast<ConstIterator>(lop)==rop;
		}

		Iterator operator+(size_type n) {
    		ptr = ptr + n;
    		return *this;
    	//	    		lhs += rhs; // reuse compound assignment
    	//	return lhs; // return the result by value (uses move constructor)
  		}

		reference operator*() { return *ptr; }

		pointer operator->() { return ptr; }

		friend Vector::difference_type operator-(const Vector::Iterator& lop, const Vector::Iterator& rop){
			return static_cast<ConstIterator>(lop) - rop;
		}

		operator ConstIterator() const{
			return ConstIterator(ptr);
		}
	};
};

template<typename T>
ostream& operator<< (ostream& os, const Vector<T>& V){
	return V.print(os);
}
#endif
