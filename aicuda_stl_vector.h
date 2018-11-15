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

#ifndef _AICUDA_STL_VECTOR_H_
#define _AICUDA_STL_VECTOR_H_ 1

#include <aicuda_stl_allocator.h>
#include <aicuda_stl_construct.h>
#include <aicuda_stl_function.h>
#include <aicuda_stl_iterator.h>
#include <aicuda_stl_uninitialized.h>
#include <stdio.h>

namespace aicuda {
namespace stl {

template <typename _Tp, typename _Alloc>
struct _Vector_base {
  typedef typename _Alloc::template rebind<_Tp>::other _Tp_alloc_type;

  struct _Vector_impl : public _Tp_alloc_type {
    typename _Tp_alloc_type::pointer _M_start;
    typename _Tp_alloc_type::pointer _M_finish;
    typename _Tp_alloc_type::pointer _M_end_of_storage;

    __device__ _Vector_impl()
        : _Tp_alloc_type(), _M_start(0), _M_finish(0), _M_end_of_storage(0) {}

    __device__ _Vector_impl(_Tp_alloc_type const &__a)
        : _Tp_alloc_type(__a),
          _M_start(0),
          _M_finish(0),
          _M_end_of_storage(0) {}
  };

 public:
  typedef _Alloc allocator_type;

  __device__ _Tp_alloc_type &_M_get_Tp_allocator() {
    return *static_cast<_Tp_alloc_type *>(&this->_M_impl);
  }

  __device__ const _Tp_alloc_type &_M_get_Tp_allocator() const {
    return *static_cast<const _Tp_alloc_type *>(&this->_M_impl);
  }

  __device__ allocator_type get_allocator() const {
    return allocator_type(_M_get_Tp_allocator());
  }

  __device__ _Vector_base() : _M_impl() {}

  __device__ _Vector_base(const allocator_type &__a) : _M_impl(__a) {}

  __device__ _Vector_base(size_t __n, const allocator_type &__a)
      : _M_impl(__a) {
    this->_M_impl._M_start = this->_M_allocate(__n);
    this->_M_impl._M_finish = this->_M_impl._M_start;
    this->_M_impl._M_end_of_storage = this->_M_impl._M_start + __n;
  }

  __device__ ~_Vector_base() {
    _M_deallocate(this->_M_impl._M_start,
                  this->_M_impl._M_end_of_storage - this->_M_impl._M_start);
  }

 public:
  _Vector_impl _M_impl;

  typename _Tp_alloc_type::pointer __device__ _M_allocate(size_t __n) {
    return __n != 0 ? _M_impl.allocate(__n) : 0;
  }

  __device__ void _M_deallocate(typename _Tp_alloc_type::pointer __p,
                                size_t __n) {
    if (__p) _M_impl.deallocate(__p, __n);
  }
};

template <typename _Tp, typename _Alloc = aicuda::stl::allocator<_Tp>>
class vector : protected _Vector_base<_Tp, _Alloc> {
  typedef typename _Alloc::value_type _Alloc_value_type;

  typedef _Vector_base<_Tp, _Alloc> _Base;
  typedef typename _Base::_Tp_alloc_type _Tp_alloc_type;

 public:
  typedef _Tp value_type;
  typedef typename _Tp_alloc_type::pointer pointer;
  typedef typename _Tp_alloc_type::const_pointer const_pointer;
  typedef typename _Tp_alloc_type::reference reference;
  typedef typename _Tp_alloc_type::const_reference const_reference;
  typedef aicuda::stl::__normal_iterator<pointer, vector> iterator;
  typedef aicuda::stl::__normal_iterator<const_pointer, vector> const_iterator;
  typedef aicuda::stl::reverse_iterator<const_iterator> const_reverse_iterator;
  typedef aicuda::stl::reverse_iterator<iterator> reverse_iterator;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef _Alloc allocator_type;

 protected:
  using _Base::_M_allocate;
  using _Base::_M_deallocate;
  using _Base::_M_get_Tp_allocator;
  using _Base::_M_impl;

 public:
  __device__ vector() : _Base() {}

  __device__ explicit vector(const allocator_type &__a) : _Base(__a) {}

