
/***********************************************************************
 *  This code is part of CMPL
 *
 *  Copyright (C) 2007, 2008, 2009, 2010, 2011
 *  Mike Steglich - Technical University of Applied Sciences
 *  Wildau, Germany and Thomas Schleiff - Halle(Saale),
 *  Germany
 *
 *  Coliop3 and CMPL are projects of the Technical University of
 *  Applied Sciences Wildau and the Institute for Operations Research
 *  and Business Management at the Martin Luther University
 *  Halle-Wittenberg.
 *  Please visit the project homepage <www.coliop.org>
 *
 *  CMPL is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  CMPL is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef SOLUTION_HH
#define SOLUTION_HH

#include "../Control/MainData.hh"
#include "OptModel.hh"

using namespace std;



namespace cmpl
{

class Solution;

/**
 * class for storing a model element (variable or constraint) to the solution
 */
class ModelElement {
private:

    string _name;                       ///< name of the variable or constraint
    string _type;                       ///< type of the variable or constraint

    double _lowerBound;                 ///< lower bound of the variable or constraint
    double _upperBound;                 ///< upper bound of the variable or constraint

public:

    /**
     * constructor
     */
    ModelElement();

    /**
     * gets the name of the variable or constraint
     */
    inline string name() { return _name;}

    /**
     * sets the name of the variable or constraint
     * @param name        name of the variable or constraint
     */
    inline void setName(string name) { _name=name;}

    /**
     * gets the type of the variable or constraint
     */
    inline string type() { return _type;}

    /**
     * sets the type of the variable or constraint
     * @param type        type of the variable or constraint
     */
    inline void setType(string type) { _type=type;}

    /**
     * gets the lower bound of the variable or constraint
     */
    inline double lowerBound() {return _lowerBound; }

    /**
     * sets the lower bound of the variable or constraint
     * @param lowerBound        lower bound of the variable or constraint
     */
    inline void setLowerBound(double lowerBound) { _lowerBound=lowerBound;}

    /**
     * gets the upper bound of the variable or constraint
     */
    inline double upperBound() {return _upperBound; }

    /**
     * sets the upper bound of the variable or constraint
     * @param upperBound        upper bound of the variable or constraint
     */
    inline void setUpperBound(double upperBound) { _upperBound=upperBound;}

};

/**
 * class for storing a solution element (variable or constraint)
 */
class SolutionElement {

private:
    double _activity;                ///< activity of the variable or constraint
    double _marginal;                ///< marginal of the variable or constraint

    ModelElement *_modElement;      ///< model element of the variable or constraint


public:

    /**
     * constructor
     */
    SolutionElement();

    /**
     * sets the corresponding model element for the variable or constraint
     * @param modElement        model element
     */
    inline void setModelElement(ModelElement *modElement) { _modElement=modElement;}

    /**
     * gets the name of the variable or constraint
     */
    inline string name() { return _modElement->name();}

    /**
     * gets the type of the variable or constraint
     */
    inline string type() { return _modElement->type();}

    /**
     * gets the activity of the variable or constraint
     */
    inline double activity() { return _activity;}

    /**
     * sets the activity bound of the variable or constraint
     * @param activity        activity  of the variable or constraint
     */
    inline void setActivity(double activity) { _activity=activity;}

    /**
     * gets the lower bound of the variable or constraint
     */
    inline double lowerBound() {return _modElement->lowerBound(); }

    /**
     * gets the upper bound of the variable or constraint
     */
    inline double upperBound() {return _modElement->upperBound(); }

    /**
     * gets the marginal of the variable or constraint
     */
    inline double marginal() { return _marginal;}

    /**
     * sets the marginal bound of the variable or constraint
     * @param marginal        marginal  of the variable or constraint
     */
    inline void setMarginal(double marginal) { _marginal=marginal;}

};


/**
 * class for storing a single solution
 */
class SingleSolution {

private:

    string _status;
    double _value;
    vector<SolutionElement> _variables;
    vector<SolutionElement> _constraints;


public:

    /**
     * @brief Constructor
     */
    SingleSolution();

    /**
     * @brief returns the status of the solution
     * @return      status
     */
    inline string status() { return _status;}

    /**
     * @brief returns the status of the solution
     * @return      status
     */
    inline void setStatus(string status) { _status=status;}

    /**
     * @brief returns the status of the solution
     * @return      status
     */
    inline double value() { return _value;}

    /**
     * @brief sets the objective function value of the solution
     * @param value     objective function value of the solution
     */
    inline void setValue(double value) { _value=value;}

