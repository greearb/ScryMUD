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


#include <stdio.h>
#include <string2.h>
#include <string.h>
#include "code_gen.h"
#include <fstream.h>
#include <PtrArray.h>

LogStream mudlog("/tmp/code_gen.log", 65535);
int __node_cnt;
int __list_cnt;
int __cell_cnt;

char* header =
"//
//ScryMUD Server Code
//Copyright (C) 1998  Ben Greear
//
// NOTE:  This code was autogenerated by the code_gen.cc program.  If you
// need to make modifications, do it there! --BLG
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
//
// NOTE:  This code was autogenerated by the code_gen.cc program.  If you
// need to make modifications, do it there! --BLG

";

int core_dump(const char* msg) {
   cerr << "ERROR:  " << msg << endl;
   exit(1);
}

LanguageEntry::LanguageEntry(const LanguageEntry& src)
      : enum_name(src.enum_name) {
   Cell<LVPair*> cll(src.vals);
   LVPair* ptr;
   while ((ptr = cll.next())) {
      vals.append(new LVPair(*ptr));
   }
}//copy constructor


void LanguageEntry::clear() {
   enum_name = "";
   clear_ptr_list(vals);
}//clear

int LanguageEntry::read(ifstream& dafile) {
   char junk[200];
   String tmp_lang(50);
   String tmp_val;

   // Grab comments, if there are any.
   while (TRUE) {
      dafile >> enum_name;
      if (!dafile)
         return -1; //EOF
      if (enum_name.charAt(0) == '#') {
         dafile.getline(junk, 200);
      }
      else {
         break;
      }
   }//while
   
   dafile >> tmp_lang; //Language type
   while (tmp_lang != "~") {

      if (!dafile)
         return -1; //EOF

      tmp_val.Clear();
      if (tmp_val.readToken('\"', dafile, TRUE) < 0) {
         cerr << "ERROR: trying to read token: -:" << tmp_val
              << ":-  Exiting." << endl;
         exit(2);
      }//if
      vals.append(new LVPair(tmp_lang, tmp_val));
      
      dafile >> tmp_lang;
   }//while
   return 0;
}//read


String LanguageEntry::getConstArray() const {
   String retval(500);
   String translations[NUM_LANGUAGES];
/*
enum LanguageE {
   English,
   Spanish,
   Portugues,
   LastLanguage
};
*/
   retval = "   {\n      ";
   int slen;

   // NOTE:  To add a new language, modify the while loop below
   // to contain the ones you want to support.  Then make additions
   // to the translation.spec file.
   Cell<LVPair*> cll(vals);
   LVPair* ptr;
   while ((ptr = cll.next())) {
      slen = ptr->lang.Strlen();
      if (strncasecmp(ptr->lang, "English", slen) == 0) {
         translations[0] = ptr->val;
      }
      else if (strncasecmp(ptr->lang, "Spanish", slen) == 0) {
         translations[1] = ptr->val;
      }
      else if (strncasecmp(ptr->lang, "Portugues", slen) == 0) {
         translations[2] = ptr->val;
      }
      else {
         cerr << "WARNING: Unknown language: -:" << ptr->lang << ":-" << endl;
      }
   }//while

   for (int i = 0; i<NUM_LANGUAGES; i++) {
      if (translations[i].Strlen()) {
         retval += translations[i];
         if ((i + 1) != NUM_LANGUAGES)
            retval += ",\n      ";
         else
            retval += "\n";
      }
      else {
         if ((i + 1) != NUM_LANGUAGES)
            retval += "NULL,\n      ";
         else
            retval += "NULL\n";
      }//else
   }//for

   retval += "   }";

   return retval;
}//getConstArray


void GenCmdInput::clear() {
   for (int i = 0; i<ALIASES_ARRAY_LEN; i++) {
      aliases[i].Clear();
   }
   help.Clear();
   cmd.Clear();
}

