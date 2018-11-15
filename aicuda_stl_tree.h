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

#ifndef _AICUDA_STL_TREE_H_
#define _AICUDA_STL_TREE_H_ 1

#include <aicuda_stl_allocator.h>
#include <aicuda_stl_pair.h>
#include <aicuda_stl_function.h>
#include <aicuda_stl_iterator.h>

namespace aicuda
{
namespace stl
{

enum _Rb_tree_color
{
  _S_red = false,
  _S_black = true
};

class _Rb_tree_node_base
{
public:
  typedef _Rb_tree_node_base *_Base_ptr;
  typedef const _Rb_tree_node_base *_Const_Base_ptr;

  _Rb_tree_color _M_color;
  _Base_ptr _M_parent;
  _Base_ptr _M_left;
  _Base_ptr _M_right;

  __device__ static _Base_ptr
  _S_minimum(_Base_ptr __x)
  {
    while (__x->_M_left != 0)
      __x = __x->_M_left;
    return __x;
  }

  __device__ static _Const_Base_ptr
  _S_minimum(_Const_Base_ptr __x)
  {
    while (__x->_M_left != 0)
      __x = __x->_M_left;
    return __x;
  }

  __device__ static _Base_ptr
  _S_maximum(_Base_ptr __x)
  {
    while (__x->_M_right != 0)
      __x = __x->_M_right;
    return __x;
  }

  __device__ static _Const_Base_ptr
  _S_maximum(_Const_Base_ptr __x)
  {
    while (__x->_M_right != 0)
      __x = __x->_M_right;
    return __x;
  }

  __device__ static _Rb_tree_node_base *
  _Rb_tree_increment(_Rb_tree_node_base *__x)
  {
    return local_Rb_tree_increment(__x);
  }

  __device__ static const _Rb_tree_node_base *
  _Rb_tree_increment(const _Rb_tree_node_base *__x)
  {
    return local_Rb_tree_increment(const_cast<_Rb_tree_node_base *>(__x));
  }

  __device__ static _Rb_tree_node_base *
  _Rb_tree_decrement(_Rb_tree_node_base *__x)
  {
    return local_Rb_tree_decrement(__x);
  }

  __device__ static const _Rb_tree_node_base *
  _Rb_tree_decrement(const _Rb_tree_node_base *__x)
  {
    return local_Rb_tree_decrement(const_cast<_Rb_tree_node_base *>(__x));
  }

  __device__ static unsigned int
  _Rb_tree_black_count(const _Rb_tree_node_base *__node,
                       const _Rb_tree_node_base *__root)
  {
    if (__node == 0)
      return 0;
    unsigned int __sum = 0;
    do
    {
      if (__node->_M_color == _S_black)
        ++__sum;
      if (__node == __root)
        break;
      __node = __node->_M_parent;
    } while (1);
    return __sum;
  }

  __device__ static void
  _Rb_tree_insert_and_rebalance(const bool __insert_left,
                                _Rb_tree_node_base *__x,
                                _Rb_tree_node_base *__p,
                                _Rb_tree_node_base &__header)
  {
    _Rb_tree_node_base *&__root = __header._M_parent;

    // Initialize fields in new node to insert.
    __x->_M_parent = __p;
    __x->_M_left = 0;
    __x->_M_right = 0;
    __x->_M_color = _S_red;

    // Insert.
    // Make new node child of parent and maintain root, leftmost and
    // rightmost nodes.
    // N.B. First node is always inserted left.
    if (__insert_left)
    {
      __p->_M_left = __x; // also makes leftmost = __x when __p == &__header

      if (__p == &__header)
      {
        __header._M_parent = __x;
        __header._M_right = __x;
      }
      else if (__p == __header._M_left)
        __header._M_left = __x; // maintain leftmost pointing to min node
    }
    else
    {
      __p->_M_right = __x;

      if (__p == __header._M_right)
        __header._M_right = __x; // maintain rightmost pointing to max node
    }
    // Rebalance.
    while (__x != __root && __x->_M_parent->_M_color == _S_red)
    {
      _Rb_tree_node_base *const __xpp = __x->_M_parent->_M_parent;

      if (__x->_M_parent == __xpp->_M_left)
      {
        _Rb_tree_node_base *const __y = __xpp->_M_right;
        if (__y && __y->_M_color == _S_red)
        {
          __x->_M_parent->_M_color = _S_black;
          __y->_M_color = _S_black;
          __xpp->_M_color = _S_red;
          __x = __xpp;
        }
        else
        {
          if (__x == __x->_M_parent->_M_right)
          {
            __x = __x->_M_parent;
            local_Rb_tree_rotate_left(__x, __root);
          }
          __x->_M_parent->_M_color = _S_black;
          __xpp->_M_color = _S_red;
          local_Rb_tree_rotate_right(__xpp, __root);
        }
      }
      else
      {
        _Rb_tree_node_base *const __y = __xpp->_M_left;
        if (__y && __y->_M_color == _S_red)
        {
          __x->_M_parent->_M_color = _S_black;
          __y->_M_color = _S_black;
          __xpp->_M_color = _S_red;
          __x = __xpp;
        }
        else
        {
          if (__x == __x->_M_parent->_M_left)
          {
            __x = __x->_M_parent;
            local_Rb_tree_rotate_right(__x, __root);
          }
          __x->_M_parent->_M_color = _S_black;
          __xpp->_M_color = _S_red;
          local_Rb_tree_rotate_left(__xpp, __root);
        }
      }
    }
    __root->_M_color = _S_black;
  }

