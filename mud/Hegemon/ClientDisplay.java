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

import java.awt.event.*;
import java.awt.*;
import java.util.*;
import java.net.*;
import javax.swing.*;

/**  This is our main GUI, it holds all the rest, including the
 * HegemonDisplay, which draws the stuff coming from the server. */
class ClientDisplay extends JFrame implements ActionListener, WindowListener {
    boolean first_time_help = true;
    HegemonManager hegManager; //keep track of all our different objects
    HelpFrame helpFrame;
    ColorChoicesFrame colorChoicesFrame;
    LogFrame logFrame;
    
    OlEditor olEditor; //for quick description changes
    PathCellEditor pathCellEditor;
    MobScriptEditor mobScriptEditor;
    KeywordEditor keywordEditor;
    
    CommandHistory commandHistory;
    
    //TODO Make actionPerfomed check for labels so these don't have to be global?
    //TODO Spin menus off into their own class like AMButtons, etc.
    JMenuBar menubar;
    JMenu fileMenu, connectionsMenu, olcMenu, controlsMenu, helpMenu;
    JMenuItem quit_action, logging_action, color_view_mi, color_choices_mi,
            action_edit_mi, alias_edit_mi, cmd_history_mi;
    
    HegemonDisplay output_field;
    InputArea inputArea;
    
    JMenuItem help_about_mi, help_view_mi;
    JMenuItem alias_turn_off_mi, alias_turn_on_mi;
    JMenuItem open_connect_mi, select_connect_mi, disconnect_mi;
    JMenuItem asave, aosave, amsave, olc_editor, path_editor;
    JMenuItem create_door, create_room, create_object, create_mob;
    JMenuItem scriptEditor, keywordEditorMI;
    JMenuItem bug_mi, pso_mi;
    
