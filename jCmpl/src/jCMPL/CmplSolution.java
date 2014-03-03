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
import java.util.ArrayList;

import java.io.FileReader;
import java.io.IOException;
import java.util.Arrays;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Solution handling of jCMPL
 */
public class CmplSolution {

    private int _idx;
    private String _status;
    private double _value;
    private ArrayList<CmplSolElement> _variables;
    private ArrayList<CmplSolElement> _constraints;

    /**
     * Constructor
     */
    public CmplSolution() {
        _idx = 0;
        String status = "";
        _value = 0;
        _variables = new ArrayList<CmplSolElement>();
        _constraints = new ArrayList<CmplSolElement>();

    }

    /**
     * Returns the index of the current solution 
     * @return index
     */
    public int idx() {
        return _idx;
    }

    /**
     * Setter of the solution index 
     * Used by CmplSolutions.readSolution
     * @param _idx solution index
     */
    protected void setIdx(int _idx) {
        this._idx = _idx;
    }

    /**
     * Returns the a string with the status of the current solution 
     * provided by the invoked solver
     * @return solution status
     */
    public String status() {
        return _status;
    }

    /**
     * Setter for the solution status
     * Used by CmplSolutions.readSolution
     * @param _status solution status
     */
    protected void setStatus(String _status) {
        this._status = _status;
    }

    /**
     * Returns the value of the objective function of the current solution 
     * @return Objective value
     */
    public double value() {
        return _value;
    }

    /**
     * Setter for the obj value
     * Used by CmplSolutions.readSolution
     * @param value  obj value
     */
    protected void setValue(double value) {
        this._value = value;
    }

    /**
     * Returns a list of CmplSolLine objects for the variables of the current solution 
     * @return list of CmplSolLine objects
     */
    public ArrayList<CmplSolElement> variables() {
        return _variables;
    }

    /**
     * Setter for the variable list
     * Used by CmplSolutions.readSolution
     * @param var CmplSolElement 
     */
    protected void setVar(CmplSolElement var) {
        _variables.add(var);
    }

    /**
     * Returns a list of CmplSolLine objects for the constraints 
     * of the current solution 
     * @return list of CmplSolLine objects
     */
    public ArrayList<CmplSolElement> constraints() {
        return _constraints;
    }

    /**
     * Setter for the constraint list 
     * Used by CmplSolutions.readSolution
     * @param con CmplSolElement
     */
    protected void setCon(CmplSolElement con) {
        _constraints.add(con);
    }
}

/**
 * JCMPL handling of all solutions
 */
class CmplSolutions {

    private long _nrOfVariables;
    private long _nrOfConstraints;
    private String _objectiveName;
    private String _objectiveSense;
    private int _nrOfSolutions;
    private String _solverName;
    private String _solverMsg;
    private String _varDisplayOptions;
    private String _conDisplayOptions;
    private String _asciiSolFile;
    private String _csvSolFile;
    private String _cmplSolFile;
    private ArrayList<CmplSolution> _solList;
    private String _solutionFile;
    private ArrayList<String> _solFileContent;
    private boolean _isIntegerProgram;

    /**
     * Constructor
     */
    public CmplSolutions(String solFile) {
        setDefaults();
        _solutionFile = solFile;

    }

    /**
     * Constructor
     */
    public CmplSolutions() {
        setDefaults();
    }

    /**
     * set the defaults 
     */
    private void setDefaults() {
        _nrOfVariables = 0;
        _nrOfConstraints = 0;
        _objectiveName = "";
        _objectiveSense = "";
        _nrOfSolutions = 0;
        _solverName = "";
        _solverMsg = "";
        _varDisplayOptions = "";
        _conDisplayOptions = "";
        _asciiSolFile = "";
        _csvSolFile = "";
        _cmplSolFile = "";
        _solList = new ArrayList<CmplSolution>();
        _solutionFile = "";
        _isIntegerProgram = false;
    }

    // Getter and Setter Methods
    
    /**
     * Returns the number of variables of the generated and solved CMPL model
     * Used in Cmpl
     * @return Number of variables
     */
    public long nrOfVariables() {
        return _nrOfVariables;
    }

    /**
     * Returns the number of constraints of the generated and solved CMPL model
     * Used in Cmpl
     * @return Number of constraints
     */
    public long nrOfConstraints() {
        return _nrOfConstraints;
    }

    /**
     * Returns the name of the objective function of the generated and solved CMPL model
     * Used in Cmpl
     * @return objective name
     */
    public String objectiveName() {
        return _objectiveName;
    }

    /**
     * Returns the objective sense of the generated and solved CMPL model
     * Used in Cmpl
     * @return objective sense
     */
    public String objectiveSense() {
        return _objectiveSense;
    }

    /**
     * Returns the number of solutions of the generated and solved CMPL model
     * Used in Cmpl
     * @return Number of solutions
     */
    public int nrOfSolutions() {
        return _nrOfSolutions;
    }