  __device__ static _Rb_tree_node_base *
  _Rb_tree_rebalance_for_erase(_Rb_tree_node_base *const __z,
                               _Rb_tree_node_base &__header)
  {
    _Rb_tree_node_base *&__root = __header._M_parent;
    _Rb_tree_node_base *&__leftmost = __header._M_left;
    _Rb_tree_node_base *&__rightmost = __header._M_right;
    _Rb_tree_node_base *__y = __z;
    _Rb_tree_node_base *__x = 0;
    _Rb_tree_node_base *__x_parent = 0;

    if (__y->_M_left == 0)       // __z has at most one non-null child. y == z.
      __x = __y->_M_right;       // __x might be null.
    else if (__y->_M_right == 0) // __z has exactly one non-null child. y == z.
      __x = __y->_M_left;        // __x is not null.
    else
    {
      // __z has two non-null children.  Set __y to
      __y = __y->_M_right; //   __z's successor.  __x might be null.
      while (__y->_M_left != 0)
        __y = __y->_M_left;
      __x = __y->_M_right;
    }
    if (__y != __z)
    {
      // relink y in place of z.  y is z's successor
      __z->_M_left->_M_parent = __y;
      __y->_M_left = __z->_M_left;
      if (__y != __z->_M_right)
      {
        __x_parent = __y->_M_parent;
        if (__x)
          __x->_M_parent = __y->_M_parent;
        __y->_M_parent->_M_left = __x; // __y must be a child of _M_left
        __y->_M_right = __z->_M_right;
        __z->_M_right->_M_parent = __y;
      }
      else
        __x_parent = __y;
      if (__root == __z)
        __root = __y;
      else if (__z->_M_parent->_M_left == __z)
        __z->_M_parent->_M_left = __y;
      else
        __z->_M_parent->_M_right = __y;
      __y->_M_parent = __z->_M_parent;
      aicuda::stl::swap(__y->_M_color, __z->_M_color);
      __y = __z;
      // __y now points to node to be actually deleted
    }
    else
    { // __y == __z
      __x_parent = __y->_M_parent;
      if (__x)
        __x->_M_parent = __y->_M_parent;
      if (__root == __z)
        __root = __x;
      else if (__z->_M_parent->_M_left == __z)
        __z->_M_parent->_M_left = __x;
      else
        __z->_M_parent->_M_right = __x;
      if (__leftmost == __z)
      {
        if (__z->_M_right == 0) // __z->_M_left must be null also
          __leftmost = __z->_M_parent;
        // makes __leftmost == _M_header if __z == __root
        else
          __leftmost = _S_minimum(__x);
      }
      if (__rightmost == __z)
      {
        if (__z->_M_left == 0) // __z->_M_right must be null also
          __rightmost = __z->_M_parent;
        // makes __rightmost == _M_header if __z == __root
        else // __x == __z->_M_left
          __rightmost = _S_maximum(__x);
      }
    }
    if (__y->_M_color != _S_red)
    {
      while (__x != __root && (__x == 0 || __x->_M_color == _S_black))
        if (__x == __x_parent->_M_left)
        {
          _Rb_tree_node_base *__w = __x_parent->_M_right;
          if (__w->_M_color == _S_red)
          {
            __w->_M_color = _S_black;
            __x_parent->_M_color = _S_red;
            local_Rb_tree_rotate_left(__x_parent, __root);
            __w = __x_parent->_M_right;
          }
          if ((__w->_M_left == 0 ||
               __w->_M_left->_M_color == _S_black) &&
              (__w->_M_right == 0 ||
               __w->_M_right->_M_color == _S_black))
          {
            __w->_M_color = _S_red;
            __x = __x_parent;
            __x_parent = __x_parent->_M_parent;
          }
          else
          {
            if (__w->_M_right == 0 || __w->_M_right->_M_color == _S_black)
            {
              __w->_M_left->_M_color = _S_black;
              __w->_M_color = _S_red;
              local_Rb_tree_rotate_right(__w, __root);
              __w = __x_parent->_M_right;
            }
            __w->_M_color = __x_parent->_M_color;
            __x_parent->_M_color = _S_black;
            if (__w->_M_right)
              __w->_M_right->_M_color = _S_black;
            local_Rb_tree_rotate_left(__x_parent, __root);
            break;
          }
        }
        else
        {
          // same as above, with _M_right <-> _M_left.
          _Rb_tree_node_base *__w = __x_parent->_M_left;
          if (__w->_M_color == _S_red)
          {
            __w->_M_color = _S_black;
            __x_parent->_M_color = _S_red;
            local_Rb_tree_rotate_right(__x_parent, __root);
            __w = __x_parent->_M_left;
          }
          if ((__w->_M_right == 0 ||
               __w->_M_right->_M_color == _S_black) &&
              (__w->_M_left == 0 ||
               __w->_M_left->_M_color == _S_black))
          {
            __w->_M_color = _S_red;
            __x = __x_parent;
            __x_parent = __x_parent->_M_parent;
          }
          else
          {
            if (__w->_M_left == 0 || __w->_M_left->_M_color == _S_black)
            {
              __w->_M_right->_M_color = _S_black;
              __w->_M_color = _S_red;
              local_Rb_tree_rotate_left(__w, __root);
              __w = __x_parent->_M_left;
            }
            __w->_M_color = __x_parent->_M_color;
            __x_parent->_M_color = _S_black;
            if (__w->_M_left)
              __w->_M_left->_M_color = _S_black;
            local_Rb_tree_rotate_right(__x_parent, __root);
            break;
          }
        }
      if (__x)
        __x->_M_color = _S_black;
    }
    return __y;
  }

private:
  __device__ static _Rb_tree_node_base *
  local_Rb_tree_increment(_Rb_tree_node_base *__x)
  {
    if (__x->_M_right != 0)
    {
      __x = __x->_M_right;
      while (__x->_M_left != 0)
        __x = __x->_M_left;
    }
    else
    {
      _Rb_tree_node_base *__y = __x->_M_parent;
      while (__x == __y->_M_right)
      {
        __x = __y;
        __y = __y->_M_parent;
      }
      if (__x->_M_right != __y)
        __x = __y;
    }
    return __x;
  }

  __device__ static _Rb_tree_node_base *
  local_Rb_tree_decrement(_Rb_tree_node_base *__x)
  {
    if (__x->_M_color == _S_red && __x->_M_parent->_M_parent == __x)
      __x = __x->_M_right;
    else if (__x->_M_left != 0)
    {
      _Rb_tree_node_base *__y = __x->_M_left;
      while (__y->_M_right != 0)
        __y = __y->_M_right;
      __x = __y;
    }
    else
    {
      _Rb_tree_node_base *__y = __x->_M_parent;
      while (__x == __y->_M_left)
      {
        __x = __y;
        __y = __y->_M_parent;
      }
      __x = __y;
    }
    return __x;
  }