    /**
     * Creates an instance of ClientDisplay
     *
     * @param h HegemonManager who owns this ClientDisplay
     */
    public ClientDisplay(HegemonManager h) {
        super("Hegemon Client");
        hegManager = h;
        
        addWindowListener(this);
        
        
        //our help system :)
        helpFrame = hegManager.getHelpFrame();
        helpFrame.setLocation(325, 110);
        
        colorChoicesFrame = new ColorChoicesFrame("Color & Font Choices");
        
        commandHistory = hegManager.getCommandHistory();
        commandHistory.setLocation(200, 50);
        
        olEditor = hegManager.getOlEditor();
        olEditor.setLocation(345, 50);
        
        pathCellEditor = hegManager.getPathCellEditor();
        pathCellEditor.setLocation(355, 60);
        
        mobScriptEditor = hegManager.getMobScriptEditor();
        mobScriptEditor.setLocation(350, 55);
        
        keywordEditor = hegManager.getKeywordEditor();
        keywordEditor.setLocation(360, 65);
        
        logFrame = new LogFrame(this, hegManager);
        
        ///*******************  File Menu  *****************************///
        quit_action = new JMenuItem("Quit");
        logging_action = new JMenuItem("Logging");
        color_view_mi = new JMenuItem("Colors & Fonts");
        color_choices_mi = new JMenuItem("Color Choices");
        action_edit_mi = new JMenuItem("Edit Actions");
        alias_edit_mi = new JMenuItem("Edit Aliases");
        alias_turn_on_mi = new JMenuItem("Enable Aliases");
        alias_turn_off_mi = new JMenuItem("Disable Aliases");
        cmd_history_mi = new JMenuItem("Command History");
        alias_turn_on_mi.setEnabled(false); //they start out ON already
        color_view_mi.addActionListener(this);
        color_choices_mi.addActionListener(this);
        action_edit_mi.addActionListener(this);
        quit_action.addActionListener(this);
        logging_action.addActionListener(this);
        
        fileMenu = new JMenu("File");
        fileMenu.add(quit_action);
        fileMenu.add(logging_action);
        fileMenu.addSeparator();
        fileMenu.add(color_view_mi);
        fileMenu.add(color_choices_mi);
        fileMenu.addSeparator();
        fileMenu.add(action_edit_mi);
        fileMenu.add(alias_edit_mi);
        fileMenu.add(alias_turn_on_mi);
        fileMenu.add(alias_turn_off_mi);
        fileMenu.add(cmd_history_mi);
        
        //TODO Make alias on/off controlled in AliasFrame, too
        alias_edit_mi.addActionListener(this);
        alias_turn_on_mi.addActionListener(this);
        alias_turn_off_mi.addActionListener(this);
        cmd_history_mi.addActionListener(this);
        
        ///********************  connection menu ****************///
        open_connect_mi = new JMenuItem("Open Connection");
        select_connect_mi = new JMenuItem("Select Server");
        disconnect_mi = new JMenuItem("Close Connection");
        connectionsMenu = new JMenu("Connections");
        
        open_connect_mi.addActionListener(this);
        select_connect_mi.addActionListener(this);
        disconnect_mi.addActionListener(this);
        
        connectionsMenu.add(open_connect_mi);
        connectionsMenu.add(select_connect_mi);
        connectionsMenu.add(disconnect_mi);
        
        
        ///******************  olcMenu  ********************************///
        create_door = new JMenuItem("Create Door");
        create_room = new JMenuItem("Create Room");
        create_mob = new JMenuItem("Create Mob");
        create_object = new JMenuItem("Create Object");
        olc_editor = new JMenuItem("Description Editor");
        path_editor = new JMenuItem("Vehicle Path Editor");
        scriptEditor = new JMenuItem("Script Editor (Mob/Room/Obj)");
        keywordEditorMI = new JMenuItem("Keyword Editor");
        asave = new JMenuItem("Save Current Zone");
        aosave = new JMenuItem("Save Current Zone's Objects");
        amsave = new JMenuItem("Save Current Zone's Mobs");
        
        olcMenu = new JMenu("OLC");
        olcMenu.add(create_door);
        olcMenu.add(create_room);
        olcMenu.add(create_object);
        olcMenu.add(create_mob);
        olcMenu.add(olc_editor);
        olcMenu.add(path_editor);
        olcMenu.add(scriptEditor);
        olcMenu.add(keywordEditorMI);
        olcMenu.addSeparator();
        olcMenu.add(asave);
        olcMenu.add(aosave);
        olcMenu.add(amsave);
        
        create_door.addActionListener(this);
        create_room.addActionListener(this);
        create_mob.addActionListener(this);
        create_object.addActionListener(this);
        olc_editor.addActionListener(this);
        path_editor.addActionListener(this);
        scriptEditor.addActionListener(this);
        keywordEditorMI.addActionListener(this);
        asave.addActionListener(this);
        aosave.addActionListener(this);
        amsave.addActionListener(this);
        
        ///*************************  helpMenu  ********************///
        help_view_mi = new JMenuItem("View Help Topics...");
        help_about_mi = new JMenuItem("About...");
        
        help_view_mi.addActionListener(this);
        help_about_mi.addActionListener(this);
        
        helpMenu = new JMenu("Help");
        helpMenu.add(help_view_mi);
        helpMenu.add(help_about_mi);
        
        ///*************************  Controls Menu  ********************///
        pso_mi = new JMenuItem("Player-Run Stores");
        bug_mi = new JMenuItem("Bug Reports");
        
        pso_mi.addActionListener(this);
        bug_mi.addActionListener(this);
        
        controlsMenu = new JMenu("Controls");
        controlsMenu.add(pso_mi);
        controlsMenu.add(bug_mi);
        
        ///*************************  MenuBar *******************///
        menubar = new JMenuBar();
        setJMenuBar(menubar);
        menubar.add(fileMenu);
        menubar.add(connectionsMenu);
        menubar.add(olcMenu);
        menubar.add(controlsMenu);
        menubar.add(helpMenu);
        
        ///*******************  IO fields ********************///
        output_field = new HegemonDisplay(hegManager);
        inputArea = new InputArea(this, hegManager);
        
        ///*********************  Layout  ****************************///
        
        GridBagLayout gridbag = new GridBagLayout();
        GridBagConstraints c = new GridBagConstraints();
        // Scroll panel
        setLayout(gridbag);
        int REM = c.gridwidth = GridBagConstraints.REMAINDER;
        c.fill = GridBagConstraints.BOTH;
        
        c.weighty = 1.0;
        c.weightx = 1.0;
        gridbag.setConstraints(output_field, c);
        add(output_field);
        
        //Input Field
        c.fill = GridBagConstraints.NONE;
        gridbag.setConstraints(inputArea, c);
        add(inputArea);
        
        this.pack();
        setSize(750, 700);
    }//constructor
    