  __device__ explicit vector(size_type __n,
                             const value_type &__value = value_type(),
                             const allocator_type &__a = allocator_type())
      : _Base(__n, __a) {
    _M_fill_initialize(__n, __value);
  }

  __device__ vector(const vector &__x)
      : _Base(__x.size(), __x._M_get_Tp_allocator()) {
    this->_M_impl._M_finish = aicuda::stl::__uninitialized_copy_a(
        __x.begin(), __x.end(), this->_M_impl._M_start, _M_get_Tp_allocator());
  }

  template <typename _InputIterator>
  __device__ vector(_InputIterator __first, _InputIterator __last,
                    const allocator_type &__a = allocator_type())
      : _Base(__a) {
    typedef
        typename aicuda::stl::__is_integer<_InputIterator>::__type _Integral;
    _M_initialize_dispatch(__first, __last, _Integral());
  }

  __device__ ~vector() {
    aicuda::stl::_Destroy(this->_M_impl._M_start, this->_M_impl._M_finish,
                          _M_get_Tp_allocator());
  }

  __device__ vector &operator=(const vector &__x);

  __device__ void assign(size_type __n, const value_type &__val) {
    _M_fill_assign(__n, __val);
  }

  template <typename _InputIterator>
  __device__ void assign(_InputIterator __first, _InputIterator __last) {
    typedef
        typename aicuda::stl::__is_integer<_InputIterator>::__type _Integral;
    _M_assign_dispatch(__first, __last, _Integral());
  }

  using _Base::get_allocator;

  __device__ iterator begin() { return iterator(this->_M_impl._M_start); }

  __device__ const_iterator begin() const {
    return const_iterator(this->_M_impl._M_start);
  }

  __device__ iterator end() { return iterator(this->_M_impl._M_finish); }

  __device__ const_iterator end() const {
    return const_iterator(this->_M_impl._M_finish);
  }

  __device__ reverse_iterator rbegin() { return reverse_iterator(end()); }

  __device__ const_reverse_iterator rbegin() const {
    return const_reverse_iterator(end());
  }

  __device__ reverse_iterator rend() { return reverse_iterator(begin()); }

  __device__ const_reverse_iterator rend() const {
    return const_reverse_iterator(begin());
  }

  __device__ size_type size() const {
    return size_type(this->_M_impl._M_finish - this->_M_impl._M_start);
  }

  __device__ size_type max_size() const {
    return _M_get_Tp_allocator().max_size();
  }

  __device__ void resize(size_type __new_size, value_type __x = value_type()) {
    if (__new_size < size())
      _M_erase_at_end(this->_M_impl._M_start + __new_size);
    else
      insert(end(), __new_size - size(), __x);
  }

  __device__ size_type capacity() const {
    return size_type(this->_M_impl._M_end_of_storage - this->_M_impl._M_start);
  }

  __device__ bool empty() const { return begin() == end(); }

  __device__ void reserve(size_type __n);

  __device__ reference operator[](size_type __n) {
    return *(this->_M_impl._M_start + __n);
  }

  __device__ const_reference operator[](size_type __n) const {
    return *(this->_M_impl._M_start + __n);
  }

 protected:
  __device__ void _M_range_check(size_type __n) const {
    if (__n >= this->size()) {
      printf("vector::_M_range_check\n");
      assert(1 < 0);
    }
  }

 public:
  __device__ reference at(size_type __n) {
    _M_range_check(__n);
    return (*this)[__n];
  }

  __device__ const_reference at(size_type __n) const {
    _M_range_check(__n);
    return (*this)[__n];
  }

  __device__ reference front() { return *begin(); }

  __device__ const_reference front() const { return *begin(); }

  __device__ reference back() { return *(end() - 1); }

  __device__ const_reference back() const { return *(end() - 1); }

  __device__ pointer data() { return pointer(this->_M_impl._M_start); }

  __device__ const_pointer data() const {
    return const_pointer(this->_M_impl._M_start);
  }

  __device__ void push_back(const value_type &__x) {
    if (this->_M_impl._M_finish != this->_M_impl._M_end_of_storage) {
      this->_M_impl.construct(this->_M_impl._M_finish, __x);
      ++this->_M_impl._M_finish;
    } else
      _M_insert_aux(end(), __x);
  }

