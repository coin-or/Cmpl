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
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;

/**
 * Handling of the solution of a var or con array
 */
public class CmplSolArray    {
    
    private HashMap<String,CmplSolElement> map;
    
    public CmplSolArray() {
        map =  new HashMap<String,CmplSolElement>();
    }

    public void put(String key , CmplSolElement val) {
        map.put(key, val);
    }
 
    
    private String concatKeys(List<Object> valList) {
        String keyString = "";
        long i = 0;
        int ol = valList.size();

        for (Object o : valList) {
            keyString += String.valueOf(o);
            if (i < ol - 1) {
                keyString += ",";
            }
            i++;
        }
        return keyString;
    }
            
            
    public CmplSolElement get(Object key) {
        CmplSolElement tmpVal=null;
        ArrayList<Object>keyList=null;
        String keyString="";
        
        if (key.getClass().toString().contains("List")) {
            keyList = new ArrayList<Object>();
            for (Object o : (List<Object>)key) {
                keyList.add(o);
            }
            keyString = concatKeys(keyList);
            tmpVal = map.get(keyString);
            
        } else if (key.getClass().isArray()) {
            keyList = new ArrayList<Object>();
            if (key instanceof int[]) {
                for (int i = 0; i < ((int[]) key).length; i++) {
                    keyList.add(((int[])key)[i]);
                }
            } else if (key instanceof long[]) {
                for (int i = 0; i < ((long[]) key).length; i++) {
                    keyList.add(((long[])key)[i]);
                }
            } else if (key instanceof Integer[]) {
                for (int i = 0; i < ((Integer[]) key).length; i++) {
                    keyList.add(((Integer[]) key)[i]);
                }
            } else if (key instanceof Long[]) {
                for (int i = 0; i < ((Long[]) key).length; i++) {
                    keyList.add(((Long[]) key)[i]);
                }
            } else if (key instanceof String[]) {
                for (int i = 0; i < ((String[]) key).length; i++) {
                    keyList.add(((String[]) key)[i]);
                }
            }
            keyString = concatKeys(keyList);
            tmpVal = map.get(keyString);

        } else {
            tmpVal = map.get(key.toString());
        }
        
        return tmpVal;
    }
    
  
    public CmplSolElement get(Object... objList) {
        String keyString=concatKeys(Arrays.asList(objList))     ;  
        return map.get(keyString);
    }
    
    
    
}
