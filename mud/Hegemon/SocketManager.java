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

import java.io.*;
import java.net.*;
import java.lang.*;
import java.awt.*;
import java.util.*;

public class SocketManager {
   Socket sock;
   SocketReader reader;
   SocketWriter writer;
   HegemonManager hm;
   
   public SocketManager(String str, int port, HegemonManager h) {
      hm = h;
      try {
         h.getClientDisplay().setTitle("Hegemon Client:  Attempting Connection to: "
                                       + str + " port: " + port);
         //new MessageDialog("ATTEMPTING CONNECTION:",
         //                  "Host:  " + str + "  port:  " + port,
         //                  "green", "black");

         sock = new Socket (str, port);
         reader = new SocketReader (this, h);
         writer = new SocketWriter (this, h);
         reader.setPriority (6);
         reader.start ();
         h.getClientDisplay().setTitle("Hegemon Client:  Connected to: "
                                       + str + " port: " + port);
      }
      catch (IOException e) {
         System.err.println ("Constructing SocketManager:  " + e);
         new MessageDialog("ERROR OPENING SOCKET", e.toString(),
                           "black", "red");
      }
   }//constructor

   public Socket getSock() {
      return sock;
   }

   public SocketReader getReader() {
      return reader;
   }

   public SocketWriter getWriter() {
      return writer;
   }
   
   public final void write(String str) {
      writer.send(str);
   }
   
   /** used for snazy new output device. */
   public void setReaderOutput(HegemonScroll output) {
      reader.setOutput(output);
   }

   public void destroy() {
      if (reader != null) {
         reader.destroy();
         reader = null;
      }
      
      if (writer != null) {
         writer.destroy();
         writer = null;
      }
   }
         

   public void signalWriterDeath() {
      Log.instance().init("In signalWriterDeath().");
      writer = null;
      hm.getScroll().append("\n\n#### LOST CONNECTION on Write!\n");
      hm.getScroll().getPar().paintScroll();
      hm.getScroll().getProperties().reset();
      hm.getClientDisplay().setTitle("Hegemon Client:  Not Connected");
      new MessageDialog("LOST CONNECTION", "Cannot write to socket.",
                        "black", "red");
   }//
   
   public void signalReaderDeath() {
      Log.instance().init("In signalReaderDeath().");
      try {
         if (reader.getDis() != null)
           reader.getDis().close();
         sock.close();
      }
      catch (IOException e) {
         Log.instance().err("signalReaderDeath:  " + e);
      }

      reader = null;
      hm.getScroll().append("\n\n#### LOST CONNECTION!\n");
      hm.getScroll().getPar().paintScroll();
      hm.getScroll().getProperties().reset();
      hm.getClientDisplay().setTitle("Hegemon Client:  Not Connected");
      new MessageDialog("LOST CONNECTION", "Cannot read from socket.",
                        "black", "red");
   }//

   public void close() {
      reader = null;
      writer = null;
   }

   public void flush() {
      if (writer != null)
        writer.flush();
      if (reader != null)
        reader.flush();
   }
}//SocketManager
      