void GenCmdInput::print() { 
   cout << "ALIAS: ";
   for (int i = 0; i<ALIASES_ARRAY_LEN; i++) {
      if (aliases[i].Strlen()) {
         cout << "[" << aliases[i] << "] ";
      }
   }
   cout << "\nHELP: [" << help << "]\n";
   cout << "CMD: [" << cmd << "]\n";
}

int GenCmdInput::read(ifstream& dafile) {
   clear();

   char ch_buf[200];
   String tmp;
   int idx = 0;

   while (TRUE) {
      dafile >> tmp;
      if ((tmp.Strlen() == 0) || (tmp == "~")) {
         // first was a ~, so we are at EOF
         return -1;
      }

      if ((tmp == "#") || (tmp.charAt(0) == '#')) {
         dafile.getline(ch_buf, 199);
         continue;
      }
      else {
         break;
      }
   }//while, read and junk comments

   // no comments allowed within entries.
   while (tmp != "~") {
      //cout << "In while loop, tmp -:" << tmp << ":- idx: " << idx << endl;
      aliases[idx] = tmp;
      idx++;
      dafile >> tmp;
   }

   //cout << "Done with while loop, idx: " << idx << endl;
   if (idx > 1) {
      // last one is the help
      //cout << "Using last one for help.." << endl;
      help = aliases[idx - 1];
      aliases[idx - 1].Clear();
   }
   else if (idx == 1) {
      //cout << "Using first one for help." << endl;
      help = aliases[0]; //default to the first one is the help too
   }
   else {
      return -1; //all done, I spose!
   }

   // junk the first, it's the end of the line of aliases
   cmd.Getline(dafile, 200);

   cmd.Getline(dafile, 200);
   return 0;
}


int main(int argc, char** argv) {
   const char* usage = "code_gen -[CL] [input_file_name] [out.cc] [out.h]\n";

   if (argc != 5) {
      cerr << usage << endl;
      cerr << "ERROR:  argc should be 5, but was: " << argc << endl;
      exit(1);
   }
   
   ifstream input_file(argv[2]);
   if (!input_file) {
      cerr << "ERROR:  could not open input_file." << endl;
      exit(1);
   }

   ofstream of_cc(argv[3]);
   if (!of_cc) {
      cerr << "ERROR:  could not open output .cc file." << endl;
      exit(1);
   }

   ofstream of_h(argv[4]);
   if (!of_h) {
      cerr << "ERROR:  could not open output .h file." << endl;
      exit(1);
   }

   if (strcasecmp(argv[1], "-C") == 0) {
      // Then create some commands
      return code_gen_commands(input_file, of_cc, of_h);
   }
   else if (strcasecmp(argv[1], "-L") == 0) {
      // Create the language array and code
      return code_gen_language(input_file, of_cc, of_h);
   }
   else {
      cerr << usage << endl;
      exit(1);
   }
}//main


