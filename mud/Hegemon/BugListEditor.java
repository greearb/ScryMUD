// $Id: BugListEditor.java,v 1.3 2001/03/31 07:05:58 greear Exp $
// $Revision: 1.3 $  $Author: greear $ $Date$

//
//Hegemon Client Code:  Java Client for ScryMUD Server Code
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
// To contact the Author, Ben Greear:  greearb@candelatech.com, (preferred)
//                                     bgreear@mayannetworks.com
//

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;


/**  Bug List (ScryMUD's) editor.
 */
class BugListEditor extends Frame {
   boolean is_idea;
   LabeledTextField room_num;
   LabeledTextField bug_num;
   LabeledTextField assigned_to;
   LabeledTextField created_by;
   LabeledTextField bug_title;
   LabeledTextField create_date;
   LabeledChoice bug_state;
   LabeledTextArea new_comment;
   Button reload_b;
   Button new_b;
   Button state_b;
   Button assign_b;

   Vector comment_array;

   Panel comment_panel;
   ScrollPane scroll_pane;

   GridBagLayout gridbag;
   GridBagConstraints c;

   BugComment bug_comment_cache;

   HegemonManager hm;
   int REM; //gridbag remainder


   public String getRoomNum() { return room_num.getText(); }
   public String getBugNumName() { return bug_num.getText(); }
   
