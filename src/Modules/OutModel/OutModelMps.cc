
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

#include "OutModelMps.hh"
#include "OutModelExtData.hh"
#include "../../Control/MainControl.hh"
#include "../../Control/ModulesConf.hh"



namespace cmpl
{
	/*********** module definition **********/

	// defines the module "writeMps" that is implemented in class "OutModelMps".
	// to register this module you have to insert "MODULE_CLASS_REG(<execution step>, writeMps, <description>)" to file "modules.reg"
	MODULE_CLASS( writeMps, OutModelMps )



	/*********** initialization **********/

	/**
	 * initialize modul, to call immediately after the constructor.
	 * @param ctrl			main object
	 * @param data			data object
	 * @param name			module name (or alias)
	 */
	void OutModelMps::init(MainControl *ctrl, MainData *data, const char *name)
	{
		ModuleBase::init(ctrl, data, name);

		_standardMps = NULL;
		_freeMps = NULL;
        _realFormat = CMPLREAL_STDFORMAT;

        _addConForUnusedVar = 1;

        _formatDefault = FormatExtensionNone;
        _formatHeader = FormatExtensionDefault;
        _formatSOS = FormatExtensionDefault;
        _formatQP = FormatExtensionDefault;

        _syntaxStructure = NULL;
        _syntaxElems = NULL;

        _exportOnly=false;

        _objName="";

        _integerRelaxation=false;
	}

	/**
	 * destructor
	 */
	OutModelMps::~OutModelMps()
	{
		if (_standardMps)
			delete _standardMps;

		if (_freeMps)
			delete _freeMps;

        if (_syntaxElems)
            delete _syntaxElems;
    }


	/*********** handling of command line options **********/

#define OPTION_OUT_MODEL_MPS_STANDARD           10
#define OPTION_OUT_MODEL_MPS_FREE               11
#define OPTION_OUT_MODEL_MPS_FREE_FOR_SOLVER	12

#define OPTION_OUT_MODEL_MPS_REALFORMAT         20

#define OPTION_OUT_MODEL_MPS_OBJNAME            30

#define OPTION_OUT_MODEL_MPS_ADD_CON_UV         40

#define OPTION_OUT_MODEL_MPS_FORMAT_DEFAULT     50
#define OPTION_OUT_MODEL_MPS_FORMAT_HEADER      51
#define OPTION_OUT_MODEL_MPS_FORMAT_SOS         52
#define OPTION_OUT_MODEL_MPS_FORMAT_QP          53

#define OPTION_OUT_MODEL_MPS_INTEGERRELAXATION          54



	/**
	 * register command line options options for delivery to this module
	 * @param modOptReg		vector to register in
	 */
	void OutModelMps::regModOptions(vector<CmdLineOptList::RegOption> &modOptReg)
	{
		ModuleBase::regModOptions(modOptReg);

		REG_CMDL_OPTION( OPTION_OUT_MODEL_MPS_STANDARD, "m", 0, 1, CMDL_OPTION_NEG_NO_ARG, true );
        REG_CMDL_OPTION( OPTION_OUT_MODEL_MPS_FREE, "fm", 0, 1, CMDL_OPTION_NEG_NO_ARG, true );
        REG_CMDL_OPTION( OPTION_OUT_MODEL_MPS_FREE_FOR_SOLVER, "fms", 1, 1, CMDL_OPTION_NEG_NO_ARG, true );

        REG_CMDL_OPTION( OPTION_OUT_MODEL_MPS_REALFORMAT, "f%", 1, 1, CMDL_OPTION_NEG_NO_ARG, true );
        REG_CMDL_OPTION( OPTION_OUT_MODEL_MPS_OBJNAME, "obj", 1, 1, CMDL_OPTION_NEG_NO_ARG, true );

        REG_CMDL_OPTION( OPTION_OUT_MODEL_MPS_ADD_CON_UV, "add-con-uv", 0, 1, CMDL_OPTION_NEG_NO_ARG, true );

        REG_CMDL_OPTION( OPTION_OUT_MODEL_MPS_FORMAT_DEFAULT, "mps-format", 1, 1, CMDL_OPTION_NEG_NO_ARG, true );
        REG_CMDL_OPTION( OPTION_OUT_MODEL_MPS_FORMAT_HEADER, "mps-format-header", 1, 1, CMDL_OPTION_NEG_NO_ARG, true );
        REG_CMDL_OPTION( OPTION_OUT_MODEL_MPS_FORMAT_SOS, "mps-format-sos", 1, 1, CMDL_OPTION_NEG_NO_ARG, true );
        REG_CMDL_OPTION( OPTION_OUT_MODEL_MPS_FORMAT_QP, "mps-format-qp", 1, 1, CMDL_OPTION_NEG_NO_ARG, true );
        REG_CMDL_OPTION( OPTION_OUT_MODEL_MPS_INTEGERRELAXATION, "int-relax", 0, 0, CMDL_OPTION_NEG_DELIV, false );

    }

