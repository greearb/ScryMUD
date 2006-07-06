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
"""Classes for handling ScryMUD BC_BUGS and BC_IDEAS files."""

import sys, re, time
import util

class bugList(object):
    """Instantiate with a pathname to a ScryMUD BUG/IDEA file and it will
       populate an internal list which can be iterated over.
    """
    def __init__(self, fname):
        self._List = list()
        bugFile = file(fname, "r")

        # skip version and bug count
        bugFile.readline()
        bugFile.readline()

        while ( True ):
            newBug = aBug().fromFile(bugFile)
            if ( newBug == None ): break
            self._List.append(newBug)

        bugFile.close()

    def __iter__(self):
        for bug in self._List:
            yield bug

class aBug(object):
    """
    This class represents a bug entry from the BC_BUGS file.
    ScryMUD bug-states are:
        0: open
        1: assigned
        2: retest
        3: closed
        4: deferred
        5: all
        6: none
        7: high_state
    I have no idea what states 5-7 mean.
    """

    # Various regexes used to parse the file / modify input.
    _begTok     = re.compile(r"^(\d+) (\d+) (\d+) bug_num, room_num, state")
    _quoteTok   = re.compile(r"\"")

    def __init__(self):
        self.bugNum      = -1
        self.roomNum     = -1
        self.date        = ""
        self.summary     = ""
        self.desc        = ""
        self.reporter    = ""
        self.assignee    = ""
        self.comments    = []
        self.state       = -1

    def as_csv(self):
        """
        This function will format a string for CSV output.
           bug_number, creation_date, summary, description, room_num, status,
           resolution, last_modified
        """
        quoted_summary = "\"%s\"" %( aBug._quoteTok.sub("\"\"", self.summary, 0) )
        quoted_desc = "\"%s\"" %( aBug._quoteTok.sub("\"\"", self.desc, 0) )
        return "%d,%s,%s,%s,%d,%d,%d,%s" % (
                                                self.bugNum,
                                                self.date,
                                                quoted_summary,
                                                quoted_desc,
                                                self.roomNum,
                                                self.state,
                                                self.getResolution(),
                                                self.getLastUpdate() )

    def getResolved(self):
        """
        Return the guestimated date of closure, or none.
        """
        if ( self.state == 2 or self.state == 3 ):
            return self.getLastUpdate()
        return None

    def getResolution(self):
        """Return a 0, Unresolved; 1, Resolved field."""
        if ( self.state == 2 or self.state == 3 ):
            return 1
        else:
            return 0

    def getLastUpdate(self):
        """
        Figure out and return the date string for the last update.
        """
        last_update = self.date
        for cmt in self.comments:
            if ( cmt.date ):
                last_update = cmt.date
        return last_update

    def mkDescription(self):
        """
        Builds the description entry from the summary + comments. Also
        includes some information about the people that created this issue
        in-game.
        """
        self.desc = "----[Reported by %s]----\n" %(self.reporter)
        self.desc += self.summary
        for cmt in self.comments:
            self.desc += "\n\n----[Update by %s on %s]----\n%s\n" %(
                          cmt.reporter, cmt.date, cmt.report)
        
    def fromFile(self, f):
        """
        "f" is a file object from which to read a bug-entry.
        """
        tStr = f.readline()
        if ( tStr == "-1" ):
            return None
        mObj = aBug._begTok.match(tStr)
        if ( mObj ):
            self.bugNum = int(mObj.group(1))
            self.roomNum = int(mObj.group(2))
            self.state = int(mObj.group(3))
            f.readline() # skip the bitfield... why is there a bitfield?
            self.date = time.strptime(f.readline().rstrip(), "%a %b %d %H:%M:%S %Y")
            self.reporter = f.readline().rstrip()
            self.asignee = f.readline().rstrip()
            self.summary = util.termedRead(f)
            while ( True ):
                tCmt = aComment().fromFile(f)
                if ( not tCmt ): break
                self.comments.append(tCmt)
            f.readline() # empty line between bugs
            self.mkDescription()
            return self

class aComment(object):
    def __init__(self):
        self.date=""
        self.reporter=""
        self.report=""

    def fromFile(self, f):
        sentinel = f.readline().rstrip()
        if ( sentinel == "-1" ): return(None)
        self.date = time.strptime(util.termedRead(f).strip(), "%a %b %d %H:%M:%S %Y")
        # now, for some really bizarre reason, there can be a _TON_ of
        # whitespace before the date.
        self.reporter = f.readline().rstrip()
        self.report = util.termedRead(f)
        return(self)