   public BugListEditor(HegemonManager h) {
      super("Bug List Editor");
      hm = h;

      is_idea = false;
      comment_array = new Vector();

      bug_title = new LabeledTextField("Title", "", 70);
      bug_num = new LabeledTextField("Entry#", "", 8);
      bug_state = new LabeledChoice("State");
      assigned_to = new LabeledTextField("Assigned To", "", 10);
      created_by = new LabeledTextField("Creator", "", 10);
      create_date = new LabeledTextField("Create Date", "", 26);
      room_num = new LabeledTextField("Room#", "", 8);
      new_comment = new LabeledTextArea("Add New Comment to Bug", "", 5, 70);
      new_comment.setEnabled(false);

      bug_state.c.add("open");
      bug_state.c.add("assigned");
      bug_state.c.add("retest");
      bug_state.c.add("closed");

      reload_b = new Button("Refresh");
      new_b = new Button("Send Comment");
      state_b = new Button("Change State");
      assign_b = new Button("Re-Assign");
      comment_panel = new Panel();
      scroll_pane = new ScrollPane();

      Label cp_label = new Label("Comments and/or work-arounds for this Issue.");      


      reload_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            do_relist();
         }});

      new_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            doNewComment();
         }});

      state_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            doNewState();
         }});

      assign_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            doModifyAssignment();
         }});

      REM = GridBagConstraints.REMAINDER;
      gridbag = new GridBagLayout();
      c = new GridBagConstraints();

      setLayout (gridbag);
      comment_panel.setLayout(gridbag);

      c.fill = GridBagConstraints.HORIZONTAL;
      c.gridwidth = REM;

      // First, take care of the Comment panel.
      gridbag.setConstraints(cp_label, c);
      comment_panel.add(cp_label);
      scroll_pane.add(comment_panel);

      // Now, back to this' layout.
      gridbag.setConstraints(bug_title, c);
      add(bug_title);

      c.gridwidth = 1;

      gridbag.setConstraints(bug_num, c);
      add(bug_num);

      gridbag.setConstraints(create_date, c);
      add(create_date);

      gridbag.setConstraints(created_by, c);
      add(created_by);

      c.gridwidth = REM;
      gridbag.setConstraints(room_num, c);
      add(room_num);

      c.gridwidth = 1;
      gridbag.setConstraints(assigned_to, c);
      add(assigned_to);

      gridbag.setConstraints(bug_state, c);
      add(bug_state);

      gridbag.setConstraints(assign_b, c);
      add(assign_b);

      gridbag.setConstraints(state_b, c);
      add(state_b);

      gridbag.setConstraints(new_b, c);
      add(new_b);

      c.gridwidth = REM;
      gridbag.setConstraints(reload_b, c);
      add(reload_b);

      gridbag.setConstraints(new_comment, c);
      add(new_comment);

      c.fill = GridBagConstraints.BOTH;
      c.weighty = 1;
      gridbag.setConstraints(scroll_pane, c);
      add(scroll_pane);

      addWindowListener (new WindowAdapter () {
         public void windowClosing (WindowEvent e) {
            setVisible(false);
         }});

      pack();
   }//constructor


   void setBugEntry(String num, String state, String cr_date, String reporter,
                    String rm_num, String _assigned_to, String col_type) {
      bug_num.setText(num);
      bug_state.c.select(state);
      create_date.setText(cr_date);
      created_by.setText(reporter);
      room_num.setText(rm_num);
      assigned_to.setText(_assigned_to);
      
      if (col_type.equalsIgnoreCase("IDEAS")) {
         is_idea = true;
      }
      else {
         is_idea = false;
      }
   }//setBugEntry


   void signalEntryDone() {
      bug_state.setEnabled(true);
      new_comment.setEnabled(true);
   }


   void signalCommentDone() {
      Log.instance().dbg("BugListEditor::signalCommentDone");
      comment_array.addElement(bug_comment_cache);
      c.gridwidth = REM;
      gridbag.setConstraints(bug_comment_cache, c);
      comment_panel.add(bug_comment_cache);
      pack();
      bug_comment_cache = new BugComment("", "", "", hm);
   }

   void doNewComment() {
       try {
          if (new_comment.getText().length() < 10) {
             MessageDialog md = 
               new MessageDialog("INPUT ERROR",
                                 "You must enter at least 10 letters of comment,"
                                 + " preferably more!", "red", "black");
             return;
          }

          if (!is_idea) {
             hm.getSocketManager().write("buglist comment " + bug_num.getText()
                                         + "\n" + new_comment.getText() + "\n~\n");
          }//if
          else {
             hm.getSocketManager().write("idealist comment " + bug_num.getText()
                                         + "\n" + new_comment.getText() + "\n~\n");
          }//if
          new_comment.getTextArea().setText("");
       }//try
       catch (Exception e) {
          MessageDialog md = new MessageDialog("Network Error",
                                               "Could not add entry: "
                                               + e, "red", "black");
          e.printStackTrace();
       }//catch
       do_relist();
   }//doNewComment


   void doModifyAssignment() {
       try {
          if (assigned_to.getText().length() < 3) {
             MessageDialog md = 
               new MessageDialog("INPUT ERROR",
                                 "You must enter at least 3 letters for the name"
                                 + "of the new assignee.", "red", "black");
             return;
          }

          if (!is_idea) {
             hm.getSocketManager().write("buglist assign " + bug_num.getText()
                                         + " " + assigned_to.getText() + "\n");
          }//if
          else {
             hm.getSocketManager().write("idealist assign " + bug_num.getText()
                                         + " " + assigned_to.getText() + "\n");
          }//if
       }//try
       catch (Exception e) {
          MessageDialog md = new MessageDialog("Network Error",
                                               "Could not re-assign: "
                                               + e, "red", "black");
          e.printStackTrace();
       }//catch
       do_relist();
   }//doModifyAssignment


   void do_relist() {
      try {
         if (!is_idea) { //ie if a bug
            hm.getSocketManager().write("buglist stat " + bug_num.getText()
                                        + "\n");
         }
         else {
            hm.getSocketManager().write("idealist stat " + bug_num.getText()
                                        + "\n");
         }
      }//try
      catch (Exception e) {
         MessageDialog md = new MessageDialog("Network Error",
                                              "Could not refresh: "
                                              + e, "red", "black");
         e.printStackTrace();
      }//catch
   }//do_relist


   void doNewState() {
      try {
         if (!is_idea) { //ie if a bug
            hm.getSocketManager().write("buglist chstat " + bug_num.getText()
                                        + " " + bug_state.c.getSelectedItem()
                                        + "\n");
         }
         else {
            hm.getSocketManager().write("idealist stat " + bug_num.getText()
                                        + " " + bug_state.c.getSelectedItem()
                                        + "\n");
         }
      }//try
      catch (Exception e) {
         MessageDialog md = new MessageDialog("Network Error",
                                              "Could not Change the state: "
                                              + e, "red", "black");
         e.printStackTrace();
      }//catch
      do_relist();
   }//doNewState

   void newEntry(String _date, String _reporter) {
      Log.instance().dbg("BugListEditor::newEntry, date: " + _date + " reporter: "
                         + _reporter);
      bug_comment_cache = new BugComment(_reporter, _date, "", hm);
   }

   public void clear() {
      bug_title.clear();
      bug_num.clear();
      bug_state.setEnabled(false);
      assigned_to.clear();
      created_by.clear();
      create_date.clear();
      room_num.clear();
      new_comment.clear();
      new_comment.setEnabled(false);

      for (int i = 0; i<comment_array.size(); i++) {
         comment_panel.remove((BugComment)(comment_array.elementAt(i)));
      }//for

      comment_array = new Vector();
      pack();
   }//

   public void appendTitle(String txt) {
      bug_title.append(txt);
   }

   public void appendComment(String txt) {
      bug_comment_cache.comment.append(txt);
   }
   
}//BugListEditor


class BugComment extends Panel {
   protected TextField commenter;
   protected TextField date;
   protected TextArea comment;

   protected HegemonManager hm;

   public BugComment(String _commenter, String _date, String _comment,
                     HegemonManager h) {
      hm = h;

      commenter = new TextField(_commenter);
      date = new TextField(_date);
      comment = new TextArea(_comment, 3, 80, TextArea.SCROLLBARS_VERTICAL_ONLY);

      commenter.setEditable(false);
      date.setEditable(false);
      comment.setEditable(false);


      GridBagLayout gridbag = new GridBagLayout();
      GridBagConstraints c = new GridBagConstraints();
      int REM = GridBagConstraints.REMAINDER;
      setLayout (gridbag);

      c.fill = GridBagConstraints.HORIZONTAL;
      c.gridwidth = 1;

      gridbag.setConstraints(commenter, c);
      add(commenter);

      c.gridwidth = REM;
      gridbag.setConstraints(date, c);
      add(date);

      gridbag.setConstraints(comment, c);
      add(comment);

   }//constructor

}//ValueEntry

