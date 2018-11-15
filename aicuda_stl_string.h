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

#ifndef _AICUDA_STL_STRING_H_
#define _AICUDA_STL_STRING_H_ 1

#include <aicuda_stl_allocator.h>
#include <aicuda_stl_function.h>
#include <aicuda_stl_iterator.h>
#include <aicuda_stl_memory.h>
#include <aicuda_stl_type_traits.h>
#include <wchar.h> // for mbstate_t
#include <stdio.h>

namespace aicuda
{
namespace stl
{

typedef long streamoff;
typedef ptrdiff_t streamsize;
template <typename _StateT>
class fpos
{
private:
  streamoff _M_off;
  _StateT _M_state;

public:
  __device__ fpos() : _M_off(0), _M_state() {}
  __device__ fpos(streamoff __off) : _M_off(__off), _M_state() {}

  __device__ operator streamoff() const { return _M_off; }

  __device__ void state(_StateT __st) { _M_state = __st; }

  __device__ _StateT state() const { return _M_state; }

  __device__ fpos &operator+=(streamoff __off)
  {
    _M_off += __off;
    return *this;
  }

  __device__ fpos &operator-=(streamoff __off)
  {
    _M_off -= __off;
    return *this;
  }

  __device__ fpos operator+(streamoff __off) const
  {
    fpos __pos(*this);
    __pos += __off;
    return __pos;
  }

  __device__ fpos operator-(streamoff __off) const
  {
    fpos __pos(*this);
    __pos -= __off;
    return __pos;
  }

  __device__ streamoff operator-(const fpos &__other) const
  {
    return _M_off - __other._M_off;
  }
};

template <typename _StateT>
__device__ inline bool operator==(const fpos<_StateT> &__lhs,
                                  const fpos<_StateT> &__rhs)
{
  return streamoff(__lhs) == streamoff(__rhs);
}

template <typename _StateT>
__device__ inline bool operator!=(const fpos<_StateT> &__lhs,
                                  const fpos<_StateT> &__rhs)
{
  return streamoff(__lhs) != streamoff(__rhs);
}

typedef fpos<mbstate_t> streampos;
typedef fpos<mbstate_t> wstreampos;

template <typename _CharT>
struct _Char_types
{
  typedef unsigned long int_type;
  typedef streampos pos_type;
  typedef streamoff off_type;
  typedef mbstate_t state_type;
};

template <typename _CharT>
struct char_traits
{
  typedef _CharT char_type;
  typedef typename _Char_types<_CharT>::int_type int_type;
  typedef typename _Char_types<_CharT>::pos_type pos_type;
  typedef typename _Char_types<_CharT>::off_type off_type;
  typedef typename _Char_types<_CharT>::state_type state_type;

  __device__ static void assign(char_type &__c1, const char_type &__c2)
  {
    __c1 = __c2;
  }

  __device__ static bool eq(const char_type &__c1, const char_type &__c2)
  {
    return __c1 == __c2;
  }

  __device__ static bool lt(const char_type &__c1, const char_type &__c2)
  {
    return __c1 < __c2;
  }

  __device__ static int compare(const char_type *__s1, const char_type *__s2,
                                size_t __n);

  __device__ static size_t length(const char_type *__s);

  __device__ static const char_type *find(const char_type *__s, size_t __n,
                                          const char_type &__a);

  __device__ static char_type *move(char_type *__s1, const char_type *__s2,
                                    size_t __n);

  __device__ static char_type *copy(char_type *__s1, const char_type *__s2,
                                    size_t __n);

  __device__ static char_type *assign(char_type *__s, size_t __n,
                                      char_type __a);

  __device__ static char_type to_char_type(const int_type &__c)
  {
    return static_cast<char_type>(__c);
  }

  __device__ static int_type to_int_type(const char_type &__c)
  {
    return static_cast<int_type>(__c);
  }

  __device__ static bool eq_int_type(const int_type &__c1,
                                     const int_type &__c2)
  {
    return __c1 == __c2;
  }

  __device__ static int_type eof() { return static_cast<int_type>((-1)); }

  __device__ static int_type not_eof(const int_type &__c)
  {
    return !eq_int_type(__c, eof()) ? __c : to_int_type(char_type());
  }
};

template <typename _CharT>
__device__ int char_traits<_CharT>::compare(const char_type *__s1,
                                            const char_type *__s2, size_t __n)
{
  for (size_t __i = 0; __i < __n; ++__i)
    if (lt(__s1[__i], __s2[__i]))
      return -1;
    else if (lt(__s2[__i], __s1[__i]))
      return 1;
  return 0;
}

template <typename _CharT>
__device__ size_t char_traits<_CharT>::length(const char_type *__p)
{
  size_t __i = 0;
  while (!eq(__p[__i], char_type()))
    ++__i;
  return __i;
}

template <typename _CharT>
__device__ const typename char_traits<_CharT>::char_type *
char_traits<_CharT>::find(const char_type *__s, size_t __n,
                          const char_type &__a)
{
  for (size_t __i = 0; __i < __n; ++__i)
    if (eq(__s[__i], __a))
      return __s + __i;
  return 0;
}

template <typename _CharT>
__device__ typename char_traits<_CharT>::char_type *char_traits<_CharT>::move(
    char_type *__s1, const char_type *__s2, size_t __n)
{
  return static_cast<_CharT *>(
      string_op::memmove(__s1, __s2, __n * sizeof(char_type)));
}

template <typename _CharT>
__device__ typename char_traits<_CharT>::char_type *char_traits<_CharT>::copy(
    char_type *__s1, const char_type *__s2, size_t __n)
{
  aicuda::stl::copy(__s2, __s2 + __n, __s1);
  return __s1;
}

template <typename _CharT>
__device__ typename char_traits<_CharT>::char_type *char_traits<_CharT>::assign(
    char_type *__s, size_t __n, char_type __a)
{
  aicuda::stl::fill_n(__s, __n, __a);
  return __s;
}

template <>
struct char_traits<char>
{
  typedef char char_type;
  typedef int int_type;
  typedef streampos pos_type;
  typedef streamoff off_type;
  typedef mbstate_t state_type;

  __device__ static void assign(char_type &__c1, const char_type &__c2)
  {
    __c1 = __c2;
  }

  __device__ static bool eq(const char_type &__c1, const char_type &__c2)
  {
    return __c1 == __c2;
  }

  __device__ static bool lt(const char_type &__c1, const char_type &__c2)
  {
    return __c1 < __c2;
  }

  __device__ static int compare(const char_type *__s1, const char_type *__s2,
                                size_t __n)
  {
    return string_op::memcmp(__s1, __s2, __n);
  }

  __device__ static size_t length(const char_type *__s)
  {
    return string_op::strlen(__s);
  }

  __device__ static const char_type *find(const char_type *__s, size_t __n,
                                          const char_type &__a)
  {
    return static_cast<const char_type *>(string_op::memchr(__s, __a, __n));
  }

  __device__ static char_type *move(char_type *__s1, const char_type *__s2,
                                    size_t __n)
  {
    return static_cast<char_type *>(string_op::memmove(__s1, __s2, __n));
  }

  __device__ static char_type *copy(char_type *__s1, const char_type *__s2,
                                    size_t __n)
  {
    return static_cast<char_type *>(string_op::memcpy(__s1, __s2, __n));
  }

  __device__ static char_type *assign(char_type *__s, size_t __n,
                                      char_type __a)
  {
    return static_cast<char_type *>(string_op::memset(__s, __a, __n));
  }

  __device__ static char_type to_char_type(const int_type &__c)
  {
    return static_cast<char_type>(__c);
  }

  __device__ static int_type to_int_type(const char_type &__c)
  {
    return static_cast<int_type>(static_cast<unsigned char>(__c));
  }

  __device__ static bool eq_int_type(const int_type &__c1,
                                     const int_type &__c2)
  {
    return __c1 == __c2;
  }

  __device__ static int_type eof() { return static_cast<int_type>((-1)); }

