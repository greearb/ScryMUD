//
//ScryMUD Server Code
//Copyright (C) 1998  Ben Greear
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// To contact the Author, Ben Greear:  greear@cyberhighway.net, (preferred)
//                                     greearb@agcs.com
//

#include <iostream>
#include <stdlib.h>
#include <string2.h>

#include <sstream>

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif

void log(const char* msg) {
   cerr << "LOG:  " << msg << endl;
}

int core_dump(const char* str) {
   cerr << "Trying to core dump, str -:" << str << ":-" << endl;
   exit(1);
}

int do_shutdown;

const char* USAGE = "Usage:  help2html <targ_file> <files to add...>";


char* get_name(const char* fname) {
   static char buf[100];
   // Either:  IMM_cmd_name_n  or
   // cmd_name_n
   int len;
   
   if (fname[0] == 'I') {
      strcpy(buf, (fname + 4));
   }
   else {
      strcpy(buf, fname);
   }

   // Now get rid of the last two chars

   len = strlen(buf);
   buf[len - 2] = 0;

   return buf;
}//get_name


//gobbles up tags used in the mud markup language...
const char* read_tag(char* line, int offset, int& new_posn) {
   String retval(50);
   char ch;
   ch = line[offset];

   if (ch == '<') {
      new_posn = offset + 1;
      return "&lt;";
   }
   else { //wasn't an escaped <
      while (TRUE) {
         if (ch == '>') {
            new_posn = offset + 1;
            return "";
         }//if
         offset++;
         ch = line[offset];
      }//while
      cerr << "ERROR:  eol reached before closing '>' " << endl;
      return "";
   }//else
}//read_tag      
   
   
//do a line..deal with any mud markup language tags (ignore them)
String filter_line(char* line) {
   char chr;
   int len = strlen(line);
   String retval(100);
   int new_posn;
   const char* str;
   const char* ending_txt = NULL;
   
   if (strncasecmp("syntax", line, 6) == 0) {
      retval += "<font color=\"#ff0000\">";
      ending_txt = "</font>";
   }
   else if (strncasecmp("example", line, 7) == 0) {
      retval += "<font color=\"#00ff00\">";
      ending_txt = "</font>";
   }
   else if (strncasecmp("see also", line, 8) == 0) {
      retval += "<font color=\"#0000ff\"> See Also: ";
      // Assume the rest of the line is comma seperated names.
      char tmp[200];
      int strt = 8;
      if (line[8] == ':')
         strt = 9;

      my_sstream ss(line + strt);
      while (ss) {
         ss >> tmp;
         if (strlen(tmp) == 0) {
            break;
         }
         
         if (tmp[strlen(tmp) - 1] == ',') {
            tmp[strlen(tmp) - 1] = 0;
         }

         //Now, lets find the links.
         String fname(200);
         fname = tmp;
         fname.Strip();
         fname.Append("_1");
         ifstream is(fname);
         if (is) {
            retval += "<A href = \"#";
            retval += fname;
            retval += "\">";
            retval += tmp;
            retval += "</a> ";               
         }
         else {
            fname.Prepend("IMM_");
            ifstream is2(fname);
            if (is2) {
               retval += "<A href = \"#";
               retval += fname;
               retval += "\">";
               retval += tmp;
               retval += "</a> ";               
            }//if
            else {
               if (strcasecmp(tmp, "asave") == 0) {
                  retval += " <A href = \"#IMM_write_zone_1\">asave</a>";
               }
               else {
                  retval += " ";
                  retval += tmp;
                  retval += " ";
               }
            }//else
         }//else
      }//while we can get the next token
      retval += "</font>";
      return retval;
   }

         
   for (int i = 0; i<len; i++) {
      chr = line[i];

      if (chr == '<') {
         str = read_tag(line, i+1, new_posn);
         i = new_posn - 1;
         if (str) {
            retval += (str);
         }
      }
      else {
         retval += (chr);
      }
   }//for

   if (ending_txt)
      retval += ending_txt;

   return retval;
}//filter_line
   