    /**
     * Returns the name of the invoked solver of the generated and solved CMPL model
     * Used in Cmpl
     * @return Invoked solve
     */
    public String solver() {
        return _solverName;
    }

    /**
     * Returns the message of the invoked solver of the generated and solved CMPL model
     * @return Message of the invoked solver
     */
    public String solverMessage() {
        return _solverMsg;
    }

    /**
     * Returns a string with the display options for the constraints of the generated and solved CMPL model
     * Used in Cmpl
     * @return Display options for the constraints
     */
    public String conDisplayOptions() {
        return _conDisplayOptions;
    }

    /**
     * Returns a string with the display options for the variables of the generated and solved CMPL model
     * @return Display options for the variables
     */
    public String varDisplayOptions() {
        return _varDisplayOptions;
    }

    /**
     * Returns the file name of the Ascii solution file 
     * @return file name of the Ascii solution file 
     */
    public String asciiSolFile() {
        return _asciiSolFile;
    }

    /**
     * Returns the file name of the Csv solution file 
     * @return file name of the Csv solution file 
     */
    public String csvSolFile() {
        return _csvSolFile;
    }

    /**
     * Returns the file name of the CmplSolution file 
     * @return file name of the CmplSolution file 
     */
    public String cmplSolFile() {
        return _cmplSolFile;
    }

    
    /**
     * Returns a list of CmplSolutions objects 
     * Used in Cmpl
     * @return  List of CmplSolution object for solutions found 
     */
    public ArrayList<CmplSolution> solutions() {
        return _solList;
    }

    /**
     * Returns the first (optimal) CmplSolutions object 
     * Used in Cmpl
     * @return first (optimal) solution 
     */
    public CmplSolution solution() {
        return _solList.get(0);
    }
    
    /**
     * Returns the content of the CmplSolution file  
     * Used in Cmpl
     * @return content of the CmplSolution as an ArrayList
     */
    public ArrayList<String> solFileContent() {
        return _solFileContent;
    }
    
    /**
     * Deltes the content of the CmplSolution file  
     * Used in Cmpl
     */
    public void delSolFileContent() {
        _solFileContent.clear();
    }

    /**
     * Returns whether the model is integer or not 
     * @return True | False 
     */
    public boolean isIntegerProgram() {
        return _isIntegerProgram;
    }

    
    

    /**
     * Internal function for reading vars and cons
     * @param line Solution line
     * @return CmplSolElement of a var or con
     * @throws CmplException 
     */
    private CmplSolElement readSolLine(String line) throws CmplException {

        ArrayList<String> tmpList = new ArrayList<String>();

        String patternStr = "\"([^\"]*)\"";

        Pattern pattern = Pattern.compile(patternStr);
        Matcher matcher = pattern.matcher(line);

        while (matcher.find()) {
            tmpList.add(matcher.group(1));
        }

        CmplSolElement x = new CmplSolElement();
        x.setIdx(CmplTools.xmlStrToInteger(tmpList.get(0)));
        x.setName(tmpList.get(1));
        x.setType(tmpList.get(2));


        if (line.contains("<constraint")) {
            x.setActivity(CmplTools.xmlStrToDouble(tmpList.get(3)));
        } else {
            if (x.type().equals("I") || x.type().equals("B")) {
               x.setActivity(CmplTools.xmlStrToLong(tmpList.get(3)));
               _isIntegerProgram=true;
            } else {
               x.setActivity(CmplTools.xmlStrToDouble(tmpList.get(3)));
            }
        }

        x.setLowerBound(CmplTools.xmlStrToDouble(tmpList.get(4)));
        x.setUpperBound(CmplTools.xmlStrToDouble(tmpList.get(5)));

        if (tmpList.get(6).equals("-")) {
            x.setMarginal(Float.NaN);
        } else {
            x.setMarginal(CmplTools.xmlStrToDouble(tmpList.get(6)));
        }

        return x;
    }

    /**
     * Internal function for reading solutions
     * @throws CmplException 
     */
    protected void readSolution() throws CmplException {
        readSolution("");
    }
    
