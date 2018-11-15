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

#ifndef _AICUDA_STL_SET_H_
#define _AICUDA_STL_SET_H_ 1

#include <aicuda_stl_tree.h>
#include <assert.h>

namespace aicuda {
namespace stl {

template <typename _Key, typename _Compare = aicuda::stl::less<_Key>,
          typename _Alloc = aicuda::stl::allocator<_Key>>
class set {
  typedef typename _Alloc::value_type _Alloc_value_type;

 public:
  typedef _Key key_type;
  typedef _Key value_type;
  typedef _Compare key_compare;
  typedef _Compare value_compare;
  typedef _Alloc allocator_type;

 private:
  typedef typename _Alloc::template rebind<_Key>::other _Key_alloc_type;

  typedef _Rb_tree<key_type, value_type, _Identity<value_type>, key_compare,
                   _Key_alloc_type>
      _Rep_type;
  _Rep_type _M_t;

 public:
  typedef typename _Key_alloc_type::pointer pointer;
  typedef typename _Key_alloc_type::const_pointer const_pointer;
  typedef typename _Key_alloc_type::reference reference;
  typedef typename _Key_alloc_type::const_reference const_reference;
  typedef typename _Rep_type::const_iterator iterator;
  typedef typename _Rep_type::const_iterator const_iterator;
  typedef typename _Rep_type::const_reverse_iterator reverse_iterator;
  typedef typename _Rep_type::const_reverse_iterator const_reverse_iterator;
  typedef typename _Rep_type::size_type size_type;
  typedef typename _Rep_type::difference_type difference_type;

  __device__ set() : _M_t() {}

  __device__ explicit set(const _Compare &__comp,
                          const allocator_type &__a = allocator_type())
      : _M_t(__comp, __a) {}

  template <typename _InputIterator>
  __device__ set(_InputIterator __first, _InputIterator __last) : _M_t() {
    _M_t._M_insert_unique(__first, __last);
  }

  template <typename _InputIterator>
  __device__ set(_InputIterator __first, _InputIterator __last,
                 const _Compare &__comp,
                 const allocator_type &__a = allocator_type())
      : _M_t(__comp, __a) {
    _M_t._M_insert_unique(__first, __last);
  }

  __device__ set(const set &__x) : _M_t(__x._M_t) {}

  __device__ set &operator=(const set &__x) {
    _M_t = __x._M_t;
    return *this;
  }

  __device__ key_compare key_comp() const { return _M_t.key_comp(); }

  __device__ value_compare value_comp() const { return _M_t.key_comp(); }

  __device__ allocator_type get_allocator() const {
    return _M_t.get_allocator();
  }

  __device__ iterator begin() const { return _M_t.begin(); }

  __device__ iterator end() const { return _M_t.end(); }

  __device__ reverse_iterator rbegin() const { return _M_t.rbegin(); }

  __device__ reverse_iterator rend() const { return _M_t.rend(); }

  __device__ bool empty() const { return _M_t.empty(); }

  __device__ size_type size() const { return _M_t.size(); }

  __device__ size_type max_size() const { return _M_t.max_size(); }

  __device__ void swap(set &__x) { _M_t.swap(__x._M_t); }

  __device__ aicuda::stl::pair<iterator, bool> insert(const value_type &__x) {
    aicuda::stl::pair<typename _Rep_type::iterator, bool> __p =
        _M_t._M_insert_unique(__x);
    return aicuda::stl::pair<iterator, bool>(__p.first, __p.second);
  }

  __device__ iterator insert(iterator __position, const value_type &__x) {
    return _M_t._M_insert_unique_(__position, __x);
  }

  template <typename _InputIterator>
  __device__ void insert(_InputIterator __first, _InputIterator __last) {
    _M_t._M_insert_unique(__first, __last);
  }

  __device__ void erase(iterator __position) { _M_t.erase(__position); }

  __device__ size_type erase(const key_type &__x) { return _M_t.erase(__x); }

  __device__ void erase(iterator __first, iterator __last) {
    _M_t.erase(__first, __last);
  }

  __device__ void clear() { _M_t.clear(); }

  __device__ size_type count(const key_type &__x) const {
    return _M_t.find(__x) == _M_t.end() ? 0 : 1;
  }

  __device__ iterator find(const key_type &__x) { return _M_t.find(__x); }

  __device__ const_iterator find(const key_type &__x) const {
    return _M_t.find(__x);
  }

  __device__ iterator lower_bound(const key_type &__x) {
    return _M_t.lower_bound(__x);
  }

  __device__ const_iterator lower_bound(const key_type &__x) const {
    return _M_t.lower_bound(__x);
  }

  __device__ iterator upper_bound(const key_type &__x) {
    return _M_t.upper_bound(__x);
  }

  __device__ const_iterator upper_bound(const key_type &__x) const {
    return _M_t.upper_bound(__x);
  }

  __device__ aicuda::stl::pair<iterator, iterator> equal_range(
      const key_type &__x) {
    return _M_t.equal_range(__x);
  }

