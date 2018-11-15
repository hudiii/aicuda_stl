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

#ifndef _AICUDA_STL_ITERATOR_H_
#define _AICUDA_STL_ITERATOR_H_ 1

#include <stddef.h>
#include <aicuda_stl_type_traits.h>

namespace aicuda
{
namespace stl
{

struct input_iterator_tag
{
};

struct output_iterator_tag
{
};

struct forward_iterator_tag : public input_iterator_tag
{
};

struct bidirectional_iterator_tag : public forward_iterator_tag
{
};

struct random_access_iterator_tag : public bidirectional_iterator_tag
{
};

template <typename _Category, typename _Tp, typename _Distance = ptrdiff_t,
          typename _Pointer = _Tp *, typename _Reference = _Tp &>
struct iterator
{
  typedef _Category iterator_category;
  typedef _Tp value_type;
  typedef _Distance difference_type;
  typedef _Pointer pointer;
  typedef _Reference reference;
};

template <typename _Iterator>
struct iterator_traits
{
  typedef typename _Iterator::iterator_category iterator_category;
  typedef typename _Iterator::value_type value_type;
  typedef typename _Iterator::difference_type difference_type;
  typedef typename _Iterator::pointer pointer;
  typedef typename _Iterator::reference reference;
};

template <typename _Tp>
struct iterator_traits<_Tp *>
{
  typedef random_access_iterator_tag iterator_category;
  typedef _Tp value_type;
  typedef ptrdiff_t difference_type;
  typedef _Tp *pointer;
  typedef _Tp &reference;
};

template <typename _Tp>
struct iterator_traits<const _Tp *>
{
  typedef random_access_iterator_tag iterator_category;
  typedef _Tp value_type;
  typedef ptrdiff_t difference_type;
  typedef const _Tp *pointer;
  typedef const _Tp &reference;
};

template <typename _Iter>
__device__ inline typename iterator_traits<_Iter>::iterator_category
__iterator_category(const _Iter &)
{
  return typename iterator_traits<_Iter>::iterator_category();
}

template <typename _InputIterator>
__device__ inline typename iterator_traits<_InputIterator>::difference_type
__distance(_InputIterator __first, _InputIterator __last,
           input_iterator_tag)
{
  typename iterator_traits<_InputIterator>::difference_type __n = 0;
  while (__first != __last)
  {
    ++__first;
    ++__n;
  }
  return __n;
}

template <typename _RandomAccessIterator>
__device__ inline typename iterator_traits<_RandomAccessIterator>::difference_type
__distance(_RandomAccessIterator __first, _RandomAccessIterator __last,
           random_access_iterator_tag)
{
  return __last - __first;
}

template <typename _InputIterator>
__device__ inline typename iterator_traits<_InputIterator>::difference_type
distance(_InputIterator __first, _InputIterator __last)
{

  return aicuda::stl::__distance(__first, __last, aicuda::stl::__iterator_category(__first));
}

template <typename _InputIterator, typename _Distance>
__device__ inline void
__advance(_InputIterator &__i, _Distance __n, input_iterator_tag)
{
  while (__n--)
    ++__i;
}

template <typename _BidirectionalIterator, typename _Distance>
__device__ inline void
__advance(_BidirectionalIterator &__i, _Distance __n,
          bidirectional_iterator_tag)
{
  if (__n > 0)
    while (__n--)
      ++__i;
  else
    while (__n++)
      --__i;
}

template <typename _RandomAccessIterator, typename _Distance>
__device__ inline void
__advance(_RandomAccessIterator &__i, _Distance __n,
          random_access_iterator_tag)
{
  __i += __n;
}

template <typename _InputIterator, typename _Distance>
__device__ inline void
advance(_InputIterator &__i, _Distance __n)
{
  typename iterator_traits<_InputIterator>::difference_type __d = __n;
  aicuda::stl::__advance(__i, __d, aicuda::stl::__iterator_category(__i));
}

template <typename _Iterator>
class reverse_iterator
    : public iterator<typename iterator_traits<_Iterator>::iterator_category,
                      typename iterator_traits<_Iterator>::value_type,
                      typename iterator_traits<_Iterator>::difference_type,
                      typename iterator_traits<_Iterator>::pointer,
                      typename iterator_traits<_Iterator>::reference>
{
protected:
  _Iterator current;

public:
  typedef _Iterator iterator_type;
  typedef typename iterator_traits<_Iterator>::difference_type
      difference_type;
  typedef typename iterator_traits<_Iterator>::reference reference;
  typedef typename iterator_traits<_Iterator>::pointer pointer;

public:
  __device__ reverse_iterator() : current() {}

  __device__ explicit reverse_iterator(iterator_type __x) : current(__x) {}

  __device__ reverse_iterator(const reverse_iterator &__x)
      : current(__x.current) {}

  template <typename _Iter>
  __device__ reverse_iterator(const reverse_iterator<_Iter> &__x)
      : current(__x.base()) {}

  __device__ iterator_type
  base() const
  {
    return current;
  }

  __device__ reference
  operator*() const
  {
    _Iterator __tmp = current;
    return *--__tmp;
  }

  __device__ pointer
  operator->() const
  {
    return &(operator*());
  }

  __device__ reverse_iterator &
  operator++()
  {
    --current;
    return *this;
  }

  __device__ reverse_iterator
  operator++(int)
  {
    reverse_iterator __tmp = *this;
    --current;
    return __tmp;
  }

  __device__ reverse_iterator &
  operator--()
  {
    ++current;
    return *this;
  }

  __device__ reverse_iterator
  operator--(int)
  {
    reverse_iterator __tmp = *this;
    ++current;
    return __tmp;
  }

  __device__ reverse_iterator
  operator+(difference_type __n) const
  {
    return reverse_iterator(current - __n);
  }

  __device__ reverse_iterator &
  operator+=(difference_type __n)
  {
    current -= __n;
    return *this;
  }

  __device__ reverse_iterator
  operator-(difference_type __n) const
  {
    return reverse_iterator(current + __n);
  }

  __device__ reverse_iterator &
  operator-=(difference_type __n)
  {
    current += __n;
    return *this;
  }

  __device__ reference
  operator[](difference_type __n) const
  {
    return *(*this + __n);
  }
};

template <typename _Iterator>
__device__ inline bool
operator==(const reverse_iterator<_Iterator> &__x,
           const reverse_iterator<_Iterator> &__y)
{
  return __x.base() == __y.base();
}

template <typename _Iterator>
__device__ inline bool
operator<(const reverse_iterator<_Iterator> &__x,
          const reverse_iterator<_Iterator> &__y)
{
  return __y.base() < __x.base();
}

template <typename _Iterator>
__device__ inline bool
operator!=(const reverse_iterator<_Iterator> &__x,
           const reverse_iterator<_Iterator> &__y)
{
  return !(__x == __y);
}

template <typename _Iterator>
__device__ inline bool
operator>(const reverse_iterator<_Iterator> &__x,
          const reverse_iterator<_Iterator> &__y)
{
  return __y < __x;
}

template <typename _Iterator>
__device__ inline bool
operator<=(const reverse_iterator<_Iterator> &__x,
           const reverse_iterator<_Iterator> &__y)
{
  return !(__y < __x);
}

template <typename _Iterator>
__device__ inline bool
operator>=(const reverse_iterator<_Iterator> &__x,
           const reverse_iterator<_Iterator> &__y)
{
  return !(__x < __y);
}

template <typename _Iterator>
__device__ inline typename reverse_iterator<_Iterator>::difference_type
operator-(const reverse_iterator<_Iterator> &__x,
          const reverse_iterator<_Iterator> &__y)
{
  return __y.base() - __x.base();
}

template <typename _Iterator>
__device__ inline reverse_iterator<_Iterator>
operator+(typename reverse_iterator<_Iterator>::difference_type __n,
          const reverse_iterator<_Iterator> &__x)
{
  return reverse_iterator<_Iterator>(__x.base() - __n);
}

template <typename _IteratorL, typename _IteratorR>
__device__ inline bool
operator==(const reverse_iterator<_IteratorL> &__x,
           const reverse_iterator<_IteratorR> &__y)
{
  return __x.base() == __y.base();
}

template <typename _IteratorL, typename _IteratorR>
__device__ inline bool
operator<(const reverse_iterator<_IteratorL> &__x,
          const reverse_iterator<_IteratorR> &__y)
{
  return __y.base() < __x.base();
}

template <typename _IteratorL, typename _IteratorR>
__device__ inline bool
operator!=(const reverse_iterator<_IteratorL> &__x,
           const reverse_iterator<_IteratorR> &__y)
{
  return !(__x == __y);
}

template <typename _IteratorL, typename _IteratorR>
__device__ inline bool
operator>(const reverse_iterator<_IteratorL> &__x,
          const reverse_iterator<_IteratorR> &__y)
{
  return __y < __x;
}

template <typename _IteratorL, typename _IteratorR>
__device__ inline bool
operator<=(const reverse_iterator<_IteratorL> &__x,
           const reverse_iterator<_IteratorR> &__y)
{
  return !(__y < __x);
}

template <typename _IteratorL, typename _IteratorR>
__device__ inline bool
operator>=(const reverse_iterator<_IteratorL> &__x,
           const reverse_iterator<_IteratorR> &__y)
{
  return !(__x < __y);
}

template <typename _IteratorL, typename _IteratorR>
__device__ inline typename reverse_iterator<_IteratorL>::difference_type
operator-(const reverse_iterator<_IteratorL> &__x,
          const reverse_iterator<_IteratorR> &__y)

{
  return __y.base() - __x.base();
}

template <typename _Container>
class back_insert_iterator
    : public iterator<output_iterator_tag, void, void, void, void>
{
protected:
  _Container *container;

public:
  typedef _Container container_type;

  __device__ explicit back_insert_iterator(_Container &__x) : container(&__x) {}

  __device__ back_insert_iterator &
  operator=(typename _Container::const_reference __value)
  {
    container->push_back(__value);
    return *this;
  }

  __device__ back_insert_iterator &
  operator*()
  {
    return *this;
  }

  __device__ back_insert_iterator &
  operator++()
  {
    return *this;
  }

  __device__ back_insert_iterator
  operator++(int)
  {
    return *this;
  }
};

template <typename _Container>
__device__ inline back_insert_iterator<_Container>
back_inserter(_Container &__x)
{
  return back_insert_iterator<_Container>(__x);
}

template <typename _Container>
class front_insert_iterator
    : public iterator<output_iterator_tag, void, void, void, void>
{
protected:
  _Container *container;

public:
  typedef _Container container_type;

  __device__ explicit front_insert_iterator(_Container &__x) : container(&__x) {}

  __device__ front_insert_iterator &
  operator=(typename _Container::const_reference __value)
  {
    container->push_front(__value);
    return *this;
  }

  __device__ front_insert_iterator &
  operator*()
  {
    return *this;
  }

  __device__ front_insert_iterator &
  operator++()
  {
    return *this;
  }

  __device__ front_insert_iterator
  operator++(int)
  {
    return *this;
  }
};

template <typename _Container>
__device__ inline front_insert_iterator<_Container>
front_inserter(_Container &__x)
{
  return front_insert_iterator<_Container>(__x);
}

template <typename _Container>
class insert_iterator
    : public iterator<output_iterator_tag, void, void, void, void>
{
protected:
  _Container *container;
  typename _Container::iterator iter;

public:
  typedef _Container container_type;

  __device__ insert_iterator(_Container &__x, typename _Container::iterator __i)
      : container(&__x), iter(__i) {}

  __device__ insert_iterator &
  operator=(typename _Container::const_reference __value)
  {
    iter = container->insert(iter, __value);
    ++iter;
    return *this;
  }

  __device__ insert_iterator &
  operator*()
  {
    return *this;
  }

  __device__ insert_iterator &
  operator++()
  {
    return *this;
  }

  __device__ insert_iterator &
  operator++(int)
  {
    return *this;
  }
};

template <typename _Container, typename _Iterator>
__device__ inline insert_iterator<_Container>
inserter(_Container &__x, _Iterator __i)
{
  return insert_iterator<_Container>(__x,
                                     typename _Container::iterator(__i));
}

template <typename _Iterator, typename _Container>
class __normal_iterator
{
protected:
  _Iterator _M_current;

public:
  typedef _Iterator iterator_type;
  typedef typename iterator_traits<_Iterator>::iterator_category
      iterator_category;
  typedef typename iterator_traits<_Iterator>::value_type value_type;
  typedef typename iterator_traits<_Iterator>::difference_type
      difference_type;
  typedef typename iterator_traits<_Iterator>::reference reference;
  typedef typename iterator_traits<_Iterator>::pointer pointer;

