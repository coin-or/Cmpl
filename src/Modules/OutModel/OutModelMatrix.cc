
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


#include <iomanip>

#include "OutModelMatrix.hh"
#include "OutModelExtData.hh"
#include "../../Control/MainControl.hh"
#include "../../Control/ModulesConf.hh"


namespace cmpl
{
/*********** module definition **********/

MODULE_CLASS( writeMatrix, OutModelMatrix )



/*********** initialization **********/

/**
     * initialize modul, to call immediately after the constructor.
     * @param ctrl			main object
     * @param data			data object
     * @param name			module name (or alias)
     */
void OutModelMatrix::init(MainControl *ctrl, MainData *data, const char *name)
{
    ModuleBase::init(ctrl, data, name);

    _outMatrix = NULL;
    _objName="";
}

/**
     * destructor
     */
OutModelMatrix::~OutModelMatrix()
{
    if (_outMatrix)
        delete _outMatrix;

}


/*********** handling of command line options **********/

#define OPTION_OUT_MODEL_MATRIX           10
#define OPTION_OUT_MODEL_MATRIX_OBJNAME    20



/**
     * register command line options options for delivery to this module
     * @param modOptReg		vector to register in
     */
void OutModelMatrix::regModOptions(vector<CmdLineOptList::RegOption> &modOptReg)
{
    ModuleBase::regModOptions(modOptReg);

    REG_CMDL_OPTION( OPTION_OUT_MODEL_MATRIX , "matrix", 0, 1, CMDL_OPTION_NEG_NO_ARG, true );
    REG_CMDL_OPTION( OPTION_OUT_MODEL_MATRIX_OBJNAME, "obj", 1, 1, CMDL_OPTION_NEG_NO_ARG, false );

}

/**
     * parse single option from command line options, this function is called for every delivered option
     * @param ref			reference number of option registration, should be used for discriminate the options
     * @param prio			priority value of option
     * @param opt			option
     * @return				true if option is used by the module
     */
bool OutModelMatrix::parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt)
{
    if (ModuleBase::parseOption(ref, prio, opt))
        return true;

    switch (ref) {
    case OPTION_OUT_MODEL_MATRIX:
        if (_outMatrix) {
            delete _outMatrix;
            _outMatrix = NULL;
        }
        if (!(opt->neg())) {
            _outMatrix = new FileOutput();
            _outMatrix->setFile(_data, IO_MODE_FILE, (opt->size() > 0 ? &((*opt)[0]) : NULL), IO_FILE_STANDARD_MATRIX, true);
            if (opt->size() > 0)
                _outMatrix->setLocSrc(opt->argPos(0));
            else
                _outMatrix->setLocSrc(opt->loc(true));
        }

        return true;

    case OPTION_OUT_MODEL_MATRIX_OBJNAME:
        if (opt->neg())
            _objName.clear();
        else {
            _objName = (*opt)[0];
        }
        return true;
    }

    return false;
}

/**
     * writes usage info for the module to stream
     * @param s				stream to write to
     */
void OutModelMatrix::usage(ostream& s)
{
    ModuleBase::usage(s);

    s << "  -matrix [<file>]              export model as matrix in a file or stdout" << endl;
}


/**
     * run module function
     */
void OutModelMatrix::run()
{
    _ctrl->errHandler().setExecStep("run");

    PROTO_OUTL("Start run module " << moduleName());


    if (_outMatrix) {
        // get model data
        _ctrl->errHandler().setExecStep("get model data");
        GET_DATA(OptModel, om);

        if (om->cols().size()==0 )
            _ctrl->errHandler().internalError("Cannot write matrix file because no model has been generated");

        om->setExportOnly(true);

        if (om) {

            if (om->isLinearModel()) {
                // cannot handle any special extension data
                map<int, list<OutModelExtDataBase::Info>> mki;
                OutModelExtDataBase::getInfo(this, mki);

                writeMatrix(om,_outMatrix);
                delete _outMatrix;
                _outMatrix = NULL;
            } else {
                _ctrl->errHandler().internalError("A matrix can only be written for linear problems"  );

            }
        }
        else {
            _ctrl->errHandler().error(ERROR_LVL_WARN, "no generated model to write as Matrix" , _outMatrix->locSrc());
        }
    }

    PROTO_OUTL("End run module " << moduleName());
}


/**
     * write generated matrix to file
     * @param om			generated model
     * @param file			output to this file
     * @param fm			use free MPS format
     */
void OutModelMatrix::writeMatrix(OptModel *om, FileOutput *file)
{
    try {
        PROTO_OUTL("write model as matrix to file '" << file->fileNameReplaced()) ;

        CmplOutput(cout, "Writing model instance as matrix to file > " + file->fileNameReplaced());
        ostream *ostr = file->open();
        writeMatrix(om, *ostr);
        file->close();
    }

    catch (FileException& e) {
        _ctrl->errHandler().error(ERROR_LVL_NORMAL, _ctrl->printBuffer("%s: write model to file '%s'", e.what(), e.filename().c_str()), file->locSrc());
    }
    catch (exception& ex) {
        _ctrl->errHandler().error(ERROR_LVL_NORMAL, "write model as Matrix file error", file->locSrc(), &ex);
    }
}

/**
     * write generated model to stream
     * @param om			generated model
     * @param ostr			output to this stream
     * @param fm			use free MPS format
     */
