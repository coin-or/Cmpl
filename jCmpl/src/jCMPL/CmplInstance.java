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

import java.util.ArrayList;
import java.io.File;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import org.apache.commons.lang3.StringEscapeUtils;
import java.util.HashMap;
import java.util.Map;

/**
 * Handling of the the CmplInstance for jCMPL
 */
public class CmplInstance {

    private StringBuilder _instStr;
    private HashMap<String, String> _cmplDataList;

    /**
     * Constructor
     */
    protected CmplInstance() {
        _instStr = new StringBuilder();
        _cmplDataList = new HashMap<String, String>();
    }

    /**
     * Creates a CmplInstance string
     *
     * @param cmplFileName CMPL file
     * @param optList List of the options
     * @param dataString CmplData string
     * @param jobId JobId
     * @return CmplInstance string
     * @throws CmplException
     */
    protected String cmplInstanceStr(String cmplFileName, HashMap<Integer, String> optList, String dataString, String jobId) throws CmplException {
        File cmplFile = new File(cmplFileName);
        if (cmplFile.exists()) {
            if (!dataString.isEmpty()) {
                _cmplDataList.put("__cmplData__" + cmplFile.getName().substring(0, cmplFile.getName().lastIndexOf('.')) + ".cdat", dataString);
            }
        } else {
            throw new CmplException("CMPL file " + cmplFileName + " does not exist.");
        }

        try {
            boolean commentSection = false;
            int lineNr = 0;

            BufferedReader in = new BufferedReader(new FileReader(cmplFileName));

            String tmpName = "";
            String tmpName1 = "";

            ArrayList<String> lines = new ArrayList<String>();

            String tmpLine = "";
            while ((tmpLine = in.readLine()) != null) {
                lines.add(tmpLine);
            }
            in.close();

            for (String line : lines) {
                line = line.trim();

                if (line.startsWith("/*")) {
                    commentSection = true;
                    line = line.substring(0, line.indexOf("/*") - 1);
                }
                if (line.contains("*/")) {
                    commentSection = false;
                    line = line.substring(line.indexOf("*/") + 1);
                }
                if (commentSection) {
                    continue;
                }

                if (line.startsWith("%data")) {
                    if (line.contains(":")) {
                        tmpName = line.substring(5, line.indexOf(":")).trim();
                    } else {
                        tmpName = line.substring(5).trim();
                    }

                    if (tmpName.isEmpty()) {
                        if (!dataString.isEmpty()) {
                            lines.set(lineNr, line.replace("%data", "%data __cmplData__" + cmplFile.getName().substring(0, cmplFile.getName().lastIndexOf('.')) + ".cdat"));
                            tmpName = "__cmplData__";
                        } else {
                            tmpName = cmplFile.getName().substring(0, cmplFile.getName().lastIndexOf('.')) + ".cdat";
                        }
                    }

                    if (!(_cmplDataList.containsKey(tmpName) || tmpName.equals("__cmplData__"))) {
                        tmpName1 = "";
                        if (cmplFile.getParent() == null) {
                            tmpName1 = cmplFile.getName().substring(0, cmplFile.getName().lastIndexOf('.')) + ".cdat";
                        } else {
                            tmpName1 = cmplFile.getParent() + File.separator + cmplFile.getName().substring(0, cmplFile.getName().lastIndexOf('.')) + ".cdat";
                        }

                        BufferedReader cin = new BufferedReader(new FileReader(tmpName1));

                        String dline = "";
                        String tmpString = "";

                        while ((dline = cin.readLine()) != null) {
                            tmpString += dline;
                        }
                        cin.close();
                        _cmplDataList.put(tmpName1, tmpString);
                    }
                }
            
            lineNr += 1;

        }

        _instStr.append("<?xml version = \"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n");
        _instStr.append("<CmplInstance version=\"1.0\">\n");
        _instStr.append("<general>\n");
        _instStr.append("<name>").append(cmplFile.getName()).append("</name>\n");
        _instStr.append("<jobId>").append(jobId).append("</jobId>\n");
        _instStr.append("</general>\n");

        if (optList.size() > 0) {
            _instStr.append("<options>\n");

            for (Map.Entry<Integer, String> o : optList.entrySet()) {
                _instStr.append("<opt>").append(o.getValue()).append("</opt>\n");
            }

            _instStr.append("</options>\n");
        }

        _instStr.append("<problemFiles>\n");
        _instStr.append("<file name=\"").append(cmplFile.getName()).append("\" type=\"cmplMain\">\n");

        String tmpStr = "";
        for (String line : lines) {
            tmpStr += line + "\n";
        }
        _instStr.append(StringEscapeUtils.escapeXml(tmpStr));

        _instStr.append("\n");
        _instStr.append("</file>\n");

        for (Map.Entry<String, String> e : _cmplDataList.entrySet()) {
            _instStr.append("<file name=\"").append(e.getKey()).append("\" type=\"cmplData\">\n");
            _instStr.append(StringEscapeUtils.escapeXml(e.getValue()));
            _instStr.append("\n");
            _instStr.append("</file>\n");
        }
        _instStr.append("</problemFiles>\n");
        _instStr.append("</CmplInstance>\n");

    }
    catch(IOException e

    
        ) {
            throw new CmplException("IO error : " + e);
    }

    return _instStr.toString() ;
}

}