  __device__ aicuda::stl::pair<const_iterator, const_iterator> equal_range(
      const key_type &__x) const {
    return _M_t.equal_range(__x);
  }
};

template <typename _Key, typename _Compare, typename _Alloc>
__device__ inline void swap(set<_Key, _Compare, _Alloc> &__x,
                            set<_Key, _Compare, _Alloc> &__y) {
  __x.swap(__y);
}

template <typename _Key, typename _Compare = aicuda::stl::less<_Key>,
          typename _Alloc = aicuda::stl::allocator<_Key>>
class multiset {
  typedef typename _Alloc::value_type _Alloc_value_type;

 public:
  typedef _Key key_type;
  typedef _Key value_type;
  typedef _Compare key_compare;
  typedef _Compare value_compare;
  typedef _Alloc allocator_type;

 private:
  typedef typename _Alloc::template rebind<_Key>::other _Key_alloc_type;

  typedef _Rb_tree<key_type, value_type, _Identity<value_type>, key_compare,
                   _Key_alloc_type>
      _Rep_type;

  _Rep_type _M_t;

 public:
  typedef typename _Key_alloc_type::pointer pointer;
  typedef typename _Key_alloc_type::const_pointer const_pointer;
  typedef typename _Key_alloc_type::reference reference;
  typedef typename _Key_alloc_type::const_reference const_reference;

  typedef typename _Rep_type::const_iterator iterator;
  typedef typename _Rep_type::const_iterator const_iterator;
  typedef typename _Rep_type::const_reverse_iterator reverse_iterator;
  typedef typename _Rep_type::const_reverse_iterator const_reverse_iterator;
  typedef typename _Rep_type::size_type size_type;
  typedef typename _Rep_type::difference_type difference_type;

  __device__ multiset() : _M_t() {}

  __device__ explicit multiset(const _Compare &__comp,
                               const allocator_type &__a = allocator_type())
      : _M_t(__comp, __a) {}

  template <typename _InputIterator>
  __device__ multiset(_InputIterator __first, _InputIterator __last) : _M_t() {
    _M_t._M_insert_equal(__first, __last);
  }

  template <typename _InputIterator>
  __device__ multiset(_InputIterator __first, _InputIterator __last,
                      const _Compare &__comp,
                      const allocator_type &__a = allocator_type())
      : _M_t(__comp, __a) {
    _M_t._M_insert_equal(__first, __last);
  }

  __device__ multiset(const multiset &__x) : _M_t(__x._M_t) {}

  __device__ multiset &operator=(const multiset &__x) {
    _M_t = __x._M_t;
    return *this;
  }

  __device__ key_compare key_comp() const { return _M_t.key_comp(); }

  __device__ value_compare value_comp() const { return _M_t.key_comp(); }

  __device__ allocator_type get_allocator() const {
    return _M_t.get_allocator();
  }

  __device__ iterator begin() const { return _M_t.begin(); }

  __device__ iterator end() const { return _M_t.end(); }

  __device__ reverse_iterator rbegin() const { return _M_t.rbegin(); }

  __device__ reverse_iterator rend() const { return _M_t.rend(); }

  __device__ bool empty() const { return _M_t.empty(); }

  __device__ size_type size() const { return _M_t.size(); }

  __device__ size_type max_size() const { return _M_t.max_size(); }

  __device__ void swap(multiset &__x) { _M_t.swap(__x._M_t); }

  __device__ iterator insert(const value_type &__x) {
    return _M_t._M_insert_equal(__x);
  }

  __device__ iterator insert(iterator __position, const value_type &__x) {
    return _M_t._M_insert_equal_(__position, __x);
  }

  template <typename _InputIterator>
  __device__ void insert(_InputIterator __first, _InputIterator __last) {
    _M_t._M_insert_equal(__first, __last);
  }

  __device__ void erase(iterator __position) { _M_t.erase(__position); }

  __device__ size_type erase(const key_type &__x) { return _M_t.erase(__x); }

  __device__ void erase(iterator __first, iterator __last) {
    _M_t.erase(__first, __last);
  }

  __device__ void clear() { _M_t.clear(); }

  __device__ size_type count(const key_type &__x) const {
    return _M_t.count(__x);
  }

  __device__ iterator find(const key_type &__x) { return _M_t.find(__x); }

  __device__ const_iterator find(const key_type &__x) const {
    return _M_t.find(__x);
  }

  __device__ iterator lower_bound(const key_type &__x) {
    return _M_t.lower_bound(__x);
  }

  __device__ const_iterator lower_bound(const key_type &__x) const {
    return _M_t.lower_bound(__x);
  }

  __device__ iterator upper_bound(const key_type &__x) {
    return _M_t.upper_bound(__x);
  }

  __device__ const_iterator upper_bound(const key_type &__x) const {
    return _M_t.upper_bound(__x);
  }

  __device__ aicuda::stl::pair<iterator, iterator> equal_range(
      const key_type &__x) {
    return _M_t.equal_range(__x);
  }

  __device__ aicuda::stl::pair<const_iterator, const_iterator> equal_range(
      const key_type &__x) const {
    return _M_t.equal_range(__x);
  }
};

template <typename _Key, typename _Compare, typename _Alloc>
__device__ inline void swap(multiset<_Key, _Compare, _Alloc> &__x,
                            multiset<_Key, _Compare, _Alloc> &__y) {
  __x.swap(__y);
}

}  // namespace stl
}  // namespace aicuda

#endif /* _AICUDA_STL_SET_H_ */
