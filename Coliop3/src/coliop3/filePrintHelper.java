
/***********************************************************************
*  This code is part of Coliop3
*
*  Coliop3 and CMPL are projects of the Institute for Operations Research
*  and Business Management at the Martin Luther University Halle-Wittenberg 
*  and the University of Applied Sciences Wildau. 
*  Please visit the project homepage <www.coliop.org>
*
*  Coliop3 is free software: you can redistribute it and/or modify it
*  under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  Coliop3 is distributed in the hope that it will be useful, but WITHOUT
*  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
*  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
*  License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with Coliop. If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/


package coliop3;

import java.util.*;

/**
 *
 * @author Guido krueger ->www.javabook.de
 */
public class filePrintHelper {
    
      //---Membervariablen----------------------------------
  Vector pageinfo;

  //---Konstruktor--------------------------------------
  public filePrintHelper()
  {
    pageinfo = new Vector();
  }

  //---Seitendefinition und -abfrage--------------------
  public void createPage(int page)
  {
    for (int i = pageinfo.size(); i <= page; ++i) {
      pageinfo.addElement(new Entry());
    }
  }

  public boolean knownPage(int page)
  {
    return page < pageinfo.size();
  }

  //---Verwaltung der Offsets---------------------------
  public long getFileOffset(int page)
  {
    Entry entry = (Entry)pageinfo.elementAt(page);
    return entry.fileoffset;
  }

  public void setFileOffset(int page, long fileoffset)
  {
    Entry entry = (Entry)pageinfo.elementAt(page);
    entry.fileoffset = fileoffset;
  }

  //---Lokale Klasse Entry------------------------------
  static class Entry
  {
    public long fileoffset;

    public Entry()
    {
      this.fileoffset = -1;
    }
  }

}
