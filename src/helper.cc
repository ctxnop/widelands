/*
 * Copyright (C) 2002 by the Widelands Development Team
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "widelands.h"
#include "helper.h"

void zmem(void* mem, const ulong size) {
   uint i;
   for(i=0; i<size; i++)
      ((char*) mem)[i]='\0';
}

void to_lower(char* str) {
   uint i;
   for(i=0; i<strlen(str); i++) {
      if(str[i]>=65 && str[i]<=90)
         str[i]+=32;
   }
}

/** log(const char *, ...)
 *
 */
void log(const char *fmt, ...)
{
	char buf[2048];
	va_list va;
	
	va_start(va, fmt);
	vsnprintf(buf, sizeof(buf), fmt, va);
	va_end(va);
	
	// use iostreams instead of vprintf because other parts of Widelands use iostreams
	cout << buf;
	cout.flush();
}

/*
==============================================================================

class wexception implementation

==============================================================================
*/

wexception::wexception(const char *fmt, ...) throw()
{
	va_list va;
	
	va_start(va, fmt);
	vsnprintf(m_string, sizeof(m_string), fmt, va);
	va_end(va);
}

wexception::~wexception() throw()
{
}
	
const char *wexception::what() const throw()
{
	return m_string;
}

void myassert(int line, const char* file, const char* condt) throw(wexception)
{
	throw wexception("%s:%i: assertion \"%s\" failed!\n", file, line, condt);
}

