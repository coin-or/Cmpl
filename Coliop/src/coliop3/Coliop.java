/**
 * *********************************************************************
 * This code is part of Coliop3
 * 
* Coliop3 and CMPL are projects of the Institute for Operations Research and
 * Business Management at the Martin Luther University Halle-Wittenberg and the
 * University of Applied Sciences Wildau. Please visit the project homepage
 * <www.coliop.org>
 *
 * Coliop3 is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 * 
* Coliop3 is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
* You should have received a copy of the GNU General Public License along with
 * Coliop. If not, see <http://www.gnu.org/licenses/>.
**********************************************************************
 */
package coliop3;

import java.awt.Font;
import java.io.*;
import java.util.ArrayList;
import javax.swing.JOptionPane;

/**
 *
 * @author Mike Steglich
 */
public class Coliop {

    public String name;
    public String oldName;
    public String cmplFileName;
 
    public String actCmplFile;
    public ArrayList cmplFileList;

    public boolean genFile;
    public boolean solFile;
    public boolean probChanged;

    public String cmplFile;
    public String cmplArguments;

    public String browserFile;
    public String browserArguments;

    public File homePath;

    public String clpPath;

    public String optFileName;


    public Font clpFont;

    public String clpExtension;

    public Coliop() {

        if (checkOptFile() == 0) {
            newProblem();
        } else {
            System.exit(-1);
        }

    }

    public int checkOptFile() {

        String optFilePath;

        if (System.getProperty("os.name", "unknown").indexOf("Windows") == -1) {
            optFilePath = System.getenv("HOME");
        } else {
            optFilePath = System.getenv("HOMEPATH");
        }

        optFilePath += "/.coliop/";
        optFileName = optFilePath + "coliop-3.opt";

        File optFile = new File(optFileName);

        if (!optFile.exists()) {
            FileReader fileReader;

            try {

                File optPath = new File(optFilePath);
                if (!optPath.isDirectory()) {
                    optPath.mkdir();
                }

                File optDefaultFile = new File("coliop-3.opt");
                fileReader = new FileReader(optDefaultFile);
                FileWriter fileWriter = new FileWriter(optFileName);

                for (int i = 0; i < optDefaultFile.length(); i++) {
                    fileWriter.write(fileReader.read());
                }

                fileReader.close();
                fileWriter.close();

            } catch (IOException e) {

                JOptionPane.showMessageDialog(null,
                        "Internal Error - Cant read the option file!\n" + e + "\n" + optFile,
                        "ERROR", JOptionPane.WARNING_MESSAGE);

                return -1;
            }

        } else {
            loadOptions();
        }

        return 0;
    }

    public void newProblem() {
        name = "new problem";
        actCmplFile = "new problem";

        oldName = "";
        cmplFileName = "nn";

        genFile = false;
        solFile = false;
        probChanged = false;

        cmplFile = "";
        cmplArguments = "";

        browserFile = "";
        browserArguments = "";

        homePath = new File("");

        clpPath = "";
        cmplFileList = new ArrayList();
 
        loadOptions();

        clpExtension = "";

    }

    public int loadOptions() {

        BufferedReader clpOptFile;
        String optFile = this.optFileName;

        String zeile = null;
        int i = 0;
  
        try {
            clpOptFile = new BufferedReader(new FileReader(optFile));

            while ((zeile = clpOptFile.readLine()) != null) {
                i++;

                switch (i) {

                    case 1: {
                        homePath = new File(zeile);
                        break;
                    }
                    case 2: {
                        browserFile = zeile;
                        break;
                    }
                    case 3: {
                        browserArguments = zeile;
                        break;
                    }
                    case 4: {
                        cmplFile = zeile;
                        break;
                    }
                    case 5: {
                        cmplArguments = zeile;
                        break;
                    }
                    case 6: {
                        clpPath = zeile;
                        break;
                    }
                    case 7: {
                        clpFont = Font.decode(zeile);
                        break;
                    }

                }

            }

            clpOptFile.close();

            return 1;

        } catch (IOException e) {

            JOptionPane.showMessageDialog(null,
                    "Internal Error - Cant read the option file!\n" + e + "\n" + optFile,
                    "ERROR", JOptionPane.WARNING_MESSAGE);

            return -1;

        }

    }

}
