// $Id$
// $Revision$  $Author$ $Date$

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


import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;

/**
 * GUI JFrame for managing Actions.
 */
class ActionManager extends JFrame implements ActionListener {
    HegemonManager hegManager;
    List actions;          //TODO Pass Vector (see below) to this. Migrate to JList
    Action[] action_list;    //TODO migrate to Vector
    boolean actions_enabled;
    //TODO "anchored" means begining of line. Make that clear.
    
    final static String SENTINEL = "~";
    
    /**
     * Create an instance of ActionFrame.
     *
     * @param hm HegemonManager class reference to be associated with ActionFrame
     */
    public ActionManager(HegemonManager hm) {
        super("Actions Manager");
        actions = new List();
        hegManager = hm;
        
        //Attempt to read in data:
        try {
            readObject();
        } catch (Exception e) {
            Log.instance().err("ActionFrame():  " + e);
            
            // Poke in some default values...
            actions.add("^Enter thy name: | admin");
            actions.add("^Enter your password: | admin");
        }
        
        actions.addActionListener(this);
        
        //Disabled by default:
        actions_enabled = false;
        action_list = updateActions();
        add(actions, "North");
        add(new AMButtons(this), "South");
        pack();
    }//constructor
    
    public final void setActionsEnabled(boolean ae) {
        actions_enabled = ae;
    }
    
    public final boolean getActionsEnabled() {
        return actions_enabled;
    }
    
    public void addAction(Action act) {
        actions.add(act.toString());
        try {
            writeObject();
        } catch (IOException e) {
            Log.instance().err("addAction  " + e);
        }
        //Log.trc("updating action_list");
        action_list = updateActions();
    }
    
    public void addAction(String glom) {
        actions.add(glom);
        try {
            writeObject();
        } catch (IOException e) {
            Log.instance().err("addAction  " + e);
        }
        //Log.it("updating action_list");
        action_list = updateActions();
    }
    
    public void createAction() {
        ActionDialog hd = new ActionDialog(this, new Action());
        hd.show();
    }
    
    public Action[] getActions() {
        return action_list;
    }
    
    private Action[] updateActions() {
        String[] items = actions.getItems();
        int len = items.length;
        Action retval[] = new Action[len];
        
        for (int i = 0; i<len; i++) {
            //Log.dbg("Adding action:  " + items[i]);
            retval[i] = new Action(items[i]);
        }
        return retval;
    }//getActions
    
    public Action getAction(String trigger, String response) {
        String[] items = actions.getItems();
        Action a;
        int len = items.length;
        
        for (int i = 0; i<len; i++) {
            a = new Action(items[i]);
            if (trigger != null) {
                if ((a.getTrigger()).equals(trigger))
                    return a;
            } else if (response != null) {
                if ((a.getAction()).equals(response))
                    return a;
            } else
                return null;
        }
        return null;
    }//getAction

    public void do_delete() {
        String str;
        if ((str = actions.getSelectedItem()) != null) {
            actions.remove(str);
            try {
                writeObject();
                action_list = updateActions();
            } catch (Exception e) {
                Log.instance().err("do_delete:  " + e);
            }
        } else {
            MessageDialog md =
                    new MessageDialog("Connection Manager",
                    "You need to select a connection to delete.",
                    "red", "black");
        }
    }//do_delete
    
    public void do_new() {
        createAction();
    }
    
    public void do_close() {
        setVisible(false);
    }
    
    public void do_edit() {
        String str;
        if ((str = actions.getSelectedItem()) != null) {
            actions.remove(str); //editing will re-add it
            ActionDialog hd = new ActionDialog(this, new Action(str));
            hd.show();
        } else {
            MessageDialog md =
                    new MessageDialog("Action Manager",
                    "You need to select an action to edit.",
                    "red", "black");
        }
    }//do_edit
    
