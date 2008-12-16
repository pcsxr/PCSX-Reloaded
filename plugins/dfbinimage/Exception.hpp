/************************************************************************

Exception.hpp

Copyright (C) 2007 Virus
Copyright (C) 2002 mooby

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

************************************************************************/

#pragma warning(disable:4786)

#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

#include <FL/fl_ask.H>

// the exception data for the plugin.  stores the line number too
// for easy debugging
class Exception
{
public:
   Exception() : line(0) {}

   Exception(const std::string& str)
      : line(0) {error.push_back(str);}

   inline void setLine(const unsigned long l) {line = l;}
   inline void setFile(const std::string& str) {file = str;}
   inline void addText(const std::string& str) {error.push_back(str);}
   inline std::string text() {std::ostringstream str; str << *this;  return str.str();}

   inline friend std::ostream& operator<<(std::ostream& o, const Exception& m);

private:
   unsigned long line;
   std::string file;
   std::vector<std::string> error;
};

inline std::ostream& operator<<(std::ostream& o, const Exception& m)
{
   std::vector<std::string>::size_type index;
   for(index = 0; index < m.error.size(); index++)
      o << m.error[index]<< std::endl;
   o << "On line: " << m.line << std::endl
     << "In file: " << m.file << std::endl;
   return o;
}

#define THROW(e) e.setLine(__LINE__); e.setFile(__FILE__); moobyMessage(e.text().c_str()); throw(e);

#endif
