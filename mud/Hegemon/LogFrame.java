// $Id: LogFrame.java,v 1.3 1999/06/05 23:29:12 greear Exp $
// $Revision: 1.3 $  $Author: greear $ $Date: 1999/06/05 23:29:12 $

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

public class LogFrame extends Frame {
   HegemonManager hm;
   ClientDisplay cd;
   Checkbox err;
   Checkbox wrn;
   Checkbox inf;
   Checkbox init;
   Checkbox trc;
   Checkbox dbg;
   Checkbox io;

   public LogFrame(ClientDisplay disp, HegemonManager h) {
      hm = h;
      cd = disp;
      LFItemListener listener = new LFItemListener(this);

      err = new Checkbox("Error", true);
      wrn = new Checkbox("Warning", true);
      inf = new Checkbox("Info", true);
      init = new Checkbox("Init", false);
      trc = new Checkbox("Trace", false);
      dbg = new Checkbox("Debug", false);
      io = new Checkbox("Input/Output", false);

      addWindowListener (new WindowAdapter () {
         public void windowClosing (WindowEvent e) {
            setVisible(false);
         }});


      err.addItemListener(listener);
      wrn.addItemListener(listener);
      inf.addItemListener(listener);
      init.addItemListener(listener);
      trc.addItemListener(listener);
      dbg.addItemListener(listener);
      io.addItemListener(listener);

      int REM = GridBagConstraints.REMAINDER;
           
      GridBagLayout gridbag = new GridBagLayout();
      GridBagConstraints c = new GridBagConstraints();
      setLayout(gridbag);
      
      c.fill = GridBagConstraints.BOTH;
      c.weightx = c.weighty = 0.0;
      
      c.gridwidth = 1;
      gridbag.setConstraints(err, c);
      add(err);

      gridbag.setConstraints(wrn, c);
      add(wrn);

      c.gridwidth = REM;
      gridbag.setConstraints(inf, c);
      add(inf);

      c.gridwidth = 1;
      gridbag.setConstraints(init, c);
      add(init);

      gridbag.setConstraints(trc, c);
      add(trc);

      c.gridwidth = REM;
      gridbag.setConstraints(dbg, c);
      add(dbg);

      c.gridwidth = 1;
      gridbag.setConstraints(io, c);
      add(io);

      pack();

   }//Constructor


   public void updateState() {
      // ALL
      int new_level = 0;

      if (err.getState()) {
         new_level |= Log.ERROR;
      }
      if (wrn.getState()) {
         new_level |= Log.WARNING;
      }
      if (inf.getState()) {
         new_level |= Log.INFO;
      }
      if (init.getState()) {
         new_level |= Log.INIT;
      }
      if (trc.getState()) {
         new_level |= Log.TRACE;
      }
      if (dbg.getState()) {
         new_level |= Log.DEBUG;
      }
      if (io.getState()) {
         new_level |= Log.IO;
      }

      Log.instance().setLevel(new_level);
   }//updateState

}//LogFrame


class LFItemListener implements ItemListener {
   LogFrame par;

   public LFItemListener(LogFrame parent) {
      par = parent;
   }

   public void itemStateChanged(ItemEvent e) {
      par.updateState();
   }
}//LFItemListener   
