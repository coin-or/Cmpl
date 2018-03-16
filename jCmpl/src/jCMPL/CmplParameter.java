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
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.List;

/**
 * Handling of CMPL parameters in jCMPL
 */
public class CmplParameter 
{
    
    
    private String _name;
    private Object _valueList;
    private ArrayList<CmplSet> _setList;
    private long _count;
    private int _rank;
    private int _valType; // 1 List 2 Array 3 scalar
    
    /**
     * internal function for setting defaults
     * @param name Name of the set
     * @throws CmplException 
     */
    private void setDefaults(String name) throws CmplException {
        _count = 1;
        _rank = 0;

        Pattern pattern = Pattern.compile("\\s");
        Matcher matcher = pattern.matcher(name);

        if (!matcher.find()) {
            this._name = name;
        } else {
            throw new CmplException("parameter name " + _name + " contains whitespaces");
        }
        
        _rank = 0;
        _setList = new ArrayList<CmplSet>();
    }
    
    /**
     * Constructor for a scalar parameter 
     * @param name Name of the set
     * @throws CmplException 
     */
    public CmplParameter(String name) throws CmplException {
        setDefaults(name);
    }
    
    /**
     * Constructor for a parameter array
     * @param name Name of the set
     * @param sets CmplSets over that the parameter is defined
     * @throws CmplException 
     */
    public CmplParameter(String name, CmplSet... sets) throws CmplException {
        setDefaults(name);
        for (CmplSet set : sets) {
            _setList.add(set);
            _rank += set.rank();
        }
    }
    
    /**
     * Internal function to count all set elements
     * @throws CmplException 
     */
    private long countElements(Object val) throws CmplException {
        long _tmpcount = 0;
        if (val.getClass().toString().contains("List")) {
            
            _tmpcount = ((ArrayList)val).size();
            
            if (((ArrayList)val).get(0).getClass().toString().contains("List") || ((ArrayList)val).get(0).getClass().isArray() ) {
                 _tmpcount *= countElements( ((ArrayList)val).get(0));
            } 
        } else if (val.getClass().isArray()) {
            
              if (val instanceof int[]) {
                  _tmpcount = ((int[])val).length;
                  _tmpcount *= countElements( ((int[])val)[0] );
              } else if (val instanceof long[]) {
                  _tmpcount = ((long[])val).length;
                  _tmpcount *= countElements( ((long[])val)[0] );
              } else if (val instanceof float[]) {
                  _tmpcount = ((float[])val).length; 
                  _tmpcount *= countElements( ((float[])val)[0] );                  
              } else if (val instanceof double[]) {
                  _tmpcount = ((double[])val).length;
                   _tmpcount *= countElements( ((double[])val)[0] );
              } else if (val instanceof Object[]) {
                  _tmpcount = ((Object[])val).length;
                   _tmpcount *= countElements( ((Object[])val)[0] );
              } else {
                    throw new CmplException("parameter name " + _name + " containts data with unsupported data type" + val.getClass().toString());
              }
                
        } else { return 1; }
        
        return _tmpcount;
    }

    /**
     * Internal function to set the value of a scalar param
     * @param val value of the parameter
     * @throws CmplException 
     */
    private void setScalarParam(Object val) throws CmplException {
        if (_rank > 0) {
            throw new CmplException("incompatible data for a parameter array : " + val.toString());
        }

        _valueList = val;
        _valType = 3;
    }

    /**
     * Sets the value of a scalar Integer parameter
     * @param val value
     * @throws CmplException 
     */
    public void setValues(Integer val) throws CmplException {
        setScalarParam(val);
    }

    /**
     * Sets the value of a scalar Long parameter
     * @param val value
     * @throws CmplException
     */
    public void setValues(Long val) throws CmplException {
        setScalarParam(val);
    }
    
    
    /**
     * Sets the value of a scalar Float parameter
     * @param val value
     * @throws CmplException
     */
    public void setValues(Float val) throws CmplException {
        setScalarParam(val);
    }
    
     
   /**
     * Sets the value of a scaler Double parameter
     * @param val value
     * @throws CmplException
     */
    public void setValues(Double val) throws CmplException {
        setScalarParam(val);
    }
    

    /**
     * Sets the value of a scalar String parameter
     * @param val value
     * @throws CmplException
     */
    public void setValues(String val) throws CmplException {
        setScalarParam(val);
    }
    
    
  /**
   * Defines the values of a parameter array
   * @param val List or array of parameter values
   * @throws CmplException 
   */
    public void setValues(Object val) throws CmplException {
        
        if (!val.getClass().toString().contains("List") && !val.getClass().isArray()) {
            throw new CmplException("Incompatible data - not a list or an array - for parameter :" + _name);
        }
         
        if (_rank == 0) {
            throw new CmplException("incompatible data for a scalar parameter : " + val.toString());
        }

        _valueList =  val;
        
        if (val.getClass().toString().contains("List")) {
            _valType=1;
        } else if (val.getClass().isArray()) {
            _valType=2;
        }
       
        long sCount = 1;
        for (int i = 0; i < _setList.size(); i++) {
            sCount *= _setList.get(i).len();
        }

        _count = countElements(_valueList);
        if (_count != sCount) {
            throw new CmplException("The dimension of the paramter " + _name + " doesn't match the dimension of the set(s).");
        }
        
    }

    /**
     * Returns a List of the invoked sets
     * @return set list
     */
    public List<CmplSet> setList() {
        return _setList;
    }

    /**
     * List of the values of a parameter 
     * @return value list
     */
    public Object values() {
        return _valueList;
    }

    /**
     * Value of a scalar parameter 
     * @return value of the scalar parameter
     */
    public Object value() {
        return _valueList;
    }

    /**
     * Name of the parameter
     * @return  name of the CMPL parameter (not the name of the CmplParameter object)
     */
    public String name() {
        return _name;
    }

    /**
     * Rank of the parameter
     * @return rank of the CMPL parameter 
     */
    public int rank() {
        return _rank;
    }

    /**
     * Length of the parameter array
     * @return number of elements in the parameter array
     */
    public long len() {
        return _count;
    }
   
    /**
     * Returns whether the parameter is organized as list, array or single value
     * @return valType
     */
    public int valType() {
        return _valType;
    }
    
}