void filter_file(ofstream& out_file, const char* in_file_name) {
   ifstream in_file(in_file_name);
   if (!in_file) {
      cerr << "ERROR:  could not open this file:  " << in_file_name << endl;
      return;
   }
   else {
      out_file << "<pre>" << endl;
      char buf[100];

      in_file.getline(buf, 99);
      while (in_file) {
         out_file << filter_line(buf) << endl;        
         in_file.getline(buf, 99);
      }//while
      out_file << "</pre><p>" << endl;
   }//else
}//filter_file


int main(int argc, char** argv) {
   
   if (argc < 3) {
      cout << USAGE << endl;
   }
   else {
      ofstream targ_file(argv[1]);
      if (!targ_file) {
	 cout << "Can't open targ file." << endl;
	 cout << USAGE << endl;
      }
      else {
	 //First, lets create the top portion of the HTML output file.
	 targ_file << 
	    "<html>\n"
	    "<title>ScryMUD Help</title>\n"
	    "<BODY TEXT=\"#3366AA\" BGCOLOR=\"#FFFFFF\" LINK=\"#AA7700\" VLINK=\"#AA7700\" ALINK=\"#FF0000\">\n"
	    "<P>\n"
	    "<h2><center> Help on commands for ScryMUD </center><P>\n"
	    << endl;
	 // Now, a list of all topics available...
	 targ_file << 
	    "<center>\n"
	    "<table width=80 border=3>\n"
	    "<caption align=top><strong>Help Topics</strong></caption><h4>" << endl;

         for (int i = 2; i<argc; i++) {
            //grab each name
            targ_file << "<tr>" << endl;

            targ_file << "<td nowrap>"
                      << "<A href = \"#" << argv[i] << "\"> "
                      << get_name(argv[i]) << "</a>" << endl
                      << "</td>" << endl;
            
            i++;
            if (i<argc) {
               targ_file << "<td nowrap> "
                         << "<A href = \"#" << argv[i] << "\"> "
                         << get_name(argv[i]) << "</a>" << endl
                         << "</td>" << endl;
            }

            i++;
            if (i<argc) {
               targ_file << "<td nowrap> "
                         << "<A href = \"#" << argv[i] << "\"> "
                         << get_name(argv[i]) << "</a>" << endl
                         << "</td>" << endl;
            }

            i++;
            if (i<argc) {
               targ_file << "<td nowrap> "
                         << "<A href = \"#" << argv[i] << "\"> "
                         << get_name(argv[i]) << "</a>" << endl
                         << "</td>" << endl;
            }

            i++;
            if (i<argc) {
               targ_file << "<td nowrap> "
                         << "<A href = \"#" << argv[i] << "\"> "
                         << get_name(argv[i]) << "</a>" << endl
                         << "</td>" << endl;
            }

            i++;
            if (i<argc) {
               targ_file << "<td nowrap> "
                         << "<A href = \"#" << argv[i] << "\"> "
                         << get_name(argv[i]) << "</a>" << endl
                         << "</td>" << endl;
            }
            
            targ_file << "</tr>" << endl;
         }//if

         targ_file << "</table></center><P>" << endl;

         // Now, lets add a filtered version of each file....
 
         targ_file << "\n<dl>\n";
         
         for (int i = 2; i<argc; i++) {

            targ_file << "<br><img src=../images/greenbar.jpg><P>"
                      << " <A name = \"" << argv[i] << "\">" << endl
                      << "<dt> Help Command:  " << get_name(argv[i])
                      << "</dt><dd>" << endl;
            
            filter_file(targ_file, argv[i]);
            targ_file << "</dd>" << endl;
         }

         // Now add closing html stuff...

         targ_file << "</body></html>" << endl << flush;
      }//else
   }//else      
}//main



         
