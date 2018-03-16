/* ****************************************************************************
 * This code is part of jCMPL
 *
 * Copyright (C) 2013 Mike Steglich / B. Knie Technical University of Applied
 * Sciences Wildau, Germany
 *
 * jCMPL is a project of the Technical University of Applied Sciences Wildau
 * and the Institute for Operations Research and Business Management at the
 * Martin Luther University Halle-Wittenberg.
 *
 * Please visit the project homepage <http://www.coliop.org>
 *
 * jCMPL is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option) any
 * later version.
 *
 * jCMPL is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU Lesser General Public # License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 * ****************************************************************************/

package jCMPL;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;

/**
 * Handling of a CMPL infos and statistics for jCMPL
 *
 * @author mike
 */
public class CmplInfo {

    private String _cmplInfoFile;
    private String _instanceName;
    private String _varProdFileName;
    private String _varProdText;
    private String _statisticsFileName;
    private String _statisticsText;
    private String _matrixFileName;
    private String _matrixText;

    private void setDefaults() {
        _cmplInfoFile = "";
        _instanceName = "";
        _varProdFileName = "";
        _varProdText = "";
        _statisticsFileName = "";
        _statisticsText = "";
        _matrixFileName = "";
        _matrixText = "";

    }

    /**
     * Constructor
     */
    public CmplInfo() {
        setDefaults();

    }

    /**
     * Constructor
     *
     * @param cmplInfoFile Name of the cmplInfo file
     */
    public CmplInfo(String cmplInfoFile) {
        setDefaults();
        _cmplInfoFile = cmplInfoFile;
    }

    /**
     * Returns the file name for variable product statistics
     *
     * @return file name
     */
    public String varProdFileName() {
        return _varProdFileName;
    }

    /**
     * Returns variable product statistics
     *
     * @return string containing variable product statistics
     */
    public String varProdText() {
        return _varProdText;
    }

    /**
     * Returns the file name for CMPL statistics
     *
     * @return file name
     */
    public String statisticsFileName() {
        return _statisticsFileName;
    }

    /**
     * Returns CMPL statistics
     *
     * @return string containing CMPL statistics
     */
    public String statisticsText() {
        return _statisticsText;
    }

    /**
     * Returns the file name for CMPL matrix
     *
     * @return file name
     */
    public String matrixFileName() {
        return _matrixFileName;
    }

    /**
     * Returns CMPL matrix
     *
     * @return string containing CMPL matrix
     */
    public String matrixText() {
        return _matrixText;
    }

    /**
     * Reads the CmplInfos into the jCMPL structures from the corresponding
     * CmplMessage file
     *
     * @throws CmplException
     */
    public void readCmplInfo() throws CmplException {
        readCmplInfo("");
    }

    /**
     * Reads the CmplInfos into the jCMPL structures from the corresponding
     * CmplMessage file or a CmplMessage string
     *
     * @param cmplInfoStr CmplInfo string
     * @throws CmplException
     */
    public void readCmplInfo(String cmplInfoStr) throws CmplException {

        if (cmplInfoStr.isEmpty() && _cmplInfoFile.isEmpty()) {
            throw new CmplException("Neither cmplInfoFile nor cmplInfoString defined");
        }


        ArrayList<String> lines = null;

        if (!_cmplInfoFile.isEmpty()) {
            String line = "";
            try {
                BufferedReader in = new BufferedReader(new FileReader(_cmplInfoFile));
                lines = new ArrayList<String>();

                while ((line = in.readLine()) != null) {
                    lines.add(line);
                }

                in.close();

            } catch (IOException err) {
                throw new CmplException("Cannot read solution file :" + _cmplInfoFile + " ->" + err);
            }
        } else {
            lines = new ArrayList<String>(Arrays.asList(cmplInfoStr.split("\n")));
        }


        int lineNr = 1;
        Boolean generalSection = false;
        Boolean varProdSection = false;
        Boolean statSection = false;
        Boolean matrixSection = false;


        for (String line : lines) {

            if (lineNr == 1) {
                if (line.contains("<?xml version")) {
                    lineNr++;
                    continue;
                } else {
                    throw new CmplException("Cant't read cmplInfo file  - Not a XML file !");
                }
            }
            if (lineNr == 2) {
                if (line.contains("<CmplInfo")) {
                    lineNr++;
                    continue;
                } else {
                    throw new CmplException("Cant't read cmplInfo file  - Not a CmplInfo file !");
                }
            }
            if (line.contains("<general>")) {
                generalSection = true;
                varProdSection = false;
                statSection = false;
                matrixSection = false;
                continue;
            }
            if (line.contains("</general>")) {
                generalSection = false;
                continue;
            }
            if (line.contains("<statistics")) {
                _statisticsFileName = CmplTools.xmlStrToStringList(line, "\"([^\"]*)\"").get(0);
                generalSection = false;
                varProdSection = false;
                statSection = true;
                matrixSection = false;
                continue;
            }
            if (line.contains("</statistics")) {
                statSection = false;
                continue;
            }
            if (line.contains("<variableProducts")) {
                _varProdFileName = CmplTools.xmlStrToStringList(line, "\"([^\"]*)\"").get(0);
                generalSection = false;
                varProdSection = true;
                statSection = false;
                matrixSection = false;
                continue;
            }
            if (line.contains("</variableProducts")) {
                varProdSection = false;
                continue;
            }
            if (line.contains("<matrix")) {
                _matrixFileName = CmplTools.xmlStrToStringList(line, "\"([^\"]*)\"").get(0);
                generalSection = false;
                varProdSection = false;
                statSection = false;
                matrixSection = true;
                continue;
            }
            if (line.contains("</matrix")) {
                matrixSection = false;
                continue;
            }

            if (generalSection) {
                if (line.contains("<generalStatus")) {
                    _instanceName = CmplTools.xmlStrToString(line, "<instanceName>([^\"]*)</instanceName>");
                    continue;
                }
            }
            if (statSection) {
                _statisticsText += line + "\n";
            }
            if (varProdSection) {
                _varProdText += line + "\n"; 
            }
            if (matrixSection) {
                _matrixText += line + "\n";
            }
        }
    }
}
