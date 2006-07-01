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

#
# At some point I'd really like to clean up the HTML output code. it's pretty
# nasty. --eroper
#

""" Converts ScryMUD help files to html """

import sys, os, re, getopt
import scrymud.help as help
import scrymud.util 

def usage():
   sys.stderr.write(
   "Usage: %s --output-dir= --source-dir= --html-head= --html-foot=\n" %(
    sys.argv[1:1]) )
   sys.exit(-1)

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

def toHtml(doc):   

   def para(m): return "<p>%s</p>"%(m.group(1))

   ret_val = ""
   ret_val += '<div class="help_name">'
   ret_val += doc.name
   ret_val += '</div><div class="help_syntax">'
   ret_val += doc.syntax
   ret_val += '</div><div class="help_example">'
   ret_val += doc.example
   ret_val += '</div><div class="help_content">'


   for p in scrymud.util.paragraphs(doc.body):
      ret_val += "<p>%s</p>" %(p)
   ret_val += '</div><div id="see_also"'

   for link in doc.see_also:
      ret_val += "<a href=\"%s.html\">%s</a> " %(link, link)

   ret_val += '</div>'

   return(ret_val)


def idxHtml(idx):
   ret_val = ""
   keys = idx.keys()
   keys.sort()
   ret_val += ("<div class=\"help_index_quicklinks\">")
   for letter in scrymud.util.alphabet:
      if ( idx.has_key(letter) ): ret_val += ("<a href=\"#%s\">%s</a> " %(letter,letter))
      else: ret_val += ("%s " %(letter))
   for k in keys:
      ret_val += ("<div class=\"help_index_section\">\n<a name=\"%s\" /><div class=\"help_index_sortkey\">%s</div>\n" %(k,k))
      ret_val += ("<ul class=\"help_index\">\n")
      for entry in idx[k]:
	 ret_val += ("<li><a href=\"%s.html\">%s</a></li>\n" %(entry,entry))
      ret_val += ("</ul></div>\n")
   return(ret_val)

if ( __name__ == '__main__' ):
   try:
      config = cfg(sys.argv[1:])
   except cfg.cfgError, e:
      sys.stderr.write("Configuration error: %s\n" %(e))
      usage()

   try:
      os.mkdir("%s/imm" %(config.output_dir))
   except OSError, e:
      # errno 17 == E_EXISTS, at some point I should figure out if there is a
      # constant definition for this.
      if ( e.errno == 17 ): pass
      else: raise

   f = open(config.html_head, 'r')
   hh = f.read()
   f.close
   f = open(config.html_foot, 'r')
   hf = f.read()
   f.close

   idx = dict()
   imm_idx = dict()

   lib = help.helpLibrary(config.source_dir)
   for doc in lib.all(sort=True):

      if ( doc.imm ):
	 f = open("%s/imm/%s.html" %(config.output_dir, doc.name), 'w')
	 try: imm_idx[doc.name[:1].upper()].append(doc.name)
	 except KeyError, e: imm_idx[doc.name[:1].upper()] = [doc.name]
	 
      else:
	 f = open("%s/%s.html" %(config.output_dir, doc.name), 'w')
	 try: idx[doc.name[:1].upper()].append(doc.name)
	 except KeyError, e: idx[doc.name[:1].upper()] = [doc.name]

      f.write(hh)
      f.write(toHtml(doc))
      f.write(hf)
      f.close()

   f = open("%s/index.html" %(config.output_dir),"w")
   f.write(hh)
   f.write(idxHtml(idx))
   f.write(hf)
   f.close()

   f = open("%s/imm/index.html" %(config.output_dir),"w")
   f.write(hh)
   f.write(idxHtml(imm_idx))
   f.write(hf)
   f.close()