  __device__ static void
  local_Rb_tree_rotate_right(_Rb_tree_node_base *const __x,
                             _Rb_tree_node_base *&__root)
  {
    _Rb_tree_node_base *const __y = __x->_M_left;

    __x->_M_left = __y->_M_right;
    if (__y->_M_right != 0)
      __y->_M_right->_M_parent = __x;
    __y->_M_parent = __x->_M_parent;

    if (__x == __root)
      __root = __y;
    else if (__x == __x->_M_parent->_M_right)
      __x->_M_parent->_M_right = __y;
    else
      __x->_M_parent->_M_left = __y;
    __y->_M_right = __x;
    __x->_M_parent = __y;
  }

  __device__ static void
  local_Rb_tree_rotate_left(_Rb_tree_node_base *const __x,
                            _Rb_tree_node_base *&__root)
  {
    _Rb_tree_node_base *const __y = __x->_M_right;

    __x->_M_right = __y->_M_left;
    if (__y->_M_left != 0)
      __y->_M_left->_M_parent = __x;
    __y->_M_parent = __x->_M_parent;

    if (__x == __root)
      __root = __y;
    else if (__x == __x->_M_parent->_M_left)
      __x->_M_parent->_M_left = __y;
    else
      __x->_M_parent->_M_right = __y;
    __y->_M_left = __x;
    __x->_M_parent = __y;
  }
};

template <typename _Val>
class _Rb_tree_node : public _Rb_tree_node_base
{
public:
  typedef _Rb_tree_node<_Val> *_Link_type;
  _Val _M_value_field;
};

template <typename _Tp>
struct _Rb_tree_iterator
{
  typedef _Tp value_type;
  typedef _Tp &reference;
  typedef _Tp *pointer;

  typedef bidirectional_iterator_tag iterator_category;
  typedef ptrdiff_t difference_type;

  typedef _Rb_tree_iterator<_Tp> _Self;
  typedef _Rb_tree_node_base::_Base_ptr _Base_ptr;
  typedef _Rb_tree_node<_Tp> *_Link_type;

  __device__ _Rb_tree_iterator()
      : _M_node() {}

  __device__ explicit _Rb_tree_iterator(_Link_type __x)
      : _M_node(__x) {}

  __device__ reference
  operator*() const
  {
    return static_cast<_Link_type>(_M_node)->_M_value_field;
  }

  __device__ pointer
  operator->() const
  {
    return &static_cast<_Link_type>(_M_node)->_M_value_field;
  }

  __device__ _Self &
  operator++()
  {
    _M_node = _Rb_tree_node_base::_Rb_tree_increment(_M_node);
    return *this;
  }

  __device__ _Self
  operator++(int)
  {
    _Self __tmp = *this;
    _M_node = _Rb_tree_node_base::_Rb_tree_increment(_M_node);
    return __tmp;
  }

  __device__ _Self &
  operator--()
  {
    _M_node = _Rb_tree_node_base::_Rb_tree_decrement(_M_node);
    return *this;
  }

  __device__ _Self
  operator--(int)
  {
    _Self __tmp = *this;
    _M_node = _Rb_tree_node_base::_Rb_tree_decrement(_M_node);
    return __tmp;
  }

  __device__ bool
  operator==(const _Self &__x) const
  {
    return _M_node == __x._M_node;
  }

  __device__ bool
  operator!=(const _Self &__x) const
  {
    return _M_node != __x._M_node;
  }

  _Base_ptr _M_node;
};

template <typename _Tp>
struct _Rb_tree_const_iterator
{
  typedef _Tp value_type;
  typedef const _Tp &reference;
  typedef const _Tp *pointer;

  typedef _Rb_tree_iterator<_Tp> iterator;

  typedef bidirectional_iterator_tag iterator_category;
  typedef ptrdiff_t difference_type;

  typedef _Rb_tree_const_iterator<_Tp> _Self;
  typedef _Rb_tree_node_base::_Const_Base_ptr _Base_ptr;
  typedef const _Rb_tree_node<_Tp> *_Link_type;

  __device__ _Rb_tree_const_iterator()
      : _M_node() {}

  __device__ explicit _Rb_tree_const_iterator(_Link_type __x)
      : _M_node(__x) {}

  __device__ _Rb_tree_const_iterator(const iterator &__it)
      : _M_node(__it._M_node) {}

  __device__ reference
  operator*() const
  {
    return static_cast<_Link_type>(_M_node)->_M_value_field;
  }

  __device__ pointer
  operator->() const
  {
    return &static_cast<_Link_type>(_M_node)->_M_value_field;
  }

  __device__ _Self &
  operator++()
  {
    _M_node = _Rb_tree_node_base::_Rb_tree_increment(_M_node);
    return *this;
  }

  __device__ _Self
  operator++(int)
  {
    _Self __tmp = *this;
    _M_node = _Rb_tree_node_base::_Rb_tree_increment(_M_node);
    return __tmp;
  }

  __device__ _Self &
  operator--()
  {
    _M_node = _Rb_tree_node_base::_Rb_tree_decrement(_M_node);
    return *this;
  }

  __device__ _Self
  operator--(int)
  {
    _Self __tmp = *this;
    _M_node = _Rb_tree_node_base::_Rb_tree_decrement(_M_node);
    return __tmp;
  }

  __device__ bool
  operator==(const _Self &__x) const
  {
    return _M_node == __x._M_node;
  }

  __device__ bool
  operator!=(const _Self &__x) const
  {
    return _M_node != __x._M_node;
  }

