// Components for manipulating sequences of characters -*- C++ -*-

// Copyright (C) 1997-2015 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

#ifndef _AICUDA_STL_LIST_H_
#define _AICUDA_STL_LIST_H_ 1

#include <aicuda_stl_allocator.h>
#include <aicuda_stl_iterator.h>

namespace aicuda {
namespace stl {

struct _List_node_base {
  _List_node_base *_M_next;
  _List_node_base *_M_prev;

  __device__ static void swap(_List_node_base &__x, _List_node_base &__y) {
    if (__x._M_next != &__x) {
      if (__y._M_next != &__y) {
        // Both __x and __y are not empty.
        aicuda::stl::swap(__x._M_next, __y._M_next);
        aicuda::stl::swap(__x._M_prev, __y._M_prev);
        __x._M_next->_M_prev = __x._M_prev->_M_next = &__x;
        __y._M_next->_M_prev = __y._M_prev->_M_next = &__y;
      } else {
        // __x is not empty, __y is empty.
        __y._M_next = __x._M_next;
        __y._M_prev = __x._M_prev;
        __y._M_next->_M_prev = __y._M_prev->_M_next = &__y;
        __x._M_next = __x._M_prev = &__x;
      }
    } else if (__y._M_next != &__y) {
      // __x is empty, __y is not empty.
      __x._M_next = __y._M_next;
      __x._M_prev = __y._M_prev;
      __x._M_next->_M_prev = __x._M_prev->_M_next = &__x;
      __y._M_next = __y._M_prev = &__y;
    }
  }

  __device__ void transfer(_List_node_base *const __first,
                           _List_node_base *const __last) {
    if (this != __last) {
      // Remove [first, last) from its old position.
      __last->_M_prev->_M_next = this;
      __first->_M_prev->_M_next = __last;
      this->_M_prev->_M_next = __first;

      // Splice [first, last) into its new position.
      _List_node_base *const __tmp = this->_M_prev;
      this->_M_prev = __last->_M_prev;
      __last->_M_prev = __first->_M_prev;
      __first->_M_prev = __tmp;
    }
  }

  __device__ void reverse() {
    _List_node_base *__tmp = this;
    do {
      aicuda::stl::swap(__tmp->_M_next, __tmp->_M_prev);

      // Old next node is now prev.
      __tmp = __tmp->_M_prev;
    } while (__tmp != this);
  }

  __device__ void hook(_List_node_base *const __position) {
    this->_M_next = __position;
    this->_M_prev = __position->_M_prev;
    __position->_M_prev->_M_next = this;
    __position->_M_prev = this;
  }

  __device__ void unhook() {
    _List_node_base *const __next_node = this->_M_next;
    _List_node_base *const __prev_node = this->_M_prev;
    __prev_node->_M_next = __next_node;
    __next_node->_M_prev = __prev_node;
  }
};

template <typename _Tp>
struct _List_node : public _List_node_base {
  _Tp _M_data;
};

template <typename _Tp>
struct _List_iterator {
  typedef _List_iterator<_Tp> _Self;
  typedef _List_node<_Tp> _Node;

  typedef ptrdiff_t difference_type;
  typedef aicuda::stl::bidirectional_iterator_tag iterator_category;
  typedef _Tp value_type;
  typedef _Tp *pointer;
  typedef _Tp &reference;

  __device__ _List_iterator() : _M_node() {}

  __device__ explicit _List_iterator(_List_node_base *__x) : _M_node(__x) {}

  __device__ reference operator*() const {
    return static_cast<_Node *>(_M_node)->_M_data;
  }

  __device__ pointer operator->() const {
    return &static_cast<_Node *>(_M_node)->_M_data;
  }

  __device__ _Self &operator++() {
    _M_node = _M_node->_M_next;
    return *this;
  }

  __device__ _Self operator++(int) {
    _Self __tmp = *this;
    _M_node = _M_node->_M_next;
    return __tmp;
  }

  __device__ _Self &operator--() {
    _M_node = _M_node->_M_prev;
    return *this;
  }

  __device__ _Self operator--(int) {
    _Self __tmp = *this;
    _M_node = _M_node->_M_prev;
    return __tmp;
  }

  __device__ bool operator==(const _Self &__x) const {
    return _M_node == __x._M_node;
  }