  __device__ __normal_iterator() : _M_current(_Iterator()) {}

  __device__ explicit __normal_iterator(const _Iterator &__i) : _M_current(__i) {}

  template <typename _Iter>
  __device__ __normal_iterator(const __normal_iterator<_Iter,
                                                       typename aicuda::stl::__enable_if<
                                                           (aicuda::stl::__are_same<_Iter, typename _Container::pointer>::__value),
                                                           _Container>::__type> &__i)
      : _M_current(__i.base()) {}

  __device__ reference
  operator*() const
  {
    return *_M_current;
  }

  __device__ pointer
  operator->() const
  {
    return _M_current;
  }

  __device__ __normal_iterator &
  operator++()
  {
    ++_M_current;
    return *this;
  }

  __device__ __normal_iterator
  operator++(int)
  {
    return __normal_iterator(_M_current++);
  }

  __device__ __normal_iterator &
  operator--()
  {
    --_M_current;
    return *this;
  }

  __device__ __normal_iterator
  operator--(int)
  {
    return __normal_iterator(_M_current--);
  }

  __device__ reference
  operator[](const difference_type &__n) const
  {
    return _M_current[__n];
  }

  __device__ __normal_iterator &
  operator+=(const difference_type &__n)
  {
    _M_current += __n;
    return *this;
  }

