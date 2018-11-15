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

#ifndef _AICUDA_STL_UNINITIALIZED_H_
#define _AICUDA_STL_UNINITIALIZED_H_ 1

namespace aicuda {
namespace stl {

template <bool>
struct __uninitialized_copy {
  template <typename _InputIterator, typename _ForwardIterator>
  __device__ static _ForwardIterator uninitialized_copy(
      _InputIterator __first, _InputIterator __last,
      _ForwardIterator __result) {
    _ForwardIterator __cur = __result;

    for (; __first != __last; ++__first, ++__cur)
      ::new (static_cast<void *>(&*__cur))
          typename iterator_traits<_ForwardIterator>::value_type(*__first);
    return __cur;
  }
};

template <>
struct __uninitialized_copy<true> {
  template <typename _InputIterator, typename _ForwardIterator>
  __device__ static _ForwardIterator uninitialized_copy(
      _InputIterator __first, _InputIterator __last,
      _ForwardIterator __result) {
    return aicuda::stl::copy(__first, __last, __result);
  }
};

template <typename _InputIterator, typename _ForwardIterator>
__device__ inline _ForwardIterator uninitialized_copy(
    _InputIterator __first, _InputIterator __last, _ForwardIterator __result) {
  typedef typename iterator_traits<_InputIterator>::value_type _ValueType1;
  typedef typename iterator_traits<_ForwardIterator>::value_type _ValueType2;

  return aicuda::stl::__uninitialized_copy<(
      __is_pod(_ValueType1) &&
      __is_pod(_ValueType2))>::uninitialized_copy(__first, __last, __result);
}

template <bool>
struct __uninitialized_fill {
  template <typename _ForwardIterator, typename _Tp>
  __device__ static void uninitialized_fill(_ForwardIterator __first,
                                            _ForwardIterator __last,
                                            const _Tp &__x) {
    _ForwardIterator __cur = __first;

    for (; __cur != __last; ++__cur) aicuda::stl::_Construct(&*__cur, __x);
  }
};

template <>
struct __uninitialized_fill<true> {
  template <typename _ForwardIterator, typename _Tp>
  __device__ static void uninitialized_fill(_ForwardIterator __first,
                                            _ForwardIterator __last,
                                            const _Tp &__x) {
    aicuda::stl::fill(__first, __last, __x);
  }
};

template <typename _ForwardIterator, typename _Tp>
__device__ inline void uninitialized_fill(_ForwardIterator __first,
                                          _ForwardIterator __last,
                                          const _Tp &__x) {
  typedef typename iterator_traits<_ForwardIterator>::value_type _ValueType;

  aicuda::stl::__uninitialized_fill<__is_pod(_ValueType)>::uninitialized_fill(
      __first, __last, __x);
}

template <bool>
struct __uninitialized_fill_n {
  template <typename _ForwardIterator, typename _Size, typename _Tp>
  __device__ static void uninitialized_fill_n(_ForwardIterator __first,
                                              _Size __n, const _Tp &__x) {
    _ForwardIterator __cur = __first;

    for (; __n > 0; --__n, ++__cur) aicuda::stl::_Construct(&*__cur, __x);
  }
};

template <>
struct __uninitialized_fill_n<true> {
  template <typename _ForwardIterator, typename _Size, typename _Tp>
  __device__ static void uninitialized_fill_n(_ForwardIterator __first,
                                              _Size __n, const _Tp &__x) {
    aicuda::stl::fill_n(__first, __n, __x);
  }
};

template <typename _ForwardIterator, typename _Size, typename _Tp>
__device__ inline void uninitialized_fill_n(_ForwardIterator __first, _Size __n,
                                            const _Tp &__x) {
  typedef typename iterator_traits<_ForwardIterator>::value_type _ValueType;

  aicuda::stl::__uninitialized_fill_n<__is_pod(
      _ValueType)>::uninitialized_fill_n(__first, __n, __x);
}

template <typename _InputIterator, typename _ForwardIterator,
          typename _Allocator>
__device__ _ForwardIterator __uninitialized_copy_a(_InputIterator __first,
                                                   _InputIterator __last,
                                                   _ForwardIterator __result,
                                                   _Allocator &__alloc) {
  _ForwardIterator __cur = __result;

  for (; __first != __last; ++__first, ++__cur)
    __alloc.construct(&*__cur, *__first);
  return __cur;
}

template <typename _InputIterator, typename _ForwardIterator, typename _Tp>
__device__ inline _ForwardIterator __uninitialized_copy_a(
    _InputIterator __first, _InputIterator __last, _ForwardIterator __result,
    allocator<_Tp> &) {
  return aicuda::stl::uninitialized_copy(__first, __last, __result);
}

template <typename _InputIterator, typename _ForwardIterator,
          typename _Allocator>
__device__ inline _ForwardIterator __uninitialized_move_a(
    _InputIterator __first, _InputIterator __last, _ForwardIterator __result,
    _Allocator &__alloc) {
  return aicuda::stl::__uninitialized_copy_a((__first), (__last), __result,
                                             __alloc);
}

template <typename _ForwardIterator, typename _Tp, typename _Allocator>
__device__ void __uninitialized_fill_a(_ForwardIterator __first,
                                       _ForwardIterator __last, const _Tp &__x,
                                       _Allocator &__alloc) {
  _ForwardIterator __cur = __first;

  for (; __cur != __last; ++__cur) __alloc.construct(&*__cur, __x);
}

template <typename _ForwardIterator, typename _Tp, typename _Tp2>
__device__ inline void __uninitialized_fill_a(_ForwardIterator __first,
                                              _ForwardIterator __last,
                                              const _Tp &__x,
                                              allocator<_Tp2> &) {
  aicuda::stl::uninitialized_fill(__first, __last, __x);
}

template <typename _ForwardIterator, typename _Size, typename _Tp,
          typename _Allocator>
__device__ void __uninitialized_fill_n_a(_ForwardIterator __first, _Size __n,
                                         const _Tp &__x, _Allocator &__alloc) {
  _ForwardIterator __cur = __first;

  for (; __n > 0; --__n, ++__cur) __alloc.construct(&*__cur, __x);
}

template <typename _ForwardIterator, typename _Size, typename _Tp,
          typename _Tp2>
__device__ inline void __uninitialized_fill_n_a(_ForwardIterator __first,
                                                _Size __n, const _Tp &__x,
                                                allocator<_Tp2> &) {
  aicuda::stl::uninitialized_fill_n(__first, __n, __x);
}

template <typename _InputIterator1, typename _InputIterator2,
          typename _ForwardIterator, typename _Allocator>
__device__ inline _ForwardIterator __uninitialized_copy_move(
    _InputIterator1 __first1, _InputIterator1 __last1, _InputIterator2 __first2,
    _InputIterator2 __last2, _ForwardIterator __result, _Allocator &__alloc) {
  _ForwardIterator __mid =
      aicuda::stl::__uninitialized_copy_a(__first1, __last1, __result, __alloc);

  return aicuda::stl::__uninitialized_move_a(__first2, __last2, __mid, __alloc);
}

template <typename _InputIterator1, typename _InputIterator2,
          typename _ForwardIterator, typename _Allocator>
__device__ inline _ForwardIterator __uninitialized_move_copy(
    _InputIterator1 __first1, _InputIterator1 __last1, _InputIterator2 __first2,
    _InputIterator2 __last2, _ForwardIterator __result, _Allocator &__alloc) {
  _ForwardIterator __mid =
      aicuda::stl::__uninitialized_move_a(__first1, __last1, __result, __alloc);

  return aicuda::stl::__uninitialized_copy_a(__first2, __last2, __mid, __alloc);
}

template <typename _ForwardIterator, typename _Tp, typename _InputIterator,
          typename _Allocator>
__device__ inline _ForwardIterator __uninitialized_fill_move(
    _ForwardIterator __result, _ForwardIterator __mid, const _Tp &__x,
    _InputIterator __first, _InputIterator __last, _Allocator &__alloc) {
  aicuda::stl::__uninitialized_fill_a(__result, __mid, __x, __alloc);

  return aicuda::stl::__uninitialized_move_a(__first, __last, __mid, __alloc);
}

template <typename _InputIterator, typename _ForwardIterator, typename _Tp,
          typename _Allocator>
__device__ inline void __uninitialized_move_fill(
    _InputIterator __first1, _InputIterator __last1, _ForwardIterator __first2,
    _ForwardIterator __last2, const _Tp &__x, _Allocator &__alloc) {
  _ForwardIterator __mid2 =
      aicuda::stl::__uninitialized_move_a(__first1, __last1, __first2, __alloc);

  aicuda::stl::__uninitialized_fill_a(__mid2, __last2, __x, __alloc);
}

}  // namespace stl
}  // namespace aicuda

#endif /* _AICUDA_STL_UNINITIALIZED_H_ */
