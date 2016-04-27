/**
 * Include this in your precompiled header
 */ 
#pragma once

// We enable a whole lot of library code once tstring is included
#ifndef TSTRING_INCLUDED
#define TSTRING_INCLUDED

#pragma warning (push)
#pragma warning (disable:4995)
#include <tchar.h>
#include <string>
#include <sstream>

#pragma warning (pop)

typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> >   tstring;

typedef std::basic_stringstream<TCHAR, std::char_traits<TCHAR>,   std::allocator<TCHAR> > tstringstream;

#endif // TSTRING_INCLUDED
