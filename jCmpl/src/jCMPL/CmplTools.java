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

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Some helpful tools for jCMPL
 */
public class CmplTools {

    /**
     * Converts a XML string into INteger
     * @param txt XML String
     * @return Integer value
     * @throws CmplException 
     */
    static Integer xmlStrToInteger(String txt) throws CmplException {
        try {
            int val;
            val = Integer.parseInt(txt.trim());
            return val;
        } catch (NumberFormatException e) {
            throw new CmplException("Can't convert string " + txt + " to long.");
        }
    }
    
    
     /**
     * Converts a XML string into Integer using a regular expression
     * @param txt XML String
     * @param patternStr regular expression
     * @return Long value
     * @throws CmplException 
     */
    static Integer xmlStrToInteger(String txt, String patternStr) throws CmplException {

        int val = 0;

        Pattern pattern = Pattern.compile(patternStr);
        Matcher matcher = pattern.matcher(txt);

        if (matcher.find()) {
            val = CmplTools.xmlStrToInteger(matcher.group(1));
        }
        return val;
    }
    
    
    
    /**
     * Converts a XML string into Long
     * @param txt XML String
     * @return Long value
     * @throws CmplException 
     */
    static Long xmlStrToLong(String txt) throws CmplException {
        try {
            long val;
            val = Long.parseLong(txt.trim());
            return val;
        } catch (NumberFormatException e) {
            throw new CmplException("Can't convert string " + txt + " to long.");
        }
    }

    /**
     * Converts a XML string into Long using a regular expression
     * @param txt XML String
     * @param patternStr regular expression
     * @return Long value
     * @throws CmplException 
     */
    static Long xmlStrToLong(String txt, String patternStr) throws CmplException {

        long val = 0;

        Pattern pattern = Pattern.compile(patternStr);
        Matcher matcher = pattern.matcher(txt);

        if (matcher.find()) {
            val = CmplTools.xmlStrToLong(matcher.group(1));
        }
        return val;
    }

    /**
     * Converts a XML string into Double
     * @param txt XML String
     * @return Double value
     * @throws CmplException 
     */
    static Double xmlStrToDouble(String txt) throws CmplException {
        try {
            double val;
            val = Double.parseDouble(txt.trim());
            return val;
        } catch (NumberFormatException e) {
            throw new CmplException("Can't convert string " + txt + " to double.");
        }
    }

    /**
     * Converts a XML string into String using a regular expression
     * @param txt XML String
     * @param patternStr regular expression
     * @return Double value
     */
    static String xmlStrToString(String txt, String patternStr) {

        String tmpStr = "";

        Pattern pattern = Pattern.compile(patternStr);
        Matcher matcher = pattern.matcher(txt);

        if (matcher.find()) {
            tmpStr = matcher.group(1);
        }
        return tmpStr;
    }
    
    /**
     * Converts a XML string into String list using a regular expression
     * @param txt XML String
     * @param patternStr regular expression
     * @return ArrayList<String> list
     */
    static ArrayList<String> xmlStrToStringList(String txt, String patternString) {
        ArrayList<String> tmpList = new ArrayList<String>();
        Pattern pattern = Pattern.compile(patternString);
        Matcher matcher = pattern.matcher(txt);
        while (matcher.find()) {
            tmpList.add(matcher.group(1));
        }
        return tmpList;
    }
    
    
    /**
     * Deletes a tmp file 
     * @param name file name
     */
    static void delTmpFile(String name) {
        File  f = new File(name);
        if (f.exists()) {
            f.delete();
        }
    }
    
    /**
     * Writes an Ascii file
     *
     * @param name file name
     * @param txt file content
     */
    static void writeAsciiFile(String name, String txt) throws CmplException {
        try {
            BufferedWriter out = new BufferedWriter(new FileWriter(name));
            out.write(txt);
            out.close();
        } catch (IOException e) {
            throw new CmplException("IO error for file " + name + ": " + e);
        }
    }
}