  __device__ void pop_back() {
    --this->_M_impl._M_finish;
    this->_M_impl.destroy(this->_M_impl._M_finish);
  }

  __device__ iterator insert(iterator __position, const value_type &__x);

  __device__ void insert(iterator __position, size_type __n,
                         const value_type &__x) {
    _M_fill_insert(__position, __n, __x);
  }

  template <typename _InputIterator>
  __device__ void insert(iterator __position, _InputIterator __first,
                         _InputIterator __last) {
    typedef
        typename aicuda::stl::__is_integer<_InputIterator>::__type _Integral;
    _M_insert_dispatch(__position, __first, __last, _Integral());
  }

  __device__ iterator erase(iterator __position);

  __device__ iterator erase(iterator __first, iterator __last);

  __device__ void swap(vector &__x) {
    aicuda::stl::swap(this->_M_impl._M_start, __x._M_impl._M_start);
    aicuda::stl::swap(this->_M_impl._M_finish, __x._M_impl._M_finish);
    aicuda::stl::swap(this->_M_impl._M_end_of_storage,
                      __x._M_impl._M_end_of_storage);

    aicuda::stl::__alloc_swap<_Tp_alloc_type>::_S_do_it(
        _M_get_Tp_allocator(), __x._M_get_Tp_allocator());
  }

  __device__ void clear() { _M_erase_at_end(this->_M_impl._M_start); }

 protected:
  template <typename _ForwardIterator>
  __device__ pointer _M_allocate_and_copy(size_type __n,
                                          _ForwardIterator __first,
                                          _ForwardIterator __last) {
    pointer __result = this->_M_allocate(__n);

    aicuda::stl::__uninitialized_copy_a(__first, __last, __result,
                                        _M_get_Tp_allocator());
    return __result;
  }

  template <typename _Integer>
  __device__ void _M_initialize_dispatch(_Integer __n, _Integer __value,
                                         __true_type) {
    this->_M_impl._M_start = _M_allocate(static_cast<size_type>(__n));
    this->_M_impl._M_end_of_storage =
        this->_M_impl._M_start + static_cast<size_type>(__n);
    _M_fill_initialize(static_cast<size_type>(__n), __value);
  }

  template <typename _InputIterator>
  __device__ void _M_initialize_dispatch(_InputIterator __first,
                                         _InputIterator __last, __false_type) {
    typedef
        typename aicuda::stl::iterator_traits<_InputIterator>::iterator_category
            _IterCategory;
    _M_range_initialize(__first, __last, _IterCategory());
  }

  template <typename _InputIterator>
  __device__ void _M_range_initialize(_InputIterator __first,
                                      _InputIterator __last,
                                      aicuda::stl::input_iterator_tag) {
    for (; __first != __last; ++__first) push_back(*__first);
  }

  template <typename _ForwardIterator>
  __device__ void _M_range_initialize(_ForwardIterator __first,
                                      _ForwardIterator __last,
                                      aicuda::stl::forward_iterator_tag) {
    const size_type __n = aicuda::stl::distance(__first, __last);
    this->_M_impl._M_start = this->_M_allocate(__n);
    this->_M_impl._M_end_of_storage = this->_M_impl._M_start + __n;
    this->_M_impl._M_finish = aicuda::stl::__uninitialized_copy_a(
        __first, __last, this->_M_impl._M_start, _M_get_Tp_allocator());
  }

  __device__ void _M_fill_initialize(size_type __n, const value_type &__value) {
    aicuda::stl::__uninitialized_fill_n_a(this->_M_impl._M_start, __n, __value,
                                          _M_get_Tp_allocator());
    this->_M_impl._M_finish = this->_M_impl._M_end_of_storage;
  }

  template <typename _Integer>
  __device__ void _M_assign_dispatch(_Integer __n, _Integer __val,
                                     __true_type) {
    _M_fill_assign(__n, __val);
  }

  template <typename _InputIterator>
  __device__ void _M_assign_dispatch(_InputIterator __first,
                                     _InputIterator __last, __false_type) {
    typedef
        typename aicuda::stl::iterator_traits<_InputIterator>::iterator_category
            _IterCategory;
    _M_assign_aux(__first, __last, _IterCategory());
  }

