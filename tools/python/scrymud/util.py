#!/usr/bin/env python
# $Id$
# Copyright (C) 2006, Edward Roper <edro+scrymud@wanfear.net>
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
"""Various small utility functions"""

def paragraphs(str, seperator=None):
   """Can be used as an iterator, returning one paragraph at a time from a body
      of text.
   """
   if not callable(seperator):
      if ( seperator != None ): raise TypeError, "seperator must return bool"
      def seperator(line): return line == '\n'
   paragraph = []
   for line in str.splitlines(True):
      if seperator(line):
         if paragraph:
            yield "".join(paragraph)
            paragraph = [];
      else:
         paragraph.append(line)
   if paragraph: yield "".join(paragraph)

# This exists because I am too brain dead to come up with a better way
# to build help indexes. --eroper
alphabet = [ "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
	     "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" ]
