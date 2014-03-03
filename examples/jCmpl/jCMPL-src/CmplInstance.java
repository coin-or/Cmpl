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
import java.io.File;
import java.io.BufferedReader;
import java.io.FileReader;
import org.apache.commons.lang3.StringEscapeUtils;
import java.util.HashMap;
import java.util.Map;

/**
 * Handling of the the CmplInstance for jCMPL
 */
public class CmplInstance 
{
    
    private String _instStr;
    private HashMap<String, String> _cmplDataList;
              
    /**
     * Constructor
     */
    protected CmplInstance()
    {
        _instStr = "";
        _cmplDataList = new HashMap<String, String>();
    }
    
    /**
     * Creates a CmplInstance string 
     * @param cmplFileName CMPL file
     * @param optList List of the options
     * @param dataString CmplData string
     * @param jobId JobId
     * @return CmplInstance string 
     * @throws CmplException 
     */
    protected String cmplInstanceStr(String cmplFileName, HashMap<Integer, String>  optList, String dataString, String jobId) throws CmplException
    {
        File cmplFile = new File(cmplFileName);
        if(cmplFile.exists()) {
            _cmplDataList.put(cmplFile.getName().substring(0, cmplFile.getName().lastIndexOf('.')) + ".cdat", dataString);
        } else {
            throw new CmplException("CMPL file " + cmplFileName + " does not exist.");
        }
        
        try
        {
            boolean commentSection = false;
            int lineNr = 0;
            
            BufferedReader in = new BufferedReader(new FileReader(cmplFileName));
         
            String tmpName = "";
            
            ArrayList<String> lines = new ArrayList<String>() ;
            
            String tmpLine="";
            while((tmpLine = in.readLine()) != null){
               lines.add(tmpLine);
            }
            in.close();
            
            for (String line : lines )    {
                line = line.trim();

                if (line.startsWith("/*")) {
                    commentSection = true;
                    line = line.substring(0,line.indexOf("/*")-1); 
                }
                if (line.contains("*/")) {
                    commentSection = false;
                    line = line.substring(line.indexOf("*/")+1); 
                }
                if (commentSection) {
                    continue;
                }
                
                if (line.startsWith("%data"))
                {
                    if(line.contains(":")) {
                        tmpName = line.substring(5, line.indexOf(":")).trim();
                    } else {
                        tmpName = line.substring(5).trim();
                    }
                    
                    if(!tmpName.isEmpty())
                    {
                        BufferedReader cin = new BufferedReader(new FileReader(tmpName));
                       
                        String dline = "";
                        String tmpString = "";
                        String tmpName1 = "";
                        
                        while((dline = cin.readLine()) != null)
                        {
                            tmpString += dline;
                            tmpName1 = (new File(tmpName).getName());
                            if (_cmplDataList.containsKey(tmpName1)) {
                                int Min = 100000;
                                int Max = 999999;
                                tmpName1 = Integer.toString(Min + (int) (Math.random() * ((Max - Min) + 1))) + tmpName1;
                            }
                           _cmplDataList.put(tmpName1, tmpString);

                           lines.set(lineNr, line.replace(tmpName, tmpName1));
                        }
                        cin.close();
                    }
                }
                lineNr += 1;
                
            }
                      
            _instStr += "<?xml version = \"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n";
            _instStr += "<CmplInstance version=\"1.0\">\n";
            _instStr += "<general>\n";
            _instStr += "<name>" + cmplFile.getName() + "</name>\n";
            _instStr += "<jobId>" + jobId + "</jobId>\n";
            _instStr += "</general>\n";
          
            if (optList.size() > 0) {
                _instStr += "<options>\n";

                for (Map.Entry<Integer, String> o : optList.entrySet()) {
                    _instStr += "<opt>" + o.getValue() + "</opt>\n";
                }
                
                _instStr += "</options>\n";
            }
                
            _instStr += "<problemFiles>\n";
            _instStr += "<file name=\"" + cmplFile.getName() + "\" type=\"cmplMain\">\n";
           
            String tmpStr="";
            for (String line : lines) {
                tmpStr+=line+"\n";
            }
            _instStr+=StringEscapeUtils.escapeXml(tmpStr);
           
            _instStr += "\n";
            _instStr += "</file>\n";
 
            for (Map.Entry<String, String> e : _cmplDataList.entrySet())
            {
                _instStr += "<file name=\"" + e.getKey() + "\" type=\"cmplData\">\n";
                _instStr += StringEscapeUtils.escapeXml(e.getValue());
                _instStr += "\n";
                _instStr += "</file>\n";
            }
            _instStr += "</problemFiles>\n";
            _instStr += "</CmplInstance>\n";
                    
        }
        catch(Exception e) {
            throw new  CmplException("IO error : "+e);
        }
        
       
        return _instStr;  
    }
    
   
}
