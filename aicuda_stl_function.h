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

#ifndef _AICUDA_STL_FUNCTION_H_
#define _AICUDA_STL_FUNCTION_H_ 1

#include <aicuda_stl_memory.h>
#include <aicuda_stl_type_traits.h>

namespace aicuda {
namespace stl {

template <typename _Arg, typename _Result>
struct unary_function {
  typedef _Arg argument_type;
  typedef _Result result_type;
};

template <typename _Arg1, typename _Arg2, typename _Result>
struct binary_function {
  typedef _Arg1 first_argument_type;
  typedef _Arg2 second_argument_type;
  typedef _Result result_type;
};

template <typename _Tp>
struct greater : public binary_function<_Tp, _Tp, bool> {
  __device__ bool operator()(const _Tp &__x, const _Tp &__y) const {
    return __x > __y;
  }
};

template <typename _Tp>
struct less : public binary_function<_Tp, _Tp, bool> {
  __device__ bool operator()(const _Tp &__x, const _Tp &__y) const {
    return __x < __y;
  }
};

template <typename _Tp>
__device__ inline void swap(_Tp &__a, _Tp &__b) {
  _Tp __tmp = (__a);
  __a = (__b);
  __b = (__tmp);
}

template <typename _Tp, size_t _Nm>
__device__ inline void swap(_Tp (&__a)[_Nm], _Tp (&__b)[_Nm]) {
  for (size_t __n = 0; __n < _Nm; ++__n) swap(__a[__n], __b[__n]);
}

template <typename _Iterator>
struct iterator_traits;

template <bool _BoolType>
struct __iter_swap {
  template <typename _ForwardIterator1, typename _ForwardIterator2>
  __device__ static void iter_swap(_ForwardIterator1 __a,
                                   _ForwardIterator2 __b) {
    typedef typename iterator_traits<_ForwardIterator1>::value_type _ValueType1;
    _ValueType1 __tmp = (*__a);
    *__a = (*__b);
    *__b = (__tmp);
  }
};

template <>
struct __iter_swap<true> {
  template <typename _ForwardIterator1, typename _ForwardIterator2>
  __device__ static void iter_swap(_ForwardIterator1 __a,
                                   _ForwardIterator2 __b) {
    swap(*__a, *__b);
  }
};

template <typename _ForwardIterator1, typename _ForwardIterator2>
__device__ inline void iter_swap(_ForwardIterator1 __a, _ForwardIterator2 __b) {
  typedef typename iterator_traits<_ForwardIterator1>::value_type _ValueType1;
  typedef typename iterator_traits<_ForwardIterator2>::value_type _ValueType2;

  typedef
      typename iterator_traits<_ForwardIterator1>::reference _ReferenceType1;
  typedef
      typename iterator_traits<_ForwardIterator2>::reference _ReferenceType2;
  aicuda::stl::__iter_swap<
      aicuda::stl::__are_same<_ValueType1, _ValueType2>::__value &&
      aicuda::stl::__are_same<_ValueType1 &, _ReferenceType1>::__value &&
      aicuda::stl::__are_same<_ValueType2 &,
                              _ReferenceType2>::__value>::iter_swap(__a, __b);
}

template <typename _ForwardIterator1, typename _ForwardIterator2>
__device__ _ForwardIterator2 swap_ranges(_ForwardIterator1 __first1,
                                         _ForwardIterator1 __last1,
                                         _ForwardIterator2 __first2) {
  for (; __first1 != __last1; ++__first1, ++__first2)
    aicuda::stl::iter_swap(__first1, __first2);
  return __first2;
}

template <typename _Tp>
__device__ inline const _Tp &min(const _Tp &__a, const _Tp &__b) {
  if (__b < __a) return __b;
  return __a;
}

template <typename _Tp>
__device__ inline const _Tp &max(const _Tp &__a, const _Tp &__b) {
  if (__a < __b) return __b;
  return __a;
}

template <typename _Tp, typename _Compare>
__device__ inline const _Tp &min(const _Tp &__a, const _Tp &__b,
                                 _Compare __comp) {
  if (__comp(__b, __a)) return __b;
  return __a;
}

template <typename _Tp, typename _Compare>
__device__ inline const _Tp &max(const _Tp &__a, const _Tp &__b,
                                 _Compare __comp) {
  if (__comp(__a, __b)) return __b;
  return __a;
}

template <typename _Iterator,
          bool _IsNormal = __is_normal_iterator<_Iterator>::__value>
struct __niter_base {
  __device__ static _Iterator __b(_Iterator __it) { return __it; }
};

template <typename _Iterator>
struct __niter_base<_Iterator, true> {
  __device__ static typename _Iterator::iterator_type __b(_Iterator __it) {
    return __it.base();
  }
};

template <typename _Iterator,
          bool _IsMove = __is_move_iterator<_Iterator>::__value>
struct __miter_base {
  __device__ static _Iterator __b(_Iterator __it) { return __it; }
};

template <typename _Iterator>
struct __miter_base<_Iterator, true> {
  __device__ static typename _Iterator::iterator_type __b(_Iterator __it) {
    return __it.base();
  }
};

template <bool, bool, typename>
struct __copy_move {
  template <typename _II, typename _OI>
  __device__ static _OI __copy_m(_II __first, _II __last, _OI __result) {
    for (; __first != __last; ++__result, ++__first) *__result = *__first;
    return __result;
  }
};

struct random_access_iterator_tag;

template <>
struct __copy_move<false, false, random_access_iterator_tag> {
  template <typename _II, typename _OI>
  __device__ static _OI __copy_m(_II __first, _II __last, _OI __result) {
    typedef typename iterator_traits<_II>::difference_type _Distance;
    for (_Distance __n = __last - __first; __n > 0; --__n) {
      *__result = *__first;
      ++__first;
      ++__result;
    }
    return __result;
  }
};

template <bool _IsMove>
struct __copy_move<_IsMove, true, random_access_iterator_tag> {
  template <typename _Tp>
  __device__ static _Tp *__copy_m(const _Tp *__first, const _Tp *__last,
                                  _Tp *__result) {
    string_op::memmove(__result, __first, sizeof(_Tp) * (__last - __first));
    return __result + (__last - __first);
  }
};

template <bool _IsMove, typename _II, typename _OI>
__device__ inline _OI __copy_move_a(_II __first, _II __last, _OI __result) {
  typedef typename iterator_traits<_II>::value_type _ValueTypeI;
  typedef typename iterator_traits<_OI>::value_type _ValueTypeO;
  typedef typename iterator_traits<_II>::iterator_category _Category;
  const bool __simple = (__is_pod(_ValueTypeI) && __is_pointer<_II>::__value &&
                         __is_pointer<_OI>::__value &&
                         __are_same<_ValueTypeI, _ValueTypeO>::__value);

  return aicuda::stl::__copy_move<_IsMove, __simple, _Category>::__copy_m(
      __first, __last, __result);
}

template <typename _CharT>
struct char_traits;

template <bool _IsMove, typename _II, typename _OI>
__device__ inline _OI __copy_move_a2(_II __first, _II __last, _OI __result) {
  return _OI(aicuda::stl::__copy_move_a<_IsMove>(
      aicuda::stl::__niter_base<_II>::__b(__first),
      aicuda::stl::__niter_base<_II>::__b(__last),
      aicuda::stl::__niter_base<_OI>::__b(__result)));
}

template <typename _II, typename _OI>
__device__ inline _OI copy(_II __first, _II __last, _OI __result) {
  return (aicuda::stl::__copy_move_a2<__is_move_iterator<_II>::__value>(
      aicuda::stl::__miter_base<_II>::__b(__first),
      aicuda::stl::__miter_base<_II>::__b(__last), __result));
}

template <bool, bool, typename>
struct __copy_move_backward {
  template <typename _BI1, typename _BI2>
  __device__ static _BI2 __copy_move_b(_BI1 __first, _BI1 __last,
                                       _BI2 __result) {
    while (__first != __last) *--__result = *--__last;
    return __result;
  }
};

template <>
struct __copy_move_backward<false, false, random_access_iterator_tag> {
  template <typename _BI1, typename _BI2>
  __device__ static _BI2 __copy_move_b(_BI1 __first, _BI1 __last,
                                       _BI2 __result) {
    typename iterator_traits<_BI1>::difference_type __n;
    for (__n = __last - __first; __n > 0; --__n) *--__result = *--__last;
    return __result;
  }
};

template <bool _IsMove>
struct __copy_move_backward<_IsMove, true, random_access_iterator_tag> {
  template <typename _Tp>
  __device__ static _Tp *__copy_move_b(const _Tp *__first, const _Tp *__last,
                                       _Tp *__result) {
    const ptrdiff_t _Num = __last - __first;
    string_op::memmove(__result - _Num, __first, sizeof(_Tp) * _Num);
    return __result - _Num;
  }
};

template <bool _IsMove, typename _BI1, typename _BI2>
__device__ inline _BI2 __copy_move_backward_a(_BI1 __first, _BI1 __last,
                                              _BI2 __result) {
  typedef typename iterator_traits<_BI1>::value_type _ValueType1;
  typedef typename iterator_traits<_BI2>::value_type _ValueType2;
  typedef typename iterator_traits<_BI1>::iterator_category _Category;
  const bool __simple = (__is_pod(_ValueType1) && __is_pointer<_BI1>::__value &&
                         __is_pointer<_BI2>::__value &&
                         __are_same<_ValueType1, _ValueType2>::__value);

  return aicuda::stl::__copy_move_backward<_IsMove, __simple,
                                           _Category>::__copy_move_b(__first,
                                                                     __last,
                                                                     __result);
}

template <bool _IsMove, typename _BI1, typename _BI2>
__device__ inline _BI2 __copy_move_backward_a2(_BI1 __first, _BI1 __last,
                                               _BI2 __result) {
  return _BI2(aicuda::stl::__copy_move_backward_a<_IsMove>(
      aicuda::stl::__niter_base<_BI1>::__b(__first),
      aicuda::stl::__niter_base<_BI1>::__b(__last),
      aicuda::stl::__niter_base<_BI2>::__b(__result)));
}

template <typename _BI1, typename _BI2>
__device__ inline _BI2 copy_backward(_BI1 __first, _BI1 __last, _BI2 __result) {
  return (
      aicuda::stl::__copy_move_backward_a2<__is_move_iterator<_BI1>::__value>(
          aicuda::stl::__miter_base<_BI1>::__b(__first),
          aicuda::stl::__miter_base<_BI1>::__b(__last), __result));
}

template <typename _ForwardIterator, typename _Tp>
__device__ inline
    typename aicuda::stl::__enable_if<!__is_scalar<_Tp>::__value, void>::__type
    __fill_a(_ForwardIterator __first, _ForwardIterator __last,
             const _Tp &__value) {
  for (; __first != __last; ++__first) *__first = __value;
}

template <typename _ForwardIterator, typename _Tp>
__device__ inline
    typename aicuda::stl::__enable_if<__is_scalar<_Tp>::__value, void>::__type
    __fill_a(_ForwardIterator __first, _ForwardIterator __last,
             const _Tp &__value) {
  const _Tp __tmp = __value;
  for (; __first != __last; ++__first) *__first = __tmp;
}

template <typename _Tp>
__device__ inline
    typename aicuda::stl::__enable_if<__is_byte<_Tp>::__value, void>::__type
    __fill_a(_Tp *__first, _Tp *__last, const _Tp &__c) {
  const _Tp __tmp = __c;
  string_op::memset(__first, static_cast<unsigned char>(__tmp),
                    __last - __first);
}

template <typename _ForwardIterator, typename _Tp>
__device__ inline void fill(_ForwardIterator __first, _ForwardIterator __last,
                            const _Tp &__value) {
  aicuda::stl::__fill_a(
      aicuda::stl::__niter_base<_ForwardIterator>::__b(__first),
      aicuda::stl::__niter_base<_ForwardIterator>::__b(__last), __value);
}

template <typename _OutputIterator, typename _Size, typename _Tp>
__device__ inline typename aicuda::stl::__enable_if<!__is_scalar<_Tp>::__value,
                                                    _OutputIterator>::__type
__fill_n_a(_OutputIterator __first, _Size __n, const _Tp &__value) {
  for (; __n > 0; --__n, ++__first) *__first = __value;
  return __first;
}

template <typename _OutputIterator, typename _Size, typename _Tp>
__device__ inline typename aicuda::stl::__enable_if<__is_scalar<_Tp>::__value,
                                                    _OutputIterator>::__type
__fill_n_a(_OutputIterator __first, _Size __n, const _Tp &__value) {
  const _Tp __tmp = __value;
  for (; __n > 0; --__n, ++__first) *__first = __tmp;
  return __first;
}

template <typename _Size, typename _Tp>
__device__ inline
    typename aicuda::stl::__enable_if<__is_byte<_Tp>::__value, _Tp *>::__type
    __fill_n_a(_Tp *__first, _Size __n, const _Tp &__c) {
  aicuda::stl::__fill_a(__first, __first + __n, __c);
  return __first + __n;
}

template <typename _OI, typename _Size, typename _Tp>
__device__ inline _OI fill_n(_OI __first, _Size __n, const _Tp &__value) {
  return _OI(aicuda::stl::__fill_n_a(
      aicuda::stl::__niter_base<_OI>::__b(__first), __n, __value));
}

template <typename _Tp>
struct _Identity : public unary_function<_Tp, _Tp> {
  __device__ _Tp &operator()(_Tp &__x) const { return __x; }

  __device__ const _Tp &operator()(const _Tp &__x) const { return __x; }
};

template <typename _Pair>
struct _Select1st : public unary_function<_Pair, typename _Pair::first_type> {
  __device__ typename _Pair::first_type &operator()(_Pair &__x) const {
    return __x.first;
  }

  __device__ const typename _Pair::first_type &operator()(
      const _Pair &__x) const {
    return __x.first;
  }
};

template <typename _Pair>
struct _Select2nd : public unary_function<_Pair, typename _Pair::second_type> {
  __device__ typename _Pair::second_type &operator()(_Pair &__x) const {
    return __x.second;
  }

  __device__ const typename _Pair::second_type &operator()(
      const _Pair &__x) const {
    return __x.second;
  }
};

}  // namespace stl
}  // namespace aicuda

#endif /* _AICUDA_STL_FUNCTION_H_ */