  __device__ static int_type not_eof(const int_type &__c)
  {
    return (__c == eof()) ? 0 : __c;
  }
};

template <typename _CharT, typename _Traits = aicuda::stl::char_traits<_CharT>,
          typename _Alloc = aicuda::stl::allocator<_CharT>>
class basic_string;

typedef basic_string<char> string;

template <typename _CharT, typename _Traits, typename _Alloc>
class basic_string
{
  typedef typename _Alloc::template rebind<_CharT>::other _CharT_alloc_type;

public:
  typedef _Traits traits_type;
  typedef typename _Traits::char_type value_type;
  typedef _Alloc allocator_type;
  typedef typename _CharT_alloc_type::size_type size_type;
  typedef typename _CharT_alloc_type::difference_type difference_type;
  typedef typename _CharT_alloc_type::reference reference;
  typedef typename _CharT_alloc_type::const_reference const_reference;
  typedef typename _CharT_alloc_type::pointer pointer;
  typedef typename _CharT_alloc_type::const_pointer const_pointer;
  typedef aicuda::stl::__normal_iterator<pointer, basic_string> iterator;
  typedef aicuda::stl::__normal_iterator<const_pointer, basic_string>
      const_iterator;
  typedef aicuda::stl::reverse_iterator<const_iterator> const_reverse_iterator;
  typedef aicuda::stl::reverse_iterator<iterator> reverse_iterator;

private:
  struct _Rep_base
  {
    size_type _M_length;
    size_type _M_capacity;
    int _M_refcount;
  };

  struct _Rep : _Rep_base
  {
    typedef typename _Alloc::template rebind<char>::other _Raw_bytes_alloc;

    __device__ bool _M_is_leaked() const { return this->_M_refcount < 0; }

    __device__ bool _M_is_shared() const { return this->_M_refcount > 0; }

    __device__ void _M_set_leaked() { this->_M_refcount = -1; }

    __device__ void _M_set_sharable() { this->_M_refcount = 0; }

    __device__ void _M_set_length_and_sharable(size_type __n)
    {
      this->_M_set_sharable();
      this->_M_length = __n;
      traits_type::assign(this->_M_refdata()[__n], _CharT());
    }

    __device__ _CharT *_M_refdata()
    {
      return reinterpret_cast<_CharT *>(this + 1);
    }

    __device__ _CharT *_M_grab(const _Alloc &__alloc1, const _Alloc &__alloc2)
    {
      return (!_M_is_leaked() && __alloc1 == __alloc2) ? _M_refcopy()
                                                       : _M_clone(__alloc1);
    }

    __device__ static _Rep *_S_create(size_type, size_type, const _Alloc &);

    __device__ void _M_dispose(const _Alloc &__a)
    {
      if (__inner_exchange_and_add_dispatch(&this->_M_refcount, -1) <= 0)
        _M_destroy(__a);
    }

    __device__ void _M_destroy(const _Alloc &);

    __device__ _CharT *_M_refcopy()
    {
      this->_M_refcount += 1;
      return _M_refdata();
    }

    __device__ _CharT *_M_clone(const _Alloc &, size_type __res = 0);

  private:
    __device__ inline int __inner_exchange_and_add_dispatch(int *__mem,
                                                            int __val)
    {
      int __result = *__mem;
      *__mem += __val;
      return __result;
    }
  };

  struct _Alloc_hider : _Alloc
  {
    __device__ _Alloc_hider(_CharT *__dat, const _Alloc &__a)
        : _Alloc(__a), _M_p(__dat) {}

    _CharT *_M_p;
  };

public:
  enum
  {
    npos = static_cast<size_type>(-1)
  };

private:
  mutable _Alloc_hider _M_dataplus;

  __device__ _CharT *_M_data() const { return _M_dataplus._M_p; }

  __device__ _CharT *_M_data(_CharT *__p) { return (_M_dataplus._M_p = __p); }

  __device__ _Rep *_M_rep() const
  {
    return &((reinterpret_cast<_Rep *>(_M_data()))[-1]);
  }

  __device__ iterator _M_ibegin() const { return iterator(_M_data()); }

  __device__ iterator _M_iend() const
  {
    return iterator(_M_data() + this->size());
  }

  __device__ void _M_leak()
  {
    if (!_M_rep()->_M_is_leaked())
      _M_leak_hard();
  }

  __device__ size_type _M_check(size_type __pos, const char *__s) const
  {
    if (__pos > this->size())
    {
      printf("__throw_out_of_range\n");
      assert(1 < 0);
    }
    return __pos;
  }

  __device__ void _M_check_length(size_type __n1, size_type __n2,
                                  const char *__s) const
  {
    if (this->max_size() - (this->size() - __n1) < __n2)
    {
      printf("__throw_length_error : %s\n", __s);
      assert(1 < 0);
    }
  }

  __device__ size_type _M_limit(size_type __pos, size_type __off) const
  {
    const bool __testoff = __off < this->size() - __pos;
    return __testoff ? __off : this->size() - __pos;
  }

  __device__ bool _M_disjunct(const _CharT *__s) const
  {
    return (less<const _CharT *>()(__s, _M_data()) ||
            less<const _CharT *>()(_M_data() + this->size(), __s));
  }

  __device__ static void _M_copy(_CharT *__d, const _CharT *__s,
                                 size_type __n)
  {
    if (__n == 1)
      traits_type::assign(*__d, *__s);
    else
      traits_type::copy(__d, __s, __n);
  }

  __device__ static void _M_move(_CharT *__d, const _CharT *__s,
                                 size_type __n)
  {
    if (__n == 1)
      traits_type::assign(*__d, *__s);
    else
      traits_type::move(__d, __s, __n);
  }

  __device__ static void _M_assign(_CharT *__d, size_type __n, _CharT __c)
  {
    if (__n == 1)
      traits_type::assign(*__d, __c);
    else
      traits_type::assign(__d, __n, __c);
  }

  template <class _Iterator>
  __device__ static void _S_copy_chars(_CharT *__p, _Iterator __k1,
                                       _Iterator __k2)
  {
    for (; __k1 != __k2; ++__k1, ++__p)
      traits_type::assign(*__p, *__k1);
  }

  __device__ static void _S_copy_chars(_CharT *__p, iterator __k1,
                                       iterator __k2)
  {
    _S_copy_chars(__p, __k1.base(), __k2.base());
  }

  __device__ static void _S_copy_chars(_CharT *__p, const_iterator __k1,
                                       const_iterator __k2)
  {
    _S_copy_chars(__p, __k1.base(), __k2.base());
  }

  __device__ static void _S_copy_chars(_CharT *__p, _CharT *__k1,
                                       _CharT *__k2)
  {
    _M_copy(__p, __k1, __k2 - __k1);
  }

  __device__ static void _S_copy_chars(_CharT *__p, const _CharT *__k1,
                                       const _CharT *__k2)
  {
    _M_copy(__p, __k1, __k2 - __k1);
  }

  __device__ static int _S_compare(size_type __n1, size_type __n2)
  {
    const difference_type __d = difference_type(__n1 - __n2);

    if (__d > 2147483647)
      return 2147483647;
    else if (__d < -2147483648)
      return -2147483648;
    else
      return int(__d);
  }

  __device__ void _M_mutate(size_type __pos, size_type __len1,
                            size_type __len2);

  __device__ void _M_leak_hard();

public:
  __device__ inline basic_string();

  __device__ explicit basic_string(const _Alloc &__a);

  __device__ basic_string(const basic_string &__str);

  __device__ basic_string(const basic_string &__str, size_type __pos,
                          size_type __n = npos);

  __device__ basic_string(const basic_string &__str, size_type __pos,
                          size_type __n, const _Alloc &__a);
  __device__ basic_string(const _CharT *__s, size_type __n,
                          const _Alloc &__a = _Alloc());

  __device__ basic_string(const _CharT *__s, const _Alloc &__a = _Alloc());

  __device__ basic_string(size_type __n, _CharT __c,
                          const _Alloc &__a = _Alloc());
  template <class _InputIterator>
  __device__ basic_string(_InputIterator __beg, _InputIterator __end,
                          const _Alloc &__a = _Alloc());

  __device__ ~basic_string() { _M_rep()->_M_dispose(this->get_allocator()); }

  __device__ basic_string &operator=(const basic_string &__str)
  {
    return this->assign(__str);
  }

  __device__ basic_string &operator=(const _CharT *__s)
  {
    return this->assign(__s);
  }

  __device__ basic_string &operator=(_CharT __c)
  {
    this->assign(1, __c);
    return *this;
  }

  __device__ iterator begin()
  {
    _M_leak();
    return iterator(_M_data());
  }

  __device__ const_iterator begin() const { return const_iterator(_M_data()); }

  __device__ iterator end()
  {
    _M_leak();
    return iterator(_M_data() + this->size());
  }

  __device__ const_iterator end() const
  {
    return const_iterator(_M_data() + this->size());
  }

  __device__ reverse_iterator rbegin() { return reverse_iterator(this->end()); }

