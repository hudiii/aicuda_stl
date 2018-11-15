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

#ifndef _AICUDA_STL_PAIR_H_
#define _AICUDA_STL_PAIR_H_ 1

namespace aicuda
{
namespace stl
{

template <class _T1, class _T2>
struct pair
{
  typedef _T1 first_type;
  typedef _T2 second_type;

  _T1 first;
  _T2 second;

  __device__ pair() : first(), second() {}

  __device__ pair(const _T1 &__a, const _T2 &__b) : first(__a), second(__b) {}

  template <class _U1, class _U2>
  __device__ pair(const pair<_U1, _U2> &__p)
      : first(__p.first), second(__p.second) {}
};

template <class _T1, class _T2>
__device__ inline bool operator==(const pair<_T1, _T2> &__x,
                                  const pair<_T1, _T2> &__y)
{
  return __x.first == __y.first && __x.second == __y.second;
}

template <class _T1, class _T2>
__device__ inline bool
operator<(const pair<_T1, _T2> &__x, const pair<_T1, _T2> &__y)
{
  return __x.first < __y.first || (!(__y.first < __x.first) && __x.second < __y.second);
}

template <class _T1, class _T2>
__device__ inline bool
operator!=(const pair<_T1, _T2> &__x, const pair<_T1, _T2> &__y)
{
  return !(__x == __y);
}

template <class _T1, class _T2>
__device__ inline bool
operator>(const pair<_T1, _T2> &__x, const pair<_T1, _T2> &__y)
{
  return __y < __x;
}

template <class _T1, class _T2>
__device__ inline bool
operator<=(const pair<_T1, _T2> &__x, const pair<_T1, _T2> &__y)
{
  return !(__y < __x);
}

template <class _T1, class _T2>
__device__ inline bool
operator>=(const pair<_T1, _T2> &__x, const pair<_T1, _T2> &__y)
{
  return !(__x < __y);
}

template <class _T1, class _T2>
__device__ inline pair<_T1, _T2> make_pair(_T1 __x, _T2 __y)
{
  return pair<_T1, _T2>(__x, __y);
}

} // namespace stl
} // namespace aicuda

#endif /* _AICUDA_STL_PAIR_H_ */
