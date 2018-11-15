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

#ifndef _AICUDA_STL_MEMORY_H_
#define _AICUDA_STL_MEMORY_H_ 1

namespace aicuda {
namespace stl {

class string_op {
 public:
  __device__ static inline void *memset(void *s, int c, size_t n) {
    unsigned char *p = (unsigned char *)s;
    for (size_t i = 0; i < n; ++i) {
      p[i] = c;
    }
    return s;
  }

  __device__ static inline void *memcpy(void *s1, const void *s2, size_t n) {
    unsigned char *p1 = (unsigned char *)s1;
    const unsigned char *p2 = (const unsigned char *)s2;

    for (size_t i = 0; i < n; ++i) {
      p1[i] = p2[i];
    }
    return s1;
  }

  __device__ static inline size_t strlen(const char *s) {
    size_t size = 0;
    while (*s++ != '\0') ++size;
    return size;
  }

  __device__ static inline int memcmp(const void *s1, const void *s2,
                                      size_t n) {
    const unsigned char *p1 = (const unsigned char *)s1;
    const unsigned char *p2 = (const unsigned char *)s2;

    for (size_t i = 0; i < n; ++i) {
      if (p1[i] != p2[i]) {
        return p1[i] - p2[i];
      }
    }

    return 0;
  }

  __device__ static inline void *memchr(const void *s1, int ch, size_t n) {
    char *src = (char *)s1;
    for (size_t i = 0; i < n; ++i) {
      if (src[i] == ch) {
        return src + i;
      }
    }
    return NULL;
  }

  __device__ static void *memmove(void *dst, const void *src, size_t count) {
    char *tmpdst = (char *)dst;
    char *tmpsrc = (char *)src;

    if (tmpdst <= tmpsrc || tmpdst >= tmpsrc + count) {
      while (count--) {
        *tmpdst++ = *tmpsrc++;
      }
    } else {
      tmpdst = tmpdst + count - 1;
      tmpsrc = tmpsrc + count - 1;
      while (count--) {
        *tmpdst-- = *tmpsrc--;
      }
    }

    return dst;
  }

  __device__ static inline void strlcpy(char *dest, const char *source,
                                        size_t max) {
    max = max == 0 ? 1 : max;
    const char *end = source + (max - 1);
    for (; source != end; ++source, ++dest) {
      *dest = *source;
      if (*source == '\0') return;
    }
    *dest = '\0';
  }

  __device__ static inline int strcmp(const char *left, const char *right) {
    while (*left != '\0' && *right != '\0') {
      if (*left != *right) return -1;

      ++left;
      ++right;
    }

    if (*left != *right) return -1;

    return 0;
  }
  

  __device__ static inline int isspace(int x) {
    if (x == ' ' || x == '\t' || x == '\n' || x == '\f' || x == '\b' ||
        x == '\r')
      return 1;
    else
      return 0;
  }
  __device__ static inline int isdigit(int x) {
    if (x <= '9' && x >= '0')
      return 1;
    else
      return 0;
  }
  __device__ static inline int atoi(const char *nptr) {
    int c;     /* current char */
    int total; /* current total */
    int sign;  /* if '-', then negative, otherwise positive */
    /* skip whitespace */
    while (isspace((int)(unsigned char)*nptr)) ++nptr;
    c = (int)(unsigned char)*nptr++;
    sign = c; /* save sign indication */
    if (c == '-' || c == '+') c = (int)(unsigned char)*nptr++; /* skip sign */
    total = 0;
    while (isdigit(c)) {
      total = 10 * total + (c - '0');  /* accumulate digit */
      c = (int)(unsigned char)*nptr++; /* get next char */
    }
    if (sign == '-')
      return -total;
    else
      return total; /* return result, negated if necessary */
  }

  __device__ static long inline atol(const char *nptr) {
    int c;
    int sign;
    long total;
    while (isspace((int)(unsigned char)*nptr)) {
      ++nptr;
    }
    c = (int)(unsigned char)*nptr++;
    sign = c;
    if (c == '-' || c == '+') {
      c = (int)(unsigned char)*nptr++;
    }
    total = 0;
    while (isdigit(c)) {
      total = 10 * total + (c - '0');
      c = (int)(unsigned char)*nptr++;
    }
    return (sign == '-') ? -total : total;
  }

  __device__ static inline char *strstr(const char *str1, const char *str2) {
    int n;
    if (*str2) {
      while (*str1) {
        for (n = 0; *(str1 + n) == *(str2 + n); n++) {
          if (!*(str2 + n + 1)) {
            return (char *)str1;
          }
        }
        str1++;
      }
      return NULL;
    }
    return NULL;
  }
  __device__ static inline int tolower(int c) {
    if ((c >= 'A') && (c <= 'Z')) return c + ('a' - 'A');
    return c;
  }

  __device__ static inline int toupper(int c) {
    if ((c >= 'a') && (c <= 'z')) return c + ('A' - 'a');
    return c;
  }
  __device__ static inline char *strchr(const char *str, int ch) {
    while (*str && *str != (char)ch) str++;

    if (*str == (char)ch) return ((char *)str);
    return (NULL);
  }
  __device__  static inline bool is_character_in_string(char ch, char* str)
  {
      //assert(string != NULL);
      while(*str)
      {
          if(ch==*str++)
          {
              return true;
          }
      }
      return false;
  }
  __device__ static inline int strspn(const char *s,const char *accept)
  {  
      const char *p;
      const char *a;
      int count = 0;
      for(p = s; *p != '\0'; ++p)
      {
          for (a = accept; *a != '\0'; ++a)
          {
              if (*p == *a)
              {
                  break;
              }
          }
          if (*a == '\0')
          {
              return count;
          }
          ++count;
      }
      return count;
  }
  __device__ static inline char * strpbrk(const char * cs,const char * ct)
  {
      const char *sc1,*sc2;
      for( sc1 = cs; *sc1 != '\0'; ++sc1) 
      {
          for( sc2 = ct; *sc2 != '\0'; ++sc2) 
          {
              if (*sc1 == *sc2)
                  return (char *) sc1;
          }
      }
      return NULL;
  }


  __device__ static inline char *strtok_r(char *s, const char *delim, char **save_ptr) 
  {  
      char *token;
      if (s == NULL) s = *save_ptr;
      /* Scan leading delimiters.  */
      s += strspn(s, delim);
      if (*s == '\0')
          return NULL;
      
      /* Find the end of the token.  */
      token = s;
      s = strpbrk(token, delim);
      if (s == NULL)
          /* This token finishes the string.  */
          *save_ptr = strchr(token, '\0');
      else
      {
          /* Terminate the token and make *SAVE_PTR point past it.  */
          *s = '\0';  
          *save_ptr = s + 1;
      }  
      
      return token;
  }
};

}  // namespace stl
}  // namespace aicuda

#endif /* _AICUDA_STL_MEMORY_H_ */
