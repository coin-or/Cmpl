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


import java.util.List;
import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Handling of CMPL sets in jCMPL
 */
public class CmplSet
{
  
    private String _name;
    private int _type ; 
    private Object _valueList;
    private Object[] _valueArray;
    private int _rank;
    private int _valType; // 1 List 2 Array 3 Array fuer allgsets
    private int _count;
    
    
    /**
     * internal function for default settings
     * @throws CmplException 
     */
    private void setDefaults(String name) throws CmplException {
        
        Pattern pattern = Pattern.compile("\\s");
        Matcher matcher = pattern.matcher(name);
        
        if(!matcher.find())
        {
            this._name = name;
        }
        else
        {
            throw new CmplException("Set name " + name + " contains whitespaces");
        }  
        _type=0;// 0 enumeration set - 1 tuple set - 2 alg set n..m - 3 alg set n(k)m 
        _rank = 1;
        _valueList= null;
        _count = -99;
     }
    
    /**
     * Constructor for a set with rank
     * @param name name of the set
     * @param rank rank of the set
     * @throws CmplException 
     */
    public CmplSet(String name, int rank) throws CmplException
    {
       setDefaults(name);
        _rank = rank;
    }
    
    /**
     * Constructor for a set with rank=1
     * @param name name of the set
     * @throws CmplException 
     */
    public CmplSet(String name) throws CmplException
    {
        setDefaults(name);
    }
    
    /**
     * Name of the set
     * @return name of the CMPL set (not the name of the CmplSet object)
     */
    public String name()
    {
        return this._name;
    }
    
    /**
     * Returns the type of the set
     * @return type 
     */
    public int type()
    {
        return this._type;
    }
    
    /**
     * Rank of the set
     * @return number of n of a n-tuple set
     */
    public int rank()
    {
        return this._rank;
    }
    
    /**
     * Length of the set
     * @return number of indexing entries
     * @throws CmplException 
     */
    public int len() throws CmplException
    {
        if (_count!=-99 )
            return  _count;
        
        _count = 0 ;
        
        if(_type == 0 || _type == 1)
        {
            if (_valType == 1)
                _count = ((ArrayList)_valueList).size();
            else
              if (_valueList instanceof int[]) {
                  _count = ((int[])_valueList).length;
              } else if (_valueList instanceof long[]) {
                 _count = ((long[])_valueList).length;
              } else if (_valueList instanceof Integer[]) {
                  _count = ((Integer[])_valueList).length;                  
              } else if (_valueList instanceof Long[]) {
                  _count = ((Long[])_valueList).length;
              } else if (_valueList instanceof String[]) {
                 _count = ((String[])_valueList).length;
               } else if (_valueList instanceof Object[]) {
                 _count = ((Object[])_valueList).length;
              } else {
                    throw new CmplException("set " + _name + " containts data with unsupported data type" + _valueList.getClass().toString());
              }
        }
        else if(_type == 2)        
        {
            _count = (Integer)_valueArray[1] - (Integer)_valueArray[0] + 1; 
        }
        else if(_type == 3)
        {
            if ( (Integer)_valueArray[1]>0) {
                _count = (int) (Math.ceil( Double.valueOf(   (Integer)_valueArray[2]-(Integer)_valueArray[0]+1) / Math.abs((Integer)_valueArray[1]) ));
            } else {
                _count = (int) Math.ceil (Double.valueOf(  (Integer)_valueArray[0]-(Integer)_valueArray[2] +1  )/ Math.abs((Integer)_valueArray[1]));
            }
        }
        return  _count;
    }
    
