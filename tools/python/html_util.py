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

import sys, os, getopt, scrymud.util

def usage():
    sys.stderr.write(
    "Usage: %s --output-dir= --source-dir= --html-head= --html-foot=\n" %(
     sys.argv[1:1]) )
    sys.exit(-1)

class PageCollection(object):
    def __init__(self, header, footer, output_directory):
        reader = open(header,"r")
        self.header = reader.read()
        reader.close()
        reader = open(footer,"r")
        self.footer = reader.read()
        reader.close
        self.output_directory = output_directory
        self.__pagelist__ = []
        self.__pages__ = None
    def newPage(self):
        page = Page()
        self.__pagelist__.append(page)
        return(page)
    def writeIndex(self, filename):
        if ( self.__pages__ == None ): self.__mkIndex__()
        filename = self.output_directory + "/" + filename
        writer = open(filename,"w")
        writer.write(self.header)
        keys = self.__pages__.keys()
        keys.sort()
        writer.write("<div class=\"help_index_quicklinks\">")
        for letter in scrymud.util.alphabet:
            letter = letter.lower()
            if ( self.__pages__.has_key(letter) ):
                writer.write("<a href=\"#%s\">%s</a> " %(letter, letter))
            else:
                writer.write("%s " %(letter))
        for k in keys:
            writer.write("<div class=\"help_index_section\">\n<a name=\"%s\" /><div class=\"help_index_sortkey\">%s</div>\n" %(k,k))
            writer.write("<ul class=\"help_index\">\n")
            for page in self.__pages__[k]:
                writer.write("<li><a href=\"%s.html\">%s</a></li>\n" %(page.__filename__, page.title))
            writer.write("</ul></div>\n")
        writer.write(self.footer)
        writer.close()
    def writePages(self):
        for page in self.__pagelist__:
            filename = self.output_directory + "/" + page.__filename__ + ".html"
            writer = open(filename,"w")
            writer.write(self.header)
            writer.write(page.doc)
            writer.write(self.footer)
            writer.close()
    def __mkIndex__(self):
        if self.__pages__ == None: self.__pages__ = dict()
        self.__pagelist__.sort()
        for page in self.__pagelist__:
            if ( not self.__pages__.has_key(page.title[:1])): self.__pages__[page.title[:1]] = []
            self.__pages__[page.title[:1]].append(page)
                
    
class Page(object):
    def __init__(self):
        self.__title__="<no title>"
        self.__filename__ = "<no filename>"
        self.doc="<no document>"
    def __cmp__(self, other):
        if self.__title__ < other.__title__: return -1
        elif self.__title__ == other.__title__: return 0
        else: return 1
    def __getTitle__(self):
        return self.__title__
    def __setTitle__(self, val):
        self.__title__ = val.lower()
        self.__filename__ = self.__title__.replace(" ","_")
    title = property(__getTitle__, __setTitle__)



class cfg(object):

    _valid_long_args = [ "output-dir=",
                         "source-dir=",
                         "html-head=",
                         "html-foot=",
                       ]

    class cfgError(Exception):
        def __init__(self, value):
            self.value = value
        def __str__(self):
            return self.value

    def __init__(self, argv):
        self.output_dir=None
        self.source_dir=None
        self.html_head=None
        self.html_foot=None

        (found, extra) = getopt.getopt(argv, "", cfg._valid_long_args)
        for (name, value) in found:
            if ( name == '--source-dir' ):
                self.source_dir = value
            elif ( name == '--output-dir' ):
                self.output_dir = value
            elif ( name == '--html-head' ):
                self.html_head = value
            elif ( name == '--html-foot' ):
                self.html_foot = value

        self._validate()

    def _validate(self):
        if ( not self.output_dir ): raise cfg.cfgError, "output-dir is not set."
        if ( not self.source_dir ): raise cfg.cfgError, "source-dir is not set."
        if ( not self.html_head ): raise cfg.cfgError,  "html-head is not set."
        if ( not self.html_foot ): raise cfg.cfgError,  "html-foot is not set."

        for path in [ self.output_dir, self.source_dir ]:
            if ( not os.path.isdir(path) ): raise cfg.cfgError, "%s is not a directory." %(path)
        if ( not os.access(self.output_dir, os.W_OK) ): raise cfg.cfgError, "%s is not writeable." %(path)
        if ( not os.access(self.source_dir, os.R_OK) ): raise cfg.cfgError, "%s is not readable." %(path)

        for path in [ self.html_head, self.html_foot ]:
            if ( not os.path.isfile(path) ): raise cfg.cfgError, "%s is not a file." %(path)
            if ( not os.access(path, os.R_OK) ): raise cfg.cfgError, "%s is not readable." %(path)

        return