  __device__ const_reverse_iterator rbegin() const
  {
    return const_reverse_iterator(this->end());
  }

  __device__ reverse_iterator rend() { return reverse_iterator(this->begin()); }

  __device__ const_reverse_iterator rend() const
  {
    return const_reverse_iterator(this->begin());
  }

public:
  __device__ size_type size() const { return _M_rep()->_M_length; }

  __device__ size_type length() const { return _M_rep()->_M_length; }

  __device__ constexpr size_type max_size() const
  {
    return ((((size_type)(-1) - sizeof(_Rep_base)) / sizeof(_CharT)) - 1) / 4;
  }

  __device__ void resize(size_type __n, _CharT __c);

  __device__ void resize(size_type __n) { this->resize(__n, _CharT()); }

  __device__ size_type capacity() const { return _M_rep()->_M_capacity; }
  __device__ void reserve(size_type __res_arg = 0);

  __device__ void clear() { _M_mutate(0, this->size(), 0); }

  __device__ bool empty() const { return this->size() == 0; }
  __device__ const_reference operator[](size_type __pos) const
  {
    return _M_data()[__pos];
  }
  __device__ reference operator[](size_type __pos)
  {
    _M_leak();
    return _M_data()[__pos];
  }
  __device__ const_reference at(size_type __n) const
  {
    if (__n >= this->size())
    {
      printf("basic_string:: out_of_range at %ld\n", __n);
      assert(1 < 0);
    }
    return _M_data()[__n];
  }
  __device__ reference at(size_type __n)
  {
    if (__n >= size())
    {
      printf("basic_string:: out_of_range at %ld\n", __n);
      assert(1 < 0);
    }
    _M_leak();
    return _M_data()[__n];
  }

  __device__ basic_string &operator+=(const basic_string &__str)
  {
    return this->append(__str);
  }

  __device__ basic_string &operator+=(const _CharT *__s)
  {
    return this->append(__s);
  }

  __device__ basic_string &operator+=(_CharT __c)
  {
    this->push_back(__c);
    return *this;
  }
  __device__ basic_string &append(const basic_string &__str);
  __device__ basic_string &append(const basic_string &__str, size_type __pos,
                                  size_type __n);

  __device__ basic_string &append(const _CharT *__s, size_type __n);

  __device__ basic_string &append(const _CharT *__s)
  {
    ;
    return this->append(__s, traits_type::length(__s));
  }
  __device__ basic_string &append(size_type __n, _CharT __c);
  template <class _InputIterator>
  __device__ basic_string &append(_InputIterator __first,
                                  _InputIterator __last)
  {
    return this->replace(_M_iend(), _M_iend(), __first, __last);
  }

  __device__ void push_back(_CharT __c)
  {
    const size_type __len = 1 + this->size();
    if (__len > this->capacity() || _M_rep()->_M_is_shared())
      this->reserve(__len);
    traits_type::assign(_M_data()[this->size()], __c);
    _M_rep()->_M_set_length_and_sharable(__len);
  }

  __device__ basic_string &assign(const basic_string &__str);
  __device__ basic_string &assign(const basic_string &__str, size_type __pos,
                                  size_type __n)
  {
    return this->assign(
        __str._M_data() + __str._M_check(__pos, "basic_string::assign"),
        __str._M_limit(__pos, __n));
  }
  __device__ basic_string &assign(const _CharT *__s, size_type __n);
  __device__ basic_string &assign(const _CharT *__s)
  {
    return this->assign(__s, traits_type::length(__s));
  }
  __device__ basic_string &assign(size_type __n, _CharT __c)
  {
    return _M_replace_aux(size_type(0), this->size(), __n, __c);
  }
  template <class _InputIterator>
  __device__ basic_string &assign(_InputIterator __first,
                                  _InputIterator __last)
  {
    return this->replace(_M_ibegin(), _M_iend(), __first, __last);
  }
  __device__ void insert(iterator __p, size_type __n, _CharT __c)
  {
    this->replace(__p, __p, __n, __c);
  }
  template <class _InputIterator>
  __device__ void insert(iterator __p, _InputIterator __beg,
                         _InputIterator __end)
  {
    this->replace(__p, __p, __beg, __end);
  }
  __device__ basic_string &insert(size_type __pos1, const basic_string &__str)
  {
    return this->insert(__pos1, __str, size_type(0), __str.size());
  }
  __device__ basic_string &insert(size_type __pos1, const basic_string &__str,
                                  size_type __pos2, size_type __n)
  {
    return this->insert(
        __pos1,
        __str._M_data() + __str._M_check(__pos2, "basic_string::insert"),
        __str._M_limit(__pos2, __n));
  }
  __device__ basic_string &insert(size_type __pos, const _CharT *__s,
                                  size_type __n);
  __device__ basic_string &insert(size_type __pos, const _CharT *__s)
  {
    return this->insert(__pos, __s, traits_type::length(__s));
  }
  __device__ basic_string &insert(size_type __pos, size_type __n, _CharT __c)
  {
    return _M_replace_aux(_M_check(__pos, "basic_string::insert"), size_type(0),
                          __n, __c);
  }
  __device__ iterator insert(iterator __p, _CharT __c)
  {
    const size_type __pos = __p - _M_ibegin();
    _M_replace_aux(__pos, size_type(0), size_type(1), __c);
    _M_rep()->_M_set_leaked();
    return iterator(_M_data() + __pos);
  }
  __device__ basic_string &erase(size_type __pos = 0, size_type __n = npos)
  {
    _M_mutate(_M_check(__pos, "basic_string::erase"), _M_limit(__pos, __n),
              size_type(0));
    return *this;
  }
  __device__ iterator erase(iterator __position)
  {
    const size_type __pos = __position - _M_ibegin();
    _M_mutate(__pos, size_type(1), size_type(0));
    _M_rep()->_M_set_leaked();
    return iterator(_M_data() + __pos);
  }
  __device__ iterator erase(iterator __first, iterator __last);
  __device__ basic_string &replace(size_type __pos, size_type __n,
                                   const basic_string &__str)
  {
    return this->replace(__pos, __n, __str._M_data(), __str.size());
  }
  __device__ basic_string &replace(size_type __pos1, size_type __n1,
                                   const basic_string &__str, size_type __pos2,
                                   size_type __n2)
  {
    return this->replace(
        __pos1, __n1,
        __str._M_data() + __str._M_check(__pos2, "basic_string::replace"),
        __str._M_limit(__pos2, __n2));
  }
  __device__ basic_string &replace(size_type __pos, size_type __n1,
                                   const _CharT *__s, size_type __n2);
  __device__ basic_string &replace(size_type __pos, size_type __n1,
                                   const _CharT *__s)
  {
    ;
    return this->replace(__pos, __n1, __s, traits_type::length(__s));
  }
  __device__ basic_string &replace(size_type __pos, size_type __n1,
                                   size_type __n2, _CharT __c)
  {
    return _M_replace_aux(_M_check(__pos, "basic_string::replace"),
                          _M_limit(__pos, __n1), __n2, __c);
  }
  __device__ basic_string &replace(iterator __i1, iterator __i2,
                                   const basic_string &__str)
  {
    return this->replace(__i1, __i2, __str._M_data(), __str.size());
  }
  __device__ basic_string &replace(iterator __i1, iterator __i2,
                                   const _CharT *__s, size_type __n)
  {
    return this->replace(__i1 - _M_ibegin(), __i2 - __i1, __s, __n);
  }
  __device__ basic_string &replace(iterator __i1, iterator __i2,
                                   const _CharT *__s)
  {
    return this->replace(__i1, __i2, __s, traits_type::length(__s));
  }
  __device__ basic_string &replace(iterator __i1, iterator __i2, size_type __n,
                                   _CharT __c)
  {
    return _M_replace_aux(__i1 - _M_ibegin(), __i2 - __i1, __n, __c);
  }
  template <class _InputIterator>
  __device__ basic_string &replace(iterator __i1, iterator __i2,
                                   _InputIterator __k1, _InputIterator __k2)
  {
    typedef
        typename aicuda::stl::__is_integer<_InputIterator>::__type _Integral;
    return _M_replace_dispatch(__i1, __i2, __k1, __k2, _Integral());
  }

  __device__ basic_string &replace(iterator __i1, iterator __i2, _CharT *__k1,
                                   _CharT *__k2)
  {
    return this->replace(__i1 - _M_ibegin(), __i2 - __i1, __k1, __k2 - __k1);
  }

