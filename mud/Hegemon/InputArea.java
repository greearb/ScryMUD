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
import javax.swing.*;

/**  Holds the input text area, as well as the arrow pane for directions.
 */
public class InputArea extends Panel {
    TextArea input_field;
    ClientDisplay cd;
    ExitNavigator en;
    HegemonManager hm;
    boolean ctrl_down = false;
    
    
    public Dimension getMinimumSize() {
        return getPreferredSize();
    }
    
    public Dimension getMaximumSize() {
        return getPreferredSize();
    }
    
    public Dimension getPreferredSize() {
        if (hm.getClientDisplay() != null) {
            return new Dimension(hm.getClientDisplay().getWidth(), input_field.getHeight());
        }
        return new Dimension(800, 150);
    }
    
    /**
     * Create a new instance of InputArea.
     *
     * @param disp ClientDisplay object associated with InputArea
     * @param h HegemonManager reference for InputArea's use.
     */
    public InputArea(ClientDisplay disp, HegemonManager h) {
        hm = h;
        
        cd = disp;
        en = new ExitNavigator(disp);
        setLayout(new FlowLayout());
        
        //TODO migrate to JEditorPane or JTextPane
        input_field = new TextArea("", 6, 60, TextArea.SCROLLBARS_VERTICAL_ONLY);
        input_field.setBackground(new Color(255, 255, 215)); //TODO What color is this?
        
        input_field.setEnabled(true); //TODO Do we really need to enable a text field?
        input_field.requestFocus();
        input_field.addKeyListener(new KeyAdapter() {
            public void keyPressed(KeyEvent e) {
                if (e.getKeyCode() == KeyEvent.VK_ENTER) {
                    String str = input_field.getText().trim();
                    cd.send(str + "\n");
                    
                    Font f = hm.getColorSelectionMgr().getInputFont();
                    Color c =  hm.getColorSelectionMgr().getInputSelection();
                    if (f != null) {
                        cd.getHegemonScroll().pushFont(f);
                    }
                    if (c != null) {
                        cd.getHegemonScroll().pushColor(c);
                    }
                    cd.getHegemonScroll().append(" >>" + str + "\n");
                    cd.getOutput().paintScroll(); //flush
                    
                    if (f != null) {
                        cd.getHegemonScroll().popFont();
                    }
                    if (c != null) {
                        cd.getHegemonScroll().popColor();
                    }
                    
                    if (hm.getCommandHistory() != null) {
                        hm.getCommandHistory().append(str + "\n");
                    }
                    input_field.setText("");
                }//if
                else if (e.getKeyCode() == KeyEvent.VK_CONTROL) {
                    ctrl_down = true;
                }//if
                else if (e.getKeyCode() == KeyEvent.VK_PAGE_UP) {
                    cd.getOutput().do_page_up_adjustment();
                } else if (e.getKeyCode() == KeyEvent.VK_PAGE_DOWN) {
                    cd.getOutput().do_page_down_adjustment();
                } else if (ctrl_down) {
                    if (e.getKeyCode() == KeyEvent.VK_P) {
                        input_field.setText(hm.getCommandHistory().getPreviousCmd());
                    }//if
                    else if (e.getKeyCode() == KeyEvent.VK_N) {
                        input_field.setText(hm.getCommandHistory().getNextCmd());
                    }//else
                    else if (e.getKeyCode() == KeyEvent.VK_K) {
                        input_field.setText(""); //kill
                    }
                }//if
                else {
                    super.keyPressed(e);
                }
            }//keyPressed
            
            public void keyReleased(KeyEvent e) {
                if (e.getKeyCode() == KeyEvent.VK_CONTROL) {
                    ctrl_down = false;
                }//if
                else {
                    super.keyReleased(e);
                }
            }//keyReleased
            
        });
        
        add(input_field);
        add(en);
    }//constructor
    
    public final TextArea getTA() {
        return input_field;
    }
    
    public final ExitNavigator getExitNavigator() {
        return en;
    }
}//InputArea


/** Holds the directional buttons.  */
class ExitNavigator extends JPanel implements ActionListener {
    ClientDisplay cd;
    /** JButton to represent ScryMUD movement. */
    JButton N, NE, E, SE, S, SW, W, NW, up, down;
    
