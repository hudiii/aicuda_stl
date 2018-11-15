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

#ifndef _AICUDA_STL_TYPE_TRAITS_H_
#define _AICUDA_STL_TYPE_TRAITS_H_ 1

#include <assert.h>

namespace aicuda
{
namespace stl
{

struct __true_type
{
};
struct __false_type
{
};

template <bool>
struct __truth_type
{
  typedef __false_type __type;
};

template <>
struct __truth_type<true>
{
  typedef __true_type __type;
};

template <class _Sp, class _Tp>
struct __traitor
{
  enum
  {
    __value = bool(_Sp::__value) || bool(_Tp::__value)
  };
  typedef typename __truth_type<__value>::__type __type;
};

template <typename, typename>
struct __are_same
{
  enum
  {
    __value = 0
  };
  typedef __false_type __type;
};

template <typename _Tp>
struct __are_same<_Tp, _Tp>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <typename _Tp>
struct __is_void
{
  enum
  {
    __value = 0
  };
  typedef __false_type __type;
};

template <>
struct __is_void<void>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <typename _Tp>
struct __is_boolean
{
  enum
  {
    __value = 0
  };
  typedef __false_type __type;
};

template <>
struct __is_boolean<bool>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <typename _Tp>
struct __is_integer
{
  enum
  {
    __value = 0
  };
  typedef __false_type __type;
};

template <>
struct __is_integer<bool>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <>
struct __is_integer<char>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <>
struct __is_integer<signed char>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <>
struct __is_integer<unsigned char>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <>
struct __is_integer<wchar_t>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <>
struct __is_integer<short>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <>
struct __is_integer<unsigned short>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <>
struct __is_integer<int>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <>
struct __is_integer<unsigned int>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <>
struct __is_integer<long>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <>
struct __is_integer<unsigned long>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <>
struct __is_integer<long long>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <>
struct __is_integer<unsigned long long>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <typename _Tp>
struct __is_floating
{
  enum
  {
    __value = 0
  };
  typedef __false_type __type;
};

template <>
struct __is_floating<float>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <>
struct __is_floating<double>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <>
struct __is_floating<long double>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <typename _Tp>
struct __is_pointer
{
  enum
  {
    __value = 0
  };
  typedef __false_type __type;
};

template <typename _Tp>
struct __is_pointer<_Tp *>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <typename _Tp>
struct __is_char_pointer
{
  enum
  {
    __value = 0
  };
  typedef __false_type __type;
};

template <>
struct __is_char_pointer<char*>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <>
struct __is_char_pointer<const char*>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <typename _Tp>
struct __is_normal_iterator
{
  enum
  {
    __value = 0
  };
  typedef __false_type __type;
};

template <typename _Iterator, typename _Container>
class __normal_iterator;

template <typename _Iterator, typename _Container>
struct __is_normal_iterator<aicuda::stl::__normal_iterator<_Iterator,
                                                           _Container>>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <typename _Tp>
struct __is_arithmetic
    : public __traitor<__is_integer<_Tp>, __is_floating<_Tp>>{};

template <typename _Tp>
struct __is_fundamental
    : public __traitor<__is_void<_Tp>, __is_arithmetic<_Tp>>{};

template <typename _Tp>
struct __is_scalar
    : public __traitor<__is_arithmetic<_Tp>, __is_pointer<_Tp>>{};

template <typename _Tp>
struct __is_char
{
  enum
  {
    __value = 0
  };
  typedef __false_type __type;
};

template <>
struct __is_char<char>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <>
struct __is_char<wchar_t>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <typename _Tp>
struct __is_byte
{
  enum
  {
    __value = 0
  };
  typedef __false_type __type;
};

template <>
struct __is_byte<char>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <>
struct __is_byte<signed char>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <>
struct __is_byte<unsigned char>
{
  enum
  {
    __value = 1
  };
  typedef __true_type __type;
};

template <typename _Tp>
struct __is_move_iterator
{
  enum
  {
    __value = 0
  };
  typedef __false_type __type;
};

template <bool, typename>
struct __enable_if
{
};

template <typename _Tp>
struct __enable_if<true, _Tp>
{
  typedef _Tp __type;
};

template <bool _Cond, typename _Iftrue, typename _Iffalse>
struct __conditional_type
{
  typedef _Iftrue __type;
};

template <typename _Iftrue, typename _Iffalse>
struct __conditional_type<false, _Iftrue, _Iffalse>
{
  typedef _Iffalse __type;
};

template <typename _Tp>
struct __add_unsigned
{
private:
  typedef __enable_if<aicuda::stl::__is_integer<_Tp>::__value, _Tp> __if_type;

public:
  typedef typename __if_type::__type __type;
};

template <>
struct __add_unsigned<char>
{
  typedef unsigned char __type;
};

template <>
struct __add_unsigned<signed char>
{
  typedef unsigned char __type;
};

template <>
struct __add_unsigned<short>
{
  typedef unsigned short __type;
};

template <>
struct __add_unsigned<int>
{
  typedef unsigned int __type;
};

template <>
struct __add_unsigned<long>
{
  typedef unsigned long __type;
};

template <>
struct __add_unsigned<long long>
{
  typedef unsigned long long __type;
};

template <>
struct __add_unsigned<bool>;

template <>
struct __add_unsigned<wchar_t>;

template <typename _Tp>
struct __remove_unsigned
{
private:
  typedef __enable_if<aicuda::stl::__is_integer<_Tp>::__value, _Tp> __if_type;

public:
  typedef typename __if_type::__type __type;
};

template <>
struct __remove_unsigned<char>
{
  typedef signed char __type;
};

template <>
struct __remove_unsigned<unsigned char>
{
  typedef signed char __type;
};

template <>
struct __remove_unsigned<unsigned short>
{
  typedef short __type;
};

template <>
struct __remove_unsigned<unsigned int>
{
  typedef int __type;
};

template <>
struct __remove_unsigned<unsigned long>
{
  typedef long __type;
};

template <>
struct __remove_unsigned<unsigned long long>
{
  typedef long long __type;
};

template <>
struct __remove_unsigned<bool>;

template <>
struct __remove_unsigned<wchar_t>;

template <typename _Type>
__device__ inline bool
__is_null_pointer(_Type *__ptr)
{
  return __ptr == 0;
}

template <typename _Type>
__device__ inline bool
    __is_null_pointer(_Type)
{
  return false;
}

template <typename _Tp, bool = __is_integer<_Tp>::__value>
struct __promote
{
  typedef double __type;
};

template <typename _Tp>
struct __promote<_Tp, false>
{
  typedef _Tp __type;
};

template <typename _Tp, typename _Up>
struct __promote_2
{
private:
  typedef typename __promote<_Tp>::__type __type1;
  typedef typename __promote<_Up>::__type __type2;

public:
  typedef __typeof__(__type1() + __type2()) __type;
};

template <typename _Tp, typename _Up, typename _Vp>
struct __promote_3
{
private:
  typedef typename __promote<_Tp>::__type __type1;
  typedef typename __promote<_Up>::__type __type2;
  typedef typename __promote<_Vp>::__type __type3;

public:
  typedef __typeof__(__type1() + __type2() + __type3()) __type;
};

template <typename _Tp, typename _Up, typename _Vp, typename _Wp>
struct __promote_4
{
private:
  typedef typename __promote<_Tp>::__type __type1;
  typedef typename __promote<_Up>::__type __type2;
  typedef typename __promote<_Vp>::__type __type3;
  typedef typename __promote<_Wp>::__type __type4;

public:
  typedef __typeof__(__type1() + __type2() + __type3() + __type4()) __type;
};

} // namespace stl
} // namespace aicuda

#endif /* _AICUDA_STL_TYPE_TRAITS_H_ */