  __device__ __normal_iterator
  operator+(const difference_type &__n) const
  {
    return __normal_iterator(_M_current + __n);
  }

  __device__ __normal_iterator &
  operator-=(const difference_type &__n)
  {
    _M_current -= __n;
    return *this;
  }

  __device__ __normal_iterator
  operator-(const difference_type &__n) const
  {
    return __normal_iterator(_M_current - __n);
  }

  __device__ const _Iterator &
  base() const
  {
    return _M_current;
  }
};

template <typename _IteratorL, typename _IteratorR, typename _Container>
__device__ inline bool
operator==(const __normal_iterator<_IteratorL, _Container> &__lhs,
           const __normal_iterator<_IteratorR, _Container> &__rhs)
{
  return __lhs.base() == __rhs.base();
}

template <typename _Iterator, typename _Container>
__device__ inline bool
operator==(const __normal_iterator<_Iterator, _Container> &__lhs,
           const __normal_iterator<_Iterator, _Container> &__rhs)
{
  return __lhs.base() == __rhs.base();
}

template <typename _IteratorL, typename _IteratorR, typename _Container>
__device__ inline bool
operator!=(const __normal_iterator<_IteratorL, _Container> &__lhs,
           const __normal_iterator<_IteratorR, _Container> &__rhs)
{
  return __lhs.base() != __rhs.base();
}

template <typename _Iterator, typename _Container>
__device__ inline bool
operator!=(const __normal_iterator<_Iterator, _Container> &__lhs,
           const __normal_iterator<_Iterator, _Container> &__rhs)
{
  return __lhs.base() != __rhs.base();
}

template <typename _IteratorL, typename _IteratorR, typename _Container>
__device__ inline bool
operator<(const __normal_iterator<_IteratorL, _Container> &__lhs,
          const __normal_iterator<_IteratorR, _Container> &__rhs)
{
  return __lhs.base() < __rhs.base();
}

template <typename _Iterator, typename _Container>
__device__ inline bool
operator<(const __normal_iterator<_Iterator, _Container> &__lhs,
          const __normal_iterator<_Iterator, _Container> &__rhs)
{
  return __lhs.base() < __rhs.base();
}

template <typename _IteratorL, typename _IteratorR, typename _Container>
__device__ inline bool
operator>(const __normal_iterator<_IteratorL, _Container> &__lhs,
          const __normal_iterator<_IteratorR, _Container> &__rhs)
{
  return __lhs.base() > __rhs.base();
}

template <typename _Iterator, typename _Container>
__device__ inline bool
operator>(const __normal_iterator<_Iterator, _Container> &__lhs,
          const __normal_iterator<_Iterator, _Container> &__rhs)
{
  return __lhs.base() > __rhs.base();
}

template <typename _IteratorL, typename _IteratorR, typename _Container>
__device__ inline bool
operator<=(const __normal_iterator<_IteratorL, _Container> &__lhs,
           const __normal_iterator<_IteratorR, _Container> &__rhs)
{
  return __lhs.base() <= __rhs.base();
}

template <typename _Iterator, typename _Container>
__device__ inline bool
operator<=(const __normal_iterator<_Iterator, _Container> &__lhs,
           const __normal_iterator<_Iterator, _Container> &__rhs)
{
  return __lhs.base() <= __rhs.base();
}

template <typename _IteratorL, typename _IteratorR, typename _Container>
__device__ inline bool
operator>=(const __normal_iterator<_IteratorL, _Container> &__lhs,
           const __normal_iterator<_IteratorR, _Container> &__rhs)
{
  return __lhs.base() >= __rhs.base();
}

template <typename _Iterator, typename _Container>
__device__ inline bool
operator>=(const __normal_iterator<_Iterator, _Container> &__lhs,
           const __normal_iterator<_Iterator, _Container> &__rhs)
{
  return __lhs.base() >= __rhs.base();
}

template <typename _IteratorL, typename _IteratorR, typename _Container>
__device__ inline typename __normal_iterator<_IteratorL, _Container>::difference_type
operator-(const __normal_iterator<_IteratorL, _Container> &__lhs,
          const __normal_iterator<_IteratorR, _Container> &__rhs)

{
  return __lhs.base() - __rhs.base();
}

template <typename _Iterator, typename _Container>
__device__ inline typename __normal_iterator<_Iterator, _Container>::difference_type
operator-(const __normal_iterator<_Iterator, _Container> &__lhs,
          const __normal_iterator<_Iterator, _Container> &__rhs)
{
  return __lhs.base() - __rhs.base();
}

template <typename _Iterator, typename _Container>
__device__ inline __normal_iterator<_Iterator, _Container>
operator+(typename __normal_iterator<_Iterator, _Container>::difference_type
              __n,
          const __normal_iterator<_Iterator, _Container> &__i)
{
  return __normal_iterator<_Iterator, _Container>(__i.base() + __n);
}

} // namespace stl
} // namespace aicuda

#endif /* _AICUDA_STL_ITERATOR_H_ */
