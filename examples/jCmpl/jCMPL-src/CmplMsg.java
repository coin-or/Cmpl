/* ****************************************************************************
 * This code is part of jCMPL
 *
 * Copyright (C) 2013 Mike Steglich / B. Knie Technical University of Applied
 * Sciences Wildau, Germany
 *
 * pyCMPL is a project of the Technical University of Applied Sciences Wildau
 * and the Institute for Operations Research and Business Management at the
 * Martin Luther University Halle-Wittenberg.
 *
 * Please visit the project homepage <http://www.coliop.org>
 *
 * pyCMPL is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option) any
 * later version.
 *
 * pyCMPL is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU Lesser General Public # License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 * ****************************************************************************/

package jCMPL;

import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.Arrays;
import org.apache.commons.lang3.StringEscapeUtils;

/**
 * Handling of a CMPL message for jCMPL
 * @author mike
 */
public class CmplMsg 
{
    
    private String _type;
    private String _file;
    private String _line;
    private String _description;
    
    /**
     * Constructor
     */
    public CmplMsg()    
    {
        _type = "";
        _file = "";
        _line = "";
        _description = "";
    } 

    /**
     * Returns the type of the messages
     * @return message type warning|error
     */
    public String type() {
        return _type;
    }

    /** 
     * Setter for the message type
     * Used by Cmpl
     * @param type message type
     */
    protected void setType(String type) {
        this._type = type;
    }

    /**
     * Returns the name of the CMPL file in that the error or warning occurs 
     * @return CMPL file name r CmplData file name
     */
    public String file() {
        return _file;
    }

    /**
     * Setter for the file name 
     * Used by CMPL 
     * @param file file name
     */
    protected void setFile(String file) {
        this._file = file;
    }

    /**
     * Returns the line in the CMPL file in that the error or warning occurs
     * @return line
     */
    public String line() {
        return _line;
    }

    /**
     * Setter fpr the line
     * Used by CMPL
     * @param line line
     */
    protected void setLine(String line) {
        this._line = line;
    }

    /**
     * Returns the a description of the error or warning message 
     * @return description of the error or warning  
     */
    public String description() {
        return _description;
    }

    protected void setDesrciption(String description) {
        this._description = description;
    }
}

/**
 * Handling of all CMPL messages for jCMPL
 * @author mike
 */
class CmplMessages {

    private String _cmplStatus;
    private String _cmplVersion;
    private String _cmplMessage;
    private String _msgFile;
    private ArrayList<CmplMsg> _cmplMessageList;

    /**
     * internal function - setting defaults
     */
    private void setDefaults() {
        _cmplStatus = "";
        _cmplVersion = "";
        _cmplMessage = "";
        _msgFile = "";
        _cmplMessageList = new ArrayList<CmplMsg>();
    }

    /**
     * Constructor
     */
    public CmplMessages() {
        setDefaults();
    }

   
    /**
     * Constructor using a CmplMessage file
     * @param msgFile 
     */
    public CmplMessages(String msgFile) {
        setDefaults();
        _msgFile = msgFile;
    }

    /**
     * Returns the CMPL related status of the Cmpl object
     * Used by Cmpl
     * @return cmpl status
     */
    public int cmplStatus() {
        int stat = -99;
        if (_cmplStatus.equalsIgnoreCase("normal")) {
            stat = Cmpl.CMPL_OK;
        } else if (_cmplStatus.equalsIgnoreCase("warning")) {
            stat = Cmpl.CMPL_WARNINGS;
        } else if (_cmplStatus.equalsIgnoreCase("error")) {
            stat = Cmpl.CMPL_FAILED;
        } else {
            stat = Cmpl.CMPL_UNKNOWN;
        }
        return stat;
    }

    /**
     * Returns a list of CmplMsg objects that contain the CMPL messages
     * @return List of CMPL messages
     */
    public ArrayList<CmplMsg> cmplMessageList() {
        return _cmplMessageList;
    }

    /* public String cmplMessage() {
        return _cmplMessage;
    }*/

    /**
     * Reads the CmplMessages into the jCMPL structures from th corresponding CmplMessage file
     * @throws CmplException 
     */
    protected void readCmplMessages() throws CmplException {
        readCmplMessages("");
    }

    /**
      * Reads the CmplMessages into the jCMPL structures from th corresponding CmplMessage file or a CmplMessage string 
     * @param msgStr CmplMessage string 
     * @throws CmplException 
     */
    protected void readCmplMessages(String msgStr) throws CmplException {

        if (msgStr.isEmpty() && _msgFile.isEmpty()) {
            throw new CmplException("Neither cmplMessageFile nor cmplMessageString defined");
        }

        ArrayList<String> lines = null;

        if (!_msgFile.isEmpty()) {
            String line = "";
            try {
                BufferedReader in = new BufferedReader(new FileReader(_msgFile));
                lines = new ArrayList<String>();

                while ((line = in.readLine()) != null) {
                    lines.add(line);
                }

                in.close();

            } catch (IOException err) {
                throw new CmplException("Cannot read message file :" + _msgFile + " ->" + err);
            }
        }  else {
            lines = new ArrayList<String>(Arrays.asList(msgStr.split("\n")));
        }

        int lineNr = 1;
        Boolean generalSection = false;
        Boolean msgSection = false;

        for (String line : lines) {

            if (lineNr == 1) {
                if (line.contains("<?xml version")) {
                    lineNr++;
                    continue;
                } else {
                    throw new CmplException("Cant't read cmplMessage file  - Not a XML file !");
                }
            }
            if (lineNr == 2) {
                if (line.contains("<CmplMessages")) {
                    lineNr++;
                    continue;
                } else {
                    throw new CmplException("Cant't read cmplMessage file  - Not a CmplMessages file !");
                }
            }
            if (line.contains("<general>")) {
                generalSection = true;
                msgSection = false;
                continue;
            }
            if (line.contains("</general>")) {
                generalSection = false;
                continue;
            }
            if (line.contains("<messages")) {
                generalSection = false;
                msgSection = true;
                continue;
            }
            if (line.contains("</messages")) {
                msgSection = false;
                continue;
            }
            if (generalSection) {
                if (line.contains("<generalStatus")) {
                    _cmplStatus = CmplTools.xmlStrToString(line, "<generalStatus>([^\"]*)</generalStatus>");
                    continue;
                }
                if (line.contains("<message")) {
                    _cmplMessage = CmplTools.xmlStrToString(line, "<message>([^\"]*)</message>");
                    continue;
                }
                if (line.contains("<cmplVersion")) {
                    _cmplVersion = CmplTools.xmlStrToString(line, "<cmplVersion>([^\"]*)</cmplVersion>");
                    continue;
                }
            }
            if (msgSection) {
                if (line.contains("<message")) {
                    ArrayList<String> tmpList = new ArrayList<String>();

                    String patternStr = "\"([^\"]*)\"";

                    Pattern pattern = Pattern.compile(patternStr);
                    Matcher matcher = pattern.matcher(line);

                    while (matcher.find()) {
                        tmpList.add(matcher.group(1));
                    }
                    CmplMsg x = new CmplMsg();
                    x.setType(tmpList.get(0));
                    x.setFile(tmpList.get(1));
                    x.setLine(tmpList.get(2));
                    x.setDesrciption(StringEscapeUtils.unescapeXml(tmpList.get(3)));
                    _cmplMessageList.add(x);

                }
            }
        }
    }

}