  __device__ bool operator!=(const _Self &__x) const {
    return _M_node != __x._M_node;
  }

  _List_node_base *_M_node;
};

template <typename _Tp>
struct _List_const_iterator {
  typedef _List_const_iterator<_Tp> _Self;
  typedef const _List_node<_Tp> _Node;
  typedef _List_iterator<_Tp> iterator;

  typedef ptrdiff_t difference_type;
  typedef aicuda::stl::bidirectional_iterator_tag iterator_category;
  typedef _Tp value_type;
  typedef const _Tp *pointer;
  typedef const _Tp &reference;

  __device__ _List_const_iterator() : _M_node() {}

  __device__ explicit _List_const_iterator(const _List_node_base *__x)
      : _M_node(__x) {}

  __device__ _List_const_iterator(const iterator &__x) : _M_node(__x._M_node) {}

  __device__ reference operator*() const {
    return static_cast<_Node *>(_M_node)->_M_data;
  }

  __device__ pointer operator->() const {
    return &static_cast<_Node *>(_M_node)->_M_data;
  }

  __device__ _Self &operator++() {
    _M_node = _M_node->_M_next;
    return *this;
  }

  __device__ _Self operator++(int) {
    _Self __tmp = *this;
    _M_node = _M_node->_M_next;
    return __tmp;
  }

  __device__ _Self &operator--() {
    _M_node = _M_node->_M_prev;
    return *this;
  }

  __device__ _Self operator--(int) {
    _Self __tmp = *this;
    _M_node = _M_node->_M_prev;
    return __tmp;
  }

  __device__ bool operator==(const _Self &__x) const {
    return _M_node == __x._M_node;
  }

  __device__ bool operator!=(const _Self &__x) const {
    return _M_node != __x._M_node;
  }

  const _List_node_base *_M_node;
};

template <typename _Val>
__device__ inline bool operator==(const _List_iterator<_Val> &__x,
                                  const _List_const_iterator<_Val> &__y) {
  return __x._M_node == __y._M_node;
}

template <typename _Val>
__device__ inline bool operator!=(const _List_iterator<_Val> &__x,
                                  const _List_const_iterator<_Val> &__y) {
  return __x._M_node != __y._M_node;
}

template <typename _Tp, typename _Alloc>
class _List_base {
 protected:
  typedef
      typename _Alloc::template rebind<_List_node<_Tp>>::other _Node_alloc_type;

  typedef typename _Alloc::template rebind<_Tp>::other _Tp_alloc_type;

  struct _List_impl : public _Node_alloc_type {
    _List_node_base _M_node;

    __device__ _List_impl() : _Node_alloc_type(), _M_node() {}

    __device__ _List_impl(const _Node_alloc_type &__a)
        : _Node_alloc_type(__a), _M_node() {}
  };

  _List_impl _M_impl;

  __device__ _List_node<_Tp> *_M_get_node() {
    return _M_impl._Node_alloc_type::allocate(1);
  }

  __device__ void _M_put_node(_List_node<_Tp> *__p) {
    _M_impl._Node_alloc_type::deallocate(__p, 1);
  }

 public:
  typedef _Alloc allocator_type;

  __device__ _Node_alloc_type &_M_get_Node_allocator() {
    return *static_cast<_Node_alloc_type *>(&this->_M_impl);
  }

  __device__ const _Node_alloc_type &_M_get_Node_allocator() const {
    return *static_cast<const _Node_alloc_type *>(&this->_M_impl);
  }

  __device__ _Tp_alloc_type _M_get_Tp_allocator() const {
    return _Tp_alloc_type(_M_get_Node_allocator());
  }

  __device__ allocator_type get_allocator() const {
    return allocator_type(_M_get_Node_allocator());
  }

  __device__ _List_base() : _M_impl() { _M_init(); }

  __device__ _List_base(const allocator_type &__a) : _M_impl(__a) { _M_init(); }

  __device__ ~_List_base() { _M_clear(); }

  __device__ void _M_clear();

  __device__ void _M_init() {
    this->_M_impl._M_node._M_next = &this->_M_impl._M_node;
    this->_M_impl._M_node._M_prev = &this->_M_impl._M_node;
  }
};

template <typename _Tp, typename _Alloc = aicuda::stl::allocator<_Tp>>
class list : protected _List_base<_Tp, _Alloc> {
  typedef typename _Alloc::value_type _Alloc_value_type;

