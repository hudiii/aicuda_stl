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

#ifndef _AICUDA_STL_CONSTRUCT_H_
#define _AICUDA_STL_CONSTRUCT_H_ 1

#include <aicuda_stl_type_traits.h>

namespace aicuda
{
namespace stl
{

template <typename _T1, typename _T2>
__device__ inline void
_Construct(_T1 *__p, const _T2 &__value)
{
  ::new (static_cast<void *>(__p)) _T1(__value);
}

template <typename _Tp>
__device__ inline void
_Destroy(_Tp *__pointer)
{
  __pointer->~_Tp();
}

template <bool>
struct _Destroy_aux
{
  template <typename _ForwardIterator>
  __device__ static void
  __destroy(_ForwardIterator __first, _ForwardIterator __last)
  {
    for (; __first != __last; ++__first)
      aicuda::stl::_Destroy(&*__first);
  }
};

template <>
struct _Destroy_aux<true>
{
  template <typename _ForwardIterator>
  __device__ static void
      __destroy(_ForwardIterator, _ForwardIterator) {}
};

template <typename _Iterator>
struct iterator_traits;

template <typename _ForwardIterator>
__device__ inline void
_Destroy(_ForwardIterator __first, _ForwardIterator __last)
{
  typedef typename iterator_traits<_ForwardIterator>::value_type
      _Value_type;
  aicuda::stl::_Destroy_aux<__has_trivial_destructor(_Value_type)>::
      __destroy(__first, __last);
}

template <typename _Tp>
class allocator;

template <typename _ForwardIterator, typename _Allocator>
__device__ void _Destroy(_ForwardIterator __first, _ForwardIterator __last,
                         _Allocator &__alloc)
{
  for (; __first != __last; ++__first)
    __alloc.destroy(&*__first);
}

template <typename _ForwardIterator, typename _Tp>
__device__ inline void
_Destroy(_ForwardIterator __first, _ForwardIterator __last,
         aicuda::stl::allocator<_Tp> &)
{
  _Destroy(__first, __last);
}

} // namespace stl
} // namespace aicuda

#endif /* _AICUDA_STL_CONSTRUCT_H_ */