  _Base_ptr _M_node;
};

template <typename _Val>
__device__ inline bool
operator==(const _Rb_tree_iterator<_Val> &__x,
           const _Rb_tree_const_iterator<_Val> &__y)
{
  return __x._M_node == __y._M_node;
}

template <typename _Val>
__device__ inline bool
operator!=(const _Rb_tree_iterator<_Val> &__x,
           const _Rb_tree_const_iterator<_Val> &__y)
{
  return __x._M_node != __y._M_node;
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc = aicuda::stl::allocator<_Val>>
class _Rb_tree
{
  typedef typename _Alloc::template rebind<_Rb_tree_node<_Val>>::other
      _Node_allocator;

protected:
  typedef _Rb_tree_node_base *_Base_ptr;
  typedef const _Rb_tree_node_base *_Const_Base_ptr;

public:
  typedef _Key key_type;
  typedef _Val value_type;
  typedef value_type *pointer;
  typedef const value_type *const_pointer;
  typedef value_type &reference;
  typedef const value_type &const_reference;
  typedef _Rb_tree_node<_Val> *_Link_type;
  typedef const _Rb_tree_node<_Val> *_Const_Link_type;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef _Alloc allocator_type;

  __device__ _Node_allocator &
  _M_get_Node_allocator()
  {
    return *static_cast<_Node_allocator *>(&this->_M_impl);
  }

  __device__ const _Node_allocator &
  _M_get_Node_allocator() const
  {
    return *static_cast<const _Node_allocator *>(&this->_M_impl);
  }

  __device__ allocator_type
  get_allocator() const
  {
    return allocator_type(_M_get_Node_allocator());
  }

protected:
  __device__ _Link_type
  _M_get_node()
  {
    return _M_impl._Node_allocator::allocate(1);
  }

  __device__ void
  _M_put_node(_Link_type __p)
  {
    _M_impl._Node_allocator::deallocate(__p, 1);
  }

  __device__ _Link_type
  _M_create_node(const value_type &__x)
  {
    _Link_type __tmp = _M_get_node();
    get_allocator().construct(&__tmp->_M_value_field, __x);
    return __tmp;
  }

  __device__ void
  _M_destroy_node(_Link_type __p)
  {
    get_allocator().destroy(&__p->_M_value_field);
    _M_put_node(__p);
  }

  __device__ _Link_type
  _M_clone_node(_Const_Link_type __x)
  {
    _Link_type __tmp = _M_create_node(__x->_M_value_field);
    __tmp->_M_color = __x->_M_color;
    __tmp->_M_left = 0;
    __tmp->_M_right = 0;
    return __tmp;
  }

protected:
  template <typename _Key_compare,
            bool _Is_pod_comparator = __is_pod(_Key_compare)>
  struct _Rb_tree_impl : public _Node_allocator
  {
    _Key_compare _M_key_compare;
    _Rb_tree_node_base _M_header;
    size_type _M_node_count;

    __device__ _Rb_tree_impl()
        : _Node_allocator(), _M_key_compare(), _M_header(),
          _M_node_count(0)
    {
      _M_initialize();
    }

    __device__ _Rb_tree_impl(const _Key_compare &__comp, const _Node_allocator &__a)
        : _Node_allocator(__a), _M_key_compare(__comp), _M_header(),
          _M_node_count(0)
    {
      _M_initialize();
    }

  private:
    __device__ void
    _M_initialize()
    {
      this->_M_header._M_color = _S_red;
      this->_M_header._M_parent = 0;
      this->_M_header._M_left = &this->_M_header;
      this->_M_header._M_right = &this->_M_header;
    }
  };

  _Rb_tree_impl<_Compare> _M_impl;

protected:
  __device__ _Base_ptr &
  _M_root()
  {
    return this->_M_impl._M_header._M_parent;
  }

  __device__ _Const_Base_ptr
  _M_root() const
  {
    return this->_M_impl._M_header._M_parent;
  }

  __device__ _Base_ptr &
  _M_leftmost()
  {
    return this->_M_impl._M_header._M_left;
  }

  __device__ _Const_Base_ptr
  _M_leftmost() const
  {
    return this->_M_impl._M_header._M_left;
  }

  __device__ _Base_ptr &
  _M_rightmost()
  {
    return this->_M_impl._M_header._M_right;
  }

  __device__ _Const_Base_ptr
  _M_rightmost() const
  {
    return this->_M_impl._M_header._M_right;
  }

  __device__ _Link_type
  _M_begin()
  {
    return static_cast<_Link_type>(this->_M_impl._M_header._M_parent);
  }

  __device__ _Const_Link_type
  _M_begin() const
  {
    return static_cast<_Const_Link_type>(this->_M_impl._M_header._M_parent);
  }

  __device__ _Link_type
  _M_end()
  {
    return static_cast<_Link_type>(&this->_M_impl._M_header);
  }

  __device__ _Const_Link_type
  _M_end() const
  {
    return static_cast<_Const_Link_type>(&this->_M_impl._M_header);
  }

  __device__ static const_reference
  _S_value(_Const_Link_type __x)
  {
    return __x->_M_value_field;
  }

  __device__ static const _Key &
  _S_key(_Const_Link_type __x)
  {
    return _KeyOfValue()(_S_value(__x));
  }

  __device__ static _Link_type
  _S_left(_Base_ptr __x)
  {
    return static_cast<_Link_type>(__x->_M_left);
  }

  __device__ static _Const_Link_type
  _S_left(_Const_Base_ptr __x)
  {
    return static_cast<_Const_Link_type>(__x->_M_left);
  }

  __device__ static _Link_type
  _S_right(_Base_ptr __x)
  {
    return static_cast<_Link_type>(__x->_M_right);
  }

  __device__ static _Const_Link_type
  _S_right(_Const_Base_ptr __x)
  {
    return static_cast<_Const_Link_type>(__x->_M_right);
  }

  __device__ static const_reference
  _S_value(_Const_Base_ptr __x)
  {
    return static_cast<_Const_Link_type>(__x)->_M_value_field;
  }

  __device__ static const _Key &
  _S_key(_Const_Base_ptr __x)
  {
    return _KeyOfValue()(_S_value(__x));
  }

  __device__ static _Base_ptr
  _S_minimum(_Base_ptr __x)
  {
    return _Rb_tree_node_base::_S_minimum(__x);
  }

  __device__ static _Const_Base_ptr
  _S_minimum(_Const_Base_ptr __x)
  {
    return _Rb_tree_node_base::_S_minimum(__x);
  }

  __device__ static _Base_ptr
  _S_maximum(_Base_ptr __x)
  {
    return _Rb_tree_node_base::_S_maximum(__x);
  }

  __device__ static _Const_Base_ptr
  _S_maximum(_Const_Base_ptr __x)
  {
    return _Rb_tree_node_base::_S_maximum(__x);
  }

public:
  typedef _Rb_tree_iterator<value_type> iterator;
  typedef _Rb_tree_const_iterator<value_type> const_iterator;

  typedef aicuda::stl::reverse_iterator<iterator> reverse_iterator;
  typedef aicuda::stl::reverse_iterator<const_iterator> const_reverse_iterator;

private:
  __device__ iterator
  _M_insert_(_Const_Base_ptr __x, _Const_Base_ptr __y,
             const value_type &__v);

  __device__ iterator
  _M_insert_lower(_Base_ptr __x, _Base_ptr __y, const value_type &__v);

  __device__ iterator
  _M_insert_equal_lower(const value_type &__x);

  __device__ _Link_type
  _M_copy(_Const_Link_type __x, _Link_type __p);

  __device__ void
  _M_erase(_Link_type __x);

  __device__ iterator
  _M_lower_bound(_Link_type __x, _Link_type __y,
                 const _Key &__k);

  __device__ const_iterator
  _M_lower_bound(_Const_Link_type __x, _Const_Link_type __y,
                 const _Key &__k) const;

  __device__ iterator
  _M_upper_bound(_Link_type __x, _Link_type __y,
                 const _Key &__k);

  __device__ const_iterator
  _M_upper_bound(_Const_Link_type __x, _Const_Link_type __y,
                 const _Key &__k) const;

public:
  __device__ _Rb_tree() {}

  __device__ _Rb_tree(const _Compare &__comp,
                      const allocator_type &__a = allocator_type())
      : _M_impl(__comp, __a) {}

  __device__ _Rb_tree(const _Rb_tree &__x)
      : _M_impl(__x._M_impl._M_key_compare, __x._M_get_Node_allocator())
  {
    if (__x._M_root() != 0)
    {
      _M_root() = _M_copy(__x._M_begin(), _M_end());
      _M_leftmost() = _S_minimum(_M_root());
      _M_rightmost() = _S_maximum(_M_root());
      _M_impl._M_node_count = __x._M_impl._M_node_count;
    }
  }

  __device__ ~_Rb_tree()
  {
    _M_erase(_M_begin());
  }

  __device__ _Rb_tree &
  operator=(const _Rb_tree &__x);

  __device__ _Compare
  key_comp() const
  {
    return _M_impl._M_key_compare;
  }

  __device__ iterator
  begin()
  {
    return iterator(static_cast<_Link_type>(this->_M_impl._M_header._M_left));
  }

  __device__ const_iterator
  begin() const
  {
    return const_iterator(static_cast<_Const_Link_type>(this->_M_impl._M_header._M_left));
  }

  __device__ iterator
  end()
  {
    return iterator(static_cast<_Link_type>(&this->_M_impl._M_header));
  }

  __device__ const_iterator
  end() const
  {
    return const_iterator(static_cast<_Const_Link_type>(&this->_M_impl._M_header));
  }

  __device__ reverse_iterator
  rbegin()
  {
    return reverse_iterator(end());
  }

  __device__ const_reverse_iterator
  rbegin() const
  {
    return const_reverse_iterator(end());
  }

  __device__ reverse_iterator
  rend()
  {
    return reverse_iterator(begin());
  }

  __device__ const_reverse_iterator
  rend() const
  {
    return const_reverse_iterator(begin());
  }

  __device__ bool
  empty() const
  {
    return _M_impl._M_node_count == 0;
  }

  __device__ size_type
  size() const
  {
    return _M_impl._M_node_count;
  }

  __device__ void
  swap(_Rb_tree &__t);

  __device__ pair<iterator, bool>
  _M_insert_unique(const value_type &__x);

  __device__ iterator
  _M_insert_equal(const value_type &__x);

  __device__ iterator
  _M_insert_unique_(const_iterator __position, const value_type &__x);

  __device__ iterator
  _M_insert_equal_(const_iterator __position, const value_type &__x);

  template <typename _InputIterator>
  __device__ void
  _M_insert_unique(_InputIterator __first, _InputIterator __last);

  template <typename _InputIterator>
  __device__ void
  _M_insert_equal(_InputIterator __first, _InputIterator __last);

  __device__ void
  erase(iterator __position);

  __device__ void
  erase(const_iterator __position);

  __device__ size_type
  erase(const key_type &__x);

  __device__ void
  erase(iterator __first, iterator __last);

  __device__ void
  erase(const_iterator __first, const_iterator __last);

  __device__ void
  erase(const key_type *__first, const key_type *__last);

  __device__ void
  clear()
  {
    _M_erase(_M_begin());
    _M_leftmost() = _M_end();
    _M_root() = 0;
    _M_rightmost() = _M_end();
    _M_impl._M_node_count = 0;
  }

  __device__ iterator
  find(const key_type &__k);

  __device__ const_iterator
  find(const key_type &__k) const;

  __device__ size_type
  count(const key_type &__k) const;

  __device__ iterator
  lower_bound(const key_type &__k)
  {
    return _M_lower_bound(_M_begin(), _M_end(), __k);
  }

  __device__ const_iterator
  lower_bound(const key_type &__k) const
  {
    return _M_lower_bound(_M_begin(), _M_end(), __k);
  }

  __device__ iterator
  upper_bound(const key_type &__k)
  {
    return _M_upper_bound(_M_begin(), _M_end(), __k);
  }

  __device__ const_iterator
  upper_bound(const key_type &__k) const
  {
    return _M_upper_bound(_M_begin(), _M_end(), __k);
  }

  __device__ pair<iterator, iterator>
  equal_range(const key_type &__k);

  __device__ pair<const_iterator, const_iterator>
  equal_range(const key_type &__k) const;

  __device__ bool
  __rb_verify() const;
};

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ inline void
swap(_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc> &__x,
     _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc> &__y)
{
  __x.swap(__y);
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc> &
_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
operator=(const _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc> &__x)
{
  if (this != &__x)
  {

    clear();
    _M_impl._M_key_compare = __x._M_impl._M_key_compare;
    if (__x._M_root() != 0)
    {
      _M_root() = _M_copy(__x._M_begin(), _M_end());
      _M_leftmost() = _S_minimum(_M_root());
      _M_rightmost() = _S_maximum(_M_root());
      _M_impl._M_node_count = __x._M_impl._M_node_count;
    }
  }
  return *this;
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ typename _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::iterator
_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_insert_(_Const_Base_ptr __x, _Const_Base_ptr __p, const _Val &__v)
{
  bool __insert_left = (__x != 0 || __p == _M_end() || _M_impl._M_key_compare(_KeyOfValue()(__v), _S_key(__p)));

  _Link_type __z = _M_create_node(__v);

  _Rb_tree_node_base::_Rb_tree_insert_and_rebalance(__insert_left, __z,
                                                    const_cast<_Base_ptr>(__p),
                                                    this->_M_impl._M_header);
  ++_M_impl._M_node_count;
  return iterator(__z);
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ typename _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::iterator
_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_insert_lower(_Base_ptr __x, _Base_ptr __p, const _Val &__v)
{
  bool __insert_left = (__x != 0 || __p == _M_end() || !_M_impl._M_key_compare(_S_key(__p), _KeyOfValue()(__v)));

  _Link_type __z = _M_create_node(__v);

  _Rb_tree_node_base::_Rb_tree_insert_and_rebalance(__insert_left, __z, __p,
                                                    this->_M_impl._M_header);
  ++_M_impl._M_node_count;
  return iterator(__z);
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ typename _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::iterator
_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_insert_equal_lower(const _Val &__v)
{
  _Link_type __x = _M_begin();
  _Link_type __y = _M_end();
  while (__x != 0)
  {
    __y = __x;
    __x = !_M_impl._M_key_compare(_S_key(__x), _KeyOfValue()(__v)) ? _S_left(__x) : _S_right(__x);
  }
  return _M_insert_lower(__x, __y, __v);
}

template <typename _Key, typename _Val, typename _KoV,
          typename _Compare, typename _Alloc>
__device__ typename _Rb_tree<_Key, _Val, _KoV, _Compare, _Alloc>::_Link_type
_Rb_tree<_Key, _Val, _KoV, _Compare, _Alloc>::
    _M_copy(_Const_Link_type __x, _Link_type __p)
{

  _Link_type __top = _M_clone_node(__x);
  __top->_M_parent = __p;

  if (__x->_M_right)
    __top->_M_right = _M_copy(_S_right(__x), __top);
  __p = __top;
  __x = _S_left(__x);

  while (__x != 0)
  {
    _Link_type __y = _M_clone_node(__x);
    __p->_M_left = __y;
    __y->_M_parent = __p;
    if (__x->_M_right)
      __y->_M_right = _M_copy(_S_right(__x), __y);
    __p = __y;
    __x = _S_left(__x);
  }

  return __top;
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ void _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_erase(_Link_type __x)
{

  while (__x != 0)
  {
    _M_erase(_S_right(__x));
    _Link_type __y = _S_left(__x);
    _M_destroy_node(__x);
    __x = __y;
  }
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ typename _Rb_tree<_Key, _Val, _KeyOfValue,
                             _Compare, _Alloc>::iterator
_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_lower_bound(_Link_type __x, _Link_type __y,
                   const _Key &__k)
{
  while (__x != 0)
    if (!_M_impl._M_key_compare(_S_key(__x), __k))
      __y = __x, __x = _S_left(__x);
    else
      __x = _S_right(__x);
  return iterator(__y);
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ typename _Rb_tree<_Key, _Val, _KeyOfValue,
                             _Compare, _Alloc>::const_iterator
_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_lower_bound(_Const_Link_type __x, _Const_Link_type __y,
                   const _Key &__k) const
{
  while (__x != 0)
    if (!_M_impl._M_key_compare(_S_key(__x), __k))
      __y = __x, __x = _S_left(__x);
    else
      __x = _S_right(__x);
  return const_iterator(__y);
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ typename _Rb_tree<_Key, _Val, _KeyOfValue,
                             _Compare, _Alloc>::iterator
_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_upper_bound(_Link_type __x, _Link_type __y,
                   const _Key &__k)
{
  while (__x != 0)
    if (_M_impl._M_key_compare(__k, _S_key(__x)))
      __y = __x, __x = _S_left(__x);
    else
      __x = _S_right(__x);
  return iterator(__y);
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ typename _Rb_tree<_Key, _Val, _KeyOfValue,
                             _Compare, _Alloc>::const_iterator
_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_upper_bound(_Const_Link_type __x, _Const_Link_type __y,
                   const _Key &__k) const
{
  while (__x != 0)
    if (_M_impl._M_key_compare(__k, _S_key(__x)))
      __y = __x, __x = _S_left(__x);
    else
      __x = _S_right(__x);
  return const_iterator(__y);
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ aicuda::stl::pair<typename _Rb_tree<_Key, _Val, _KeyOfValue,
                                               _Compare, _Alloc>::iterator,
                             typename _Rb_tree<_Key, _Val, _KeyOfValue,
                                               _Compare, _Alloc>::iterator>
_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    equal_range(const _Key &__k)
{
  _Link_type __x = _M_begin();
  _Link_type __y = _M_end();
  while (__x != 0)
  {
    if (_M_impl._M_key_compare(_S_key(__x), __k))
      __x = _S_right(__x);
    else if (_M_impl._M_key_compare(__k, _S_key(__x)))
      __y = __x, __x = _S_left(__x);
    else
    {
      _Link_type __xu(__x), __yu(__y);
      __y = __x, __x = _S_left(__x);
      __xu = _S_right(__xu);
      return pair<iterator,
                  iterator>(_M_lower_bound(__x, __y, __k),
                            _M_upper_bound(__xu, __yu, __k));
    }
  }
  return pair<iterator, iterator>(iterator(__y),
                                  iterator(__y));
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ aicuda::stl::pair<typename _Rb_tree<_Key, _Val, _KeyOfValue,
                                               _Compare, _Alloc>::const_iterator,
                             typename _Rb_tree<_Key, _Val, _KeyOfValue,
                                               _Compare, _Alloc>::const_iterator>
_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    equal_range(const _Key &__k) const
{
  _Const_Link_type __x = _M_begin();
  _Const_Link_type __y = _M_end();
  while (__x != 0)
  {
    if (_M_impl._M_key_compare(_S_key(__x), __k))
      __x = _S_right(__x);
    else if (_M_impl._M_key_compare(__k, _S_key(__x)))
      __y = __x, __x = _S_left(__x);
    else
    {
      _Const_Link_type __xu(__x), __yu(__y);
      __y = __x, __x = _S_left(__x);
      __xu = _S_right(__xu);
      return pair<const_iterator,
                  const_iterator>(_M_lower_bound(__x, __y, __k),
                                  _M_upper_bound(__xu, __yu, __k));
    }
  }
  return pair<const_iterator, const_iterator>(const_iterator(__y),
                                              const_iterator(__y));
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ void _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    swap(_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc> &__t)
{
  if (_M_root() == 0)
  {
    if (__t._M_root() != 0)
    {
      _M_root() = __t._M_root();
      _M_leftmost() = __t._M_leftmost();
      _M_rightmost() = __t._M_rightmost();
      _M_root()->_M_parent = _M_end();

      __t._M_root() = 0;
      __t._M_leftmost() = __t._M_end();
      __t._M_rightmost() = __t._M_end();
    }
  }
  else if (__t._M_root() == 0)
  {
    __t._M_root() = _M_root();
    __t._M_leftmost() = _M_leftmost();
    __t._M_rightmost() = _M_rightmost();
    __t._M_root()->_M_parent = __t._M_end();

    _M_root() = 0;
    _M_leftmost() = _M_end();
    _M_rightmost() = _M_end();
  }
  else
  {
    aicuda::stl::swap(_M_root(), __t._M_root());
    aicuda::stl::swap(_M_leftmost(), __t._M_leftmost());
    aicuda::stl::swap(_M_rightmost(), __t._M_rightmost());

    _M_root()->_M_parent = _M_end();
    __t._M_root()->_M_parent = __t._M_end();
  }

  aicuda::stl::swap(this->_M_impl._M_node_count, __t._M_impl._M_node_count);
  aicuda::stl::swap(this->_M_impl._M_key_compare, __t._M_impl._M_key_compare);

  aicuda::stl::__alloc_swap<_Node_allocator>::
      _S_do_it(_M_get_Node_allocator(), __t._M_get_Node_allocator());
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ aicuda::stl::pair<typename _Rb_tree<_Key, _Val, _KeyOfValue,
                                               _Compare, _Alloc>::iterator,
                             bool>
_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_insert_unique(const _Val &__v)
{
  _Link_type __x = _M_begin();
  _Link_type __y = _M_end();
  bool __comp = true;
  while (__x != 0)
  {
    __y = __x;
    __comp = _M_impl._M_key_compare(_KeyOfValue()(__v), _S_key(__x));
    __x = __comp ? _S_left(__x) : _S_right(__x);
  }
  iterator __j = iterator(__y);
  if (__comp)
  {
    if (__j == begin())
      return pair<iterator, bool>(_M_insert_(__x, __y, __v), true);
    else
      --__j;
  }
  if (_M_impl._M_key_compare(_S_key(__j._M_node), _KeyOfValue()(__v)))
    return pair<iterator, bool>(_M_insert_(__x, __y, __v), true);
  return pair<iterator, bool>(__j, false);
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ typename _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::iterator
_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_insert_equal(const _Val &__v)
{
  _Link_type __x = _M_begin();
  _Link_type __y = _M_end();
  while (__x != 0)
  {
    __y = __x;
    __x = _M_impl._M_key_compare(_KeyOfValue()(__v), _S_key(__x)) ? _S_left(__x) : _S_right(__x);
  }
  return _M_insert_(__x, __y, __v);
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ typename _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::iterator
_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_insert_unique_(const_iterator __position, const _Val &__v)
{

  if (__position._M_node == _M_end())
  {
    if (size() > 0 && _M_impl._M_key_compare(_S_key(_M_rightmost()),
                                             _KeyOfValue()(__v)))
      return _M_insert_(0, _M_rightmost(), __v);
    else
      return _M_insert_unique(__v).first;
  }
  else if (_M_impl._M_key_compare(_KeyOfValue()(__v),
                                  _S_key(__position._M_node)))
  {

    const_iterator __before = __position;
    if (__position._M_node == _M_leftmost())
      return _M_insert_(_M_leftmost(), _M_leftmost(), __v);
    else if (_M_impl._M_key_compare(_S_key((--__before)._M_node),
                                    _KeyOfValue()(__v)))
    {
      if (_S_right(__before._M_node) == 0)
        return _M_insert_(0, __before._M_node, __v);
      else
        return _M_insert_(__position._M_node,
                          __position._M_node, __v);
    }
    else
      return _M_insert_unique(__v).first;
  }
  else if (_M_impl._M_key_compare(_S_key(__position._M_node),
                                  _KeyOfValue()(__v)))
  {

    const_iterator __after = __position;
    if (__position._M_node == _M_rightmost())
      return _M_insert_(0, _M_rightmost(), __v);
    else if (_M_impl._M_key_compare(_KeyOfValue()(__v),
                                    _S_key((++__after)._M_node)))
    {
      if (_S_right(__position._M_node) == 0)
        return _M_insert_(0, __position._M_node, __v);
      else
        return _M_insert_(__after._M_node, __after._M_node, __v);
    }
    else
      return _M_insert_unique(__v).first;
  }
  else

    return iterator(static_cast<_Link_type>(const_cast<_Base_ptr>(__position._M_node)));
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ typename _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::iterator
_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_insert_equal_(const_iterator __position, const _Val &__v)
{

  if (__position._M_node == _M_end())
  {
    if (size() > 0 && !_M_impl._M_key_compare(_KeyOfValue()(__v),
                                              _S_key(_M_rightmost())))
      return _M_insert_(0, _M_rightmost(), __v);
    else
      return _M_insert_equal(__v);
  }
  else if (!_M_impl._M_key_compare(_S_key(__position._M_node),
                                   _KeyOfValue()(__v)))
  {

    const_iterator __before = __position;
    if (__position._M_node == _M_leftmost())
      return _M_insert_(_M_leftmost(), _M_leftmost(), __v);
    else if (!_M_impl._M_key_compare(_KeyOfValue()(__v),
                                     _S_key((--__before)._M_node)))
    {
      if (_S_right(__before._M_node) == 0)
        return _M_insert_(0, __before._M_node, __v);
      else
        return _M_insert_(__position._M_node,
                          __position._M_node, __v);
    }
    else
      return _M_insert_equal(__v);
  }
  else
  {

    const_iterator __after = __position;
    if (__position._M_node == _M_rightmost())
      return _M_insert_(0, _M_rightmost(), __v);
    else if (!_M_impl._M_key_compare(_S_key((++__after)._M_node),
                                     _KeyOfValue()(__v)))
    {
      if (_S_right(__position._M_node) == 0)
        return _M_insert_(0, __position._M_node, __v);
      else
        return _M_insert_(__after._M_node, __after._M_node, __v);
    }
    else
      return _M_insert_equal_lower(__v);
  }
}

template <typename _Key, typename _Val, typename _KoV,
          typename _Cmp, typename _Alloc>
template <class _II>
__device__ void _Rb_tree<_Key, _Val, _KoV, _Cmp, _Alloc>::
    _M_insert_unique(_II __first, _II __last)
{
  for (; __first != __last; ++__first)
    _M_insert_unique_(end(), *__first);
}

template <typename _Key, typename _Val, typename _KoV,
          typename _Cmp, typename _Alloc>
template <class _II>
__device__ void _Rb_tree<_Key, _Val, _KoV, _Cmp, _Alloc>::
    _M_insert_equal(_II __first, _II __last)
{
  for (; __first != __last; ++__first)
    _M_insert_equal_(end(), *__first);
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ inline void
_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    erase(iterator __position)
{
  _Link_type __y =
      static_cast<_Link_type>(_Rb_tree_node_base::_Rb_tree_rebalance_for_erase(__position._M_node,
                                                                               this->_M_impl._M_header));
  _M_destroy_node(__y);
  --_M_impl._M_node_count;
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ inline void
_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    erase(const_iterator __position)
{
  _Link_type __y =
      static_cast<_Link_type>(_Rb_tree_node_base::_Rb_tree_rebalance_for_erase(const_cast<_Base_ptr>(__position._M_node),
                                                                               this->_M_impl._M_header));
  _M_destroy_node(__y);
  --_M_impl._M_node_count;
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ typename _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::size_type
_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    erase(const _Key &__x)
{
  pair<iterator, iterator> __p = equal_range(__x);
  const size_type __old_size = size();
  erase(__p.first, __p.second);
  return __old_size - size();
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ void _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    erase(iterator __first, iterator __last)
{
  if (__first == begin() && __last == end())
    clear();
  else
    while (__first != __last)
      erase(__first++);
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ void _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    erase(const_iterator __first, const_iterator __last)
{
  if (__first == begin() && __last == end())
    clear();
  else
    while (__first != __last)
      erase(__first++);
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ void _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    erase(const _Key *__first, const _Key *__last)
{
  while (__first != __last)
    erase(*__first++);
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ typename _Rb_tree<_Key, _Val, _KeyOfValue,
                             _Compare, _Alloc>::iterator
_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    find(const _Key &__k)
{
  iterator __j = _M_lower_bound(_M_begin(), _M_end(), __k);
  return (__j == end() || _M_impl._M_key_compare(__k,
                                                 _S_key(__j._M_node)))
             ? end()
             : __j;
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ typename _Rb_tree<_Key, _Val, _KeyOfValue,
                             _Compare, _Alloc>::const_iterator
_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    find(const _Key &__k) const
{
  const_iterator __j = _M_lower_bound(_M_begin(), _M_end(), __k);
  return (__j == end() || _M_impl._M_key_compare(__k,
                                                 _S_key(__j._M_node)))
             ? end()
             : __j;
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ typename _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::size_type
_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    count(const _Key &__k) const
{
  pair<const_iterator, const_iterator> __p = equal_range(__k);
  const size_type __n = aicuda::stl::distance(__p.first, __p.second);
  return __n;
}

template <typename _Key, typename _Val, typename _KeyOfValue,
          typename _Compare, typename _Alloc>
__device__ bool _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::__rb_verify() const
{
  if (_M_impl._M_node_count == 0 || begin() == end())
    return _M_impl._M_node_count == 0 && begin() == end() && this->_M_impl._M_header._M_left == _M_end() && this->_M_impl._M_header._M_right == _M_end();

  unsigned int __len = _Rb_tree_node_base::_Rb_tree_black_count(_M_leftmost(), _M_root());
  for (const_iterator __it = begin(); __it != end(); ++__it)
  {
    _Const_Link_type __x = static_cast<_Const_Link_type>(__it._M_node);
    _Const_Link_type __L = _S_left(__x);
    _Const_Link_type __R = _S_right(__x);

    if (__x->_M_color == _S_red)
      if ((__L && __L->_M_color == _S_red) || (__R && __R->_M_color == _S_red))
        return false;

    if (__L && _M_impl._M_key_compare(_S_key(__x), _S_key(__L)))
      return false;
    if (__R && _M_impl._M_key_compare(_S_key(__R), _S_key(__x)))
      return false;

    if (!__L && !__R && _Rb_tree_node_base::_Rb_tree_black_count(__x, _M_root()) != __len)
      return false;
  }

  if (_M_leftmost() != _Rb_tree_node_base::_S_minimum(_M_root()))
    return false;
  if (_M_rightmost() != _Rb_tree_node_base::_S_maximum(_M_root()))
    return false;
  return true;
}

} // namespace stl
} // namespace aicuda

#endif /* _AICUDA_STL_TREE_H_ */