  typedef _List_base<_Tp, _Alloc> _Base;
  typedef typename _Base::_Tp_alloc_type _Tp_alloc_type;

 public:
  typedef _Tp value_type;
  typedef typename _Tp_alloc_type::pointer pointer;
  typedef typename _Tp_alloc_type::const_pointer const_pointer;
  typedef typename _Tp_alloc_type::reference reference;
  typedef typename _Tp_alloc_type::const_reference const_reference;
  typedef _List_iterator<_Tp> iterator;
  typedef _List_const_iterator<_Tp> const_iterator;
  typedef aicuda::stl::reverse_iterator<const_iterator> const_reverse_iterator;
  typedef aicuda::stl::reverse_iterator<iterator> reverse_iterator;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef _Alloc allocator_type;

 protected:
  typedef _List_node<_Tp> _Node;

  using _Base::_M_get_node;
  using _Base::_M_get_Node_allocator;
  using _Base::_M_get_Tp_allocator;
  using _Base::_M_impl;
  using _Base::_M_put_node;

  __device__ _Node *_M_create_node(const value_type &__x) {
    _Node *__p = this->_M_get_node();
    _M_get_Tp_allocator().construct(&__p->_M_data, __x);
    return __p;
  }

 public:
  __device__ list() : _Base() {}

  __device__ explicit list(const allocator_type &__a) : _Base(__a) {}

  __device__ explicit list(size_type __n,
                           const value_type &__value = value_type(),
                           const allocator_type &__a = allocator_type())
      : _Base(__a) {
    _M_fill_initialize(__n, __value);
  }

  __device__ list(const list &__x) : _Base(__x._M_get_Node_allocator()) {
    _M_initialize_dispatch(__x.begin(), __x.end(), __false_type());
  }

  template <typename _InputIterator>
  __device__ list(_InputIterator __first, _InputIterator __last,
                  const allocator_type &__a = allocator_type())
      : _Base(__a) {
    typedef
        typename aicuda::stl::__is_integer<_InputIterator>::__type _Integral;
    _M_initialize_dispatch(__first, __last, _Integral());
  }

  __device__ list &operator=(const list &__x);

  __device__ void assign(size_type __n, const value_type &__val) {
    _M_fill_assign(__n, __val);
  }

  template <typename _InputIterator>
  __device__ void assign(_InputIterator __first, _InputIterator __last) {
    typedef
        typename aicuda::stl::__is_integer<_InputIterator>::__type _Integral;
    _M_assign_dispatch(__first, __last, _Integral());
  }

  __device__ allocator_type get_allocator() const {
    return _Base::get_allocator();
  }

  __device__ iterator begin() {
    return iterator(this->_M_impl._M_node._M_next);
  }

  __device__ const_iterator begin() const {
    return const_iterator(this->_M_impl._M_node._M_next);
  }

  __device__ iterator end() { return iterator(&this->_M_impl._M_node); }

  __device__ const_iterator end() const {
    return const_iterator(&this->_M_impl._M_node);
  }

  __device__ reverse_iterator rbegin() { return reverse_iterator(end()); }

  __device__ const_reverse_iterator rbegin() const {
    return const_reverse_iterator(end());
  }

  __device__ reverse_iterator rend() { return reverse_iterator(begin()); }

  __device__ const_reverse_iterator rend() const {
    return const_reverse_iterator(begin());
  }

  __device__ bool empty() const {
    return this->_M_impl._M_node._M_next == &this->_M_impl._M_node;
  }

  __device__ size_type size() const {
    return aicuda::stl::distance(begin(), end());
  }

  __device__ size_type max_size() const {
    return _M_get_Node_allocator().max_size();
  }

  __device__ void resize(size_type __new_size, value_type __x = value_type());

  __device__ reference front() { return *begin(); }

  __device__ const_reference front() const { return *begin(); }

  __device__ reference back() {
    iterator __tmp = end();
    --__tmp;
    return *__tmp;
  }

  __device__ const_reference back() const {
    const_iterator __tmp = end();
    --__tmp;
    return *__tmp;
  }

  __device__ void push_front(const value_type &__x) {
    this->_M_insert(begin(), __x);
  }

  __device__ void pop_front() { this->_M_erase(begin()); }

  __device__ void push_back(const value_type &__x) {
    this->_M_insert(end(), __x);
  }

