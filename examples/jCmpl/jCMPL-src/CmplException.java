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
/**
 * CMPLEXception -Exception handling for jCMPL
 */
public class CmplException extends Exception
{
    private String _msg = "jCmpl error: ";

    public CmplException() {
    }


    public CmplException(String s) {
        _msg += s;
    }
    
    public CmplException(String s, ArrayList<CmplMsg> msgList) {
        _msg += s;
        for ( CmplMsg m : msgList ) {
            _msg += "\n" + m.type() + " in file " + m.file() + " at line " + m.line() + " : " + m.description();
        }
    }
    
    public String msg() {
        return _msg;
    }
    
    @Override
    public String toString() {
        return _msg;
    }

}





  
  