	/**
	 * parse single option from command line options, this function is called for every delivered option
	 * @param ref			reference number of option registration, should be used for discriminate the options
	 * @param prio			priority value of option
	 * @param opt			option
	 * @return				true if option is used by the module
	 */
	bool OutModelMps::parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt)
	{
        if (ModuleBase::parseOption(ref, prio, opt))
            return true;

        switch (ref) {
        case OPTION_OUT_MODEL_MPS_STANDARD:
            if (_standardMps) {
                delete _standardMps;
                _standardMps = NULL;
            }
            if (!(opt->neg())) {
                _standardMps = new FileOutput();
                _standardMps->setFile(_data, IO_MODE_FILE, (opt->size() > 0 ? &((*opt)[0]) : NULL), IO_FILE_STANDARD_MPS, true);
                if (opt->size() > 0)
                    _standardMps->setLocSrc(opt->argPos(0));
                else
                    _standardMps->setLocSrc(opt->loc(true));
            }
            _exportOnly=true;
            return true;

        case OPTION_OUT_MODEL_MPS_FREE_FOR_SOLVER:
        case OPTION_OUT_MODEL_MPS_FREE:
            if (_freeMps) {
                delete _freeMps;
                _freeMps = NULL;
            }
            if (!(opt->neg())) {
                _freeMps = new FileOutput();
                _freeMps->setFile(_data, IO_MODE_FILE, (opt->size() > 0 ? &((*opt)[0]) : NULL), IO_FILE_STANDARD_MPS, true);
                if (opt->size() > 0)
                    _freeMps->setLocSrc(opt->argPos(0));
                else
                    _freeMps->setLocSrc(opt->loc(true));
            }

            if (ref==OPTION_OUT_MODEL_MPS_FREE)
                _exportOnly=true;

            return true;

        case OPTION_OUT_MODEL_MPS_REALFORMAT:
            if (opt->neg())
                _realFormat = CMPLREAL_STDFORMAT;
            else
                _realFormat = (*opt)[0];
            return true;

        case OPTION_OUT_MODEL_MPS_OBJNAME:
            if (opt->neg())
                _objName.clear();
            else {
                _objName = (*opt)[0];
            }
            return true;

        case OPTION_OUT_MODEL_MPS_ADD_CON_UV:
            if (opt->neg()) {
                _addConForUnusedVar = 0;
            }
            else if (opt->size() == 0) {
                _addConForUnusedVar = 1;
            }
            else {
                _addConForUnusedVar = opt->argAsInt(0, _ctrl);
                if (_addConForUnusedVar > 2)
                    _addConForUnusedVar = 2;
                else if (_addConForUnusedVar < 0)
                    _addConForUnusedVar = 0;
            }
            return true;

        case OPTION_OUT_MODEL_MPS_FORMAT_DEFAULT:
            _formatDefault = parseFormatExtName(opt);
            if (_formatDefault == FormatExtensionDefault || _formatDefault == FormatExtensionError) {
                _ctrl->errHandler().error(ERROR_LVL_EASY, "invalid name for default format of MPS extensions", opt->loc(true));
                _formatDefault = FormatExtensionNone;
            }
            return true;

        case OPTION_OUT_MODEL_MPS_FORMAT_HEADER:
            _formatHeader = parseFormatExtName(opt);
            if (_formatHeader != FormatExtensionNone && _formatHeader != FormatExtensionDefault &&
                  _formatHeader != FormatExtensionCplex && _formatHeader != FormatExtensionGurobi &&
                  _formatHeader != FormatExtensionScip && _formatHeader != FormatExtensionHighs)  {
                _ctrl->errHandler().error(ERROR_LVL_EASY, "invalid name for format of MPS header lines", opt->loc(true));
                _formatHeader = FormatExtensionDefault;
            }
            return true;

        case OPTION_OUT_MODEL_MPS_FORMAT_SOS:
            _formatSOS = parseFormatExtName(opt);
            if (_formatSOS != FormatExtensionNone && _formatSOS != FormatExtensionDefault && _formatSOS != FormatExtensionCplex) {
                _ctrl->errHandler().error(ERROR_LVL_EASY, "invalid name for format of MPS extension for SOS", opt->loc(true));
                _formatSOS = FormatExtensionDefault;
            }
            return true;

        case OPTION_OUT_MODEL_MPS_FORMAT_QP:
            _formatQP = parseFormatExtName(opt);
            if (_formatQP != FormatExtensionNone && _formatQP != FormatExtensionDefault && _formatQP != FormatExtensionCplex) {
                _ctrl->errHandler().error(ERROR_LVL_EASY, "invalid name for format of MPS extension for quadratic problems", opt->loc(true));
                _formatQP = FormatExtensionDefault;
            }
            return true;

        case OPTION_OUT_MODEL_MPS_INTEGERRELAXATION:
            _integerRelaxation=true;
            return true;

        }

		return false;
	}

	/**
	 * writes usage info for the module to stream
	 * @param s				stream to write to
	 */
	void OutModelMps::usage(ostream& s)
	{
		ModuleBase::usage(s);

		s << "  -m [<file>]                   export model in standard MPS format in a file or stdout" << endl;
		s << "  -fm [<file>]                  export model in Free MPS format in a file or stdout" << endl;
        s << "  -f% <format>                  format spezifier (printf format) for real number output" << endl;
        s << "  -obj <name>                   name of the objective function to be invoked" << endl;

        s << "  -add-con-uv [<0-2>]           add pseudo constraint for otherwise unused variable (0: no / 1: yes if unused because of deletion of a constraint / 2: yes for all) (default is 1)" << endl;

        s << "  -mps-format <name>            default for subsequent format selection options" << endl;
        s << "  -mps-format-header <name>     format of header lines in MPS (one of: none, cplex, gurobi, scip)" << endl;
        s << "  -mps-format-sos <name>        format of MPS extension for SOS (one of: none, cplex)" << endl;
        s << "  -mps-format-qp <name>         format of MPS extension for quadratic problems (one of: none, cplex)" << endl;
        s << "  -int-relax                    integer or binary variables are used as continues variables" << endl;

        //TODO: more format extensions
    }

    /**
     * parse argument of command line option for name of MPS format extension
     * @param opt           command line option
     * @return              format extension type
     */
    OutModelMps::FormatExtension OutModelMps::parseFormatExtName(CmdLineOptList::SingleOption *opt)
    {
        if (opt->neg())
            return FormatExtensionNone;

        string name = StringStore::lowerCase((*opt)[0]);
        if (name == "none")
            return FormatExtensionNone;
        else if (name == "default")
            return FormatExtensionDefault;
        else if (name == "cplex")
            return FormatExtensionCplex;
        else if (name == "gurobi")
            return FormatExtensionGurobi;
        else if (name == "scip")
            return FormatExtensionScip;
        else if (name == "highs")
            return FormatExtensionHighs;

        return FormatExtensionError;
    }

    /**
     * initialize use of MPS format extensions
     */
    void OutModelMps::initFormatExtFromDefault()
    {
        // format of header lines in MPS
        if (_formatHeader == FormatExtensionDefault) {
            if (_formatDefault == FormatExtensionCplex || _formatDefault == FormatExtensionGurobi || _formatDefault == FormatExtensionScip || _formatDefault == FormatExtensionHighs)
                _formatHeader = _formatDefault;
            else
                _formatHeader = FormatExtensionNone;
        }

        // format of MPS extension for SOS
        if (_formatSOS == FormatExtensionDefault) {
            if (_formatDefault == FormatExtensionCplex)
                _formatSOS = _formatDefault;
            else
                _formatSOS = FormatExtensionNone;
        }

        // format of MPS extension for quadratic problems
        if (_formatQP == FormatExtensionDefault) {
            if (_formatDefault == FormatExtensionCplex)
                _formatQP = _formatDefault;
            else
                _formatQP = FormatExtensionNone;
        }

        //TODO: more format extensions
    }


	/**
	 * run module function
	 */
	void OutModelMps::run()
	{
		_ctrl->errHandler().setExecStep("run");

		PROTO_OUTL("Start run module " << moduleName());

        initFormatExtFromDefault();

		if (_standardMps || _freeMps) {
			// get model data
			_ctrl->errHandler().setExecStep("get model data");
			GET_DATA(OptModel, om); 

            if (om->cols().size()==0 )
                  _ctrl->errHandler().internalError("Cannot write MPS file because no model has been generated");

            GET_DATA(SyntaxElements, se);
            if (se)
                _syntaxStructure = se->getRoot();

			if (om) {
				// check for identical output files for MPS and Free MPS
				if (_standardMps && _freeMps && _standardMps->fileName() == _freeMps->fileName()) {
					_ctrl->errHandler().error(ERROR_LVL_WARN, "same output file given for MPS and free MPS, write only free MPS", _standardMps->locSrc());
					delete _standardMps;
					_standardMps = NULL;
				}

                if (_formatSOS != FormatExtensionNone) {
                    _mki[OutModelExtDataSOS1Key] = list<OutModelExtDataBase::Info>();

                    _mki[OutModelExtDataSOS2Key] = list<OutModelExtDataBase::Info>();
                }

                OutModelExtDataBase::getInfo(this, _mki);

				// write standard MPS
				if (_standardMps) {
					_ctrl->errHandler().setExecStep("write standard MPS");
					writeModel(om, _standardMps, false);
					delete _standardMps;
					_standardMps = NULL;
				}
				
				// write free MPS
				if (_freeMps) {
					_ctrl->errHandler().setExecStep("write Free MPS");
					writeModel(om, _freeMps, true);
					delete _freeMps;
					_freeMps = NULL;
				}

                om->setExportOnly(_exportOnly);

			}
			else {
				_ctrl->errHandler().error(ERROR_LVL_WARN, "no generated model to write as MPS" , (_standardMps ?: _freeMps)->locSrc());
			}
		}

		PROTO_OUTL("End run module " << moduleName());
	}


	/**
     * write generated model to file
	 * @param om			generated model
	 * @param file			output to this file
	 * @param fm			use free MPS format
	 */
	void OutModelMps::writeModel(OptModel *om, FileOutput *file, bool fm)
	{
		try {
            PROTO_OUTL("write model to file '" << file->fileNameReplaced() << "' " << (fm ? "(Free-MPS format)" : "(standard MPS format)"));

            string cmplOut = "Writing model instance to ";
            if (fm)
                cmplOut +=  "Free-MPS file > " ;
            else
                cmplOut += "MPS file > ";
            cmplOut += file->fileNameReplaced();

            CmplOutput(cout, cmplOut );

			ostream *ostr = file->open();

            writeModel(om, *ostr, fm);
			file->close();


		}
        catch (NonLinearModelException& e) {
            OptCon *oc = dynamic_cast<OptCon *>(om->rows().getElem(e.idRow()));
            const SyntaxElement *sep = (oc ? syntaxElement(oc->syntaxElem()) : NULL);
            _ctrl->errHandler().error(ERROR_LVL_NORMAL, _ctrl->printBuffer("%s: at row %ld", e.what(), e.idRow()), (sep ? sep->loc() : file->locSrc()));
        }
		catch (FileException& e) {
			_ctrl->errHandler().error(ERROR_LVL_NORMAL, _ctrl->printBuffer("%s: write model to file '%s'", e.what(), e.filename().c_str()), file->locSrc());
		}
		catch (exception& ex) {
			_ctrl->errHandler().error(ERROR_LVL_NORMAL, "write model as MPS file error", file->locSrc(), &ex);
		}
	}

    /**
     * write generated model to stream
     * @param om			generated model
     * @param ostr			output to this stream
     * @param fm			use free MPS format
     */
    void OutModelMps::writeModel(OptModel *om, ostream& ostr, bool fm)
    {
        // get data
        PROTO_OUTL("get linear model");
        unsigned long colCnt = om->cols().size();
        unsigned long rowCnt = om->rows().size();
        LinearModel *lm = om->getColModel((_formatQP != FormatExtensionNone));
        const vector<LinearModel::Coefficient> *coeffs = lm->coeffs();

        PROTO_OUTL("get row and column names");
        StringStore *sstore = modp()->data()->globStrings();
        unsigned mLenName = (fm ? 100 : 8);
        unsigned mLenVal = (fm ? 100 : 12);
        string *colNames = om->colNames(sstore, 0, '_', mLenName);
        string *rowNames = om->rowNames(sstore, 0, '_', mLenName);

        const LinearModel::Coefficient *rhs;
        const char *mode;
        bool fs;

        const string addConUVName = "__acuv";
        const LinearModel::Coefficient addConUVCoeff(0, (intType)1);
        const char addConUVMode = 'E';
        bool hasAddConUV = false;

        double minVal = - std::numeric_limits<double>::max();

        // write mps
        PROTO_OUTL("write mps");
        ostr << "* CMPL - " << (fm ? "Free-MPS" : "MPS") << " - Export" << endl;
        if (fm)
            ostr << "NAME " << modp()->data()->cmplFileBase() << endl;
        else
            ostr << "NAME" << setw(10) << " " << modp()->data()->cmplFileBase() << endl;

        mode = lm->mode();
        bool objFound=false;
        unsigned long objRowNr;

        for (unsigned long i = 0; i < rowCnt; i++, mode++) {

            if ( (*mode == '+' || *mode == '-' || *mode == 'N') && (_objName.empty() || rowNames[i]==_objName) ) {
            //if (*mode == '+' || *mode == '-') {
                if (_formatHeader == FormatExtensionGurobi) {
                    ostr << "* OBJNAME " << rowNames[i] << endl;
                    ostr << "OBJSENSE" << endl << "\t" << (*mode == '+' ? "max" : "min") << endl;
                }
                else if (_formatHeader == FormatExtensionCplex) {
                    ostr << "OBJSENSE" << endl << "\t" << (*mode == '+' ? "MAX" : "MIN") << endl;
                    ostr << "OBJNAME" << endl << "\t" << rowNames[i] << endl;
                }
                else if (_formatHeader == FormatExtensionScip) {
                    ostr << "* OBJNAME " << rowNames[i] << endl;
                    ostr << "OBJSENSE" << endl << "\t" << (*mode == '+' ? "MAX" : "MIN") << endl;
                }
                else if (_formatHeader == FormatExtensionHighs) {
                    ostr << "* OBJNAME " << rowNames[i] << endl;
                    ostr << "OBJSENSE " << (*mode == '+' ? "MAX" : "MIN") << endl;
                }
                else if (fm) {
                    ostr << "* OBJNAME " << rowNames[i] << endl << "* OBJSENSE " << (*mode == '+' ? "MAX" : "MIN") << endl;
                }

                om->setObjIdx(i);
                om->setObjName(rowNames[i]);


                string objSense = (*mode == '+' ? "max" : "min");
                om->setObjSense(objSense);

                objFound=true;
                objRowNr = i + 1;
                break;
            }
        }

        if (!_objName.empty() && !objFound) {
            string msg ="Unknown objective function >"+_objName;
            _ctrl->errHandler().internalError(msg.c_str());
        }

        ostr << "ROWS" << endl;
        mode = lm->mode();
        for (unsigned long i = 0; i < rowCnt; i++, mode++) {
            char m = *mode;
            if (m) {
                if (m == '+' || m == '-' || m == 'N')
                    if (i==om->objIdx())
                        m = 'N';
                    else
                        m='G';

                if (fm)
                    ostr << ' ' << m << ' ' << rowNames[i] << endl;
                else
                    ostr << ' ' << m << setw(2) << " " << left << setw(8) << rowNames[i] << endl;
            }
        }

        if (_addConForUnusedVar) {
            for (unsigned long i = 0; i < colCnt; i++) {
                OptVar *ov = dynamic_cast<OptVar *>(om->cols()[i]);
                if (ov && ov->usedByCon() <= (_addConForUnusedVar == 2 ? 0 : -1)) {
                    hasAddConUV = true;
                    if (fm)
                        ostr << ' ' << addConUVMode << ' ' << addConUVName << '[' << i << ']' << endl;
                    else
                        ostr << ' ' << addConUVMode << setw(2) << " " << left << setw(8) << addConUVName << '[' << i << ']' << endl;
                }
            }
        }

        ostr << "COLUMNS" << endl;
        bool hasInt = false;

        for (unsigned long i = 0; i < colCnt; i++) {
            OptVar *ov = dynamic_cast<OptVar *>(om->cols()[i]);
            if (ov) {
                if (ov->usedByCon() == 1) {
                    if (ov->intVar())
                        hasInt = true;
                    else
                        writeColCoeffs(ostr, fm, colNames[i], rowNames, coeffs[i]);
                }
                else if (hasAddConUV && ov->usedByCon() <= (_addConForUnusedVar == 2 ? 0 : -1))
                {
                    if (fm)
                        ostr << ' ' << left << colNames[i] << ' ' << addConUVName << '[' << i << ']' << ' ' << addConUVCoeff.outString(_realFormat.c_str(), mLenVal) << endl;
                    else
                        ostr << setw(4) << " " << setw(8) << left << colNames[i] << "  " << setw(8) << left << addConUVName << '[' << i << ']' << "  " << right << setw(12) << addConUVCoeff.outString(_realFormat.c_str(), mLenVal) << endl;
                }
            }
        }

        if (hasInt) {
            if (!_integerRelaxation) {
                if (fm)
                    ostr << " GVANF 'MARKER' 'INTORG'" << endl;
                else
                    ostr << setw(4) << " " << "GVANF" << setw(5) << " " << "'MARKER'" << setw(17) << " " <<"'INTORG'" << endl;
            }


            for (unsigned long i = 0; i < colCnt; i++) {
                OptVar *ov = dynamic_cast<OptVar *>(om->cols()[i]);
                if (ov && ov->intVar() && ov->usedByCon() == 1)
                    writeColCoeffs(ostr, fm, colNames[i], rowNames, coeffs[i]);
            }

            if (!_integerRelaxation) {
                if (fm)
                    ostr << " GVEND 'MARKER' 'INTEND'" << endl;
                else
                    ostr << setw(4) << " " << "GVEND" << setw(5) << " " << "'MARKER'" << setw(17) << " " <<"'INTEND'" << endl;
            }
        }

        ostr << "RHS" << endl;
        rhs = lm->rhs();
        mode = lm->mode();
        fs = true;
        for (unsigned long i = 0; i < rowCnt; i++, rhs++, mode++) {
            if (*mode) {

               // handling of non activated obj functions
               if ( (*mode == '+' || *mode == '-' || *mode == 'N') && i!=om->objIdx() ) {
                    if (fs) {
                        fs = false;
                        if (fm)
                            ostr << ' ' << left << "RHS" << ' ' << rowNames[i] << ' ' << right << minVal;
                        else
                            ostr << "    " << setw(8) << left << "RHS" << "  " << setw(8) << rowNames[i] << "  " << setw(12) << right << minVal;
                    } else {
                        fs = true;
                        if (fm)
                            ostr << ' ' << left << rowNames[i] << ' ' << right <<  minVal << endl;
                        else
                            ostr <<  "   " << setw(8) << left << rowNames[i] << "  " << setw(12) << right <<  minVal << endl;
                    }

               }

                if (!rhs->isNumNull()) {
                    if (fs) {
                        fs = false;
                        if (fm)
                            ostr << ' ' << left << "RHS" << ' ' << rowNames[i] << ' ' << right << rhs->outString(_realFormat.c_str(), mLenVal);
                        else
                            ostr << "    " << setw(8) << left << "RHS" << "  " << setw(8) << rowNames[i] << "  " << setw(12) << right << rhs->outString(_realFormat.c_str(), mLenVal);
                    }
                    else {
                        fs = true;
                        if (fm)
                            ostr << ' ' << left << rowNames[i] << ' ' << right << rhs->outString(_realFormat.c_str(), mLenVal) << endl;
                        else
                            ostr <<  "   " << setw(8) << left << rowNames[i] << "  " << setw(12) << right << rhs->outString(_realFormat.c_str(), mLenVal) << endl;
                    }
                }
            }
        }
        if (!fs)
            ostr << endl;

        if (hasAddConUV) {
            fs = true;
            for (unsigned long i = 0; i < colCnt; i++) {
                OptVar *ov = dynamic_cast<OptVar *>(om->cols()[i]);
                if (ov && ov->usedByCon() <= (_addConForUnusedVar == 2 ? 0 : -1)) {
                    const LinearModel::Coefficient addConUVRhs(ov);
                    if (fs) {
                        fs = false;
                        if (fm)
                            ostr << ' ' << left << "RHS" << ' ' << addConUVName << '[' << i << ']' << ' ' << right << addConUVRhs.outString(_realFormat.c_str(), mLenVal);
                        else
                            ostr << "    " << setw(8) << left << "RHS" << "  " << setw(8) << addConUVName << '[' << i << ']' << "  " << setw(12) << right << addConUVRhs.outString(_realFormat.c_str(), mLenVal);
                    }
                    else {
                        fs = true;
                        if (fm)
                            ostr << ' ' << left << addConUVName << '[' << i << ']' << ' ' << right << addConUVRhs.outString(_realFormat.c_str(), mLenVal) << endl;
                        else
                            ostr <<  "   " << setw(8) << left << addConUVName << '[' << i << ']' << "  " << setw(12) << right << addConUVRhs.outString(_realFormat.c_str(), mLenVal) << endl;
                    }
                }
            }
            if (!fs)
                ostr << endl;
        }

        //TODO: RANGES

        ostr << "BOUNDS" << endl;
        for (unsigned long i = 0; i < colCnt; i++) {
            OptVar *ov = dynamic_cast<OptVar *>(om->cols()[i]);
            if (ov) {
                if (ov->usedByCon() == 1 || _addConForUnusedVar == 2 || (_addConForUnusedVar == 1 && ov->usedByCon() == -1)) {
                    CmplVal& lb = ov->lowBound();
                    CmplVal& ub = ov->uppBound();
                    if (lb || ub) {
                        if (lb) {
                            if (!lb.isNumNull())
                                writeColBound(ostr, fm, colNames[i], "LO", &lb);
                        }
                        else
                            writeColBound(ostr, fm, colNames[i], "MI");

                        if (ub)
                            writeColBound(ostr, fm, colNames[i], "UP", &ub);
                        else
                            writeColBound(ostr, fm, colNames[i], "PL");
                    }
                    else {
                        writeColBound(ostr, fm, colNames[i], "FR");
                    }
                }
            }
        }

        if (lm->quadratic()) {
            writeQuadratics(ostr, fm, (QLinearModel *)lm, rowNames, colNames, true, objRowNr);
            writeQuadratics(ostr, fm, (QLinearModel *)lm, rowNames, colNames, false, objRowNr);
        }

        if (_formatSOS == FormatExtensionCplex) {


            list<OutModelExtDataBase::Info> &lst = _mki[OutModelExtDataSOS1Key];
            list<OutModelExtDataBase::Info> &lst2 = _mki[OutModelExtDataSOS2Key];

            if (!lst.empty() || !lst2.empty())
                ostr << "SOS" << endl;

            if (!lst.empty()) {

                 for (OutModelExtDataBase::Info info : lst) {
                     if (info.rows && !info.onlyRem) {
                         OutModelExtDataSOS1 *infosos = (OutModelExtDataSOS1 *)info.dp;
                         for (unsigned i = 0; i < info.rows; i++) {
                             vector<unsigned long> sosvars;
                             unsigned sosname;
                             //infosos->getData(info, i, &sosvars, sosname);
                             infosos->getData(info, i, sosvars, sosname);
                             string sosName=sstore->at(sosname);

                             writeSos(ostr,1,fm, i, colNames, sosvars, sosName);

                         }
                     }
                 }
             }

            if (!lst2.empty()) {

                 for (OutModelExtDataBase::Info info : lst2) {
                     if (info.rows && !info.onlyRem) {
                         OutModelExtDataSOS2 *infosos = (OutModelExtDataSOS2 *)info.dp;
                         for (unsigned i = 0; i < info.rows; i++) {
                             vector<unsigned long> sosvars;
                             unsigned sosname;
                             //infosos->getData(info, i, &sosvars, sosname);
                             infosos->getData(info, i, sosvars, sosname);
                             string sosName=sstore->at(sosname);

                             writeSos(ostr,2,fm, i, colNames, sosvars, sosName);
                         }
                     }
                 }
             }
        }

        ostr << "ENDATA" << endl;
    }


    /**
     * write mps sections for quadratic problems
     * @param ostr          output to this stream
     * @param fm            use free MPS format
     * @param qm            linear model with quadratic extension
     * @param rowNames      array of all row names
     * @param colNames      array of all column names
     * @param obj           true: write mps for quadratic objective / false: write mps for quadratic constraints
     * @param objRowNr      row number of objective
     */
    void OutModelMps::writeQuadratics(ostream& ostr, bool fm, QLinearModel *qm, string *rowNames, string *colNames, bool obj, unsigned long objRowNr)
    {
        map<unsigned long, map<unsigned long, LinearModel::Coefficient>> pm;

        for (QLinearModel::CoefficientVarProdList& vpl : qm->prods()) {
            if ((vpl.idR == objRowNr) == obj) {
                fillQuadraticMap(pm, vpl.cvp);

                if (!pm.empty()) {
                    if (_formatQP == FormatExtensionCplex) {
                        if (obj)
                            ostr << "QMATRIX" << endl;
                        else if (fm)
                            ostr << "QCMATRIX " << rowNames[vpl.idR - 1] << endl;
                        else
                            ostr << "QCMATRIX  " << left << setw(8) << rowNames[vpl.idR - 1] << endl;

                        for (auto& m1 : pm) {
                            for (auto& m2 : m1.second) {
                                if (!m2.second.isNumNull())
                                {
                                    if (fm)
                                        ostr << ' ' << colNames[m1.first - 1] << ' ' << colNames[m2.first - 1] << ' ' << m2.second.outString(_realFormat.c_str(), 100) << endl;
                                    else
                                        ostr << setw(4) << " " << setw(8) << left << colNames[m1.first - 1] << "  " << setw(8) << left << colNames[m2.first - 1] << "  " << right << setw(12) << m2.second.outString(_realFormat.c_str(), 12) << endl;
                                }
                            }
                        }
                    }

                    pm.clear();
                }
            }
        }
    }

    /**
     * fill mapping with quadratic coefficients for one row
     * @param pm            mapping to fill coefficients in
     * @param cvp           list of quadratic terms
     */
    void OutModelMps::fillQuadraticMap(map<unsigned long, map<unsigned long, LinearModel::Coefficient>>& pm, list<QLinearModel::CoefficientVarProd>& cvp)
    {
        for (QLinearModel::CoefficientVarProd& pr : cvp) {
            if (!pr.isNumNull()) {
                for (unsigned dir = 0; dir < 2; dir++) {
                    unsigned c1 = (dir ? pr.idC2 : pr.idRC);
                    unsigned c2 = (dir ? pr.idRC : pr.idC2);

                    map<unsigned long, LinearModel::Coefficient>& m1 = pm[c1];
                    LinearModel::Coefficient& m2 = m1[c2];
                    m2.add(pr);
                }
            }
        }
    }


    /**
     * write mps section for SOS
     * @param ostr          output to this stream
     * @param type          type of SOS (1 or 2)
     * @param fm            use free MPS format
     * @param i             internal SOS index
     * @param colNames      array of all column names
     * @param sosvars       variables in SOS
     * @param sosName       name of SOS
     */
    void OutModelMps::writeSos(ostream& ostr, int type, bool fm, unsigned i, string *colNames, vector<unsigned long>& sosvars, string& sosName ) {

        if (sosName.empty()) {
            string tmpName=colNames[sosvars[0]];
            vector < string > tmpName1;
            StringStore::split(tmpName, tmpName1, "[");

            if (tmpName1.size()>0)
               sosName=tmpName1[0]+"_sos["+to_string(i+1)+"]";
            else
               sosName="sos"+to_string(i+1);
        }

        string sosType = (type==1 ? "S1" : "S2");
        if (fm)
            ostr << " " << sosType << " " << sosName <<endl;
        else
            ostr << " " << sosType << " " << setw(8) << left << sosName <<endl;

        unsigned long j=1;
        for (unsigned long idx : sosvars ) {
            if (fm)
                ostr << "    "  << colNames[idx-1] << " " << j << endl;
            else
                ostr << "  " << setw(8) << left << colNames[idx] << setw(8) << left <<j << endl;
            j++;
        }

    }

    /**
     * write column coefficients for one column
     * @param ostr			output to this stream
     * @param fm			use free MPS format
     * @param cn			column name
     * @param rns			row names
     * @param coeffs		coefficients for the column
     */
    void OutModelMps::writeColCoeffs(ostream& ostr, bool fm, string& cn, string *rns, const vector<LinearModel::Coefficient>& coeffs)
    {
        bool fs = true;
        for (vector<LinearModel::Coefficient>::const_iterator it = coeffs.begin(); it != coeffs.end(); it++)
        {
            string& rn = rns[it->idRC - 1];

            if (fs) {
                fs = false;
                if (fm)
                    ostr << ' ' << left << cn << ' ' << rn << ' ' << it->outString(_realFormat.c_str(), 100);
                else
                    ostr << setw(4) << " " << setw(8) << left << cn << "  " << setw(8) << left << rn << "  " << right << setw(12) << it->outString(_realFormat.c_str(), 12);
            }
            else {
                fs = true;
                if (fm)
                    ostr << ' ' << rn << ' ' << it->outString(_realFormat.c_str(), 100) << endl;
                else
                    ostr << "   " << setw(8) << left << rn << "  " << setw(12) << right << it->outString(_realFormat.c_str(), 12) << endl;
            }
        }

        if (!fs)
            ostr << endl;
    }

    /**
     * write bounds for one column
     * @param ostr			output to this stream
     * @param fm			use free MPS format
     * @param cn			column name
     * @param bt			MPS type of bound
     * @param bv			value of bound or NULL if the bound type doesn't have one
     */
    void OutModelMps::writeColBound(ostream& ostr, bool fm, string& cn, const char *bt, CmplVal *bv)
    {
        if (fm) {
            ostr << ' ' << bt << ' ' << left << "BOUND" << ' ' << cn;
            if (bv)
                ostr << ' ' << right << bv->outStringNum(_realFormat.c_str(), 100);
        }
        else {
            ostr << ' ' << bt << ' ' << left << setw(8) << "BOUND" << "  " << setw(8) << cn;
            if (bv)
                ostr << "  " << setw(12) << right << bv->outStringNum(_realFormat.c_str(), 12);
        }

        ostr << endl;
    }

    /**
     * get the syntax element for a given id
     * @param id		id of syntax element
     * @return			syntax element
     */
    const SyntaxElement *OutModelMps::syntaxElement(unsigned id)
    {
        if (!_syntaxStructure)
            return NULL;

        if (!_syntaxElems) {
            _syntaxElems = new map<unsigned, const SyntaxElement *>();
            _syntaxStructure->fillMap(_syntaxElems);
        }

        return _syntaxElems->at(id);
    }
}