  __device__ basic_string &replace(iterator __i1, iterator __i2,
                                   const _CharT *__k1, const _CharT *__k2)
  {
    return this->replace(__i1 - _M_ibegin(), __i2 - __i1, __k1, __k2 - __k1);
  }

  __device__ basic_string &replace(iterator __i1, iterator __i2, iterator __k1,
                                   iterator __k2)
  {
    return this->replace(__i1 - _M_ibegin(), __i2 - __i1, __k1.base(),
                         __k2 - __k1);
  }

  __device__ basic_string &replace(iterator __i1, iterator __i2,
                                   const_iterator __k1, const_iterator __k2)
  {
    return this->replace(__i1 - _M_ibegin(), __i2 - __i1, __k1.base(),
                         __k2 - __k1);
  }

private:
  template <class _Integer>
  __device__ basic_string &_M_replace_dispatch(iterator __i1, iterator __i2,
                                               _Integer __n, _Integer __val,
                                               __true_type)
  {
    return _M_replace_aux(__i1 - _M_ibegin(), __i2 - __i1, __n, __val);
  }

  template <class _InputIterator>
  __device__ basic_string &_M_replace_dispatch(iterator __i1, iterator __i2,
                                               _InputIterator __k1,
                                               _InputIterator __k2,
                                               __false_type);

  __device__ basic_string &_M_replace_aux(size_type __pos1, size_type __n1,
                                          size_type __n2, _CharT __c);

  __device__ basic_string &_M_replace_safe(size_type __pos1, size_type __n1,
                                           const _CharT *__s, size_type __n2);

  template <class _InIterator>
  __device__ static _CharT *_S_construct_aux(_InIterator __beg,
                                             _InIterator __end,
                                             const _Alloc &__a, __false_type)
  {
    typedef typename iterator_traits<_InIterator>::iterator_category _Tag;
    return _S_construct(__beg, __end, __a, _Tag());
  }

  template <class _Integer>
  __device__ static _CharT *_S_construct_aux(_Integer __beg, _Integer __end,
                                             const _Alloc &__a, __true_type)
  {
    return _S_construct(static_cast<size_type>(__beg), __end, __a);
  }

  template <class _InIterator>
  __device__ static _CharT *_S_construct(_InIterator __beg, _InIterator __end,
                                         const _Alloc &__a)
  {
    typedef typename aicuda::stl::__is_integer<_InIterator>::__type _Integral;
    return _S_construct_aux(__beg, __end, __a, _Integral());
  }

  template <class _InIterator>
  __device__ static _CharT *_S_construct(_InIterator __beg, _InIterator __end,
                                         const _Alloc &__a, input_iterator_tag);

  template <class _FwdIterator>
  __device__ static _CharT *_S_construct(_FwdIterator __beg, _FwdIterator __end,
                                         const _Alloc &__a,
                                         forward_iterator_tag);

  __device__ static _CharT *_S_construct(size_type __req, _CharT __c,
                                         const _Alloc &__a);

public:
  __device__ size_type copy(_CharT *__s, size_type __n,
                            size_type __pos = 0) const;
  __device__ void swap(basic_string &__s);
  __device__ const _CharT *c_str() const { return _M_data(); }

  __device__ const _CharT *data() const { return _M_data(); }

  __device__ allocator_type get_allocator() const { return _M_dataplus; }
  __device__ size_type find(const _CharT *__s, size_type __pos,
                            size_type __n) const;
  __device__ size_type find(const basic_string &__str,
                            size_type __pos = 0) const
  {
    return this->find(__str.data(), __pos, __str.size());
  }
  __device__ size_type find(const _CharT *__s, size_type __pos = 0) const
  {
    return this->find(__s, __pos, traits_type::length(__s));
  }
  __device__ size_type find(_CharT __c, size_type __pos = 0) const;
  __device__ size_type rfind(const basic_string &__str,
                             size_type __pos = npos) const
  {
    return this->rfind(__str.data(), __pos, __str.size());
  }
  __device__ size_type rfind(const _CharT *__s, size_type __pos,
                             size_type __n) const;
  __device__ size_type rfind(const _CharT *__s, size_type __pos = npos) const
  {
    ;
    return this->rfind(__s, __pos, traits_type::length(__s));
  }
  __device__ size_type rfind(_CharT __c, size_type __pos = npos) const;
  __device__ size_type find_first_of(const basic_string &__str,
                                     size_type __pos = 0) const
  {
    return this->find_first_of(__str.data(), __pos, __str.size());
  }
  __device__ size_type find_first_of(const _CharT *__s, size_type __pos,
                                     size_type __n) const;
  __device__ size_type find_first_of(const _CharT *__s,
                                     size_type __pos = 0) const
  {
    ;
    return this->find_first_of(__s, __pos, traits_type::length(__s));
  }
  __device__ size_type find_first_of(_CharT __c, size_type __pos = 0) const
  {
    return this->find(__c, __pos);
  }
  __device__ size_type find_last_of(const basic_string &__str,
                                    size_type __pos = npos) const
  {
    return this->find_last_of(__str.data(), __pos, __str.size());
  }
  __device__ size_type find_last_of(const _CharT *__s, size_type __pos,
                                    size_type __n) const;
  __device__ size_type find_last_of(const _CharT *__s,
                                    size_type __pos = npos) const
  {
    ;
    return this->find_last_of(__s, __pos, traits_type::length(__s));
  }
  __device__ size_type find_last_of(_CharT __c, size_type __pos = npos) const
  {
    return this->rfind(__c, __pos);
  }
  __device__ size_type find_first_not_of(const basic_string &__str,
                                         size_type __pos = 0) const
  {
    return this->find_first_not_of(__str.data(), __pos, __str.size());
  }
  __device__ size_type find_first_not_of(const _CharT *__s, size_type __pos,
                                         size_type __n) const;
  __device__ size_type find_first_not_of(const _CharT *__s,
                                         size_type __pos = 0) const
  {
    ;
    return this->find_first_not_of(__s, __pos, traits_type::length(__s));
  }
  __device__ size_type find_first_not_of(_CharT __c, size_type __pos = 0) const;
  __device__ size_type find_last_not_of(const basic_string &__str,
                                        size_type __pos = npos) const
  {
    return this->find_last_not_of(__str.data(), __pos, __str.size());
  }
  __device__ size_type find_last_not_of(const _CharT *__s, size_type __pos,
                                        size_type __n) const;
  __device__ size_type find_last_not_of(const _CharT *__s,
                                        size_type __pos = npos) const
  {
    ;
    return this->find_last_not_of(__s, __pos, traits_type::length(__s));
  }
  __device__ size_type find_last_not_of(_CharT __c,
                                        size_type __pos = npos) const;
  __device__ basic_string substr(size_type __pos = 0,
                                 size_type __n = npos) const
  {
    return basic_string(*this, _M_check(__pos, "basic_string::substr"), __n);
  }
  __device__ int compare(const basic_string &__str) const
  {
    const size_type __size = this->size();
    const size_type __osize = __str.size();
    const size_type __len = aicuda::stl::min(__size, __osize);

    int __r = traits_type::compare(_M_data(), __str.data(), __len);
    if (!__r)
      __r = _S_compare(__size, __osize);
    return __r;
  }
  __device__ int compare(size_type __pos, size_type __n,
                         const basic_string &__str) const;
  __device__ int compare(size_type __pos1, size_type __n1,
                         const basic_string &__str, size_type __pos2,
                         size_type __n2) const;
  __device__ int compare(const _CharT *__s) const;
  __device__ int compare(size_type __pos, size_type __n1,
                         const _CharT *__s) const;
  __device__ int compare(size_type __pos, size_type __n1, const _CharT *__s,
                         size_type __n2) const;

  template <typename Real>
  __device__ static basic_string to_string(Real __n, int __prec = 5,
                                           int __base = 10)
  {
    typedef typename aicuda::stl::__conditional_type<aicuda::stl::__is_boolean<Real>::__value,      bool,
            typename aicuda::stl::__conditional_type<aicuda::stl::__is_integer<Real>::__value,      long long int,
            typename aicuda::stl::__conditional_type<aicuda::stl::__is_floating<Real>::__value,     double,
            typename aicuda::stl::__conditional_type<aicuda::stl::__is_char_pointer<Real>::__value, const char *,
            void>::__type>::__type>::__type>::__type __Ty;
    return to_string((__Ty)__n, __prec, __base);
  }

  __device__ static basic_string to_string(const char *__input, int __prec,
                                           int __base)
  {
    return basic_string(__input);
  }