    /**
     * Returns the indexing entry at a given position 
     * @param i position
     * @return Object of the indexing entry
     * @throws CmplException 
     */
    public Object get(int i) throws CmplException {
        
        if (_type == 1 ) {
            throw new CmplException( "CmplSet.get(int i) is not apllicable for tuple sets with rank>1 -> "+_name );
        }
        Object tmpVal = null;
        boolean err = false;
        if (_valType == 1) {
            if (i < ((ArrayList) _valueList).size()) {
                tmpVal = ((ArrayList) _valueList).get(i);
            } else {
                err = true;
            }
        } else if (_valType == 2) {
            if (i < _count) {
                if (_valueList instanceof int[]) {
                    tmpVal = ((int[]) _valueList)[i];
                } else if (_valueList instanceof long[]) {
                    tmpVal = ((long[]) _valueList)[i];
                } else if (_valueList instanceof Integer[]) {
                    tmpVal = ((Integer[]) _valueList)[i];
                } else if (_valueList instanceof Long[]) {
                    tmpVal = ((Long[]) _valueList)[i];
                } else if (_valueList instanceof String[]) {
                    tmpVal = ((String[]) _valueList)[i];
                } else {
                    throw new CmplException( "unsupported data type for the set: "+_name );
                }
            } else {
                err = true;
            }
        } else if (_valType == 3) {
            if (i < _valueArray.length) {
                tmpVal = _valueArray[i];
            } else {
                err = true;
            }
        }

        if (err) {
            throw new CmplException("Index out of range for set " + _name);
        }
        return tmpVal;
    }
     /**
     * Returns the indexing entry at a given position 
     * @param i position in the tuple list|array
     * @param j position in the tuple
     * @return Object of the indexing entry
     * @throws CmplException 
     */
     public Object get(int i, int j) throws CmplException {
        
        if (_type != 1) {
            throw new CmplException("CmplSet.get(int i, int j) is only apllicable for tuple sets with rank>1 -> " + _name);
        }
        Object tmpVal = null;
        boolean err = false;
        try {
            if (_valType == 1) {
                if (i < ((ArrayList) _valueList).size()) {
                    tmpVal = ((ArrayList) (((ArrayList) _valueList).get(i))).get(j);
                } else if ((((ArrayList) _valueList).get(i)).getClass().isArray()) {
                    if (((ArrayList) _valueList).get(i) instanceof int[]) {
                        tmpVal = ((int[]) ((ArrayList) _valueList).get(i))[j];
                    } else if (((ArrayList) _valueList).get(i) instanceof long[]) {
                        tmpVal = ((long[]) ((ArrayList) _valueList).get(i))[j];
                    } else if (((ArrayList) _valueList).get(i) instanceof Integer[]) {
                        tmpVal = ((Integer[]) ((ArrayList) _valueList).get(i))[j];
                    } else if (((ArrayList) _valueList).get(i) instanceof Long[]) {
                        tmpVal = ((Long[]) ((ArrayList) _valueList).get(i))[j];
                    } else if (((ArrayList) _valueList).get(i) instanceof String[]) {
                        tmpVal = ((String[]) ((ArrayList) _valueList).get(i))[j];
                    }
                }
            } else if (_valType == 2) {
                if (_valueList instanceof int[][]) {
                    tmpVal = ((int[][]) _valueList)[i][j];
                } else if (_valueList instanceof long[][]) {
                    tmpVal = ((long[][]) _valueList)[i][j];
                } else if (_valueList instanceof Integer[][]) {
                    tmpVal = ((Integer[][]) _valueList)[i][j];
                } else if (_valueList instanceof Long[][]) {
                    tmpVal = ((Long[][]) _valueList)[i][j];
                } else if (_valueList instanceof String[][]) {
                    tmpVal = ((String[][]) _valueList)[i][j];
                } 
            }

        } catch  (Exception e)  {
            throw new CmplException("Index out of range for set " + _name);
    }
    return tmpVal ;
}
    
