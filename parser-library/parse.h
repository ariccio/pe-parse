/*
The MIT License (MIT)

Copyright (c) 2013 Andrew Ruef

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef _PARSE_H
#define _PARSE_H
#include <string>
#include <boost/cstdint.hpp>

#include "nt-headers.h"

typedef boost::uint32_t RVA;
typedef boost::uint64_t VA;

struct buffer_detail;

typedef struct _bounded_buffer {
  boost::uint8_t  *buf;
  boost::uint32_t bufLen;
  bool            copy;
  buffer_detail   *detail;
} bounded_buffer;

struct resource {
  boost::uint32_t depth;
  std::string    type_str;
  std::string    name_str;
  std::string    lang_str;
  boost::uint32_t type;
  boost::uint32_t name;
  boost::uint32_t lang;
  boost::uint32_t codepage;
  bounded_buffer  *buf;
};

// http://msdn.microsoft.com/en-us/library/ms648009(v=vs.85).aspx
enum resource_type {
  RT_CURSOR       = 1,
  RT_BITMAP       = 2,
  RT_ICON         = 3,
  RT_MENU         = 4,
  RT_DIALOG       = 5,
  RT_STRING       = 6,
  RT_FONTDIR      = 7,
  RT_FONT         = 8,
  RT_ACCELERATOR  = 9,
  RT_RCDATA       = 10,
  RT_MESSAGETABLE = 11,
  RT_GROUP_CURSOR = 12, // MAKEINTRESOURCE((ULONG_PTR)(RT_CURSOR) + 11)
  RT_GROUP_ICON   = 14, // MAKEINTRESOURCE((ULONG_PTR)(RT_ICON) + 11)
  RT_VERSION      = 16,
  RT_DLGINCLUDE   = 17,
  RT_PLUGPLAY     = 19,
  RT_VXD          = 20,
  RT_ANICURSOR    = 21,
  RT_ANIICON      = 22,
  RT_HTML         = 23,
  RT_MANIFEST     = 24
};

bool readByte(bounded_buffer *b, boost::uint32_t offset, boost::uint8_t &out);
bool readWord(bounded_buffer *b, boost::uint32_t offset, boost::uint16_t &out);
bool readDword(bounded_buffer *b, boost::uint32_t offset, boost::uint32_t &out);

bounded_buffer *readFileToFileBuffer(const char *filePath);
bounded_buffer *splitBuffer(bounded_buffer *b, boost::uint32_t from, boost::uint32_t to);
void deleteBuffer(bounded_buffer *b);
uint64_t bufLen(bounded_buffer *b);

struct parsed_pe_internal;

typedef struct _pe_header {
  nt_header_32    nt;
} pe_header;

typedef struct _parsed_pe {
  bounded_buffer      *fileBuffer;
  parsed_pe_internal  *internal;
  pe_header           peHeader;
} parsed_pe;

//get a PE parse context from a file 
parsed_pe *ParsePEFromFile(const char *filePath);

//destruct a PE context
void DestructParsedPE(parsed_pe *pe);

//iterate over the resources
typedef int (*iterRsrc)(void *, resource);
void IterRsrc(parsed_pe *pe, iterRsrc cb, void *cbd);

//iterate over the imports by RVA and string 
typedef int (*iterVAStr)(void *, VA, std::string &, std::string &);
void IterImpVAString(parsed_pe *pe, iterVAStr cb, void *cbd);

//iterate over relocations in the PE file
typedef int (*iterReloc)(void *, VA, reloc_type);
void IterRelocs(parsed_pe *pe, iterReloc cb, void *cbd);

//iterate over the exports
typedef int (*iterExp)(void *, VA, std::string &, std::string &);
void IterExpVA(parsed_pe *pe, iterExp cb, void *cbd);

//iterate over sections
typedef int (*iterSec)(void *, VA secBase, std::string &, image_section_header, bounded_buffer *b);
void IterSec(parsed_pe *pe, iterSec cb, void *cbd);

//get byte at VA in PE
bool ReadByteAtVA(parsed_pe *pe, VA v, boost::uint8_t &b);

//get entry point into PE
bool GetEntryPoint(parsed_pe *pe, VA &v);

#endif