  template <typename _InputIterator>
  __device__ void _M_assign_aux(_InputIterator __first, _InputIterator __last,
                                aicuda::stl::input_iterator_tag);

  template <typename _ForwardIterator>
  __device__ void _M_assign_aux(_ForwardIterator __first,
                                _ForwardIterator __last,
                                aicuda::stl::forward_iterator_tag);

  __device__ void _M_fill_assign(size_type __n, const value_type &__val);

  template <typename _Integer>
  __device__ void _M_insert_dispatch(iterator __pos, _Integer __n,
                                     _Integer __val, __true_type) {
    _M_fill_insert(__pos, __n, __val);
  }

  template <typename _InputIterator>
  __device__ void _M_insert_dispatch(iterator __pos, _InputIterator __first,
                                     _InputIterator __last, __false_type) {
    typedef
        typename aicuda::stl::iterator_traits<_InputIterator>::iterator_category
            _IterCategory;
    _M_range_insert(__pos, __first, __last, _IterCategory());
  }

  template <typename _InputIterator>
  __device__ void _M_range_insert(iterator __pos, _InputIterator __first,
                                  _InputIterator __last,
                                  aicuda::stl::input_iterator_tag);

  template <typename _ForwardIterator>
  __device__ void _M_range_insert(iterator __pos, _ForwardIterator __first,
                                  _ForwardIterator __last,
                                  aicuda::stl::forward_iterator_tag);

  __device__ void _M_fill_insert(iterator __pos, size_type __n,
                                 const value_type &__x);

  __device__ void _M_insert_aux(iterator __position, const value_type &__x);

  __device__ size_type _M_check_len(size_type __n, const char *__s) const {
    if (max_size() - size() < __n) {
      printf("check len failed : %s \n", __s);
      assert(1 < 0);
    }

    const size_type __len = size() + aicuda::stl::max(size(), __n);
    return (__len < size() || __len > max_size()) ? max_size() : __len;
  }

