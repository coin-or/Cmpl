
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


import java.awt.*;
import java.awt.print.*;
import java.io.*;

/**
 *
 * @author Guido krueger ->www.javabook.de
 */
public class filePrinter 
  implements Printable
{
  //---Konstanten--------------------------------------
  private static final int RESMUL = 4;

  //---Membervariablen---------------------------------
  private PrinterJob       pjob;
  private PageFormat       pageformat;
  private filePrintHelper  fph;
  private String           fname;
  private RandomAccessFile in;

  //---Konstruktoren-----------------------------------
  public filePrinter(String fname)
  {
    this.pjob  = PrinterJob.getPrinterJob();
    this.fname = fname;
  }

  //---Öffentliche Methoden----------------------------
  public boolean setupPageFormat()
  {
    PageFormat defaultPF = pjob.defaultPage();
    this.pageformat = pjob.pageDialog(defaultPF);
    pjob.setPrintable(this, this.pageformat);
    return (this.pageformat != defaultPF);
  }

  public boolean setupJobOptions()
  {
    return pjob.printDialog();
  }

  public void printFile()
  throws PrinterException, IOException
  {
    fph = new filePrintHelper();
    in = new RandomAccessFile(fname, "r");
    pjob.print();
    in.close();
  }

  //---Implementierung von Printable-------------------
  public int print(Graphics g, PageFormat pf, int page)
  throws PrinterException
  {
    int ret = PAGE_EXISTS;
    String line = null;
    try {
      if (fph.knownPage(page)) {
        in.seek(fph.getFileOffset(page));
        line = in.readLine();
      } else {
        long offset = in.getFilePointer();
        line = in.readLine();
        if (line == null) {
          ret = NO_SUCH_PAGE;
        } else {
          fph.createPage(page);
          fph.setFileOffset(page, offset);
        }
      }
      if (ret == PAGE_EXISTS) {
        //Seite ausgeben, Grafikkontext vorbereiten
        Graphics2D g2 = (Graphics2D)g; 
        g2.scale(1.0 / RESMUL, 1.0 / RESMUL);
        int ypos = (int)pf.getImageableY() * RESMUL;
        int xpos = ((int)pf.getImageableX() + 2) * RESMUL;
        int yd = 12 * RESMUL;
        int ymax = ypos + (int)pf.getImageableHeight() * RESMUL - yd;
        //Seitentitel ausgeben
        ypos += yd; 
        g2.setColor(Color.black);
        g2.setFont(new Font("Monospaced", Font.BOLD, 10 * RESMUL));
        g.drawString("Coliop3 "+ fname + ", Page " + (page + 1), xpos, ypos);
        g.drawLine(
          xpos, 
          ypos + 6 * RESMUL, 
          xpos + (int)pf.getImageableWidth() * RESMUL, 
          ypos + 6 * RESMUL
        );
        ypos += 2 * yd;
        //Zeilen ausgeben
        g2.setColor(new Color(0, 0, 127)); 
        g2.setFont(new Font("Monospaced", Font.PLAIN, 10 * RESMUL));
        while (line != null) {
          g.drawString(line, xpos, ypos);
          ypos += yd;
          if (ypos >= ymax) {
            break;
          }
          line = in.readLine();
        }
      }
    } catch (IOException e) {
      throw new PrinterException(e.toString());
    }
    return ret;  
    
    }
    

}