  __device__ void pop_back() {
    this->_M_erase(iterator(this->_M_impl._M_node._M_prev));
  }

  __device__ iterator insert(iterator __position, const value_type &__x);

  __device__ void insert(iterator __position, size_type __n,
                         const value_type &__x) {
    list __tmp(__n, __x, _M_get_Node_allocator());
    splice(__position, __tmp);
  }

  template <typename _InputIterator>
  __device__ void insert(iterator __position, _InputIterator __first,
                         _InputIterator __last) {
    list __tmp(__first, __last, _M_get_Node_allocator());
    splice(__position, __tmp);
  }

  __device__ iterator erase(iterator __position);

  __device__ iterator erase(iterator __first, iterator __last) {
    while (__first != __last) __first = erase(__first);
    return __last;
  }

  __device__ void swap(list &__x) {
    _List_node_base::swap(this->_M_impl._M_node, __x._M_impl._M_node);
    aicuda::stl::__alloc_swap<typename _Base::_Node_alloc_type>::_S_do_it(
        _M_get_Node_allocator(), __x._M_get_Node_allocator());
  }

  __device__ void clear() {
    _Base::_M_clear();
    _Base::_M_init();
  }

  __device__ void splice(iterator __position, list &__x) {
    if (!__x.empty()) {
      _M_check_equal_allocators(__x);
      this->_M_transfer(__position, __x.begin(), __x.end());
    }
  }

  __device__ void splice(iterator __position, list &__x, iterator __i) {
    iterator __j = __i;
    ++__j;
    if (__position == __i || __position == __j) return;

    if (this != &__x) _M_check_equal_allocators(__x);

    this->_M_transfer(__position, __i, __j);
  }

  __device__ void splice(iterator __position, list &__x, iterator __first,
                         iterator __last) {
    if (__first != __last) {
      if (this != &__x) _M_check_equal_allocators(__x);

      this->_M_transfer(__position, __first, __last);
    }
  }

  __device__ void remove(const _Tp &__value);

  template <typename _Predicate>
  __device__ void remove_if(_Predicate);

  __device__ void unique();

  template <typename _BinaryPredicate>
  __device__ void unique(_BinaryPredicate);

  __device__ void merge(list &__x);

  template <typename _StrictWeakOrdering>
  __device__ void merge(list &, _StrictWeakOrdering);

  __device__ void reverse() { this->_M_impl._M_node.reverse(); }

  __device__ void sort();

  template <typename _StrictWeakOrdering>
  __device__ void sort(_StrictWeakOrdering);

 protected:
  template <typename _Integer>
  __device__ void _M_initialize_dispatch(_Integer __n, _Integer __x,
                                         __true_type) {
    _M_fill_initialize(static_cast<size_type>(__n), __x);
  }

  template <typename _InputIterator>
  __device__ void _M_initialize_dispatch(_InputIterator __first,
                                         _InputIterator __last, __false_type) {
    for (; __first != __last; ++__first) push_back(*__first);
  }

  __device__ void _M_fill_initialize(size_type __n, const value_type &__x) {
    for (; __n > 0; --__n) push_back(__x);
  }

  template <typename _Integer>
  __device__ void _M_assign_dispatch(_Integer __n, _Integer __val,
                                     __true_type) {
    _M_fill_assign(__n, __val);
  }

  template <typename _InputIterator>
  __device__ void _M_assign_dispatch(_InputIterator __first,
                                     _InputIterator __last, __false_type);

  __device__ void _M_fill_assign(size_type __n, const value_type &__val);

  __device__ void _M_transfer(iterator __position, iterator __first,
                              iterator __last) {
    __position._M_node->transfer(__first._M_node, __last._M_node);
  }

  __device__ void _M_insert(iterator __position, const value_type &__x) {
    _Node *__tmp = _M_create_node(__x);
    __tmp->hook(__position._M_node);
  }

  __device__ void _M_erase(iterator __position) {
    __position._M_node->unhook();
    _Node *__n = static_cast<_Node *>(__position._M_node);

    _M_get_Tp_allocator().destroy(&__n->_M_data);

    _M_put_node(__n);
  }

