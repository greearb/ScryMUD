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

from util import nilListParse, termedRead
import re

class SkillSpell(object):
    __re_comment = re.compile(r'#.*$')
    def __init__(self):
        self.ss_num = -1
        self.name = "<no name>"
        self.min_level = -1
        self.difficulty = -1
        self.mana_cost = -1
        self.scroll_num = -1
        self.prereqs = None
        self.enables = None
        self.restrictions = None
        self.description = "<no description>"
        
    def read(self, f):
        self.ss_num = int(f.readline().rstrip())
        self.name = f.readline().rstrip()
        buf = SkillSpell.__re_comment.sub('',f.readline().rstrip())
        
        [ self.min_level,
          self.difficulty,
          self.mana_cost,
          self.scroll_num ] = [ int(x) for x in buf.split() ]
        
        self.prereqs = nilListParse(f.readline().rstrip())
        self.enables = nilListParse(f.readline().rstrip())
        self.restrictions = nilListParse(f.readline().rstrip())
        f.readline()

class SSCollection(object):
    def __init__(self, ss_path):
        '''SSCollection(ss_path)
        ss_path should point to the directory containing the SKILLS_SPELLS
        and SS_DESC files.'''
        self.__sslist__ = []
        self.__ssfile__ = "%s/%s" %(ss_path, "SKILLS_SPELLS")
        self.__ssdesc__ = "%s/%s" %(ss_path, "SS_DESCS")
        self.__sorted__ = False

    def __iter__(self):
        if ( not self.__sorted__ ):
            self.__sslist__.sort()
            self.__sorted__ = True
        for s in self.__sslist__: yield s
        
    def read(self):    
        ssfile = open(self.__ssfile__,"r")
        while ( True ):
            k = ssfile.readline().rstrip()
            if ( k[:2] == "-1" ): break
            s = SkillSpell()
            s.read(ssfile)
            self.__sslist__.append(s)
        ssfile.close()
        
        ssfile = open(self.__ssdesc__,"r")
        while ( True ):
            name = ssfile.readline()
            if ( name == '\n' ): continue
            if ( name == '' ): break
            name = name.rstrip()
            desc = termedRead(ssfile)
            s = self.getName(name)
            s.description = desc
        ssfile.close()
        
    def getNum(self, skill_num):
        '''Fetches a skill object by number'''
        for s in self.__sslist__:
            if ( s.ss_num == skill_num ): return(s)
        return(None)
        
    def getName(self, skill_name):
        '''Fetches a skill object by name'''
        for s in self.__sslist__:
            if ( s.name.lower() == skill_name.lower() ): return(s)
        return(None)

if ( __name__ == '__main__' ):
    mySS = SSCollection("/home/eroper/devel/ScryMUD/mud/grrmud/World")
    mySS.read()
    S = mySS.getNum(2)
    if ( S.name.lower() != "block" ):
        print "SSCollection.getNum failed."

    S = mySS.getName("Block")
    if ( S.ss_num != 2 ):
        print "SSCollection.getName failed."
