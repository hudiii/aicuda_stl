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

#ifndef _AICUDA_ALLOCATOR_H_
#define _AICUDA_ALLOCATOR_H_ 1

#include <stddef.h>

namespace aicuda {
namespace stl {

template <typename _Tp>
class allocator {
 public:
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef _Tp *pointer;
  typedef const _Tp *const_pointer;
  typedef _Tp &reference;
  typedef const _Tp &const_reference;
  typedef _Tp value_type;

  template <typename _Tp1>
  struct rebind {
    typedef allocator<_Tp1> other;
  };

  __device__ allocator() {}

  __device__ allocator(const allocator &) {}

  template <typename _Tp1>
  __device__ allocator(const allocator<_Tp1> &) {}

  __device__ ~allocator() {}

  __device__ pointer address(reference __x) const { return &__x; }

  __device__ const_pointer address(const_reference __x) const { return &__x; }

  __device__ pointer allocate(size_type __n, const void * = 0) {
    return static_cast<_Tp *>(::operator new(__n * sizeof(_Tp)));
  }

  __device__ void deallocate(pointer __p, size_type) { ::operator delete(__p); }

  __device__ void construct(pointer __p, const _Tp &__val) {
    ::new ((void *)__p) _Tp(__val);
  }

  __device__ constexpr size_type max_size() const {
    return size_t(-1) / sizeof(_Tp);
  }

  __device__ void destroy(pointer __p) { __p->~_Tp(); }
};

template <typename _T1, typename _T2>
__device__ inline bool operator==(const allocator<_T1> &,
                                  const allocator<_T2> &) {
  return true;
}

template <typename _Tp>
__device__ inline bool operator==(const allocator<_Tp> &,
                                  const allocator<_Tp> &) {
  return true;
}

template <typename _T1, typename _T2>
__device__ inline bool operator!=(const allocator<_T1> &,
                                  const allocator<_T2> &) {
  return false;
}

template <typename _Tp>
__device__ inline bool operator!=(const allocator<_Tp> &,
                                  const allocator<_Tp> &) {
  return false;
}

//extern template class allocator<char>;
//extern template class allocator<wchar_t>;

template <typename _Alloc, bool = __is_empty(_Alloc)>
struct __alloc_swap {
  __device__ static void _S_do_it(_Alloc &, _Alloc &) {}
};

template <typename _Alloc>
struct __alloc_swap<_Alloc, false> {
  __device__ static void _S_do_it(_Alloc &__one, _Alloc &__two) {
    if (__one != __two) swap(__one, __two);
  }
};

template <typename _Alloc, bool = __is_empty(_Alloc)>
struct __alloc_neq {
  __device__ static bool _S_do_it(const _Alloc &, const _Alloc &) {
    return false;
  }
};

template <typename _Alloc>
struct __alloc_neq<_Alloc, false> {
  __device__ static bool _S_do_it(const _Alloc &__one, const _Alloc &__two) {
    return __one != __two;
  }
};

}  // namespace stl
}  // namespace aicuda

#endif /* _AICUDA_ALLOCATOR_H_ */
