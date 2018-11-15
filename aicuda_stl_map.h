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

#ifndef _AICUDA_STL_MAP_H_
#define _AICUDA_STL_MAP_H_ 1

#include <aicuda_stl_tree.h>
#include <assert.h>

namespace aicuda {
namespace stl {

template <typename _Key, typename _Tp,
          typename _Compare = aicuda::stl::less<_Key>,
          typename _Alloc =
              aicuda::stl::allocator<aicuda::stl::pair<const _Key, _Tp>>>
class map {
 public:
  typedef _Key key_type;
  typedef _Tp mapped_type;
  typedef aicuda::stl::pair<const _Key, _Tp> value_type;
  typedef _Compare key_compare;
  typedef _Alloc allocator_type;

 private:
  typedef typename _Alloc::value_type _Alloc_value_type;

 public:
  class value_compare
      : public aicuda::stl::binary_function<value_type, value_type, bool> {
    friend class map<_Key, _Tp, _Compare, _Alloc>;

   protected:
    _Compare comp;

    __device__ value_compare(_Compare __c) : comp(__c) {}

   public:
    __device__ bool operator()(const value_type &__x,
                               const value_type &__y) const {
      return comp(__x.first, __y.first);
    }
  };

 private:
  typedef typename _Alloc::template rebind<value_type>::other _Pair_alloc_type;

  typedef aicuda::stl::_Rb_tree<key_type, value_type, _Select1st<value_type>,
                                key_compare, _Pair_alloc_type>
      _Rep_type;

  _Rep_type _M_t;

 public:
  typedef typename _Pair_alloc_type::pointer pointer;
  typedef typename _Pair_alloc_type::const_pointer const_pointer;
  typedef typename _Pair_alloc_type::reference reference;
  typedef typename _Pair_alloc_type::const_reference const_reference;
  typedef typename _Rep_type::iterator iterator;
  typedef typename _Rep_type::const_iterator const_iterator;
  typedef typename _Rep_type::size_type size_type;
  typedef typename _Rep_type::difference_type difference_type;
  typedef typename _Rep_type::reverse_iterator reverse_iterator;
  typedef typename _Rep_type::const_reverse_iterator const_reverse_iterator;

  __device__ map() : _M_t() {}

  __device__ explicit map(const _Compare &__comp,
                          const allocator_type &__a = allocator_type())
      : _M_t(__comp, __a) {}

  __device__ map(const map &__x) : _M_t(__x._M_t) {}

  template <typename _InputIterator>
  __device__ map(_InputIterator __first, _InputIterator __last) : _M_t() {
    _M_t._M_insert_unique(__first, __last);
  }

  template <typename _InputIterator>
  __device__ map(_InputIterator __first, _InputIterator __last,
                 const _Compare &__comp,
                 const allocator_type &__a = allocator_type())
      : _M_t(__comp, __a) {
    _M_t._M_insert_unique(__first, __last);
  }

  __device__ map &operator=(const map &__x) {
    _M_t = __x._M_t;
    return *this;
  }

  __device__ allocator_type get_allocator() const {
    return _M_t.get_allocator();
  }

  __device__ iterator begin() { return _M_t.begin(); }

  __device__ const_iterator begin() const { return _M_t.begin(); }

  __device__ iterator end() { return _M_t.end(); }

  __device__ const_iterator end() const { return _M_t.end(); }

  __device__ reverse_iterator rbegin() { return _M_t.rbegin(); }

  __device__ const_reverse_iterator rbegin() const { return _M_t.rbegin(); }

  __device__ reverse_iterator rend() { return _M_t.rend(); }

  __device__ const_reverse_iterator rend() const { return _M_t.rend(); }

  __device__ bool empty() const { return _M_t.empty(); }

  __device__ size_type size() const { return _M_t.size(); }

  __device__ size_type max_size() const { return _M_t.max_size(); }

  __device__ mapped_type &operator[](const key_type &__k) {
    iterator __i = lower_bound(__k);

    if (__i == end() || key_comp()(__k, (*__i).first))
      __i = insert(__i, value_type(__k, mapped_type()));
    return (*__i).second;
  }

  __device__ mapped_type &at(const key_type &__k) {
    iterator __i = lower_bound(__k);
    if (__i == end() || key_comp()(__k, (*__i).first)) assert(1 < 0);
    return (*__i).second;
  }

  __device__ const mapped_type &at(const key_type &__k) const {
    const_iterator __i = lower_bound(__k);
    if (__i == end() || key_comp()(__k, (*__i).first)) assert(1 < 0);
    return (*__i).second;
  }