    public void actionPerformed(ActionEvent e) {
        if (e.getSource() == quit_action) {
            quit();
        } else if (e.getSource() == logging_action) {
            showLoggingFrame();
        } else if (e.getSource() == color_view_mi) {
            doViewColors();
        } else if (e.getSource() == color_choices_mi) {
            doViewChoices();
        } else if (e.getSource() == action_edit_mi) {
            do_actions();
        } else if (e.getSource() == alias_edit_mi) {
            do_aliases();
        } else if (e.getSource() == alias_turn_on_mi) {
            do_alias_enable();
        } else if (e.getSource() == alias_turn_off_mi) {
            do_alias_disable();
        } else if (e.getSource() == cmd_history_mi) {
            do_show_cmd_history();
        } else if (e.getSource() == open_connect_mi) {
            do_open_connection(); //will take selected, or if none selected,
            //the first one
        } else if (e.getSource() == select_connect_mi) {
            do_select_connection(); //pops up Connection Manager
        } else if (e.getSource() == disconnect_mi) {
            do_disconnect();
        } else if (e.getSource() == create_door) {
            create_door();
        } else if (e.getSource() == create_room) {
            create_room();
        } else if (e.getSource() == create_mob) {
            create_mob();
        } else if (e.getSource() == create_object) {
            create_object();
        } else if (e.getSource() == olc_editor) {
            doOlcEditor();
        } else if (e.getSource() == path_editor) {
            doPathEditor();
        } else if (e.getSource() == scriptEditor) {
            doMSEditor();
        } else if (e.getSource() == keywordEditor) {
            doKEditor();
        } else if (e.getSource() == asave) {
            send("asave");
        } else if (e.getSource() == amsave) {
            send("amsave");
        } else if (e.getSource() == aosave) {
            send("aosave");
        } else if (e.getSource() == help_view_mi) {
            doViewHelp();
        } else if (e.getSource() == help_about_mi) {
            doHegemonHelp();
        } else if (e.getSource() == pso_mi) {
            do_show_pso_editor();
        } else if (e.getSource() == bug_mi) {
            do_show_bug_editor();
        }
    }
    
    /* WindowListener methods: */
    public void windowClosed(WindowEvent e) {
        quit();
    }
    public void windowClosing(WindowEvent e) {
        quit();
    }
    public void windowActivated(WindowEvent e) {
        
    }
    public void windowDeactivated(WindowEvent e) {
        
    }
    public void windowDeiconified(WindowEvent e) {
        
    }
    public void windowIconified(WindowEvent e) {
        
    }
    public void windowOpened(WindowEvent e) {
        
    }
    
    
    public HegemonScroll getHegemonScroll() {
        return output_field.getScroll();
    }
    
    void do_open_connection() {
        hegManager.openConnection();
    }//open_connection
    
    void do_disconnect() {
        hegManager.closeConnection();
    }//open_connection
    
    void do_select_connection() {
        hegManager.showConnection();
    }
    
    void showLoggingFrame() {
        logFrame.show();
    }
    
    void do_actions() {
        hegManager.showActions();
    }
    
    void do_aliases() {
        hegManager.showAliases();
    }
    
    void do_alias_enable() {
        hegManager.getAliasManager().setAliasEnabled(true);
        alias_turn_on_mi.setEnabled(false); //grey it out
        alias_turn_off_mi.setEnabled(true); //un-grey it
    }
    
