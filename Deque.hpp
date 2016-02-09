#include <stddef.h>

#ifndef DEQUE_H
#define DEQUE_H

const int element_count = 40000000;//number of elements that a particular block of memory, that is allocated, can store.
const int load_factor = 100;//load factor that is used to decide whether or not to allocate more memory.
const int number_of_arrays = 2;//number of arrays collectively behaving as a single deque.

//macro to calculate the block size to be allocated
#define block_size(type) (sizeof(type)*element_count)

//macro to allocate initial memory to both the arrays
#define alloc(type) (type*)malloc(block_size(type))

#define Deque_DEFINE(type)\
  typedef struct Deque_##type Deque_##type;\
  typedef struct Deque_##type##_Iterator Deque_##type##_Iterator;\
  \
  struct Deque_##type;\
  /*
  creating Deque struct
  data:
    type_name --> name of the type. Example: "Deque_int".
    capacity_f --> current capacity of the left array.
    capacity_b --> current capacity of the right array.
    load_f --> current load of the left array.
    load_b --> current load of the right array.
    values_f --> contains values in the front array (left array that grows in reverse)
    values_b -->  contains values in the back array (right array)
    front_f --> front of values_f
    back_f --> back of values_f
    front_b --> front of values_b
    back_b --> back of values_b
    num_elements --> number of elements in the deque

    function:
      type##_comp --> comparator for the elements in the deque
      empty --> check whether the deque is empty
      at --> returns element at specified position
      size --> return the num_elements of the deque
      push_back --> insert element at the back of the deque
      push_front --> insert element at the front of the deque
      pop_back --> remove element at the back of the deque
      pop_front --> remove element at the front of the deque
      front --> return the element at the front of the deque
      back --> return the element at the back of the deque
      begin --> returns an iterator which is at the front of the deque
      end --> returns an iterator which is at a position past the deque (value = -1, index = -1)
      clear --> sets the front_f, front_b, back_f, back_b to -1 and also sets the num_elements to 0
      dtor --> frees all memory that has been allocated
  */\
  struct Deque_##type{\
    const char type_name[sizeof("Deque_"#type)] = "Deque_"#type;\
    int deque_capacity_f, deque_capacity_b;\
    int load_f, load_b;\
    type* values_f;\
    type* values_b;\
    int front_f, front_b, back_f, back_b;\
    int num_elements;\
    \
    bool (*type##_comp)(const type& o1, const type& o2);\
    bool (*empty)(Deque_##type* deq);\
    type& (*at)(Deque_##type* deq, int pos);\
    int (*size)(Deque_##type* deq);\
    void (*push_back)(Deque_##type* deq, type value);\
    void (*push_front)(Deque_##type* deq, type value);\
    void (*pop_back)(Deque_##type* deq);\
    void (*pop_front)(Deque_##type* deq);\
    type& (*front)(Deque_##type* deq);\
    type& (*back)(Deque_##type* deq);\
    Deque_##type##_Iterator (*begin)(Deque_##type* deq);\
    Deque_##type##_Iterator (*end)(Deque_##type* deq);\
    /*clears the deque by resetting all the parameters to their initial value.*/\
    void clear(Deque_##type* deq){\
      deq->front_f = -1;\
      deq->back_f = -1;\
      deq->front_b = -1;\
      deq->back_b = -1;\
      deq->num_elements = 0;\
      deq->load_f = 0;\
      deq->load_b = 0;\
      deq->deque_capacity_f = 0;\
      deq->deque_capacity_b = 0;\
    }\
    \
    /*destructor*/\
    void dtor(Deque_##type* deq){\
      free(deq->values_f);\
      free(deq->values_b);\
    }\
  };\
  \
  /*check whether the overall front of the deque is present in the left array or not.*/\
  bool is_values_f_begin(Deque_##type* deq){\
    if(deq->front_f > -1){\
      return true;\
    }\
    return false;\
  }\
  /*
  creating the Iterator struct
  data:
    value --> current value of the iterator.
    current_index --> current index of the iterator in the deque.
    deq --> pointer to the deque.   

  functions pointers:
  is_values_f_begin --> checks if the front of the deque is in values_f
  at --> moves the iterator to the corresponding position and sets the iterator value
  inc --> increments the iterator to the next element in the deque
  dec --> decrements the iterator to the next element in the deque
  deref --> returns the value at the current index of the iterator
  */\
  struct Deque_##type##_Iterator{\
    type* value;\
    int current_index;\
    Deque_##type* deq;\
    \
    bool (*is_values_f_begin)(Deque_##type* deq);\
    void (*at)(Deque_##type* deq, Deque_##type##_Iterator* iter, int pos);\
    void (*inc)(Deque_##type##_Iterator* iter);\
    void (*dec)(Deque_##type##_Iterator* iter);\
    type& (*deref)(Deque_##type##_Iterator* iter);\
  };\
  /*creating begin and end iterators*/\
  Deque_##type##_Iterator *begin_iter_##type, *end_iter_##type;\
  \
  /*checks whether the deque is empty*/\
  bool empty(Deque_##type* deq){\
    if(deq->front_f == -1 && deq->front_b == -1){\
      return true;\
    }\
    return false;\
  }\
  \
  /*returns number of elements in the deque*/\
  int size(Deque_##type* deq){\
    return deq->num_elements;\
  }\
  \
  /*pushes value to the back of the deque*/\
  void push_back(Deque_##type* deq, type value){\
    /*
    if deq is empty then,
      set front_b = back_b to 0
      increment load_b
      insert value at values_b[back_b]
      increment number of elements
    else
      if front_b == -1 && back_f > 0 then,
        decrement back_f
        insert value at values_f[back_f]
      else
        if load_b has been reached then,
		reallocate memory to the right array
		increase capacity_b by element count
	endif
        if front_b == -1 then,
          increment front_b
        endif
        increment back_b
        insert value at values_b[back_b]
      endif
    endif
    increment number of elements
    */\
    if(deq->empty(deq)){\
      deq->front_b = 0; deq->back_b = 0;\
      deq->load_b++;\
      deq->values_b[deq->front_b] = value;\
      deq->num_elements++;\
      return;\
    }\
    else{\
      if(deq->front_b == -1 && deq->back_f > 0){\
        deq->values_f[--deq->back_f] = value;\
      }\
      else{\
	if(deq->load_b == (((load_factor/100.0)*deq->deque_capacity_b))-1){\
		/*need to reallocate memory for values_b*/\
		int b_size = block_size(type);\
		int cur_bytes = deq->deque_capacity_b * sizeof(type);\
		deq->values_b = (type*)realloc(deq->values_b, (cur_bytes+b_size));\
		deq->deque_capacity_b += element_count;\
	}\
        if(deq->front_b == -1){\
          deq->front_b++;\
        }\
	deq->load_b++;\
        deq->values_b[++deq->back_b] = value;\
      }\
    }\
    deq->num_elements++;\
  }\
  \
  /*pushes value to the front of the deque*/\
  void push_front(Deque_##type* deq, type value){\
    /*
    if deq is empty then,
      set front_b = back_b to 0
      insert value at values_b[back_b]
      incrment load_f
      increment number of elements
    else
      if front_f == back_f == -1 then,
        if front_b > 0 then,
          decrement front_b
          insert value at values_b[front_b];
        else
          set front_f = back_f = 0
          insert value at values_f[front_f]
        endif
      else
	if load_f has been reached then,
		need to reallocate memory to the left array
		increase capacity_f by element count
	endif
        increment front_f
        insert value at values_f[front_f]
      endif
    endif
    increment number of elements
    */\
    if(deq->empty(deq)){\
      deq->front_f = 0; deq->back_f = 0;\
      deq->values_f[deq->back_f] = value;\
      deq->load_f++;\
      deq->num_elements++;\
      return;\
    }\
    else{\
      if(deq->front_f == -1 && deq->back_f == -1){\
        if(deq->front_b > 0){\
          deq->values_b[--deq->front_b] = value;\
        }\
        else{\
          deq->front_f = 0; deq->back_f = 0;\
          deq->values_f[deq->front_f] = value;\
        }\
      }\
      else{\
	if(deq->load_f == (((load_factor/100.0)*deq->deque_capacity_f)-1)){\
		int b_size = block_size(type);\
		int cur_bytes = deq->deque_capacity_f * sizeof(type);\
		deq->values_f = (type*)realloc(deq->values_f, (cur_bytes+b_size));\
		deq->deque_capacity_f += element_count;\
	}\
	deq->load_f++;\
        deq->values_f[++deq->front_f] = value;\
      }\
    }\
    deq->num_elements++;\
  }\
  \
  /*removes the element at the back of the deque*/\
  void pop_back(Deque_##type* deq){\
    /*
    if deque is empty then,
	return;
    endif
    if back_b > -1 then,
	if back_b == 0 then,
	  set front_b to -1
	  set back_b to -1
        else
	  decrement load_b
	  decrement back_b
	endif
    else
	if front_f == back_f
	  set front_f to -1
	  set back_f to -1
	else
	  increment back_f
	endif
    endif
    decrement number of elements
    */\
    if(deq->empty(deq)){\
	return;\
    }\
    if(deq->back_b > -1){\
      if(deq->back_b == 0){\
        deq->front_b = -1;\
        deq->back_b = -1;\
      }\
      else{\
	deq->load_b--;\
        deq->back_b--;\
      }\
    }\
    else{\
      if(deq->front_f == deq->back_f){\
        deq->front_f = -1;\
        deq->back_f = -1;\
      }\
      else{\
        deq->back_f++;\
      }\
    }\
    deq->num_elements--;\
  }\
  \
  /*removes the element at the front of the deque*/\
  void pop_front(Deque_##type* deq){\
    /*
    if deque is empty then,
	return
    endif
    if front_f > -1 then
      if front_f == 0 then
        set front_f to -1
        set back_f to -1
      else
        decrement front_f
      endif
    else
      if front_b == back_b then
        set front_b to -1
        set back_b to -1
      else
        increment front_b
      endif
    endif
    decrement number of elements
    */\
    if(deq->empty(deq)){\
	return;\
    }\
    if(deq->front_f > -1){\
      if(deq->front_f == 0){\
        deq->front_f = -1;\
        deq->back_f = -1;\
      }\
      else{\
	deq->load_f--;\
        deq->front_f--;\
      }\
    }\
    else{\
      if(deq->front_b == deq->back_b){\
        deq->front_b = -1;\
        deq->back_b = -1;\
      }\
      else{\
        deq->front_b++;\
      }\
    }\
    deq->num_elements--;\
  }\
  \
  /*returns the element at the fron to deque*/\
  type& front(Deque_##type* deq){\
    if(deq->front_f == -1){\
      return deq->values_b[deq->front_b];\
    }\
    return deq->values_f[deq->front_f];\
  }\
  \
  /*returns the element at the back of the deque*/\
  type& back(Deque_##type* deq){\
    if(deq->back_b == -1){\
      return deq->values_f[deq->back_f];\
    }\
    return deq->values_b[deq->back_b];\
  }\
  \
  /*returns element at a particular position in the deque. Position can range from 0 to (number of elements - 1)*/\
  type& at(Deque_##type* deq, int pos){\
    bool ivfe = is_values_f_begin(deq);\
    int front_f = deq->front_f;\
    int front_b = deq->front_b;\
    int rem;\
    if(pos > front_f){\
      if(front_f == -1){\
        rem = pos;\
      }\
      else{\
        rem = pos - front_f - 1;\
      }\
    }\
    else{\
      rem = -1;\
    }\
    \
    if(ivfe){\
      if(rem != -1){\
        return deq->values_b[rem];\
      }\
      else{\
        return deq->values_f[front_f - pos];\
      }\
    }\
    else{\
      return deq->values_b[front_b + rem];\
    }\
  }\
  \
  /*overload at() function for the iterator that is used by the iterator to traverse the deque*/\
  void at_iter(Deque_##type* deq, Deque_##type##_Iterator* iter, int pos){\
    if(pos > (deq->num_elements-1) || pos < 0){\
      iter->current_index = -1;\
      return;\
    }\
    bool ivfe = is_values_f_begin(deq);\
    int front_f = deq->front_f;\
    int front_b = deq->front_b;\
    int rem;\
    if(pos > front_f){\
      if(front_f == -1){\
        rem = pos;\
      }\
      else{\
        rem = pos - front_f - 1;\
      }\
    }\
    else{\
      rem = -1;\
    }\
    if(ivfe){\
      if(rem != -1){\
        iter->value = &deq->values_b[rem];\
      }\
      else{\
        iter->value = &deq->values_f[front_f - pos];\
      }\
    }\
    else{\
      iter->value = &deq->values_b[front_b + rem];\
    }\
  }\
  \
  /*moves the iterator ahead to the next element in the deque*/\
  void inc(Deque_##type##_Iterator* iter){\
    at_iter(iter->deq, iter, ++iter->current_index);\
  }\
  \
  /*moves the iterator back to the previous element in the deque*/\
  void dec(Deque_##type##_Iterator* iter){\
    if(iter->current_index == -1){\
      iter->current_index = iter->deq->num_elements;\
    }\
    at_iter(iter->deq, iter, --iter->current_index);\
  }\
  \
  /*returns the value present in the deque at the current position of the iterator*/\
  type& deref(Deque_##type##_Iterator* iter){\
    return *iter->value;\
  }\
  \
  /*check whether 2 Iterators are equal*/\
  bool Deque_##type##_Iterator_equal(Deque_##type##_Iterator iter1, Deque_##type##_Iterator iter2){\
    if(iter1.current_index != iter2.current_index){\
      return false;\
    }\
    if(iter1.current_index == -1 && iter2.current_index == -1){\
      return true;\
    }\
    return (!(iter1.deq->type##_comp(*iter1.value, *iter2.value)) && !(iter1.deq->type##_comp(*iter2.value, *iter1.value)));\
  }\
  \
  /*initialise the iterator to the front of the deque*/\
  Deque_##type##_Iterator begin(Deque_##type* deq){\
    begin_iter_##type = new Deque_##type##_Iterator;\
    type& iter_front_value = deq->front(deq);\
    begin_iter_##type->value = &iter_front_value;\
    begin_iter_##type->current_index = 0;\
    begin_iter_##type->deq = deq;\
    \
    begin_iter_##type->is_values_f_begin = is_values_f_begin;\
    begin_iter_##type->at = at_iter;\
    begin_iter_##type->inc = inc;\
    begin_iter_##type->dec = dec;\
    begin_iter_##type->deref = deref;\
    return *begin_iter_##type;\
  }\
  \
  /*intialise the iterator to one position after the last element of the deque. The iterator parameters are basically set to default values*/\
  Deque_##type##_Iterator end(Deque_##type* deq){\
    end_iter_##type = new Deque_##type##_Iterator;\
    end_iter_##type->value = 0;\
    end_iter_##type->current_index = -1;\
    end_iter_##type->deq = deq;\
    \
    end_iter_##type->at = at_iter;\
    end_iter_##type->inc = inc;\
    end_iter_##type->dec = dec;\
    end_iter_##type->deref = deref;\
    return *end_iter_##type;\
  }\
  \
  /*construct the deque object. Set the parameters and assign the function pointers to the corresponding functions*/\
  void Deque_##type##_ctor(Deque_##type* deq, bool (*fptr)(const type& o1, const type& o2)){\
    deq->values_f = alloc(type);\
    deq->values_b = alloc(type);\
    deq->deque_capacity_f = element_count;\
    deq->deque_capacity_b = element_count;\
    deq->load_f = 0;\
    deq->load_b = 0;\
    deq->front_f = -1;\
    deq->front_b = -1;\
    deq->back_f = -1;\
    deq->back_b = -1;\
    deq->num_elements = 0;\
    deq->type##_comp = fptr;\
    /*assigning function pointers*/\
    deq->empty = empty;\
    deq->at = at;\
    deq->size = size;\
    deq->push_back = push_back;\
    deq->push_front = push_front;\
    deq->pop_back = pop_back;\
    deq->pop_front = pop_front;\
    deq->front = front;\
    deq->back = back;\
    deq->begin = begin;\
    deq->end = end;\
  }\
  \
  /*check whether 2 deques are equal*/\
  bool Deque_##type##_equal(Deque_##type& deq1, Deque_##type& deq2){\
    if(deq1.num_elements != deq2.num_elements){\
      return false;\
    }\
    /*looping over both the arrays and comparing each element*/\
    for(int i = 0; i < deq1.num_elements; i++){\
      type& deq1_element = deq1.at(&deq1, i);\
      type& deq2_element = deq2.at(&deq2, i);\
      if(deq1.type##_comp(deq1_element, deq2_element) || (deq1.type##_comp(deq2_element, deq1_element))){\
        return false;\
      }\
    }\
    return true;\
  }\

#endif