  __device__ void _M_erase_at_end(pointer __pos) {
    aicuda::stl::_Destroy(__pos, this->_M_impl._M_finish,
                          _M_get_Tp_allocator());
    this->_M_impl._M_finish = __pos;
  }
};

template <typename _Tp, typename _Alloc>
__device__ inline void swap(vector<_Tp, _Alloc> &__x,
                            vector<_Tp, _Alloc> &__y) {
  __x.swap(__y);
}

template <typename _Tp, typename _Alloc>
__device__ void vector<_Tp, _Alloc>::reserve(size_type __n) {
  if (__n > this->max_size()) {
    printf("vector::reserve \n");
    assert(1 < 0);
  }

  if (this->capacity() < __n) {
    const size_type __old_size = size();
    pointer __tmp = _M_allocate_and_copy(__n, (this->_M_impl._M_start),
                                         (this->_M_impl._M_finish));
    aicuda::stl::_Destroy(this->_M_impl._M_start, this->_M_impl._M_finish,
                          _M_get_Tp_allocator());
    _M_deallocate(this->_M_impl._M_start,
                  this->_M_impl._M_end_of_storage - this->_M_impl._M_start);
    this->_M_impl._M_start = __tmp;
    this->_M_impl._M_finish = __tmp + __old_size;
    this->_M_impl._M_end_of_storage = this->_M_impl._M_start + __n;
  }
}

template <typename _Tp, typename _Alloc>
__device__ typename vector<_Tp, _Alloc>::iterator vector<_Tp, _Alloc>::insert(
    iterator __position, const value_type &__x) {
  const size_type __n = __position - begin();
  if (this->_M_impl._M_finish != this->_M_impl._M_end_of_storage &&
      __position == end()) {
    this->_M_impl.construct(this->_M_impl._M_finish, __x);
    ++this->_M_impl._M_finish;
  } else {
    _M_insert_aux(__position, __x);
  }
  return iterator(this->_M_impl._M_start + __n);
}

template <typename _Tp, typename _Alloc>
__device__ typename vector<_Tp, _Alloc>::iterator vector<_Tp, _Alloc>::erase(
    iterator __position) {
  if (__position + 1 != end())
    aicuda::stl::copy(__position + 1, end(), __position);
  --this->_M_impl._M_finish;
  this->_M_impl.destroy(this->_M_impl._M_finish);
  return __position;
}

template <typename _Tp, typename _Alloc>
__device__ typename vector<_Tp, _Alloc>::iterator vector<_Tp, _Alloc>::erase(
    iterator __first, iterator __last) {
  if (__last != end()) aicuda::stl::copy(__last, end(), __first);
  _M_erase_at_end(__first.base() + (end() - __last));
  return __first;
}

template <typename _Tp, typename _Alloc>
__device__ vector<_Tp, _Alloc> &vector<_Tp, _Alloc>::operator=(
    const vector<_Tp, _Alloc> &__x) {
  if (&__x != this) {
    const size_type __xlen = __x.size();
    if (__xlen > capacity()) {
      pointer __tmp = _M_allocate_and_copy(__xlen, __x.begin(), __x.end());
      aicuda::stl::_Destroy(this->_M_impl._M_start, this->_M_impl._M_finish,
                            _M_get_Tp_allocator());
      _M_deallocate(this->_M_impl._M_start,
                    this->_M_impl._M_end_of_storage - this->_M_impl._M_start);
      this->_M_impl._M_start = __tmp;
      this->_M_impl._M_end_of_storage = this->_M_impl._M_start + __xlen;
    } else if (size() >= __xlen) {
      aicuda::stl::_Destroy(aicuda::stl::copy(__x.begin(), __x.end(), begin()),
                            end(), _M_get_Tp_allocator());
    } else {
      aicuda::stl::copy(__x._M_impl._M_start, __x._M_impl._M_start + size(),
                        this->_M_impl._M_start);
      aicuda::stl::__uninitialized_copy_a(
          __x._M_impl._M_start + size(), __x._M_impl._M_finish,
          this->_M_impl._M_finish, _M_get_Tp_allocator());
    }
    this->_M_impl._M_finish = this->_M_impl._M_start + __xlen;
  }
  return *this;
}

template <typename _Tp, typename _Alloc>
__device__ void vector<_Tp, _Alloc>::_M_fill_assign(size_t __n,
                                                    const value_type &__val) {
  if (__n > capacity()) {
    vector __tmp(__n, __val, _M_get_Tp_allocator());
    __tmp.swap(*this);
  } else if (__n > size()) {
    aicuda::stl::fill(begin(), end(), __val);
    aicuda::stl::__uninitialized_fill_n_a(this->_M_impl._M_finish, __n - size(),
                                          __val, _M_get_Tp_allocator());
    this->_M_impl._M_finish += __n - size();
  } else
    _M_erase_at_end(aicuda::stl::fill_n(this->_M_impl._M_start, __n, __val));
}

template <typename _Tp, typename _Alloc>
template <typename _InputIterator>
__device__ void vector<_Tp, _Alloc>::_M_assign_aux(
    _InputIterator __first, _InputIterator __last,
    aicuda::stl::input_iterator_tag) {
  pointer __cur(this->_M_impl._M_start);
  for (; __first != __last && __cur != this->_M_impl._M_finish;
       ++__cur, ++__first)
    *__cur = *__first;
  if (__first == __last)
    _M_erase_at_end(__cur);
  else
    insert(end(), __first, __last);
}

template <typename _Tp, typename _Alloc>
template <typename _ForwardIterator>
__device__ void vector<_Tp, _Alloc>::_M_assign_aux(
    _ForwardIterator __first, _ForwardIterator __last,
    aicuda::stl::forward_iterator_tag) {
  const size_type __len = aicuda::stl::distance(__first, __last);

  if (__len > capacity()) {
    pointer __tmp(_M_allocate_and_copy(__len, __first, __last));
    aicuda::stl::_Destroy(this->_M_impl._M_start, this->_M_impl._M_finish,
                          _M_get_Tp_allocator());
    _M_deallocate(this->_M_impl._M_start,
                  this->_M_impl._M_end_of_storage - this->_M_impl._M_start);
    this->_M_impl._M_start = __tmp;
    this->_M_impl._M_finish = this->_M_impl._M_start + __len;
    this->_M_impl._M_end_of_storage = this->_M_impl._M_finish;
  } else if (size() >= __len)
    _M_erase_at_end(aicuda::stl::copy(__first, __last, this->_M_impl._M_start));
  else {
    _ForwardIterator __mid = __first;
    aicuda::stl::advance(__mid, size());
    aicuda::stl::copy(__first, __mid, this->_M_impl._M_start);
    this->_M_impl._M_finish = aicuda::stl::__uninitialized_copy_a(
        __mid, __last, this->_M_impl._M_finish, _M_get_Tp_allocator());
  }
}

template <typename _Tp, typename _Alloc>
__device__ void vector<_Tp, _Alloc>::_M_insert_aux(iterator __position,
                                                   const _Tp &__x)

{
  if (this->_M_impl._M_finish != this->_M_impl._M_end_of_storage) {
    this->_M_impl.construct(this->_M_impl._M_finish,
                            (*(this->_M_impl._M_finish - 1)));
    ++this->_M_impl._M_finish;

    _Tp __x_copy = __x;

    aicuda::stl::copy_backward(__position.base(), this->_M_impl._M_finish - 2,
                               this->_M_impl._M_finish - 1);

    *__position = __x_copy;
  } else {
    const size_type __len = _M_check_len(size_type(1), "vector::_M_insert_aux");
    const size_type __elems_before = __position - begin();
    pointer __new_start(this->_M_allocate(__len));
    pointer __new_finish(__new_start);

    this->_M_impl.construct(__new_start + __elems_before,

                            __x);

    __new_finish = 0;

    __new_finish = aicuda::stl::__uninitialized_move_a(
        this->_M_impl._M_start, __position.base(), __new_start,
        _M_get_Tp_allocator());
    ++__new_finish;

    __new_finish = aicuda::stl::__uninitialized_move_a(
        __position.base(), this->_M_impl._M_finish, __new_finish,
        _M_get_Tp_allocator());

    aicuda::stl::_Destroy(this->_M_impl._M_start, this->_M_impl._M_finish,
                          _M_get_Tp_allocator());
    _M_deallocate(this->_M_impl._M_start,
                  this->_M_impl._M_end_of_storage - this->_M_impl._M_start);
    this->_M_impl._M_start = __new_start;
    this->_M_impl._M_finish = __new_finish;
    this->_M_impl._M_end_of_storage = __new_start + __len;
  }
}

template <typename _Tp, typename _Alloc>
__device__ void vector<_Tp, _Alloc>::_M_fill_insert(iterator __position,
                                                    size_type __n,
                                                    const value_type &__x) {
  if (__n != 0) {
    if (size_type(this->_M_impl._M_end_of_storage - this->_M_impl._M_finish) >=
        __n) {
      value_type __x_copy = __x;
      const size_type __elems_after = end() - __position;
      pointer __old_finish(this->_M_impl._M_finish);
      if (__elems_after > __n) {
        aicuda::stl::__uninitialized_move_a(
            this->_M_impl._M_finish - __n, this->_M_impl._M_finish,
            this->_M_impl._M_finish, _M_get_Tp_allocator());
        this->_M_impl._M_finish += __n;
        aicuda::stl::copy_backward(__position.base(), __old_finish - __n,
                                   __old_finish);
        aicuda::stl::fill(__position.base(), __position.base() + __n, __x_copy);
      } else {
        aicuda::stl::__uninitialized_fill_n_a(this->_M_impl._M_finish,
                                              __n - __elems_after, __x_copy,
                                              _M_get_Tp_allocator());
        this->_M_impl._M_finish += __n - __elems_after;
        aicuda::stl::__uninitialized_move_a(__position.base(), __old_finish,
                                            this->_M_impl._M_finish,
                                            _M_get_Tp_allocator());
        this->_M_impl._M_finish += __elems_after;
        aicuda::stl::fill(__position.base(), __old_finish, __x_copy);
      }
    } else {
      const size_type __len = _M_check_len(__n, "vector::_M_fill_insert");
      const size_type __elems_before = __position - begin();
      pointer __new_start(this->_M_allocate(__len));
      pointer __new_finish(__new_start);

      aicuda::stl::__uninitialized_fill_n_a(__new_start + __elems_before, __n,
                                            __x, _M_get_Tp_allocator());
      __new_finish = 0;

      __new_finish = aicuda::stl::__uninitialized_move_a(
          this->_M_impl._M_start, __position.base(), __new_start,
          _M_get_Tp_allocator());
      __new_finish += __n;

      __new_finish = aicuda::stl::__uninitialized_move_a(
          __position.base(), this->_M_impl._M_finish, __new_finish,
          _M_get_Tp_allocator());

      aicuda::stl::_Destroy(this->_M_impl._M_start, this->_M_impl._M_finish,
                            _M_get_Tp_allocator());
      _M_deallocate(this->_M_impl._M_start,
                    this->_M_impl._M_end_of_storage - this->_M_impl._M_start);
      this->_M_impl._M_start = __new_start;
      this->_M_impl._M_finish = __new_finish;
      this->_M_impl._M_end_of_storage = __new_start + __len;
    }
  }
}

template <typename _Tp, typename _Alloc>
template <typename _InputIterator>
__device__ void vector<_Tp, _Alloc>::_M_range_insert(
    iterator __pos, _InputIterator __first, _InputIterator __last,
    aicuda::stl::input_iterator_tag) {
  for (; __first != __last; ++__first) {
    __pos = insert(__pos, *__first);
    ++__pos;
  }
}

template <typename _Tp, typename _Alloc>
template <typename _ForwardIterator>
__device__ void vector<_Tp, _Alloc>::_M_range_insert(
    iterator __position, _ForwardIterator __first, _ForwardIterator __last,
    aicuda::stl::forward_iterator_tag) {
  if (__first != __last) {
    const size_type __n = aicuda::stl::distance(__first, __last);
    if (size_type(this->_M_impl._M_end_of_storage - this->_M_impl._M_finish) >=
        __n) {
      const size_type __elems_after = end() - __position;
      pointer __old_finish(this->_M_impl._M_finish);
      if (__elems_after > __n) {
        aicuda::stl::__uninitialized_move_a(
            this->_M_impl._M_finish - __n, this->_M_impl._M_finish,
            this->_M_impl._M_finish, _M_get_Tp_allocator());
        this->_M_impl._M_finish += __n;
        aicuda::stl::copy_backward(__position.base(), __old_finish - __n,
                                   __old_finish);
        aicuda::stl::copy(__first, __last, __position);
      } else {
        _ForwardIterator __mid = __first;
        aicuda::stl::advance(__mid, __elems_after);
        aicuda::stl::__uninitialized_copy_a(
            __mid, __last, this->_M_impl._M_finish, _M_get_Tp_allocator());
        this->_M_impl._M_finish += __n - __elems_after;
        aicuda::stl::__uninitialized_move_a(__position.base(), __old_finish,
                                            this->_M_impl._M_finish,
                                            _M_get_Tp_allocator());
        this->_M_impl._M_finish += __elems_after;
        aicuda::stl::copy(__first, __mid, __position);
      }
    } else {
      const size_type __len = _M_check_len(__n, "vector::_M_range_insert");
      pointer __new_start(this->_M_allocate(__len));
      pointer __new_finish(__new_start);

      __new_finish = aicuda::stl::__uninitialized_move_a(
          this->_M_impl._M_start, __position.base(), __new_start,
          _M_get_Tp_allocator());
      __new_finish = aicuda::stl::__uninitialized_copy_a(
          __first, __last, __new_finish, _M_get_Tp_allocator());
      __new_finish = aicuda::stl::__uninitialized_move_a(
          __position.base(), this->_M_impl._M_finish, __new_finish,
          _M_get_Tp_allocator());

      aicuda::stl::_Destroy(this->_M_impl._M_start, this->_M_impl._M_finish,
                            _M_get_Tp_allocator());
      _M_deallocate(this->_M_impl._M_start,
                    this->_M_impl._M_end_of_storage - this->_M_impl._M_start);
      this->_M_impl._M_start = __new_start;
      this->_M_impl._M_finish = __new_finish;
      this->_M_impl._M_end_of_storage = __new_start + __len;
    }
  }
}

}  // namespace stl
}  // namespace aicuda

#endif /* _AICUDA_STL_VECTOR_H_ */