    void do_alias_disable() {
        hegManager.getAliasManager().setAliasEnabled(false);
        alias_turn_off_mi.setEnabled(false); //grey it out
        alias_turn_on_mi.setEnabled(true); //un-grey it
    }
    
    void quit() {
        hegManager.quit();
    }
    
    void create_door() {
        OLCDoor od = new OLCDoor(this, hegManager);
        od.show();
    }
    
    void create_room() {
        OLCRoom od = new OLCRoom(this, hegManager);
        od.show();
    }
    
    void create_mob() {
        OLCMob od = new OLCMob(this, hegManager);
        od.show();
    }
    
    void create_object() {
        OLCObject od = new OLCObject(this, hegManager);
        od.show();
    }
    
    void enterOLC() {// on the mud server, prompts should start coming...
        try {
            hegManager.getSocketManager().write("olc\n");
        } catch (Exception e) {
            Log.instance().err("enterOLC:  " + e);
        }
    }
    
    void submitRoom(OLCRoom targ) {
        if (targ.isVehicle())
            hegManager.getOlcManager().setCase0("6");
        else
            hegManager.getOlcManager().setCase0("3");
        
        hegManager.getOlcManager().setRoom(targ);
        enterOLC();
    }
    
    void submitObj(OLCObject targ) {
        hegManager.getOlcManager().setCase0("1");
        hegManager.getOlcManager().setObj(targ);
        enterOLC();
    }
    
    void submitDoor(OLCDoor targ) {
        hegManager.getOlcManager().setCase0("4");
        hegManager.getOlcManager().setDoor(targ);
        enterOLC();
    }
    
    void submitMob(OLCMob targ) {
        hegManager.getOlcManager().setCase0("2");
        hegManager.getOlcManager().setMob(targ);
        enterOLC();
    }
    
    public final HegemonDisplay getOutput() {
        return output_field;
    }
    
    void doViewHelp() {
        if (!hegManager.IS_APPLET) {
            if (first_time_help) {
                helpFrame.show("opening_page.bml");
                first_time_help = false;
            }
            helpFrame.setVisible(true);
        } else {
            try {
                URL u = new URL("http://scry.wanfear.com/ScryMUD/mort_help.html");
                hegManager.getMotherClass().getAppletContext().showDocument(u);
            }//try
            catch (Exception e) {
                e.printStackTrace();
            }
        }//else
    }//doViewHelp
    
    void doOlcEditor() {
        olEditor.setVisible(true);
    }
    
    void doPathEditor() {
        pathCellEditor.setVisible(true);
    }
    
    void doMSEditor() {
        //Log.instance().dbg("Making MobScript editor visible..");
        mobScriptEditor.setVisible(true);
    }
    
    void doKEditor() {
        //Log.it("Making Keyword editor visible..");
        keywordEditor.setVisible(true);
    }
    
    void doHegemonHelp() {
        helpFrame.setVisible(true);
        helpFrame.clear();
        helpFrame.showString(BuildInfo.getBuildInfo());
    }
    
    void doViewColors() {
        hegManager.showColors();
    }
    
    void doViewChoices() {
        colorChoicesFrame.setVisible(true);
    }
    
    public void giveFocus() {
        inputArea.getTA().requestFocus();
    }
    
    public final InputArea getInputField() {
        return inputArea;
    }
    
    public void do_show_cmd_history() {
        commandHistory.setVisible(true);
    }
    
    public void do_show_pso_editor() {
        hegManager.getPsoEditor().setVisible(true);
    }
    
    public void do_show_bug_editor() {
        hegManager.getBugListEditor().setVisible(true);
    }
    
    public void send(String str) {
        try {
            (hegManager.getSocketManager()).write(str);
        } catch (Exception ex) {
            MessageDialog md =
                    new MessageDialog("INPUT ERROR", ex.toString(), "red",
                    "black");
        }
    }
}//ClientDisplay