  __device__ aicuda::stl::pair<iterator, bool> insert(const value_type &__x) {
    return _M_t._M_insert_unique(__x);
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

  __device__ void swap(map &__x) { _M_t.swap(__x._M_t); }

  __device__ void clear() { _M_t.clear(); }

  __device__ key_compare key_comp() const { return _M_t.key_comp(); }

  __device__ value_compare value_comp() const {
    return value_compare(_M_t.key_comp());
  }

  __device__ iterator find(const key_type &__x) { return _M_t.find(__x); }

  __device__ const_iterator find(const key_type &__x) const {
    return _M_t.find(__x);
  }

  __device__ size_type count(const key_type &__x) const {
    return _M_t.find(__x) == _M_t.end() ? 0 : 1;
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

template <typename _Key, typename _Tp, typename _Compare, typename _Alloc>
__device__ inline void swap(map<_Key, _Tp, _Compare, _Alloc> &__x,
                            map<_Key, _Tp, _Compare, _Alloc> &__y) {
  __x.swap(__y);
}

template <typename _Key, typename _Tp,
          typename _Compare = aicuda::stl::less<_Key>,
          typename _Alloc =
              aicuda::stl::allocator<aicuda::stl::pair<const _Key, _Tp>>>
class multimap {
 public:
  typedef _Key key_type;
  typedef _Tp mapped_type;
  typedef aicuda::stl::pair<const _Key, _Tp> value_type;
  typedef _Compare key_compare;
  typedef _Alloc allocator_type;

 private:
  typedef typename _Alloc::value_type _Alloc_value_type;

 public:
  class value_compare
      : public aicuda::stl::binary_function<value_type, value_type, bool> {
    friend class multimap<_Key, _Tp, _Compare, _Alloc>;

   protected:
    _Compare comp;

    __device__ value_compare(_Compare __c) : comp(__c) {}

   public:
    __device__ bool operator()(const value_type &__x,
                               const value_type &__y) const {
      return comp(__x.first, __y.first);
    }
  };

 private:
  typedef typename _Alloc::template rebind<value_type>::other _Pair_alloc_type;

  typedef _Rb_tree<key_type, value_type, _Select1st<value_type>, key_compare,
                   _Pair_alloc_type>
      _Rep_type;

  _Rep_type _M_t;

 public:
  typedef typename _Pair_alloc_type::pointer pointer;
  typedef typename _Pair_alloc_type::const_pointer const_pointer;
  typedef typename _Pair_alloc_type::reference reference;
  typedef typename _Pair_alloc_type::const_reference const_reference;
  typedef typename _Rep_type::iterator iterator;
  typedef typename _Rep_type::const_iterator const_iterator;
  typedef typename _Rep_type::size_type size_type;
  typedef typename _Rep_type::difference_type difference_type;
  typedef typename _Rep_type::reverse_iterator reverse_iterator;
  typedef typename _Rep_type::const_reverse_iterator const_reverse_iterator;

  __device__ multimap() : _M_t() {}

  __device__ explicit multimap(const _Compare &__comp,
                               const allocator_type &__a = allocator_type())
      : _M_t(__comp, __a) {}

  __device__ multimap(const multimap &__x) : _M_t(__x._M_t) {}

  template <typename _InputIterator>
  __device__ multimap(_InputIterator __first, _InputIterator __last) : _M_t() {
    _M_t._M_insert_equal(__first, __last);
  }

  template <typename _InputIterator>
  __device__ multimap(_InputIterator __first, _InputIterator __last,
                      const _Compare &__comp,
                      const allocator_type &__a = allocator_type())
      : _M_t(__comp, __a) {
    _M_t._M_insert_equal(__first, __last);
  }

  __device__ multimap &operator=(const multimap &__x) {
    _M_t = __x._M_t;
    return *this;
  }

  __device__ allocator_type get_allocator() const {
    return _M_t.get_allocator();
  }

  __device__ iterator begin() { return _M_t.begin(); }

  __device__ const_iterator begin() const { return _M_t.begin(); }

  __device__ iterator end() { return _M_t.end(); }

  __device__ const_iterator end() const { return _M_t.end(); }

  __device__ reverse_iterator rbegin() { return _M_t.rbegin(); }

  __device__ const_reverse_iterator rbegin() const { return _M_t.rbegin(); }

  __device__ reverse_iterator rend() { return _M_t.rend(); }

  __device__ const_reverse_iterator rend() const { return _M_t.rend(); }

  __device__ bool empty() const { return _M_t.empty(); }

  __device__ size_type size() const { return _M_t.size(); }

  __device__ size_type max_size() const { return _M_t.max_size(); }

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

  __device__ void swap(multimap &__x) { _M_t.swap(__x._M_t); }

  __device__ void clear() { _M_t.clear(); }

  __device__ key_compare key_comp() const { return _M_t.key_comp(); }

  __device__ value_compare value_comp() const {
    return value_compare(_M_t.key_comp());
  }

  __device__ iterator find(const key_type &__x) { return _M_t.find(__x); }

  __device__ const_iterator find(const key_type &__x) const {
    return _M_t.find(__x);
  }

  __device__ size_type count(const key_type &__x) const {
    return _M_t.count(__x);
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

template <typename _Key, typename _Tp, typename _Compare, typename _Alloc>
__device__ inline void swap(multimap<_Key, _Tp, _Compare, _Alloc> &__x,
                            multimap<_Key, _Tp, _Compare, _Alloc> &__y) {
  __x.swap(__y);
}

}  // namespace stl
}  // namespace aicuda

#endif /* _AICUDA_STL_MAP_H_ */
