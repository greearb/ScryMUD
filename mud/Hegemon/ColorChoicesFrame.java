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

import java.awt.*;
import java.awt.event.*;


class ColorChoicesFrame extends Frame {
   
   public ColorChoicesFrame(String str) {
      super(str);
      

      Menu file_m = new Menu("File");
      MenuBar menubar = new MenuBar();
      setMenuBar(menubar);

      MenuItem done_mi = new MenuItem("Done");
      done_mi.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            do_close();
         }});

      file_m.add(done_mi);
      menubar.add(file_m);

      int REM = GridBagConstraints.REMAINDER;
      GridBagLayout gridbag = new GridBagLayout();
      GridBagConstraints c = new GridBagConstraints();
      setLayout(gridbag);

      c.fill = GridBagConstraints.BOTH;
      c.weightx = c.weighty = 0.0;
            
      c.gridwidth = 1;

      setBackground(new Color(0, 0, 0));

      int sofar = 0;
      for (int i = 0; i<255; i += 40) {
         for (int j = 0; j<255; j += 40) {
            for (int k = 0; k<255; k += 40) {
               Button b = new Button(i + "," + j + "," + k);
               b.setBackground(new Color(i,j,k));

               if ((i + j + k) > 375)
                  b.setForeground(new Color(0, 0, 0));
               else
                  b.setForeground(new Color(255, 255, 255));


               if (++sofar > 10) {
                  c.gridwidth = REM;
                  sofar = 0;
               }
               else
                  c.gridwidth = 1;

               gridbag.setConstraints(b, c);
               add(b);
            }//for
         }//for
      }//for
      pack();

   }//constructor

   void do_close() {
      setVisible(false);
   }

}//ColorChoicesFrame   
