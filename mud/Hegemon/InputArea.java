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
// To contact the Author, Ben Greear:  greear@cyberhighway.net, (preferred)
//                                     greearb@agcs.com
//

import java.awt.event.*;
import java.awt.*;
import java.util.*;

/**  Will hold the input text area, as well as the arrow pane for directions.
 */
public class InputArea extends Panel { 
   TextArea input_field;
   ClientDisplay cd;
   ExitNavigator en;
   HegemonManager hm;
   boolean ctrl_down = false;

   public InputArea(ClientDisplay disp, HegemonManager h) {
      hm = h;
   
      cd = disp;
      en = new ExitNavigator(disp, hm);
      setLayout(new FlowLayout());

      input_field = new TextArea("", 7, 60, TextArea.SCROLLBARS_VERTICAL_ONLY);
      input_field.setBackground(new Color(255, 255, 215));

      input_field.setEnabled(true);
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
            }
            else if (e.getKeyCode() == KeyEvent.VK_PAGE_DOWN) {
               cd.getOutput().do_page_down_adjustment();
            }
            else if (ctrl_down) {
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
class ExitNavigator extends Panel {
   ClientDisplay cd;
   HegemonManager hm;
   Color not_there;
   Color there;

   Button NW;
   Button N;
   Button NE;
   Button W;
   Button up;
   Button down;
   Button E;
   Button SW;
   Button S;
   Button SE;

   Color NW_color;
   Color N_color;
   Color NE_color;
   Color W_color;
   Color up_color;
   Color down_color;
   Color E_color;
   Color SW_color;
   Color S_color;
   Color SE_color;

   public ExitNavigator(ClientDisplay disp, HegemonManager h) {
      cd = disp;
      hm = h;
      not_there = new Color(0, 0, 255);
      there = new Color(0, 255, 0);

      NW = new Button("NW");
      N = new Button("N");
      NE = new Button("NE");
      W = new Button("W");
      up = new Button("UP");
      down = new Button("DOWN");
      E = new Button("E");
      SW = new Button("SW");
      S = new Button("S");
      SE = new Button("SE");

      NW.setBackground(not_there);
      N.setBackground(not_there);
      NE.setBackground(not_there);
      W.setBackground(not_there);
      up.setBackground(not_there);
      down.setBackground(not_there);
      E.setBackground(not_there);
      SW.setBackground(not_there);
      S.setBackground(not_there);
      SE.setBackground(not_there);

      NW.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            send("northwest");
         }});

      N.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            send("north");
         }});

      NE.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            send("northeast");
         }});

      W.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            send("west");
         }});

      up.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            send("up");
         }});

      down.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            send("down");
         }});

      Panel ud = new Panel();
      ud.setLayout(new BorderLayout());
      ud.add(up, "North");
      ud.add(down, "South");

      E.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            send("east");
         }});

      SW.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            send("southwest");
         }});

      S.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            send("south");
         }});

      SE.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            send("southeast");
         }});

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

   protected void send(String str) {
      cd.send(str + "\n");
   }//send

   public void setDirs(Vector dirs) {
      String d;

      NW_color = not_there;
      N_color = not_there;
      NE_color = not_there;
      W_color = not_there;
      up_color = not_there;
      down_color = not_there;
      E_color = not_there;
      SW_color = not_there;
      S_color = not_there;
      SE_color = not_there;

      for (int i = 1; i < dirs.size(); i++) {
         d = (String)(dirs.elementAt(i));
         if (d.equalsIgnoreCase("NW")) {
            NW_color = there;
         }
         else if (d.equalsIgnoreCase("N")) {
            N_color = there;
         }
         else if (d.equalsIgnoreCase("NE")) {
            NE_color = there;
         }
         else if (d.equalsIgnoreCase("W")) {
            W_color = there;
         }
         else if (d.equalsIgnoreCase("U")) {
            up_color = there;
         }
         else if (d.equalsIgnoreCase("D")) {
            down_color = there;
         }
         else if (d.equalsIgnoreCase("E")) {
            E_color = there;
         }
         else if (d.equalsIgnoreCase("SW")) {
            SW_color = there;
         }
         else if (d.equalsIgnoreCase("S")) {
            S_color = there;
         }
         else if (d.equalsIgnoreCase("SE")) {
            SE_color = there;
         }
      }//for

      if (NW.getBackground() != NW_color) {
         NW.setBackground(NW_color);
      }
      if (N.getBackground() != N_color) {
         N.setBackground(N_color);
      }
      if (NE.getBackground() != NE_color) {
         NE.setBackground(NE_color);
      }
      if (W.getBackground() != W_color) {
         W.setBackground(W_color);
      }
      if (up.getBackground() != up_color) {
         up.setBackground(up_color);
      }
      if (down.getBackground() != down_color) {
         down.setBackground(down_color);
      }
      if (E.getBackground() != E_color) {
         E.setBackground(E_color);
      }
      if (SW.getBackground() != SW_color) {
         SW.setBackground(SW_color);
      }
      if (S.getBackground() != S_color) {
         S.setBackground(S_color);
      }
      if (SE.getBackground() != SE_color) {
         SE.setBackground(SE_color);
      }

   }//setDirections
}//ExitNavigator
