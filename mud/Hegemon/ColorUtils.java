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

/**  Provides a few convenience functions on colors, and defines
  some not found in class Color. */
class ColorUtils extends Object {

   /**  This converts a color name to a color.  The string must be a recognized
     name of a color. */
   public static Color getColor(String color_name) throws ColorNameException {
      if (color_name.equalsIgnoreCase("black"))
        return Color.black;
      else if (color_name.equalsIgnoreCase("blue"))
        return Color.blue;
      else if (color_name.equalsIgnoreCase("cyan"))
        return Color.cyan;
      else if (color_name.equalsIgnoreCase("darkGray"))
        return Color.darkGray;
      else if (color_name.equalsIgnoreCase("gray"))
        return Color.gray;
      else if (color_name.equalsIgnoreCase("green"))
        return Color.green;
      else if (color_name.equalsIgnoreCase("lightGray"))
        return Color.lightGray;
      else if (color_name.equalsIgnoreCase("magenta"))
        return Color.magenta;
      else if (color_name.equalsIgnoreCase("orange"))
        return Color.orange;
      else if (color_name.equalsIgnoreCase("pink"))
        return Color.pink;
      else if (color_name.equalsIgnoreCase("red"))
        return Color.red;
      else if (color_name.equalsIgnoreCase("white"))
        return Color.white;
      else if (color_name.equalsIgnoreCase("yellow"))
        return Color.yellow;
      else {
         throw new ColorNameException("Color name:  " + color_name +
                                      " is not recognized!");
      }//else
   }//getColor(String)
}//ColorUtils class