    /**
     * Defines the values of an enumeration set
     * For a set of n-tuples with n=1 - list|array of single indexing entries 
     * For a set of n-tuples with n>1 - list|array of list(s) that contain tuples 
     * @param val value list|array 
     * @throws CmplException 
     */
    public void setValues(Object val) throws CmplException {
        
        if (!val.getClass().toString().contains("List") && !val.getClass().isArray()) {
            throw new CmplException("Incompatible data - not a list or an array - for set :" + _name);
        }

        if (val.getClass().toString().contains("List")) {
            _valType=1;
            if (((ArrayList)val).get(0).getClass().toString().contains("List") || (((ArrayList)val).get(0).getClass().isArray()) ) {
                _type = 1;
            } else {
                _type = 0;
            }
        } else if (val.getClass().isArray()) {
            _valType = 2;
            if (val instanceof int[] || val instanceof long[] || val instanceof Integer[] || val instanceof Long[] || val instanceof String[]) {
                _type = 0;
            } else if ((val instanceof Object[])) {
                if (((Object[]) val)[0].getClass().isArray() || ((Object[]) val)[0].getClass().toString().contains("List")) {
                    _type = 1;
                } else {
                    _type = 0;
                }
            } else {
                throw new CmplException("Incompatible data for set :" + _name);
            }
        }
        
        if (_type==0 && _rank>1) {
                throw new CmplException("incorrect definition of the tuple set: " + _name);
        }   
        _valueList=val;
        _count=-99;
        len();
        
    }

  /**
     * Defines the values of an algorithmic set 
     * (startNumber, startNumber+1, ..., endNumber)
     * @param val1 startNumber
     * @param val2 endNumber
     * @throws CmplException 
     */
    public void setValues(int val1, int val2) throws CmplException {
        _type= 2;
        _rank = 1;
        if (val2 < val1) {
             throw new CmplException("unexpected values for set " + _name + " : end value "+  String.valueOf(val2) + " is less then start value " +  String.valueOf(val1)) ;
        }
        _valueArray  = new Object[2];
        _valueArray[0] = val1;
        _valueArray[1] = val2;
        _valType=3;
        _count=-99;
        len();
    }
    
    
    
    /**
     * Defines the values of an algorithmic set 
     * (startNumber, startNumber+step, ..., endNumber)
     * @param val1 startNumber
     * @param val2 step
     * @param val3 endNumber
     * @throws CmplException 
     */
    public void setValues(int val1, int val2, int val3) throws CmplException {
        _type= 3;
        _rank = 1;
        if (val2>0) {
            if (val3 <= val1) {
                throw new CmplException("unexpected values for set " + _name + " : end value "+ String.valueOf(val3) + " is less than or equal to start value " + String.valueOf(val1) +" and the increment is positive "+ String.valueOf(val2) );
            }
        } else {
            if (val2<0) {
                if (val1 <= val3) {
                    throw new CmplException("unexpected values for set " + _name + " : end value " + String.valueOf(val3) + " is greater than or equal to start value " + String.valueOf(val1) + " and the increment is negative " + String.valueOf(val2));
                }
            } else {
                throw new CmplException("unexpected values for the set " + _name + " :  increment/decrement equals zero  ");
            }
            
        }
        
        _valueArray  = new Object[3];
        _valueArray[0] = val1;
        _valueArray[1] = val2;
        _valueArray[2] = val3;
        _valType=3;
        _count=-99;
        len();
    }
    
  
    /**
     * Returns valType
     * @return type of a value
     */
    public int valType() {
        return _valType;
    }
    
    /**
     * List|Array of the indexing entries of the set List|Array of single indexing entries for a set of n-tuples with n=1 
     * List|Array of tuple(s) - for a set of n-tuples with n>1
     * @return List<Object> or Array of the indexing entries
     */
    public Object values() {
        List<Object> valList = null;
        
        if (_type==0 || _type==1) {
            return _valueList;
        } else if (_type==2) {
            valList = new ArrayList();
            for (Integer i =(Integer)_valueArray[0]; i<=(Integer)_valueArray[1]; i++ ) {
                valList.add(i);
            }
        } 
        if (_type==3) {
            valList = new ArrayList();
            if ((Integer)_valueArray[1] > 0) {
                for (Integer i = (Integer)_valueArray[0]; i <= (Integer)_valueArray[2]; i += (Integer)_valueArray[1]) {
                    valList.add(i);
                }
            } else {
                for (Integer i = (Integer)_valueArray[0]; i >= (Integer)_valueArray[2]; i += (Integer)_valueArray[1]) {
                    valList.add(i);
                }

            }
        } 
        return valList;
        
    }

}