void OutModelMatrix::writeMatrix(OptModel *om, ostream& ostr)
{
    // get data
    PROTO_OUTL("get linear model");
    unsigned long colCnt = om->cols().size();
    unsigned long rowCnt = om->rows().size();

    LinearModel *lm = om->getRowModel();
    const vector<LinearModel::Coefficient> *coeffs = lm->coeffs();

    string realFormat = CMPLREAL_STDFORMAT;

    PROTO_OUTL("get row and column names");
    StringStore *sstore = modp()->data()->globStrings();
    string *colNames = om->colNames(sstore, 0, '_', 100);
    string *rowNames = om->rowNames(sstore, 0, '_', 100);

    const LinearModel::Coefficient *rhs;

    PROTO_OUTL("write matrix");
    ostr << "CMPL - matrix export " << endl;
    string prob = string(modp()->data()->cmplFileBase());
    ostr << "NAME " << StringStore::modelName(prob) << ".cmpl" << endl <<endl;


    ostr << setw(20) << left << "Variable name" << setw(4) << "";
    for (unsigned long j=0; j<colCnt; j++)
        ostr << right << setw(15) << colNames[j] ;
    ostr <<endl;

    ostr << setw(20) << left << "Variable type" << setw(4) << "";
    for (unsigned long j=0; j<colCnt; j++) {
        OptVar *ov = dynamic_cast<OptVar *>(om->cols()[j]);
        ostr << right << setw(15) ;
        if (ov->binVar())
            ostr << "B" ;
        else if (ov->intVar())
            ostr << "I" ;
        else
            ostr << "C";
    }
    ostr << endl;
    ostr << endl;

    const char *mode;
    mode = lm->mode();

    unsigned long objRow;
    bool objFound=false;
    for (unsigned long i = 0; i < rowCnt; i++, mode++) {
        if ( (*mode == '+' || *mode == '-') && (_objName.empty() || rowNames[i]==_objName) ) {

            ostr << setw(20) << left << rowNames[i] << setw(4) << (*mode == '+' ? "MAX" : "MIN") ;
            writeColCoeffs(ostr, coeffs[i], colCnt);
            ostr << endl;
            objRow=i;
            objFound=true;

            break;
        }
    }

    if (!objFound) {
        string msg ="Unknown objetive function >"+_objName;
        _ctrl->errHandler().internalError(msg.c_str());
    }

    ostr << left << setw(24) << "Subject to " ;
    ostr << setw( colCnt * 15 ) << "" << right << setw(15) << "RHS" << endl;

    ostr << endl ;

    rhs = lm->rhs();
    mode = lm->mode();
    for (unsigned long i = 0; i < rowCnt; i++, rhs++, mode++) {
        if (i!=objRow) {
            char m = *mode;
            if (m) {
                if (*mode == '+' || *mode == '-')
                    m='N';
                ostr << setw(20) << left << rowNames[i] << setw(4) << m ;
                writeColCoeffs(ostr, coeffs[i], colCnt);
                ostr << right << setw(15) <<  rhs->outString( realFormat.c_str(), 100) << endl;
            }
        }
    }

    ostr << endl;

    ostr << setw(20) << left << "Lower Bound" << setw(4) << "";
    for (unsigned long j=0; j<colCnt; j++) {
        OptVar *ov = dynamic_cast<OptVar *>(om->cols()[j]);
        if (ov) {
            CmplVal& lb = ov->lowBound();
            ostr << right << setw(15) << colBound( lb, "lb");
        }
    }
    ostr <<endl;

    ostr << setw(20) << left << "Upper Bound" << setw(4) << "";
    for (unsigned long j=0; j<colCnt; j++) {
        OptVar *ov = dynamic_cast<OptVar *>(om->cols()[j]);
        if (ov) {
            CmplVal& lb = ov->uppBound();
            ostr << right << setw(15) << colBound( lb, "ub");
        }
    }

    ostr <<endl;

}

/**
     * write column coefficients for one column
     * @param ostr			output to this stream
     * @param fm			use free MPS format
     * @param cn			column name
     * @param rns			row names
     * @param coeffs		coefficients for the column
     */
void OutModelMatrix::writeColCoeffs(ostream& ostr,  const vector<LinearModel::Coefficient>& coeffs, unsigned long colCnt)
{
    string realFormat=CMPLREAL_STDFORMAT;
    unsigned long j=0;
    unsigned long itIdx;

    map<unsigned long,unsigned long>   coeffIdx;

    for (vector<LinearModel::Coefficient>::const_iterator it = coeffs.begin(); it != coeffs.end(); it++) {
        itIdx=it->idRC - 1;
        coeffIdx[itIdx]=j;
        j++;
    }

    for (unsigned long j=0 ; j<colCnt; j++)  {

        if (coeffIdx.count(j)>0) {
            itIdx=coeffIdx[j];
            ostr << right << setw(15) << coeffs[itIdx].outString(realFormat.c_str(), 100) ;
        } else
            ostr << right << setw(15) << 0 ;
    }

}

/**
     * write bounds for one column
     * @param ostr			output to this stream
     * @param bv			value of bound or NULL if the bound type doesn't have one
     */
string OutModelMatrix::colBound( CmplVal& bv, string type)
{
    string ret;
    string realFormat=CMPLREAL_STDFORMAT;
    if (bv)
        ret = bv.outStringNum(realFormat.c_str(), 100);
    else
        if (type=="lb")
            ret = "-inf";
        else
            ret ="inf";

    return ret;

}


}

