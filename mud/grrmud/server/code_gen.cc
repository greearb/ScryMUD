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

char* header =
"//
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

";

int core_dump(const char* msg) {
   cerr << "ERROR:  " << msg << endl;
   exit(1);
}

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
      aliases[idx] = tmp;
      idx++;
      dafile >> tmp;
   }

   if (idx > 1) {
      // last one is the help
      help = aliases[idx - 1];
      aliases[idx].Clear();
   }
   else if (idx == 1) {
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
   String buf(200);
   String tmp(200);
   String cmd_enum(1000);
   String cmd_instantiations(10000);
   String exe_cmds(5000);
   String cmd_array(1000);

   const char* usage = "code_gen [input_file_name] [out.cc] [out.h]\n";

   if (argc != 4) {
      cerr << usage << endl;
      cerr << "ERROR:  argc should be 4, but was: " << argc << endl;
      exit(1);
   }
   
   ifstream input_file(argv[1]);
   if (!input_file) {
      cerr << "ERROR:  could not open input_file." << endl;
      exit(1);
   }

   ofstream of_cc(argv[2]);
   if (!of_cc) {
      cerr << "ERROR:  could not open output .cc file." << endl;
      exit(1);
   }

   ofstream of_h(argv[3]);
   if (!of_h) {
      cerr << "ERROR:  could not open output .h file." << endl;
      exit(1);
   }

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

   exe_cmds.Append("
class ExeCmd {
public:
   virtual int execute(String& str1, String& str2, String& str3, String& str4,
                       String& str5, int i, int j, int k, int l, int m,
                       int& is_dead, critter& pc, critter* c_script_owner,
                       room* r_script_owner, String* cooked_strs,
                       int* cooked_ints) = 0; //pure virtual
};\n");

exe_cmds.Append("//Add a few #defines to cut down on the huge file size!\n\n");

exe_cmds.Append("
#define CODE_GEN_EXE_HEADER \
String& str1, String& str2, String& str3, String& str4, \
String& str5, int i, int j, int k, int l, int m, \
int& is_dead, critter& pc, critter* c_script_owner, \
room* r_script_owner, String* cooked_strs, int* cooked_ints
\n");

exe_cmds.Append("
#define CODE_GEN_VAR_TEST_IF \
1 || &str1 || &str2 || &str3 || &str4 || &str5 || i || j || k || l \
|| m || is_dead || &pc ||c_script_owner || r_script_owner \
|| cooked_strs || cooked_ints\n\n");


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
            Sprintf(tmp, "   ADD_NEW_CMD(new CmdSpecifier(\"%S\", %i,
                    ci_HELP_ONLY, \"%s\"));\n",
                    &(cmd_input.aliases[i]), 
                    cmd_input.aliases[i].Strlen(),
                    ((const char*)(cmd_input.help)) + 1); //move past *
            cmd_instantiations.Append(tmp);
         }
         else {
            Sprintf(tmp, "   ADD_NEW_CMD(new CmdSpecifier(\"%S\", %i,
                    ci_%S, \"%S\"));\n",
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
               done_exe_already = TRUE;
            }
            // Now, lets add the code that will place this in an array.

            Sprintf(tmp, "   exe_cmd_array[(int)(ci_%S)] = new ExeCmd_%S;\n",
                    &(cmd_input.aliases[0]), &(cmd_input.aliases[0]));
            cmd_array.Append(tmp);
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


}//main