    /**
     * Internal function for reading solutions
     * @throws CmplException 
     */
    protected void readSolution(String solStr) throws CmplException {

        if (solStr.isEmpty() && _solutionFile.isEmpty()) {
            throw new CmplException("Solver failed or cannot be executed");
        }

        _solList.clear();
  
        if (!_solutionFile.isEmpty()) {
            String line = "";
            try {
                BufferedReader in = new BufferedReader(new FileReader(_solutionFile));
                _solFileContent = new ArrayList<String>();

                while ((line = in.readLine()) != null) {
                    _solFileContent.add(line);
                }

                in.close();

            } catch (IOException e) {
                throw new CmplException("Cannot read solution file :" + _solutionFile + " ->" + e);
            }
        } else {
            _solFileContent = new ArrayList<String>(Arrays.asList(solStr.split("\n")));
        }


        long lineNr = 1;
        Boolean generalSection = false;
        Boolean solutionSection = false;
        Boolean varSection = false;
        Boolean conSection = false;
        CmplSolution s = null;
        //CmplSolution s = new CmplSolution();
       

        for (String line : _solFileContent) {

            if (lineNr == 1) {
                if (!line.contains("<?xml version")) {
                    throw new CmplException("File " + _solutionFile + " - is not a XML file!");
                } else {
                    lineNr++;
                    continue;
                }
            }

            if (lineNr == 2) {
                if (!line.contains("<CmplSolutions")) {
                    throw new CmplException("Can't read cmplSolution file " + _solutionFile + " - wrong file type!");
                } else {
                    lineNr++;
                    continue;
                }
            }

            if (line.contains("<general>")) {
                generalSection = true;
                solutionSection = false;
                continue;
            }

            if (line.contains("</general>")) {
                generalSection = false;
                continue;
            }

            if (line.contains("<solution")) {
                generalSection = false;
                solutionSection = true;

                ArrayList<String> tmpList = new ArrayList<String>();
                String patternStr = "\"([^\"]*)\"";

                Pattern pattern = Pattern.compile(patternStr);
                Matcher matcher = pattern.matcher(line);
                while (matcher.find()) {
                    tmpList.add(matcher.group(1));
                }
                s =  new CmplSolution();
                s.setIdx(CmplTools.xmlStrToInteger(tmpList.get(0)));
                s.setStatus(tmpList.get(1));
                s.setValue(CmplTools.xmlStrToDouble(tmpList.get(2)));
                continue;
            }

            if (line.contains("</solution")) {
                solutionSection = false;
                _solList.add(s);
                continue;
            }

            if (generalSection) {

                if (line.contains("<nrOfVariables")) {
                    _nrOfVariables = CmplTools.xmlStrToLong(line, "<nrOfVariables>(.*)</nrOfVariables>");
                    continue;
                }
                if (line.contains("<nrOfConstraints")) {
                    _nrOfConstraints = CmplTools.xmlStrToLong(line, "<nrOfConstraints>([^\"]*)</nrOfConstraints>");
                    continue;
                }
                if (line.contains("<objectiveName")) {
                    _objectiveName = CmplTools.xmlStrToString(line, "<objectiveName>([^\"]*)</objectiveName>");
                    continue;
                }
                if (line.contains("<objectiveSense")) {
                    _objectiveSense = CmplTools.xmlStrToString(line, "<objectiveSense>([^\"]*)</objectiveSense>");
                    continue;
                }
                if (line.contains("<nrOfSolutions")) {
                    _nrOfSolutions = CmplTools.xmlStrToInteger(line, "<nrOfSolutions>([^\"]*)</nrOfSolutions>");
                    continue;
                }
                if (line.contains("<solverName")) {
                    _solverName = CmplTools.xmlStrToString(line, "<solverName>([^\"]*)</solverName>");
                    continue;
                }
                if (line.contains("<solverMsg")) {
                    _solverMsg = CmplTools.xmlStrToString(line, "<solverMsg>([^\"]*)</solverMsg>");
                    continue;
                }
                if (line.contains("<variablesDisplayOptions")) {
                    _varDisplayOptions = CmplTools.xmlStrToString(line, "<variablesDisplayOptions>([^\"]*)</variablesDisplayOptions>");
                    continue;
                }
                if (line.contains("<constraintsDisplayOptions")) {
                    _conDisplayOptions = CmplTools.xmlStrToString(line, "<constraintsDisplayOptions>([^\"]*)</constraintsDisplayOptions>");
                    continue;
                }
                if (line.contains("<asciiSolFile")) {
                    _asciiSolFile = CmplTools.xmlStrToString(line, "<asciiSolFile>([^\"]*)</asciiSolFile>");
                    continue;
                }
                if (line.contains("<csvSolFile")) {
                    _csvSolFile = CmplTools.xmlStrToString(line, "<csvSolFile>([^\"]*)</csvSolFile>");
                    continue;
                }
                 if (line.contains("<cmplSolFile")) {
                    _cmplSolFile = CmplTools.xmlStrToString(line, "<cmplSolFile>([^\"]*)</cmplSolFile>");
                    continue;
                }
                
            }
            if (solutionSection) {
                if (line.contains("<variables>")) {
                    varSection = true;
                    conSection = false;
                    continue;
                }
                if (line.contains("</variables>")) {
                    varSection = false;
                    continue;
                }
                if (line.contains("<linearConstraints>")) {
                    varSection = false;
                    conSection = true;
                    continue;
                }
                if (line.contains("</linearConstraints>")) {
                    conSection = false;
                    continue;
                }
                if (varSection) {
                    if (line.contains("<variable")) {
                        s.setVar(readSolLine(line));
                    }
                }
                if (conSection) {
                    if (line.contains("<constraint")) {
                        s.setCon(readSolLine(line));
                    }
                }
            }
        }
    }

}