    /**
     * @brief sets the objective function value of the solution
     * @param value     objective function value of the solution
     */
    inline vector<SolutionElement>* variables() { return &_variables;}

    /**
     * @brief adds a variable object to the solution
     * @param variable  SolutionElement object of a variable
     */
    inline void setVariable(SolutionElement& variable) {_variables.push_back(variable);}

    /**
     * @brief returns a pointer to an object of a variable
     * @param idx       Index of the variable
     * @return          pointer to an object of a variable
     */
    inline SolutionElement* variable(unsigned long idx) {return &_variables[idx]; }

    /**
     * @brief returns all constraints
     * @return  pointer to a vector of the SolutionElement objects of all constraints
     */
    inline vector<SolutionElement>* constraints() { return &_constraints;}

    /**
     * @brief adds a constraint object to the solution
     * @param variable  SolutionElement object of a constraint
     */
    inline void setConstraint(SolutionElement& constraint) {_constraints.push_back(constraint);}

    /**
     * @brief returns a pointer to an object of a constraint
     * @param idx       Index of the constraint
     * @return          pointer to an object of a constraint
     */
    inline SolutionElement* constraint(unsigned long idx) {return &_constraints[idx]; }

    /**
     * @brief calculates the activity of a constraint (used for Gurobi and Scip)
     * @param con       pointer to an object of a constraint
     * @param sol       pointer to an object of a single solution
     * @param coeffs    pointer to a vector of coefficients
     * @return          activity of the constraint
     */
    double calculateConActivity(OptModel *om, Solution* sol, const vector<LinearModel::Coefficient>& coeffs);

};




/**
 * @brief The Solution class
 */
class Solution : public MainData::DataBase
{

private:

    map<string,unsigned long>   _colNameMap;
    map<string,unsigned long>   _rowNameMap;

    map<unsigned long,unsigned long>   _colIdxMap;
    map<unsigned long,unsigned long>   _rowIdxMap;

    map<unsigned long,unsigned long>   _colSolIdxMap;

    string _probName;
    string _objName;
    string _objSense;

    unsigned long _nrOfVariables;
    unsigned long _nrOfIntegerVariables;
    unsigned long _nrOfConstraints;

    string _solver;
    string _solverLocation;

    int _nrOfSolutions;

    string _status;

    vector<SingleSolution> _solutions;

    OptModel *_om;

    bool _hasMarginal;

    vector<ModelElement> _modVariables;
    vector<ModelElement> _modConstraints;

    int _addConForUnusedVar;                ///< add pseudo constraint for otherwise unused variable: 0: no / 1: yes if unused because of deletion of a constraint / 2: yes for all)


    /**
     * @brief Returns the bounds of a variable
     * @param ov            Pointer to OptVar object
     * @param lowerBound    lowerBound var
     * @param upperBound    upperBound
     */
    void getVarBounds(OptVar *ov, double &lowerBound, double &upperBound );


public:

    /**
     * @brief Constructor
     */
    Solution (const char *m, MainData *data): MainData::DataBase(m, data)		{ _nrOfSolutions=0; }

    /**
     * @brief Destructor
     */
    inline ~Solution()	{ cleanUp(); }

    /**
     * @brief prepares the Solution data objects
     * @param probName      name of the problem
     * @param solver        name of the solver
     * @param intRelaxation true if integers should be relaxed
     * @param md            pointer to main data object
     * @param mb            pointer to ModulBase object
     */
    void prepareSolutionData(string probName, string solver, bool intRelaxation, int addConForUnusedVar, MainData *md, ModuleBase *mb);

    /**
     * @brief returns the name of the problem
     * @return      name of the problem
     */
    inline string problemName() {return _probName;}

    /**
     * @brief sets the problem name
     * @param name      name of the problem
     * @return
     */
    inline void setProblemName(string name) {_probName=name;}

    /**
     * @brief returns the objective function sense
     * @return      objective function sense <"MIN","MAX">
     */
    inline string objSense() { return _objSense;}

    /**
     * @brief returns the name of the objective function
     * @return      name of the objective function
     */
    inline string objName() { return _objName;}

    /**
     * @brief returns the count of the variables
     * @return      count of the variables
     */
    inline unsigned long nrOfVariables() {return _nrOfVariables;}

    /**
     * @brief returns the count of integer variables
     * @return      count of the integer variables
     */
    inline unsigned long nrOfIntegerVariables() {return _nrOfIntegerVariables;}

    /**
     * @brief returns the count of the constraints
     * @return      count of the constraints
     */
    inline unsigned long nrOfConstraints() {return _nrOfConstraints;}

