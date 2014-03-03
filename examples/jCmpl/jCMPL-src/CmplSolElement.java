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

/**
 * CmplSolutionElement - Class for a variable or constraint
 */
public class CmplSolElement {

    private int _idx;
    private String _name;
    private String _type;
    private Object _activity;
    private double _lowerBound;
    private double _upperBound;
    private double _marginal;

    /**
     * Constructor
     */
    public CmplSolElement() {
        _idx = 0;
        _name = "";
        _type = "";
        _activity = 0;
        _lowerBound = 0;
        _upperBound = 0;
        _marginal = 0;

    }

    /**
     * Index of the variable or constraint
     * @return Index of the variable or constraint
     */
    public int idx() {
        return _idx;
    }

    /**
     * Setter of the index of variable or constraint 
     * Used by CmplSolutions
     * @param _idx Index
     */
    protected void setIdx(int _idx) {
        this._idx = _idx;
    }

    /**
     * Name of the variable or constraint
     * @return Name of the variable or constraint
     */
    public String name() {
        return _name;
    }

    /**
     * Setter of the name of variable or constraint 
     * Used by CmplSolutions
     * @param _name name of variable or constraint
     */
    protected void setName(String _name) {
        this._name = _name;
    }

    /**
     * Type of the variable or constraint
     * C|I|B for variables
     * L|E|G for constraints
     * @return Type of the variable or constraint
     */
    public String type() {
        return _type;
    }

    /**
     * Setter of the type of variable or constraint 
     * Used by CmplSolutions
     * @param _type type of variable or constraint 
     */
    protected void setType(String _type) {
        this._type = _type;
    }

    /**
     * Activity of the variable or constraint
     * @return Activity of the variable or constraint
     */
    public Object activity() {
        return _activity;
    }

    /**
     * Setter of the activity of variable or constraint 
     * Used by CmplSolutions
     * @param _activity activity of variable or constraint 
     */
    protected void setActivity(Object _activity) {
        this._activity = _activity;
    }

    /**
     * Lower bound of the variable or constraint
     * @return Lower bound of the variable or constraint
     */
    public double lowerBound() {
        return _lowerBound;
    }

    /**
     * Setter of the lowerBound of variable or constraint 
     * Used by CmplSolutions
     * @param _lowerBound lowerBound of variable or constraint
     */
    protected void setLowerBound(double _lowerBound) {
        this._lowerBound = _lowerBound;
    }

    /**
     * Upper bound of the variable or constraint
     * @return Upper bound of the variable or constraint
     */
    public double upperBound() {
        return _upperBound;
    }

   /**
     * Setter of the upperBound of variable or constraint 
     * Used by CmplSolutions
     * @param _upperBound upperBound of variable or constraint
     */
    protected void setUpperBound(double _upperBound) {
        this._upperBound = _upperBound;
    }

    /**
     * Marginal value (shadow prices or reduced costs) bound of the variable or constraint
     * @return Marginal value of the variable or constraint
     */
    public double marginal() {
        return _marginal;
    }
    
    /**
     * Setter of the marginal of variable or constraint 
     * Used by CmplSolutions
     * @param _marginal ymarginal of variable or constraint 
     */
    protected void setMarginal(double _marginal) {
        this._marginal = _marginal;
    }
}