    //TODO Add portal/exit/etc to direcitonals.
    //TODO Add Alt- mnemonic for directions. Directions or arrows? Number pad option? vi keys?
    /**
     * Directional exit JPanel. Graphically, it holds the JButtons for
     * available directions. Unavailable directions are disable.
     */
    public ExitNavigator(ClientDisplay disp) {
        cd = disp;
        
        /* Set up directional buttons. */
        NW = new JButton("NW");
        N = new JButton("N");
        NE = new JButton("NE");
        W = new JButton("W");
        up = new JButton("UP");
        down = new JButton("DOWN");
        E = new JButton("E");
        SW = new JButton("SW");
        S = new JButton("S");
        SE = new JButton("SE");
        
        N.setEnabled(false);
        NE.setEnabled(false);
        E.setEnabled(false);
        SE.setEnabled(false);
        S.setEnabled(false);
        SW.setEnabled(false);
        W.setEnabled(false);
        NW.setEnabled(false);
        up.setEnabled(false);
        down.setEnabled(false);
        
        NW.addActionListener(this);
        N.addActionListener(this);
        NE.addActionListener(this);
        W.addActionListener(this);
        up.addActionListener(this);
        down.addActionListener(this);
        E.addActionListener(this);
        SW.addActionListener(this);
        S.addActionListener(this);
        SE.addActionListener(this);
        
        
        /* Crazy formatting and layout stuff: */
        JPanel ud = new JPanel();
        ud.setLayout(new BorderLayout());
        ud.add(up, "North");
        ud.add(down, "South");
        int REM = GridBagConstraints.REMAINDER;
        
        GridBagLayout gridbag = new GridBagLayout();
        GridBagConstraints c = new GridBagConstraints();
        setLayout(gridbag);
        
        c.fill = GridBagConstraints.BOTH;
        c.weightx = c.weighty = 0.0;
        
        c.gridwidth = 1;
        gridbag.setConstraints(NW, c);
        add(NW);
        
        gridbag.setConstraints(N, c);
        add(N);
        
        c.gridwidth = REM;
        gridbag.setConstraints(NE, c);
        add(NE);
        
        c.gridwidth = 1;
        gridbag.setConstraints(W, c);
        add(W);
        
        gridbag.setConstraints(ud, c);
        add(ud);
        
        c.gridwidth = REM;
        gridbag.setConstraints(E, c);
        add(E);
        
        c.gridwidth = 1;
        gridbag.setConstraints(SW, c);
        add(SW);
        
        gridbag.setConstraints(S, c);
        add(S);
        
        c.gridwidth = REM;
        gridbag.setConstraints(SE, c);
        add(SE);
        
    }//constructor
    
    /**
     * Method to send a string with a new line to the ClientDisplay.
     *
     * @param str String to be sent (as telnet input).
     */
    protected void send(String str) {
        cd.send(str + "\n");
    }//send
    
    /**
     * Method to adjust directional buttons to enabled/disabled depending on
     * available exits.
     *
     * @param dirs Vector of directions
     */
    public void setDirs(Vector dirs) { //TODO Find out more about dirs.
        N.setEnabled(false);
        NE.setEnabled(false);
        E.setEnabled(false);
        SE.setEnabled(false);
        S.setEnabled(false);
        SW.setEnabled(false);
        W.setEnabled(false);
        NW.setEnabled(false);
        up.setEnabled(false);
        down.setEnabled(false);
        
        String d;
        
        for (int i = 1; i < dirs.size(); i++) {
            d = (String)(dirs.elementAt(i));
            if (d.equalsIgnoreCase("NW")) {
                NW.setEnabled(true);
            } else if (d.equalsIgnoreCase("N")) {
                N.setEnabled(true);
            } else if (d.equalsIgnoreCase("NE")) {
                NE.setEnabled(true);
            } else if (d.equalsIgnoreCase("W")) {
                W.setEnabled(true);
            } else if (d.equalsIgnoreCase("U")) {
                up.setEnabled(true);
            } else if (d.equalsIgnoreCase("D")) {
                down.setEnabled(true);
            } else if (d.equalsIgnoreCase("E")) {
                E.setEnabled(true);
            } else if (d.equalsIgnoreCase("SW")) {
                SW.setEnabled(true);
            } else if (d.equalsIgnoreCase("S")) {
                S.setEnabled(true);
            } else if (d.equalsIgnoreCase("SE")) {
                SE.setEnabled(true);
            }
        }//for
    }//setDirections
    
    /**
     * Handle ActionEvents for the navigational buttons.
     *
     * @param e ActionEven of button press
     */
    public void actionPerformed(ActionEvent e) {
        if (e.getSource() == N) send("north");
        else if (e.getSource() == NE) send("northeast");
        else if (e.getSource() == E) send("east");
        else if (e.getSource() == SE) send("southeast");
        else if (e.getSource() == S) send("south");
        else if (e.getSource() == SW) send("southwest");
        else if (e.getSource() == W) send("west");
        else if (e.getSource() == NW) send("northwest");
        else if (e.getSource() == up) send("up");
        else if (e.getSource() == down) send("down");
    }//actionPerformed
}//ExitNavigator