    /**
     * @brief returns the name of the solver
     * @return      solver name
     */
    inline string solver() { return _solver;}

    /**
     * @brief sets the name of the solver
     * @param  solver       solver name
     */
    inline void setSolver(string& solver) { _solver=solver;}


    /**
     * @brief returns the count of the solutions
     * @return      count of the solutions
     */
    inline int nrOfSolutions() {return _nrOfSolutions;}

    /**
     * @brief sets the count of the solutions
     * @param nr         count of the solutions
     */
    inline void setNrOfSolutions(int nr) {_nrOfSolutions=nr;}

    /**
     * @brief returns the generell status of all solutions
     * @return      generell status
     */
    inline string status() {return _status;}

    /**
     * @brief sets the generell status of all solutions
     * @param status        generell status of all solutions
     */
    inline void setStatus(string status) {_status=status;}

    /**
     * @brief returns all solutions
     * @return      vector of all solutions
     */
    inline vector<SingleSolution> solutions() {return _solutions;}

    /**
     * @brief adds a new single solution
     * @param sol   SingleSolution object
     */
    inline void setSolution(SingleSolution& sol) { _solutions.push_back(sol);}


    /**
     * @brief returns the single solution with index zero
     * @return          SingleSolution object
     */
    inline SingleSolution solution() {return _solutions[0];}

    /**
     * @brief returns a  single solution found
     * @param idx       Index of the single solution
     * @return          SingleSolution object
     */
    inline SingleSolution* solution(int idx) {return &_solutions[idx];}

    /**
     * @brief Returns the index of a variable based on its name
     * @param name      name of the variable
     * @return          index of the variable
     */
    inline unsigned long varMpsIdxByName(string& name) { return ( (( _colNameMap.find(name) != _colNameMap.end()) ) ? _colNameMap[name] : -1);}

    /**
     * @brief Returns the original index of a variable based on its index
     * @param name      name of the variable
     * @return          index of the variable
     */
    inline unsigned long varMpsIdxByIdx(unsigned long idx) { return _colIdxMap[idx];}

    /**
     * @brief Returns the index of a constraint based on its name
     * @param name      name of the constraint
     * @return          index of the constraint
     */
    inline unsigned long conMpsIdxByName(string& name) { return (( ( _rowNameMap.find(name) != _rowNameMap.end()) ) ? _rowNameMap[name] :-1 );}

    /**
     * @brief Returns the original index of a constraint based on its index
     * @param name      name of the constraint
     * @return          index of the constraint
     */
    inline unsigned long conMpsIdxByIdx(unsigned long idx) { return _rowIdxMap[idx];}

    /**
     * @brief returns whether the solution contains marginals
     * @return          true if the solution contains marginals
     */
    inline bool hasMarginal() { return _hasMarginal;}

    /**
     * @brief sets whether the solution contains marginals
     * @param marginal         true if the solution contains marginals
     */
    inline void setHasMarginal(bool marginal) {_hasMarginal=marginal;}

    /**
     * @brief Returns a pointer to a variable object
     * @param idx       index of the variable
     * @return          pointer to the ModelElement object of the variable
     */
    inline ModelElement *modelVariable(unsigned long idx) {return &_modVariables[idx]; }

    /**
     * @brief Returns a pointer to a constraint object
     * @param idx       index of the constraint
     * @return          pointer to the ModelElement object of the constraint
     */
    inline ModelElement *modelConstraint(unsigned long idx) {return &_modConstraints[idx]; }



    /**
     * delete all data
     */
    virtual void cleanUp();

    /**
     * get count of sub data objects
     */
    virtual int subCnt()																		{ return 0; }

    /**
     * if sub data is splitted up to different parts for serialization then get count of these parts, otherwise 0
     */
    virtual int subPartCount()																	{ return 0; }

    /**
     * check if serialization to or from stream should be done for the data object
     * @param si		serialization info
     * @param data		main data object
     * @return			true if serialization should be done
     */
    virtual bool checkSerialize(MainData::SerializeInfo& si, const MainData *data);

    /**
     * write data object to output stream
     * @param si		serialization info
     * @param data		main data object
     */
    virtual void serializeTo(MainData::SerializeInfo& si, const MainData *data);

    /**
     * fill data object from input stream
     * @param si		serialization info
     * @param data		main data object
     * @param pos		position of input string, returns iterated position
     * @param subCnt	count of sub data objects
     * @param lc		line count per element or 0 if elements are len coded
     * @param rline		remaining section start line
     */
    virtual void deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline);






};


}

#endif // SOLUTION_HH