    public void actionPerformed(ActionEvent e) {
        if (e.getSource() == actions) {
            do_edit();
        }
    }
    
    /** Writes it all to disk, for later retrieval by readObject() */
    public void writeObject() throws IOException {
        if (hegManager.IS_APPLET)
            return;
        
        String fl = hegManager.getProperties().getProperty("ActionFile");
        //      Log.it("fl:  " + fl);
        BOStream ostream = new BOStream(new FileOutputStream(fl));
        String[] entries = actions.getItems();
        int len = entries.length;
        try {
            for (int i = 0; i<len; i++) {
                //Log.dbg("Writing:  " + entries[i]);
                ostream.write(entries[i]);
                ostream.write("\n~\n");
            }//for
        }//try
        catch (Exception e) {
            MessageDialog md =
                    new MessageDialog("Write Failure",
                    "Failed to write actions to disk: " +
                    e, "red", "black");
        }//catch
        
        ostream.flush();
    }//writeObject
    
    /**
     * Method to read data for ActionFrame. Will not attempt to read
     * if Hegemon is an applet.
     *
     * @throws IOException For failure to read inputer stream
     */
    public void readObject() throws IOException {
        if (hegManager.IS_APPLET)
            return;
        
        BIStream istream =
                new BIStream(new FileInputStream(hegManager.getProperties().getProperty("ActionFile")));
        String str = null;
        String result = null;
        StringBuffer action = new StringBuffer(200);
        
        try {
            while ((str = istream.readLine()) != null) {
                //If String read in was special ("~" for end of string)
                //Then add full string to the action list
                if ((str.trim()).equals(SENTINEL)) {
                    actions.add(result.trim());
                    result = null;
                } else { //Otherwise append the string to previous input
                    result += str + " ";
                }
            }//while
        }//try
        catch (IOException e) {
            Log.instance().err("in ActionFrame.readObject():  " + e);
        }
    }//readObject
}//ActionFrame

/**
 * Class to hold and handle ActionManager's buttons and operations.
 */
class AMButtons extends JPanel {
    ActionManager parent;
    JButton doneAction, editAction, newAction, deleteAction, enableAction;
    
    /**
     * Create new instance of AMButtons for ActionManager.
     *
     * @param par ActionManager parent that created this AFButtons
     */
    public AMButtons(ActionManager par) {
        super();
        parent = par;
        
        /* Instantiate buttons */
        //TODO Make buttons enabled/disabled sensibly (i.e. no action chosen for edit, etc.)
        doneAction = new JButton("Done");
        editAction = new JButton("Edit Action");
        newAction = new JButton("New Action");
        deleteAction = new JButton("Delete Action");
        enableAction = new JButton();
        if (parent.actions_enabled) enableAction.setText("Disable Actions");
        else enableAction.setText("Enable Actions");
        
        /* Add buttons to layout */
        setLayout(new FlowLayout());
        add(enableAction);
        add(newAction);
        add(editAction);
        add(deleteAction);
        add(doneAction);
    }//AFButtons constructor
    
    /**
     * Method to handle Action JButton events.
     *
     * @param e ActionEvent fired by button press
     */
    public void actionPerformed(ActionEvent e) {
        if (e.getSource() == newAction) {
            parent.do_new();
        } else if (e.getSource() == editAction) {
            parent.do_edit();
        } else if (e.getSource() == deleteAction) {
            parent.do_delete();
        } else if (e.getSource() == doneAction) {
            parent.do_close();
        } else if (e.getSource() == enableAction) {
            //TODO Find a better way to repesent actions being enabled/disabled.
            if (parent.actions_enabled) { //Actions are enabled, should be disabled
                parent.setActionsEnabled(false);
                enableAction.setText("Enable Actions");
            } else { //Actions are not yet enabled (should be)
                parent.setActionsEnabled(true);
                enableAction.setText("Disable Actions");
            }
        }
    }//actionPerformed
}//AFButtons