int code_gen_language(ifstream& input_file, ofstream& of_cc,
                      ofstream& of_h) {
   // First, lets parse the incomming file and make sure it's OK.
   PtrArray<LanguageEntry> language_entries;
   LanguageEntry le;

   // If there is a read or parse error, the program will exit
   // inside the le.read() call.
   int cnt = 0;
   while (le.read(input_file) >= 0) {
      cnt++;
      language_entries.appendShallowCopy(new LanguageEntry(le));
   }
      
   // If we got to here, then we were able to parse and read the file.

   //First, get the headers out of the way
   of_h << header;

   // NOTE:  If you change the language enum below, you will need to
   // modify the LanguageEntry code, especially the NUM_LANGUAGES #define
   // and the getConstArray() method. --BEN
   of_h << "
#ifndef __INCLUDE_AUTOGEN_LANGUAGE_H__
#define __INCLUDE_AUTOGEN_LANGUAGE_H__

#include <string2.h>

// I expect this may grow.
enum LanguageE {
   English,
   Spanish,
   Portugues,
   LastLanguage
};\n\n";

   // If the ENUM above changes, change this 3 to be equal to
   // the number of languages.
   of_h << "#define LS_PER_ENTRY 3  /* same as LastLanguage */" << endl;
   of_h << "#define LS_ENTRIES " << cnt 
        << " /* Number of Translation groupings */\n\n";
   of_h << "extern const char* language_strings[LS_ENTRIES][LS_PER_ENTRY];\n";

   of_h << "\nenum CSentryE {\n";

   for (int i = 0; i<cnt; i++) {
      of_h << "   " << language_entries[i]->getEnumName();
      if ((i + 1) != cnt)
         of_h << "," << endl;
      else
         of_h << endl;
   }

   of_h << "};//CSentryE enum\n\n";
   of_h << "
class CSHandler {
public:
   static const char* getString(CSentryE which_string, LanguageE language);
};


#endif\n";
   of_h << flush;

   // Done with .h file, lets start on the .cc file
   of_cc << header;
   of_cc << "

#include \"lang_strings.h\"

const char* language_strings[LS_ENTRIES][LS_PER_ENTRY] = {\n";

   for (int i = 0; i<cnt; i++) {
      of_cc << " /* " << language_entries[i]->getEnumName() << " */" << endl;
      of_cc << language_entries[i]->getConstArray();
      if ((i + 1) != cnt)
         of_cc << ",\n\n";
      else
         of_cc << endl;
   }//for
   
   of_cc << "}; //language_strings\n\n";

   of_cc << "
const char* CSHandler::getString(CSentryE which_string, LanguageE language) {
   const char* retval = language_strings[(int)(which_string)][(int)(language)];
   if (retval) {
      return retval;
   }
   else {
      // Default to English...
      // TODO:  Be smarter here, ie Portuguese defaults to Spanish??
      return language_strings[(int)(which_string)][0];
   }//else
}//getString\n\n";

   of_cc << flush;

   return 0;
}//code_gen_language


   
int code_gen_commands(ifstream& input_file, ofstream& of_cc,
                      ofstream& of_h) {
   String buf(200);
   String tmp(200);
   String cmd_enum(1000);
   String cmd_instantiations(10000);
   String exe_cmds(5000);
   String cmd_array(1000);

   of_h << header;
   of_h << "
#ifndef __INCLUDE_AUTOGEN_CMD_H__
#define __INCLUDE_AUTOGEN_CMD_H__

#include \"ar_skll.h\"
#include \"cr_skll.h\"
#include \"social2.h\"
#include \"ez_skll.h\"
#include \"dam_skll.h\"
#include \"misc.h\"
#include \"misc2.h\"
#include \"commands.h\"
#include \"command2.h\"
#include \"command3.h\"
#include \"command4.h\"
#include \"command5.h\"
#include \"spells.h\"
#include \"skills.h\"
#include \"olc.h\"
#include \"olc2.h\"
#include \"socials.h\"
#include \"classes.h\"
#include \"wep_skll.h\"
#include \"parse.h\"
#include \"script.h\"\n\n";

   of_cc << header;
   of_cc << "#include \"code_gen.h\"\n";
   of_cc << "#include \"parse.h\"\n";

   // add the starting stuff.
   cmd_enum.Append("enum CmdId {\n");
   cmd_instantiations.Append("#define ADD_NEW_CMD cmds_collection.addCmdSpecifierNoCreate

void initCmdsCollection() { \n");

   exe_cmds.Append("//Add a few #defines to cut down on the huge file size!\n\n");
   exe_cmds.Append("
#define CODE_GEN_EXE_HEADER \
String& str1, String& str2, String& str3, String& str4, \
String& str5, int i, int j, int k, int l, int m, int n,\
int& is_dead, critter& pc, critter* c_script_owner, \
room* r_script_owner, String* cooked_strs, int* cooked_ints, \
int do_sub, int sanity, int was_ordered
\n");

   // Hopefully, a smart compiler will get rid of this test.  It's
   // here to keep the compiler from complaining about un-used variables.
   // --BLG
   exe_cmds.Append("
#define CODE_GEN_VAR_TEST_IF \
1 || &str1 || &str2 || &str3 || &str4 || &str5 || i || j || k || l \
|| m || n || is_dead || &pc ||c_script_owner || r_script_owner \
|| cooked_strs || cooked_ints || do_sub || sanity || was_ordered\n\n");


   exe_cmds.Append("
class ExeCmd {
public:
   virtual int execute(CODE_GEN_EXE_HEADER) = 0; //pure virtual
};\n");


   cmd_array.Append("void initCmdsArray() {\n");

   GenCmdInput cmd_input;
   int cmd_count = 0;
   int done_exe_already = FALSE;
   while (input_file) {
      if (cmd_input.read(input_file) == -1) {
         // all done
         break;
      }

      // debugging
      //cmd_input.print();

      if (cmd_input.help.charAt(0) != '*') { // help only
         // create the commands, and the enum, and the ExeCmds
         Sprintf(tmp, "   ci_%S,\n", &(cmd_input.aliases[0]));
         cmd_enum.Append(tmp);
      }

      done_exe_already = FALSE;
      for (int i = 0; i<ALIASES_ARRAY_LEN; i++) {
         if (cmd_input.aliases[i].Strlen() == 0)
            break;

         if (cmd_input.help.charAt(0) == '*') { // help only
            Sprintf(tmp, "   ADD_NEW_CMD(new CmdSpecifier(\"%S\", %i, ci_HELP_ONLY, \"%s\"));\n",
                    &(cmd_input.aliases[i]), 
                    cmd_input.aliases[i].Strlen(),
                    ((const char*)(cmd_input.help)) + 1); //move past *
            cmd_instantiations.Append(tmp);
         }
         else {
            Sprintf(tmp, "   ADD_NEW_CMD(new CmdSpecifier(\"%S\", %i, ci_%S, \"%S\"));\n",
                    &(cmd_input.aliases[i]), cmd_input.aliases[i].Strlen(),
                    &(cmd_input.aliases[0]), &(cmd_input.help));
            cmd_instantiations.Append(tmp);

            if (!done_exe_already) {
               cmd_count++;
               Sprintf(tmp, "
class ExeCmd_%S : public ExeCmd {
public:
   int execute(CODE_GEN_EXE_HEADER) {
      if (CODE_GEN_VAR_TEST_IF) {
         %S
      } else return 0;
   }//execute
};//ExeCmd\n",
                       &(cmd_input.aliases[0]), &(cmd_input.cmd));

               exe_cmds.Append(tmp);

               // Now, lets add the code that will place this in an array.

               Sprintf(tmp, "   exe_cmd_array[(int)(ci_%S)] = new ExeCmd_%S;\n",
                       &(cmd_input.aliases[0]), &(cmd_input.aliases[0]));
               cmd_array.Append(tmp);
               
               done_exe_already = TRUE;
            }
         }
      }//for


   }//while

   // Now, have read everything in..lets put the code together.

   // Terminate the constructs.
   cmd_enum.Append("   ci_HELP_ONLY,\n");
   cmd_enum.Append("   ci_UNKNOWN,\n");
   cmd_enum.Append("   ci_MAX_VALUE\n};\n\n");
   cmd_instantiations.Append("}// initCmdsCollection\n\n");
   // exe_cmds should be fine
   cmd_array.Append("}//End of ExeCmd array assignment\n\n");

   // Lets do the .h file first.  Assume #includes already done.
   of_h << "class ExeCmd; //foward declaration\n\n";
   of_h << "\nextern ExeCmd* exe_cmd_array[" << cmd_count + 1 << "];\n\n";
   of_h << cmd_enum << endl;
   of_h << exe_cmds << endl;
   of_h << "\n\n#endif";

   // Now, the .cc file
   of_cc << "ExeCmd* exe_cmd_array[" << cmd_count + 1 << "];\n\n";
   of_cc << cmd_instantiations << endl;
   of_cc << cmd_array << endl;

   return 0;
}//code_gen_commands
