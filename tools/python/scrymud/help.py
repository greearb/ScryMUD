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

"""
Part of the ScryMUD collection. Parses and processes help files.
"""

import sys, os, errno, re

class helpLibrary(object):
    """
    This class reads all ScryMUD help files in and parses them.
    """

    def __init__(self, help_path):
        """
        help_path should contain the path to the ScryMUD help files
        """
        self.path = help_path
        tmp = os.listdir(help_path)
        self.docs = []
        for f in tmp:
            if ( os.path.isfile(self.path+"/"+f) ):
                newDoc = helpDoc(self.path+"/"+f)
                self.docs.append(newDoc)

    def by_name(self, name):
        """
        Returns a document object by name
        """
        for doc in self.docs:
            if ( doc.name.lower() == name.lower() ): return doc
        return None

    def all(self, sort=False):
        """
        Returns an optionally sorted list of all help documents
        """
        if ( sort ):
            self.docs.sort()
        return self.docs

    def imm_list(self, sort=False):
        """
        Returns an optionally sorted list of all IMM help documents
        """
        ret_val = []
        for doc in self.docs:
            if ( doc.imm ): ret_val.append(doc)
        if ( sort ): ret_val.sort()
        return ret_val

    def player_list(self, sort=False):
        """
        Returns an optionally sorted list of all player help documents
        """
        ret_val = []
        for doc in self.docs:
            if ( not doc.imm ): ret_val.append(doc)
        if ( sort ): ret_val.sort()
        return ret_val


class helpDoc(object):
    """
    This class is used to store a broken-out parsed copy of help documents
    """

    _re_imm = re.compile(r'IMM_(.*?)_1$')
    _re_user = re.compile(r'([^/]*)_1$')
    _re_tags = re.compile(r'<.*?>', re.MULTILINE|re.DOTALL)
    _re_special = re.compile(r'(^syntax:|^example:|^see also:)(.*)', re.IGNORECASE)

    def __cmp__(self, other): 
        if self.name < other.name: return -1
        elif self.name == other.name: return 0
        else: return 1

    def __init__(self, filename):
        self.imm  = False
        self.name = ""
        self.syntax = ""
        self.example = ""
        self.body = ""
        self.see_also = []

        if ( helpDoc._re_imm.search(filename) ):
            self.imm = True
            m = helpDoc._re_imm.search(filename)
            self.name = m.group(1)
        elif ( helpDoc._re_user.search(filename) ):
            m = helpDoc._re_user.search(filename)
            self.imm = False
            self.name = m.group(1)

        self._readFile(filename)
        self.see_also.sort()

    def _readFile(self, filename):
        f = open(filename, "r")
        buf = f.read()
        f.close()
        buf = helpDoc._re_tags.sub('', buf)
        lines = buf.splitlines()
        for line in lines:
            m  = helpDoc._re_special.match(line)
            if ( m ):
                type = m.group(1).lower()
                if ( type == 'syntax:' ):
                    self.syntax += m.group(2).lstrip()
                elif ( type == 'example:' ):
                    self.example += m.group(2).lstrip()
                elif ( type == 'see also:' ):
                    new_refs = m.group(2).strip().replace(',','').split(' ')
                    for new_ref in new_refs:
                        self.see_also.append(new_ref.strip())
                else:
                    raise(ValueError, "found an unexpected special-type")
            else:
                self.body += line + "\n"

if ( __name__ == '__main__'):
    sys.exit(0)