  __device__ void _M_check_equal_allocators(list &__x) {
    if (aicuda::stl::__alloc_neq<typename _Base::_Node_alloc_type>::_S_do_it(
            _M_get_Node_allocator(), __x._M_get_Node_allocator())) {
      printf("list::_M_check_equal_allocators\n");
      assert(1 < 0);
    }
  }
};

template <typename _Tp, typename _Alloc>
__device__ inline void swap(list<_Tp, _Alloc> &__x, list<_Tp, _Alloc> &__y) {
  __x.swap(__y);
}

template <typename _Tp, typename _Alloc>
__device__ void _List_base<_Tp, _Alloc>::_M_clear() {
  typedef _List_node<_Tp> _Node;
  _Node *__cur = static_cast<_Node *>(this->_M_impl._M_node._M_next);
  while (__cur != &this->_M_impl._M_node) {
    _Node *__tmp = __cur;
    __cur = static_cast<_Node *>(__cur->_M_next);

    _M_get_Tp_allocator().destroy(&__tmp->_M_data);

    _M_put_node(__tmp);
  }
}

template <typename _Tp, typename _Alloc>
__device__ typename list<_Tp, _Alloc>::iterator list<_Tp, _Alloc>::insert(
    iterator __position, const value_type &__x) {
  _Node *__tmp = _M_create_node(__x);
  __tmp->hook(__position._M_node);
  return iterator(__tmp);
}

template <typename _Tp, typename _Alloc>
__device__ typename list<_Tp, _Alloc>::iterator list<_Tp, _Alloc>::erase(
    iterator __position) {
  iterator __ret = iterator(__position._M_node->_M_next);
  _M_erase(__position);
  return __ret;
}

template <typename _Tp, typename _Alloc>
__device__ void list<_Tp, _Alloc>::resize(size_type __new_size,
                                          value_type __x) {
  iterator __i = begin();
  size_type __len = 0;
  for (; __i != end() && __len < __new_size; ++__i, ++__len)
    ;
  if (__len == __new_size)
    erase(__i, end());
  else
    insert(end(), __new_size - __len, __x);
}

template <typename _Tp, typename _Alloc>
__device__ list<_Tp, _Alloc> &list<_Tp, _Alloc>::operator=(const list &__x) {
  if (this != &__x) {
    iterator __first1 = begin();
    iterator __last1 = end();
    const_iterator __first2 = __x.begin();
    const_iterator __last2 = __x.end();
    for (; __first1 != __last1 && __first2 != __last2; ++__first1, ++__first2)
      *__first1 = *__first2;
    if (__first2 == __last2)
      erase(__first1, __last1);
    else
      insert(__last1, __first2, __last2);
  }
  return *this;
}

template <typename _Tp, typename _Alloc>
__device__ void list<_Tp, _Alloc>::_M_fill_assign(size_type __n,
                                                  const value_type &__val) {
  iterator __i = begin();
  for (; __i != end() && __n > 0; ++__i, --__n) *__i = __val;
  if (__n > 0)
    insert(end(), __n, __val);
  else
    erase(__i, end());
}

template <typename _Tp, typename _Alloc>
template <typename _InputIterator>
__device__ void list<_Tp, _Alloc>::_M_assign_dispatch(_InputIterator __first2,
                                                      _InputIterator __last2,
                                                      __false_type) {
  iterator __first1 = begin();
  iterator __last1 = end();
  for (; __first1 != __last1 && __first2 != __last2; ++__first1, ++__first2)
    *__first1 = *__first2;
  if (__first2 == __last2)
    erase(__first1, __last1);
  else
    insert(__last1, __first2, __last2);
}

template <typename _Tp, typename _Alloc>
__device__ void list<_Tp, _Alloc>::remove(const value_type &__value) {
  iterator __first = begin();
  iterator __last = end();
  iterator __extra = __last;
  while (__first != __last) {
    iterator __next = __first;
    ++__next;
    if (*__first == __value) {
      if (&*__first != &__value)
        _M_erase(__first);
      else
        __extra = __first;
    }
    __first = __next;
  }
  if (__extra != __last) _M_erase(__extra);
}

template <typename _Tp, typename _Alloc>
__device__ void list<_Tp, _Alloc>::unique() {
  iterator __first = begin();
  iterator __last = end();
  if (__first == __last) return;
  iterator __next = __first;
  while (++__next != __last) {
    if (*__first == *__next)
      _M_erase(__next);
    else
      __first = __next;
    __next = __first;
  }
}

template <typename _Tp, typename _Alloc>
__device__ void list<_Tp, _Alloc>::merge(list &__x) {
  if (this != &__x) {
    _M_check_equal_allocators(__x);

    iterator __first1 = begin();
    iterator __last1 = end();
    iterator __first2 = __x.begin();
    iterator __last2 = __x.end();
    while (__first1 != __last1 && __first2 != __last2)
      if (*__first2 < *__first1) {
        iterator __next = __first2;
        _M_transfer(__first1, __first2, ++__next);
        __first2 = __next;
      } else
        ++__first1;
    if (__first2 != __last2) _M_transfer(__last1, __first2, __last2);
  }
}

template <typename _Tp, typename _Alloc>
template <typename _StrictWeakOrdering>
__device__ void list<_Tp, _Alloc>::merge(list &__x,
                                         _StrictWeakOrdering __comp) {
  if (this != &__x) {
    _M_check_equal_allocators(__x);

    iterator __first1 = begin();
    iterator __last1 = end();
    iterator __first2 = __x.begin();
    iterator __last2 = __x.end();
    while (__first1 != __last1 && __first2 != __last2)
      if (__comp(*__first2, *__first1)) {
        iterator __next = __first2;
        _M_transfer(__first1, __first2, ++__next);
        __first2 = __next;
      } else
        ++__first1;
    if (__first2 != __last2) _M_transfer(__last1, __first2, __last2);
  }
}

template <typename _Tp, typename _Alloc>
__device__ void list<_Tp, _Alloc>::sort() {
  if (this->_M_impl._M_node._M_next != &this->_M_impl._M_node &&
      this->_M_impl._M_node._M_next->_M_next != &this->_M_impl._M_node) {
    list __carry;
    list __tmp[64];
    list *__fill = &__tmp[0];
    list *__counter;

    do {
      __carry.splice(__carry.begin(), *this, begin());

      for (__counter = &__tmp[0]; __counter != __fill && !__counter->empty();
           ++__counter) {
        __counter->merge(__carry);
        __carry.swap(*__counter);
      }
      __carry.swap(*__counter);
      if (__counter == __fill) ++__fill;
    } while (!empty());

    for (__counter = &__tmp[1]; __counter != __fill; ++__counter)
      __counter->merge(*(__counter - 1));
    swap(*(__fill - 1));
  }
}

template <typename _Tp, typename _Alloc>
template <typename _Predicate>
__device__ void list<_Tp, _Alloc>::remove_if(_Predicate __pred) {
  iterator __first = begin();
  iterator __last = end();
  while (__first != __last) {
    iterator __next = __first;
    ++__next;
    if (__pred(*__first)) _M_erase(__first);
    __first = __next;
  }
}

template <typename _Tp, typename _Alloc>
template <typename _BinaryPredicate>
__device__ void list<_Tp, _Alloc>::unique(_BinaryPredicate __binary_pred) {
  iterator __first = begin();
  iterator __last = end();
  if (__first == __last) return;
  iterator __next = __first;
  while (++__next != __last) {
    if (__binary_pred(*__first, *__next))
      _M_erase(__next);
    else
      __first = __next;
    __next = __first;
  }
}

template <typename _Tp, typename _Alloc>
template <typename _StrictWeakOrdering>
__device__ void list<_Tp, _Alloc>::sort(_StrictWeakOrdering __comp) {
  if (this->_M_impl._M_node._M_next != &this->_M_impl._M_node &&
      this->_M_impl._M_node._M_next->_M_next != &this->_M_impl._M_node) {
    list __carry;
    list __tmp[64];
    list *__fill = &__tmp[0];
    list *__counter;

    do {
      __carry.splice(__carry.begin(), *this, begin());

      for (__counter = &__tmp[0]; __counter != __fill && !__counter->empty();
           ++__counter) {
        __counter->merge(__carry, __comp);
        __carry.swap(*__counter);
      }
      __carry.swap(*__counter);
      if (__counter == __fill) ++__fill;
    } while (!empty());

    for (__counter = &__tmp[1]; __counter != __fill; ++__counter)
      __counter->merge(*(__counter - 1), __comp);
    swap(*(__fill - 1));
  }
}

}  // namespace stl
}  // namespace aicuda

#endif /* _AICUDA_STL_LIST_H_ */