  __device__ static basic_string to_string(bool __input, int __prec,
                                           int __base)
  {
    return __input ? "true" : "false";
  }

  __device__ static basic_string to_string(long long int __input, int __prec,
                                           int __base)
  {
    char buf[36];
    char *p = &buf[34];
    buf[35] = 0;
    int minus = 0;
    if (__input < 0)
    {
      minus = 1;
      __input = -__input;
    }
    if (__input == 0)
      *--p = '0';
    else
      while (__input > 0)
      {
        *--p = "0123456789abcdef"[__input % __base];
        __input /= __base;
      }
    if (minus)
      *--p = '-';
    return p;
  }

  __device__ static basic_string to_string(double __input, int __prec,
                                           int __base)
  {
    long long int __Integer = (long long int)__input;
    basic_string __ret = to_string(__Integer, __prec, __base);
    __ret.append(1, '.');
    __input -= __Integer;
    for (int i = 0; i < __prec; ++i)
      __input *= 10;
    basic_string postfix = to_string((long long int)__input, __prec, __base);
    int c = __prec - postfix.size();
    if (c > 0)
      __ret.append(c, '0');

    return __ret + postfix;
  }
};

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ inline basic_string<_CharT, _Traits, _Alloc>::basic_string()
    : _M_dataplus(_S_construct(size_type(), _CharT(), _Alloc()), _Alloc()){}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ basic_string<_CharT, _Traits, _Alloc> operator+(
    const basic_string<_CharT, _Traits, _Alloc> &__lhs,
    const basic_string<_CharT, _Traits, _Alloc> &__rhs)
{
  basic_string<_CharT, _Traits, _Alloc> __str(__lhs);
  __str.append(__rhs);
  return __str;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ basic_string<_CharT, _Traits, _Alloc> operator+(
    const _CharT *__lhs, const basic_string<_CharT, _Traits, _Alloc> &__rhs);

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ basic_string<_CharT, _Traits, _Alloc> operator+(
    _CharT __lhs, const basic_string<_CharT, _Traits, _Alloc> &__rhs);

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ inline basic_string<_CharT, _Traits, _Alloc> operator+(
    const basic_string<_CharT, _Traits, _Alloc> &__lhs, const _CharT *__rhs)
{
  basic_string<_CharT, _Traits, _Alloc> __str(__lhs);
  __str.append(__rhs);
  return __str;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ inline basic_string<_CharT, _Traits, _Alloc> operator+(
    const basic_string<_CharT, _Traits, _Alloc> &__lhs, _CharT __rhs)
{
  typedef basic_string<_CharT, _Traits, _Alloc> __string_type;
  typedef typename __string_type::size_type __size_type;
  __string_type __str(__lhs);
  __str.append(__size_type(1), __rhs);
  return __str;
}
template <typename _CharT, typename _Traits, typename _Alloc>
__device__ inline bool operator==(
    const basic_string<_CharT, _Traits, _Alloc> &__lhs,
    const basic_string<_CharT, _Traits, _Alloc> &__rhs)
{
  return __lhs.compare(__rhs) == 0;
}

template <typename _CharT>
__device__ inline
    typename aicuda::stl::__enable_if<__is_char<_CharT>::__value, bool>::__type
    operator==(const basic_string<_CharT> &__lhs,
               const basic_string<_CharT> &__rhs)
{
  return (__lhs.size() == __rhs.size() &&
          !aicuda::stl::char_traits<_CharT>::compare(__lhs.data(), __rhs.data(),
                                                     __lhs.size()));
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ inline bool operator==(
    const _CharT *__lhs, const basic_string<_CharT, _Traits, _Alloc> &__rhs)
{
  return __rhs.compare(__lhs) == 0;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ inline bool operator==(
    const basic_string<_CharT, _Traits, _Alloc> &__lhs, const _CharT *__rhs)
{
  return __lhs.compare(__rhs) == 0;
}
template <typename _CharT, typename _Traits, typename _Alloc>
__device__ inline bool operator!=(
    const basic_string<_CharT, _Traits, _Alloc> &__lhs,
    const basic_string<_CharT, _Traits, _Alloc> &__rhs)
{
  return !(__lhs == __rhs);
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ inline bool operator!=(
    const _CharT *__lhs, const basic_string<_CharT, _Traits, _Alloc> &__rhs)
{
  return !(__lhs == __rhs);
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ inline bool operator!=(
    const basic_string<_CharT, _Traits, _Alloc> &__lhs, const _CharT *__rhs)
{
  return !(__lhs == __rhs);
}
template <typename _CharT, typename _Traits, typename _Alloc>
__device__ inline bool operator<(
    const basic_string<_CharT, _Traits, _Alloc> &__lhs,
    const basic_string<_CharT, _Traits, _Alloc> &__rhs)
{
  return __lhs.compare(__rhs) < 0;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ inline bool operator<(
    const basic_string<_CharT, _Traits, _Alloc> &__lhs, const _CharT *__rhs)
{
  return __lhs.compare(__rhs) < 0;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ inline bool operator<(
    const _CharT *__lhs, const basic_string<_CharT, _Traits, _Alloc> &__rhs)
{
  return __rhs.compare(__lhs) > 0;
}
template <typename _CharT, typename _Traits, typename _Alloc>
__device__ inline bool operator>(
    const basic_string<_CharT, _Traits, _Alloc> &__lhs,
    const basic_string<_CharT, _Traits, _Alloc> &__rhs)
{
  return __lhs.compare(__rhs) > 0;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ inline bool operator>(
    const basic_string<_CharT, _Traits, _Alloc> &__lhs, const _CharT *__rhs)
{
  return __lhs.compare(__rhs) > 0;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ inline bool operator>(
    const _CharT *__lhs, const basic_string<_CharT, _Traits, _Alloc> &__rhs)
{
  return __rhs.compare(__lhs) < 0;
}
template <typename _CharT, typename _Traits, typename _Alloc>
__device__ inline bool operator<=(
    const basic_string<_CharT, _Traits, _Alloc> &__lhs,
    const basic_string<_CharT, _Traits, _Alloc> &__rhs)
{
  return __lhs.compare(__rhs) <= 0;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ inline bool operator<=(
    const basic_string<_CharT, _Traits, _Alloc> &__lhs, const _CharT *__rhs)
{
  return __lhs.compare(__rhs) <= 0;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ inline bool operator<=(
    const _CharT *__lhs, const basic_string<_CharT, _Traits, _Alloc> &__rhs)
{
  return __rhs.compare(__lhs) >= 0;
}
template <typename _CharT, typename _Traits, typename _Alloc>
__device__ inline bool operator>=(
    const basic_string<_CharT, _Traits, _Alloc> &__lhs,
    const basic_string<_CharT, _Traits, _Alloc> &__rhs)
{
  return __lhs.compare(__rhs) >= 0;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ inline bool operator>=(
    const basic_string<_CharT, _Traits, _Alloc> &__lhs, const _CharT *__rhs)
{
  return __lhs.compare(__rhs) >= 0;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ inline bool operator>=(
    const _CharT *__lhs, const basic_string<_CharT, _Traits, _Alloc> &__rhs)
{
  return __rhs.compare(__lhs) <= 0;
}
template <typename _CharT, typename _Traits, typename _Alloc>
__device__ inline void swap(basic_string<_CharT, _Traits, _Alloc> &__lhs,
                            basic_string<_CharT, _Traits, _Alloc> &__rhs)
{
  __lhs.swap(__rhs);
}

template <typename _CharT, typename _Traits, typename _Alloc>
template <typename _InIterator>
__device__ _CharT *basic_string<_CharT, _Traits, _Alloc>::_S_construct(
    _InIterator __beg, _InIterator __end, const _Alloc &__a,
    input_iterator_tag)
{
  _CharT __buf[128];
  size_type __len = 0;
  while (__beg != __end && __len < sizeof(__buf) / sizeof(_CharT))
  {
    __buf[__len++] = *__beg;
    ++__beg;
  }
  _Rep *__r = _Rep::_S_create(__len, size_type(0), __a);
  _M_copy(__r->_M_refdata(), __buf, __len);

  while (__beg != __end)
  {
    if (__len == __r->_M_capacity)
    {
      _Rep *__another = _Rep::_S_create(__len + 1, __len, __a);
      _M_copy(__another->_M_refdata(), __r->_M_refdata(), __len);
      __r->_M_destroy(__a);
      __r = __another;
    }
    __r->_M_refdata()[__len++] = *__beg;
    ++__beg;
  }

  __r->_M_set_length_and_sharable(__len);
  return __r->_M_refdata();
}

template <typename _CharT, typename _Traits, typename _Alloc>
template <typename _InIterator>
__device__ _CharT *basic_string<_CharT, _Traits, _Alloc>::_S_construct(
    _InIterator __beg, _InIterator __end, const _Alloc &__a,
    forward_iterator_tag)
{
  if (aicuda::stl::__is_null_pointer(__beg) && __beg != __end)
  {
    printf("basic_string::_S_construct NULL not valid\n");
    assert(1 < 0);
  }

  const size_type __dnew =
      static_cast<size_type>(aicuda::stl::distance(__beg, __end));

  _Rep *__r = _Rep::_S_create(__dnew, size_type(0), __a);

  _S_copy_chars(__r->_M_refdata(), __beg, __end);

  __r->_M_set_length_and_sharable(__dnew);
  return __r->_M_refdata();
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ _CharT *basic_string<_CharT, _Traits, _Alloc>::_S_construct(
    size_type __n, _CharT __c, const _Alloc &__a)
{
  _Rep *__r = _Rep::_S_create(__n, size_type(0), __a);
  if (__n)
    _M_assign(__r->_M_refdata(), __n, __c);

  __r->_M_set_length_and_sharable(__n);
  return __r->_M_refdata();
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ basic_string<_CharT, _Traits, _Alloc>::basic_string(
    const basic_string &__str)
    : _M_dataplus(__str._M_rep()->_M_grab(_Alloc(__str.get_allocator()),
                                          __str.get_allocator()),
                  __str.get_allocator()) {}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ basic_string<_CharT, _Traits, _Alloc>::basic_string(
    const _Alloc &__a)
    : _M_dataplus(_S_construct(size_type(), _CharT(), __a), __a) {}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ basic_string<_CharT, _Traits, _Alloc>::basic_string(
    const basic_string &__str, size_type __pos, size_type __n)
    : _M_dataplus(
          _S_construct(__str._M_data() +
                           __str._M_check(__pos, "basic_string::basic_string"),
                       __str._M_data() + __str._M_limit(__pos, __n) + __pos,
                       _Alloc()),
          _Alloc()) {}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ basic_string<_CharT, _Traits, _Alloc>::basic_string(
    const basic_string &__str, size_type __pos, size_type __n,
    const _Alloc &__a)
    : _M_dataplus(
          _S_construct(__str._M_data() +
                           __str._M_check(__pos, "basic_string::basic_string"),
                       __str._M_data() + __str._M_limit(__pos, __n) + __pos,
                       __a),
          __a) {}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ basic_string<_CharT, _Traits, _Alloc>::basic_string(
    const _CharT *__s, size_type __n, const _Alloc &__a)
    : _M_dataplus(_S_construct(__s, __s + __n, __a), __a) {}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ basic_string<_CharT, _Traits, _Alloc>::basic_string(
    const _CharT *__s, const _Alloc &__a)
    : _M_dataplus(
          _S_construct(__s, __s ? __s + traits_type::length(__s) : __s + npos,
                       __a),
          __a) {}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ basic_string<_CharT, _Traits, _Alloc>::basic_string(
    size_type __n, _CharT __c, const _Alloc &__a)
    : _M_dataplus(_S_construct(__n, __c, __a), __a) {}

template <typename _CharT, typename _Traits, typename _Alloc>
template <typename _InputIterator>
__device__ basic_string<_CharT, _Traits, _Alloc>::basic_string(
    _InputIterator __beg, _InputIterator __end, const _Alloc &__a)
    : _M_dataplus(_S_construct(__beg, __end, __a), __a) {}
template <typename _CharT, typename _Traits, typename _Alloc>
__device__ basic_string<_CharT, _Traits, _Alloc>
    &basic_string<_CharT, _Traits, _Alloc>::assign(const basic_string &__str)
{
  if (_M_rep() != __str._M_rep())
  {
    const allocator_type __a = this->get_allocator();
    _CharT *__tmp = __str._M_rep()->_M_grab(__a, __str.get_allocator());
    _M_rep()->_M_dispose(__a);
    _M_data(__tmp);
  }
  return *this;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ basic_string<_CharT, _Traits, _Alloc>
    &basic_string<_CharT, _Traits, _Alloc>::assign(const _CharT *__s,
                                                   size_type __n)
{
  ;
  _M_check_length(this->size(), __n, "basic_string::assign");
  if (_M_disjunct(__s) || _M_rep()->_M_is_shared())
    return _M_replace_safe(size_type(0), this->size(), __s, __n);
  else
  {
    const size_type __pos = __s - _M_data();
    if (__pos >= __n)
      _M_copy(_M_data(), __s, __n);
    else if (__pos)
      _M_move(_M_data(), __s, __n);
    _M_rep()->_M_set_length_and_sharable(__n);
    return *this;
  }
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ basic_string<_CharT, _Traits, _Alloc>
    &basic_string<_CharT, _Traits, _Alloc>::append(size_type __n, _CharT __c)
{
  if (__n)
  {
    _M_check_length(size_type(0), __n, "basic_string::append");
    const size_type __len = __n + this->size();
    if (__len > this->capacity() || _M_rep()->_M_is_shared())
      this->reserve(__len);
    _M_assign(_M_data() + this->size(), __n, __c);
    _M_rep()->_M_set_length_and_sharable(__len);
  }
  return *this;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ basic_string<_CharT, _Traits, _Alloc>
    &basic_string<_CharT, _Traits, _Alloc>::append(const _CharT *__s,
                                                   size_type __n)
{
  ;
  if (__n)
  {
    _M_check_length(size_type(0), __n, "basic_string::append");
    const size_type __len = __n + this->size();
    if (__len > this->capacity() || _M_rep()->_M_is_shared())
    {
      if (_M_disjunct(__s))
        this->reserve(__len);
      else
      {
        const size_type __off = __s - _M_data();
        this->reserve(__len);
        __s = _M_data() + __off;
      }
    }
    _M_copy(_M_data() + this->size(), __s, __n);
    _M_rep()->_M_set_length_and_sharable(__len);
  }
  return *this;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ basic_string<_CharT, _Traits, _Alloc>
    &basic_string<_CharT, _Traits, _Alloc>::append(const basic_string &__str)
{
  const size_type __size = __str.size();
  if (__size)
  {
    const size_type __len = __size + this->size();
    if (__len > this->capacity() || _M_rep()->_M_is_shared())
      this->reserve(__len);
    _M_copy(_M_data() + this->size(), __str._M_data(), __size);
    _M_rep()->_M_set_length_and_sharable(__len);
  }
  return *this;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ basic_string<_CharT, _Traits, _Alloc> &
basic_string<_CharT, _Traits, _Alloc>::append(const basic_string &__str,
                                              size_type __pos, size_type __n)
{
  __str._M_check(__pos, "basic_string::append");
  __n = __str._M_limit(__pos, __n);
  if (__n)
  {
    const size_type __len = __n + this->size();
    if (__len > this->capacity() || _M_rep()->_M_is_shared())
      this->reserve(__len);
    _M_copy(_M_data() + this->size(), __str._M_data() + __pos, __n);
    _M_rep()->_M_set_length_and_sharable(__len);
  }
  return *this;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ basic_string<_CharT, _Traits, _Alloc>
    &basic_string<_CharT, _Traits, _Alloc>::insert(size_type __pos,
                                                   const _CharT *__s,
                                                   size_type __n)
{
  ;
  _M_check(__pos, "basic_string::insert");
  _M_check_length(size_type(0), __n, "basic_string::insert");
  if (_M_disjunct(__s) || _M_rep()->_M_is_shared())
    return _M_replace_safe(__pos, size_type(0), __s, __n);
  else
  {
    const size_type __off = __s - _M_data();
    _M_mutate(__pos, 0, __n);
    __s = _M_data() + __off;
    _CharT *__p = _M_data() + __pos;
    if (__s + __n <= __p)
      _M_copy(__p, __s, __n);
    else if (__s >= __p)
      _M_copy(__p, __s + __n, __n);
    else
    {
      const size_type __nleft = __p - __s;
      _M_copy(__p, __s, __nleft);
      _M_copy(__p + __nleft, __p + __n, __n - __nleft);
    }
    return *this;
  }
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ typename basic_string<_CharT, _Traits, _Alloc>::iterator
basic_string<_CharT, _Traits, _Alloc>::erase(iterator __first,
                                             iterator __last)
{
  const size_type __size = __last - __first;
  if (__size)
  {
    const size_type __pos = __first - _M_ibegin();
    _M_mutate(__pos, __size, size_type(0));
    _M_rep()->_M_set_leaked();
    return iterator(_M_data() + __pos);
  }
  else
    return __first;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ basic_string<_CharT, _Traits, _Alloc> &
basic_string<_CharT, _Traits, _Alloc>::replace(size_type __pos, size_type __n1,
                                               const _CharT *__s,
                                               size_type __n2)
{
  ;
  _M_check(__pos, "basic_string::replace");
  __n1 = _M_limit(__pos, __n1);
  _M_check_length(__n1, __n2, "basic_string::replace");
  bool __left;
  if (_M_disjunct(__s) || _M_rep()->_M_is_shared())
    return _M_replace_safe(__pos, __n1, __s, __n2);
  else if ((__left = __s + __n2 <= _M_data() + __pos) ||
           _M_data() + __pos + __n1 <= __s)
  {
    size_type __off = __s - _M_data();
    __left ? __off : (__off += __n2 - __n1);
    _M_mutate(__pos, __n1, __n2);
    _M_copy(_M_data() + __pos, _M_data() + __off, __n2);
    return *this;
  }
  else
  {
    const basic_string __tmp(__s, __n2);
    return _M_replace_safe(__pos, __n1, __tmp._M_data(), __n2);
  }
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ void basic_string<_CharT, _Traits, _Alloc>::_Rep::_M_destroy(
    const _Alloc &__a)
{
  const size_type __size =
      sizeof(_Rep_base) + (this->_M_capacity + 1) * sizeof(_CharT);
  _Raw_bytes_alloc(__a).deallocate(reinterpret_cast<char *>(this), __size);
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ void basic_string<_CharT, _Traits, _Alloc>::_M_leak_hard()
{
  if (_M_rep()->_M_is_shared())
    _M_mutate(0, 0, 0);
  _M_rep()->_M_set_leaked();
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ void basic_string<_CharT, _Traits, _Alloc>::_M_mutate(
    size_type __pos, size_type __len1, size_type __len2)
{
  const size_type __old_size = this->size();
  const size_type __new_size = __old_size + __len2 - __len1;
  const size_type __how_much = __old_size - __pos - __len1;

  if (__new_size > this->capacity() || _M_rep()->_M_is_shared())
  {
    const allocator_type __a = get_allocator();
    _Rep *__r = _Rep::_S_create(__new_size, this->capacity(), __a);

    if (__pos)
      _M_copy(__r->_M_refdata(), _M_data(), __pos);
    if (__how_much)
      _M_copy(__r->_M_refdata() + __pos + __len2, _M_data() + __pos + __len1,
              __how_much);

    _M_rep()->_M_dispose(__a);
    _M_data(__r->_M_refdata());
  }
  else if (__how_much && __len1 != __len2)
  {
    _M_move(_M_data() + __pos + __len2, _M_data() + __pos + __len1, __how_much);
  }
  _M_rep()->_M_set_length_and_sharable(__new_size);
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ void basic_string<_CharT, _Traits, _Alloc>::reserve(
    size_type __res)
{
  if (__res != this->capacity() || _M_rep()->_M_is_shared())
  {
    if (__res < this->size())
      __res = this->size();
    const allocator_type __a = get_allocator();
    _CharT *__tmp = _M_rep()->_M_clone(__a, __res - this->size());
    _M_rep()->_M_dispose(__a);
    _M_data(__tmp);
  }
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ void basic_string<_CharT, _Traits, _Alloc>::swap(basic_string &__s)
{
  if (_M_rep()->_M_is_leaked())
    _M_rep()->_M_set_sharable();
  if (__s._M_rep()->_M_is_leaked())
    __s._M_rep()->_M_set_sharable();
  if (this->get_allocator() == __s.get_allocator())
  {
    _CharT *__tmp = _M_data();
    _M_data(__s._M_data());
    __s._M_data(__tmp);
  }

  else
  {
    const basic_string __tmp1(_M_ibegin(), _M_iend(), __s.get_allocator());
    const basic_string __tmp2(__s._M_ibegin(), __s._M_iend(),
                              this->get_allocator());
    *this = __tmp2;
    __s = __tmp1;
  }
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ typename basic_string<_CharT, _Traits, _Alloc>::_Rep *
basic_string<_CharT, _Traits, _Alloc>::_Rep::_S_create(size_type __capacity,
                                                       size_type __old_capacity,
                                                       const _Alloc &__alloc)
{
  constexpr size_type __max_size =
      ((((size_type)(-1) - sizeof(_Rep_base)) / sizeof(_CharT)) - 1) / 4;
  if (__capacity > __max_size)
  {
    printf("__throw_length_error basic_string:: _S_create\n");
    assert(1 < 0);
  }

  const size_type __pagesize = 4096;
  const size_type __malloc_header_size = 4 * sizeof(void *);

  if (__capacity > __old_capacity && __capacity < 2 * __old_capacity)
    __capacity = 2 * __old_capacity;

  size_type __size = (__capacity + 1) * sizeof(_CharT) + sizeof(_Rep);

  const size_type __adj_size = __size + __malloc_header_size;
  if (__adj_size > __pagesize && __capacity > __old_capacity)
  {
    const size_type __extra = __pagesize - __adj_size % __pagesize;
    __capacity += __extra / sizeof(_CharT);

    if (__capacity > __max_size)
      __capacity = __max_size;
    __size = (__capacity + 1) * sizeof(_CharT) + sizeof(_Rep);
  }

  void *__place = _Raw_bytes_alloc(__alloc).allocate(__size);
  _Rep *__p = new (__place) _Rep;
  __p->_M_capacity = __capacity;

  __p->_M_set_sharable();
  return __p;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ _CharT *basic_string<_CharT, _Traits, _Alloc>::_Rep::_M_clone(
    const _Alloc &__alloc, size_type __res)
{
  const size_type __requested_cap = this->_M_length + __res;
  _Rep *__r = _Rep::_S_create(__requested_cap, this->_M_capacity, __alloc);
  if (this->_M_length)
    _M_copy(__r->_M_refdata(), _M_refdata(), this->_M_length);

  __r->_M_set_length_and_sharable(this->_M_length);
  return __r->_M_refdata();
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ void basic_string<_CharT, _Traits, _Alloc>::resize(size_type __n,
                                                              _CharT __c)
{
  const size_type __size = this->size();
  _M_check_length(__size, __n, "basic_string::resize");
  if (__size < __n)
    this->append(__n - __size, __c);
  else if (__n < __size)
    this->erase(__n);
}

template <typename _CharT, typename _Traits, typename _Alloc>
template <typename _InputIterator>
__device__ basic_string<_CharT, _Traits, _Alloc> &
basic_string<_CharT, _Traits, _Alloc>::_M_replace_dispatch(iterator __i1,
                                                           iterator __i2,
                                                           _InputIterator __k1,
                                                           _InputIterator __k2,
                                                           __false_type)
{
  const basic_string __s(__k1, __k2);
  const size_type __n1 = __i2 - __i1;
  _M_check_length(__n1, __s.size(), "basic_string::_M_replace_dispatch");
  return _M_replace_safe(__i1 - _M_ibegin(), __n1, __s._M_data(), __s.size());
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ basic_string<_CharT, _Traits, _Alloc>
    &basic_string<_CharT, _Traits, _Alloc>::_M_replace_aux(size_type __pos1,
                                                           size_type __n1,
                                                           size_type __n2,
                                                           _CharT __c)
{
  _M_check_length(__n1, __n2, "basic_string::_M_replace_aux");
  _M_mutate(__pos1, __n1, __n2);
  if (__n2)
    _M_assign(_M_data() + __pos1, __n2, __c);
  return *this;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ basic_string<_CharT, _Traits, _Alloc>
    &basic_string<_CharT, _Traits, _Alloc>::_M_replace_safe(size_type __pos1,
                                                            size_type __n1,
                                                            const _CharT *__s,
                                                            size_type __n2)
{
  _M_mutate(__pos1, __n1, __n2);
  if (__n2)
    _M_copy(_M_data() + __pos1, __s, __n2);
  return *this;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ basic_string<_CharT, _Traits, _Alloc> operator+(
    const _CharT *__lhs, const basic_string<_CharT, _Traits, _Alloc> &__rhs)
{
  ;
  typedef basic_string<_CharT, _Traits, _Alloc> __string_type;
  typedef typename __string_type::size_type __size_type;
  const __size_type __len = _Traits::length(__lhs);
  __string_type __str;
  __str.reserve(__len + __rhs.size());
  __str.append(__lhs, __len);
  __str.append(__rhs);
  return __str;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ basic_string<_CharT, _Traits, _Alloc> operator+(
    _CharT __lhs, const basic_string<_CharT, _Traits, _Alloc> &__rhs)
{
  typedef basic_string<_CharT, _Traits, _Alloc> __string_type;
  typedef typename __string_type::size_type __size_type;
  __string_type __str;
  const __size_type __len = __rhs.size();
  __str.reserve(__len + 1);
  __str.append(__size_type(1), __lhs);
  __str.append(__rhs);
  return __str;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ typename basic_string<_CharT, _Traits, _Alloc>::size_type
basic_string<_CharT, _Traits, _Alloc>::copy(_CharT *__s, size_type __n,
                                            size_type __pos) const
{
  _M_check(__pos, "basic_string::copy");
  __n = _M_limit(__pos, __n);
  ;
  if (__n)
    _M_copy(__s, _M_data() + __pos, __n);

  return __n;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ typename basic_string<_CharT, _Traits, _Alloc>::size_type
basic_string<_CharT, _Traits, _Alloc>::find(const _CharT *__s, size_type __pos,
                                            size_type __n) const
{
  const size_type __size = this->size();
  const _CharT *__data = _M_data();

  if (__n == 0)
    return __pos <= __size ? __pos : npos;

  if (__n <= __size)
  {
    for (; __pos <= __size - __n; ++__pos)
      if (traits_type::eq(__data[__pos], __s[0]) &&
          traits_type::compare(__data + __pos + 1, __s + 1, __n - 1) == 0)
        return __pos;
  }
  return npos;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ typename basic_string<_CharT, _Traits, _Alloc>::size_type
basic_string<_CharT, _Traits, _Alloc>::find(_CharT __c, size_type __pos) const
{
  size_type __ret = npos;
  const size_type __size = this->size();
  if (__pos < __size)
  {
    const _CharT *__data = _M_data();
    const size_type __n = __size - __pos;
    const _CharT *__p = traits_type::find(__data + __pos, __n, __c);
    if (__p)
      __ret = __p - __data;
  }
  return __ret;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ typename basic_string<_CharT, _Traits, _Alloc>::size_type
basic_string<_CharT, _Traits, _Alloc>::rfind(const _CharT *__s, size_type __pos,
                                             size_type __n) const
{
  const size_type __size = this->size();
  if (__n <= __size)
  {
    __pos = aicuda::stl::min(size_type(__size - __n), __pos);
    const _CharT *__data = _M_data();
    do
    {
      if (traits_type::compare(__data + __pos, __s, __n) == 0)
        return __pos;
    } while (__pos-- > 0);
  }
  return npos;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ typename basic_string<_CharT, _Traits, _Alloc>::size_type
basic_string<_CharT, _Traits, _Alloc>::rfind(_CharT __c,
                                             size_type __pos) const
{
  size_type __size = this->size();
  if (__size)
  {
    if (--__size > __pos)
      __size = __pos;
    for (++__size; __size-- > 0;)
      if (traits_type::eq(_M_data()[__size], __c))
        return __size;
  }
  return npos;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ typename basic_string<_CharT, _Traits, _Alloc>::size_type
basic_string<_CharT, _Traits, _Alloc>::find_first_of(const _CharT *__s,
                                                     size_type __pos,
                                                     size_type __n) const
{
  for (; __n && __pos < this->size(); ++__pos)
  {
    const _CharT *__p = traits_type::find(__s, __n, _M_data()[__pos]);
    if (__p)
      return __pos;
  }
  return npos;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ typename basic_string<_CharT, _Traits, _Alloc>::size_type
basic_string<_CharT, _Traits, _Alloc>::find_last_of(const _CharT *__s,
                                                    size_type __pos,
                                                    size_type __n) const
{
  size_type __size = this->size();
  if (__size && __n)
  {
    if (--__size > __pos)
      __size = __pos;
    do
    {
      if (traits_type::find(__s, __n, _M_data()[__size]))
        return __size;
    } while (__size-- != 0);
  }
  return npos;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ typename basic_string<_CharT, _Traits, _Alloc>::size_type
basic_string<_CharT, _Traits, _Alloc>::find_first_not_of(const _CharT *__s,
                                                         size_type __pos,
                                                         size_type __n) const
{
  for (; __pos < this->size(); ++__pos)
    if (!traits_type::find(__s, __n, _M_data()[__pos]))
      return __pos;
  return npos;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ typename basic_string<_CharT, _Traits, _Alloc>::size_type
basic_string<_CharT, _Traits, _Alloc>::find_first_not_of(
    _CharT __c, size_type __pos) const
{
  for (; __pos < this->size(); ++__pos)
    if (!traits_type::eq(_M_data()[__pos], __c))
      return __pos;
  return npos;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ typename basic_string<_CharT, _Traits, _Alloc>::size_type
basic_string<_CharT, _Traits, _Alloc>::find_last_not_of(const _CharT *__s,
                                                        size_type __pos,
                                                        size_type __n) const
{
  size_type __size = this->size();
  if (__size)
  {
    if (--__size > __pos)
      __size = __pos;
    do
    {
      if (!traits_type::find(__s, __n, _M_data()[__size]))
        return __size;
    } while (__size--);
  }
  return npos;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ typename basic_string<_CharT, _Traits, _Alloc>::size_type
basic_string<_CharT, _Traits, _Alloc>::find_last_not_of(_CharT __c,
                                                        size_type __pos) const
{
  size_type __size = this->size();
  if (__size)
  {
    if (--__size > __pos)
      __size = __pos;
    do
    {
      if (!traits_type::eq(_M_data()[__size], __c))
        return __size;
    } while (__size--);
  }
  return npos;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ int basic_string<_CharT, _Traits, _Alloc>::compare(
    size_type __pos, size_type __n, const basic_string &__str) const
{
  _M_check(__pos, "basic_string::compare");
  __n = _M_limit(__pos, __n);
  const size_type __osize = __str.size();
  const size_type __len = aicuda::stl::min(__n, __osize);
  int __r = traits_type::compare(_M_data() + __pos, __str.data(), __len);
  if (!__r)
    __r = _S_compare(__n, __osize);
  return __r;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ int basic_string<_CharT, _Traits, _Alloc>::compare(
    size_type __pos1, size_type __n1, const basic_string &__str,
    size_type __pos2, size_type __n2) const
{
  _M_check(__pos1, "basic_string::compare");
  __str._M_check(__pos2, "basic_string::compare");
  __n1 = _M_limit(__pos1, __n1);
  __n2 = __str._M_limit(__pos2, __n2);
  const size_type __len = aicuda::stl::min(__n1, __n2);
  int __r =
      traits_type::compare(_M_data() + __pos1, __str.data() + __pos2, __len);
  if (!__r)
    __r = _S_compare(__n1, __n2);
  return __r;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ int basic_string<_CharT, _Traits, _Alloc>::compare(
    const _CharT *__s) const
{
  const size_type __size = this->size();
  const size_type __osize = traits_type::length(__s);
  const size_type __len = aicuda::stl::min(__size, __osize);
  int __r = traits_type::compare(_M_data(), __s, __len);
  if (!__r)
    __r = _S_compare(__size, __osize);
  return __r;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ int basic_string<_CharT, _Traits, _Alloc>::compare(
    size_type __pos, size_type __n1, const _CharT *__s) const
{
  _M_check(__pos, "basic_string::compare");
  __n1 = _M_limit(__pos, __n1);
  const size_type __osize = traits_type::length(__s);
  const size_type __len = aicuda::stl::min(__n1, __osize);
  int __r = traits_type::compare(_M_data() + __pos, __s, __len);
  if (!__r)
    __r = _S_compare(__n1, __osize);
  return __r;
}

template <typename _CharT, typename _Traits, typename _Alloc>
__device__ int basic_string<_CharT, _Traits, _Alloc>::compare(
    size_type __pos, size_type __n1, const _CharT *__s, size_type __n2) const
{
  _M_check(__pos, "basic_string::compare");
  __n1 = _M_limit(__pos, __n1);
  const size_type __len = aicuda::stl::min(__n1, __n2);
  int __r = traits_type::compare(_M_data() + __pos, __s, __len);
  if (!__r)
    __r = _S_compare(__n1, __n2);
  return __r;
}

} // namespace stl
} // namespace aicuda

#endif /* _AICUDA_STL_STRING_H_ */
