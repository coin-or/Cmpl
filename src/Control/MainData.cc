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


#include "MainData.hh"
#include "ErrorHandler.hh"
#include "MainControl.hh"


namespace cmpl
{

	/************** MainData::DataBase **********/

	/**
	 * constructor
	 * @param m			module name
	 * @param data		main data object
	 */
	MainData::DataBase::DataBase(const char *m, MainData *data)
	{
		_modName = data->globStrings()->store(m);
	}



	/************** common **********/

	/**
	 * constructor
	 */
	MainData::MainData()
	{
        _globStrings = new StringStore(true);
        _globLocs = new map<unsigned long, vector<LocationInfo *>>;

		_cmplFileBase = NULL;
		_executedModules = NULL;

		_errorList = NULL;
		_errorLevelMax = _errorCnt = _warnCnt = 0;
	}

	/**
	 * destructor
	 */
	MainData::~MainData()
	{
		cleanUp(true);
		delete _globStrings;
		delete _globLocs;
	}


	/**
	 * delete all data
	 * @param delData	delete subdata
	 */
    void MainData::cleanUp(bool delData)
	{
        //TODO: Pruefung Aufruf mit !delData:
        //  (scheint derzeit nirgends verwendet)
        //  sinnvoll hoechstens fuer src nach Aufruf copyShallowFrom(src), weil dann das neue MainData-Objekt owner der Subobjekte ist
        //  aber auch copyShallowFrom() scheint nicht verwendet

		_cmplFileBase = NULL;
		
		if (delData && _executedModules) {
			delete _executedModules;
		}
		_executedModules = NULL;

		if (delData) {
			resetErrors();
		}
		else {
			_errorList = NULL;
			_errorLevelMax = _errorCnt = _warnCnt = 0;
		}

		if (delData) {
			for (map<string, MainData::DataBase *>::iterator it = _data.begin(); it != _data.end(); it++)
				delete it->second;
		}
		_data.clear();
		
		if (delData) {
			_globStrings->clear();
			_globLocs->clear();
		}
		else {
            _globStrings = new StringStore(true);
            _globLocs = new map<unsigned long, vector<LocationInfo *>>;
		}
	}


	/**
	 * fills this as a shallow copy from another data object
	 * @param src       source data object
	 */
	void MainData::copyShallowFrom(MainData& src)
	{
		_cmplFileBase = src._cmplFileBase;
		_executedModules = src._executedModules;

		_errorList = src._errorList;
		_errorLevelMax = src._errorLevelMax;
		_errorCnt = src._errorCnt;
		_warnCnt = src._warnCnt;

		_data = src._data;

		delete _globStrings;
		_globStrings = src._globStrings;

		delete _globLocs;
		_globLocs = src._globLocs;
	}


	/**
	 * makes an aequivalent persistent location, i.e. a location that refers nothing outside of this
	 * @param loc		given location
	 * @return			aequivalent persistent location
	 */
	LocationInfo MainData::persistentLocation(const LocationInfo& loc)
	{
		PositionInfo pb = loc.begin();
		PositionInfo pe = loc.end();

		pb.setName(_globStrings->store(pb.name()));
		pe.setName(_globStrings->store(pe.name()));

		bool eqInLoc = false;
		if (pb.inLoc()) {
			eqInLoc = (pe.inLoc() && pb.inLoc() == pe.inLoc());
			pb.setInLoc(searchInsertInGlobLocs(pb.inLoc()));
		}

		if (pe.inLoc()) {
			if (eqInLoc)
				pe.setInLoc(pb.inLoc());
			else
				pe.setInLoc(searchInsertInGlobLocs(pe.inLoc()));
		}

		return LocationInfo(pb, pe);
	}

	/**
	 * change position to a persistent position, i.e. a position that refers nothing outside of this
	 * @param pos		given position
	 */
	void MainData::persistentPosition(PositionInfo& pos)
	{
		pos.setName(_globStrings->store(pos.name()));

		if (pos.inLoc())
			pos.setInLoc(searchInsertInGlobLocs(pos.inLoc()));
	}

	/**
	 * search aequivalent location in _globLocs and insert it if not found
	 * @param loc		given location
	 * @return			aequivalent location in _globLocs
	 */
	LocationInfo *MainData::searchInsertInGlobLocs(const LocationInfo *loc)
	{
        //TODO: Suche geht so nicht bei groesseren Modellen.
        //  _globLocs muss irgendwie so organisiert werden, dass besser darin gesucht werden kann

        // LocationInfo-Objekte muessen auf dem Heap angelegt werden, damit es dauerhaft gueltige Pointer darauf gibt
        // Die owner-Struktur muss diese Pointer speichern
        // Fuer schnelle Suche sollte Schluessel aus den Eigenschaften gebildet werden, die nicht von persistentLocation() veraendert werden
        // So ein Schluessel kann fuer verschiedene LocationInfo-Objekte gleich sein, alle zum selben Schluessel muessen dann wie bisher durchsucht werden
        //  -> map<key, vector<LocationInfo *>>
        // key ist dafuer ein aus LocationInfo holbares Objekt einer zusaetzlichen Klasse, dass den Operator < definiert
        //  oder vielleicht besser: key ist long (oder long long?); und wird mit bitshift geeignet gefuellt (dann ist kein eigenes Objekt notwendig, das erzeugt werden muesste)

        unsigned long key = loc->key();
        vector<LocationInfo*>& vl = (*_globLocs)[key];

        if (!vl.empty()) {
            for (vector<LocationInfo*>::iterator it = vl.begin(); it != vl.end(); it++) {
                if (*loc == **it)
                    return *it;
            }
        }

		LocationInfo *res = new LocationInfo(persistentLocation(*loc));
        vl.push_back(res);
		return res;
	}

	/**
	 * search aequivalent location in _globLocs and return number of it or -1 if not found
     * @param si		serialization info (si.mapLocsTo must be filled)
     * @param loc		given location
	 */
    int MainData::searchLoc(SerializeInfo& si, const LocationInfo *loc) const
	{
        if (si.mapLocsTo.count(loc))
            return (int)si.mapLocsTo[loc];

        unsigned long key = loc->key();
        if (_globLocs->count(key)) {
            vector<LocationInfo*>& vl = (*_globLocs)[key];
            for (vector<LocationInfo*>::iterator it = vl.begin(); it != vl.end(); it++) {
                if (*loc == **it) {
                    LocationInfo *gloc = *it;
                    if (si.mapLocsTo.count(gloc))
                        return (int)si.mapLocsTo[gloc];
                    else
                        throw invalid_argument("serialize info object is not filled");
                }
            }
        }

        return -1;
    }



	/************** executed modules **********/

	/**
	 * add name of the already executed modules
	 * @param m			name of module
	 * @param o			original module name if module is an alias name (otherwise NULL)
	 * @param i			name of the cmpl instance which has executed the module
	 */
	void MainData::addExecutedModules(const char *m, const char *o, const char *i)
	{
		if (!_executedModules) {
			_executedModules = new list<ExecutedModule>;
		}
		
		_executedModules->push_back(ExecutedModule(_globStrings->store(m), (o ? _globStrings->store(o) : NULL), _globStrings->store(i)));
	}

	/**
	 * set execution of a module as finished
	 * @param m			name of module
	 * @param i			name of the cmpl instance which has executed the module
	 */
	void MainData::setExecutedModuleFinished(const char *m, const char *i)
	{
		m = _globStrings->store(m);
		i = _globStrings->store(i);

		if (_executedModules) {
			for (list<ExecutedModule>::iterator it = _executedModules->begin(); it != _executedModules->end(); it++) {
				if (it->module == m && it->instance == i) {
					it->finished = true;
					break;
				}
			}
		}
	}
	


	/************** error handling **********/

	/**
	 * clear error list
	 */
	void MainData::resetErrors()
	{
        if (_errorList) {
            delete _errorList;
            _errorList = NULL;
        }

        _errorLevelMax = _errorCnt = _warnCnt = 0;
    }
	
	/**
	 * add an error entry.
	 * @param lvl		error level (0:warning, 1:normal, 2:fatal) (one of ERROR_LVL_*)
	 * @param cont		entry continues the previous entry
	 * @param msg		error message
	 * @param exc		original exception / NULL: no exception
	 * @param modul		name of module in which the error occured
	 * @param step		name of the exec step within the module where the error occured
	 * @param loc		location of the error
	 */
	void MainData::addError(int lvl, bool cont, const char *msg, exception *exc, const char *modul, const char *step, const LocationInfo& loc)
	{
		if (!_errorList)
			_errorList = new list<ErrorEntry>;

		_errorList->push_back(ErrorEntry(lvl, cont, _globStrings->store(msg), (exc ? _globStrings->store(exc->what()) : (const char *)NULL), _globStrings->store(modul), _globStrings->store(step), searchInsertInGlobLocs(&loc)));

		if (lvl > _errorLevelMax)
			_errorLevelMax = lvl;

		if (!cont) {
			if (lvl == ERROR_LVL_WARN)
				_warnCnt++;
			else
				_errorCnt++;
		}
	}


	/**
	 * set cmpl data
	 * @param n			name of data
	 * @param d			object with data
	 * @param del		delete previous data object if exists
	 */
	void MainData::setData(string n, MainData::DataBase *d, bool del)
	{
		if (_data.count(n) > 0) {
			MainData::DataBase *p = _data[n];
			if (d == p)
				return;

			if (p && del)
				delete p;
		}

		_data[n] = d;
	}

	/**
	 * get cmpl data
	 * @param n			name of data
	 * @return			data object or NULL if not set
	 */
	MainData::DataBase *MainData::data(string n)
	{
		map<string, MainData::DataBase *>::iterator it = _data.find(n);
		if (it == _data.end())
			return NULL;
		else
			return it->second;
	}

	/**
	 * return whether cmpl data is set
	 * @param n			name of data class
	 * @return			true if the data object is set
	 */
	bool MainData::hasData(string n) const
	{
		map<string, MainData::DataBase *>::const_iterator it = _data.find(n);
		return (it != _data.end() && it->second != NULL);
	}


	/************** serialization to stream **********/

	/**
	 * write data object to output stream
	 * @param si		serialization info
	 */
	void MainData::serializeTo(SerializeInfo& si)
	{
		int cnt = 1 + (si.ioCmdLineOpts ? 1 : 0) + (si.ioModules || si.ioExtensions ? 1 : 0) + 1;
		(*si.ostr) << "****CmplData:" << cnt << ';' << version() << ';' << _globStrings->storeInd(si.modp->ctrl()->instanceName()) << ';' << (si.lineNums ? 1 : 0) << endl;

		serializeCommonTo(si);

		if (si.ioCmdLineOpts)
			serializeCmdLineOptsTo(si);

		if (si.ioModules || si.ioExtensions)
			serializeCodePartsTo(si);

		serializeDataTo(si);
	}

	/**
	 * write common data to output stream
	 * @param si		serialization info
	 */
	void MainData::serializeCommonTo(SerializeInfo& si)
	{
		int i;

		// store additional needed strings and locations to _globStrings and _globLocs
		if (si.ioCmdLineOpts)
			si.modp->ctrl()->insertCmdLineOptsStringsAndLocs(this);

		if (si.ioModules)
			si.modp->ctrl()->insertModuleStrings(this);

		if (si.ioExtensions)
			si.modp->ctrl()->insertExtensionStringsAndLocs(this);

		// write data
		(*si.ostr) << "***Common:3" << endl;

		// strings
		int cnt = _globStrings->size();
		const char *c;
		(*si.ostr) << "**Strings" << endl << "*:" << cnt << ";0" << endl;

		for (i = 0; i < cnt; i++) {
			if (si.lineNums)
				(*si.ostr) << i << '#';

			c = (*_globStrings)[i];
			(*si.ostr) << strlen(c) << ':' << c << endl;
		}

		// locations
        si.mapLocsTo.clear();
        si.mapLocsFrom.clear();

        i = 0;
        for (auto it = _globLocs->begin(); it != _globLocs->end(); ++it) {
            vector<LocationInfo*>& vl = it->second;
            for (auto it2 = vl.begin(); it2 != vl.end(); ++it2, ++i) {
                LocationInfo *loc = *it2;
                si.mapLocsTo[loc] = i;
                si.mapLocsFrom.push_back(loc);
            }
        }

        (*si.ostr) << "**Locations" << endl << "*:" << si.mapLocsFrom.size() << ";2" << endl;

		i = 0;
        for (auto it = si.mapLocsFrom.begin(); it != si.mapLocsFrom.end(); ++it, ++i) {
            LocationInfo *loc = *it;
            for (int j = 0; j < 2; j++) {
                if (si.lineNums)
                    (*si.ostr) << i << '#';

                const PositionInfo &pos = (j == 0 ? loc->begin() : loc->end());
                const LocationInfo *inLoc = (pos.type() == POSITION_TYPE_FIXLOC ? pos.fixLoc() : pos.inLoc());
                int inLocNo = (inLoc ? (int)si.mapLocsTo[inLoc] : -1);
                (*si.ostr) << pos.type() << ';' << _globStrings->search(pos.name()) << ';' << pos.line() << ';' << pos.col() << ';' << inLocNo << endl;
            }
        }

		// data of MainControl
		(*si.ostr) << "**MainControl:3" << endl;

		// base name for deriving default file names
		if (_cmplFileBase) {
			(*si.ostr) << "*CmplFileBase:1;1" << endl;
			if (si.lineNums)
				(*si.ostr) << "0#";
			(*si.ostr) << _globStrings->search(_cmplFileBase) << endl;
		}
		else {
			(*si.ostr) << "*CmplFileBase:0;1" << endl;
		}

		// list of names of the already executed modules
		if (_executedModules) {
			cnt = _executedModules->size();
			(*si.ostr) << "*ExecutedModules:" << cnt << ";1" << endl;

			i = 0;
			for (list<ExecutedModule>::iterator it = _executedModules->begin(); it != _executedModules->end(); it++, i++) {
				if (si.lineNums)
					(*si.ostr) << i << '#';

				(*si.ostr) << _globStrings->search(it->module) << ';' << (it->orgMod ? (int)(_globStrings->search(it->orgMod)) : -1) << ';' << _globStrings->search(it->instance) << ';' <<
					(it->finished ? 1 : 0) << endl;
			}
		}
		else {
			(*si.ostr) << "*ExecutedModules:0;1" << endl;
		}

		// errors
		if (_errorList) {
			cnt = _errorList->size();
			(*si.ostr) << "*Errors:" << cnt << ";1;" << _errorLevelMax << ';' << _errorCnt << ';' << _warnCnt << endl;

			i = 0;
			for (list<MainData::ErrorEntry>::iterator it = _errorList->begin(); it != _errorList->end(); it++, i++) {
				if (si.lineNums)
					(*si.ostr) << i << '#';

				MainData::ErrorEntry &err = *it;
				(*si.ostr) << err.lvl << ';' << (err.cont ? 1 : 0) << ";" << _globStrings->search(err.msg) << ';' << (err.exc ? (int)(_globStrings->search(err.exc)) : -1) << ';'
                        << _globStrings->search(err.module) << ';' << _globStrings->search(err.step) << ';' << si.mapLocsTo[err.loc] << endl;
			}
		}
		else {
			(*si.ostr) << "*Errors:0;1;0;0;0" << endl;
		}

	}

	/**
	 * write command line options to output stream
	 * @param si		serialization info
	 */
	void MainData::serializeCmdLineOptsTo(SerializeInfo& si)
	{
		int cnt = si.modp->ctrl()->subCntCmdLineOpts();
		(*si.ostr) << "***CmdLineOpts" << endl << "**:" << cnt << endl;
		si.modp->ctrl()->serializeCmdLineOptsTo(si, this);
	}

	/**
	 * write modules and extensions to output stream
	 * @param si		serialization info
	 */
	void MainData::serializeCodePartsTo(SerializeInfo& si)
	{
		int cnt = (si.ioModules ? 1 : 0) + (si.ioExtensions ? 1 : 0);
		(*si.ostr) << "***CodeParts:" << cnt << endl;

		if (si.ioModules) {
			cnt = si.modp->ctrl()->subCntModules();
			(*si.ostr) << "**Modules:" << cnt << endl;
			si.modp->ctrl()->serializeModulesTo(si, this);
		}

		if (si.ioExtensions) {
			cnt = si.modp->ctrl()->subCntExtensions();
			(*si.ostr) << "**Extensions:" << cnt << endl;
			si.modp->ctrl()->serializeExtensionsTo(si, this);
		}
	}

	/**
	 * write data from data object to output stream
	 * @param si		serialization info
	 */
	void MainData::serializeDataTo(SerializeInfo& si)
	{
		vector<bool> ds;
		ds.reserve(_data.size());

		int cnt = 0;
		for (map<string, MainData::DataBase *>::iterator it = _data.begin(); it != _data.end(); it++) {
			MainData::DataBase *d = it->second;
			if (d && checkDataSerialize(si, d, it->first)) {
				cnt++;
				ds.push_back(true);
			}
			else {
				ds.push_back(false);
			}
		}

		(*si.ostr) << "***Data:" << cnt << endl;

		int i = 0;
		for (map<string, MainData::DataBase *>::iterator it = _data.begin(); it != _data.end(); it++, i++) {
			if (ds[i]) {
				(*si.ostr) << "**" << it->first;

				MainData::DataBase *d = it->second;
				if (d->subPartCount())
					(*si.ostr) << ':' << d->subPartCount() << ';' << _globStrings->search(d->modName()) << endl;
				else
					(*si.ostr) << endl << "*:" << d->subCnt() << ';' << d->linesPerData() << ';' << _globStrings->search(d->modName());

				d->serializeTo(si, this);
			}
		}
	}

	/**
	 * check whether a data object has to be serialized
	 * @param data			main data object
	 * @param d				data object
	 * @param nc			name of data object
	 * @return				true if data object has to be serialized
	 */
	bool MainData::checkDataSerialize(SerializeInfo& si, DataBase *d, const string& nc)
	{
		bool r = (si.ioDataDefault > 0 || (si.ioDataDefault == 0 && d->checkSerialize(si, this)));

		string m = d->modName();

		if (!r && si.ioDataModules && si.ioDataModules->count(m) != 0)
			r = true;

		if (r && si.ioDataNoModules && si.ioDataNoModules->count(m) != 0)
			r = false;

		if (!r && si.ioDataClasses && si.ioDataClasses->count(nc) != 0)
			r = true;

		if (r && si.ioDataNoClasses && si.ioDataNoClasses->count(nc) != 0)
			r = false;

		return r;
	}


	/************** read from serialization stream **********/

	/**
	 * fill data object from input stream
	 * @param si		serialization info
	 */
	void MainData::deserializeFrom(SerializeInfo& si)
	{
		// read start section
		int outLevel, cnt, subCnt, lc;
		string rline;

		string sect = readSectionStart(si, 4, outLevel, cnt, lc, rline);
		if (outLevel != 4 || sect != "CmplData")
			throw MainDataInputException("invalid start of data", *si.pos);

		si.version = getNextLong(rline, si.pos);
		si.instance = getNextLong(rline, si.pos);
		si.lineNums = (getNextLong(rline, si.pos) != 0);

		si.pos->addLines();

		for (int ss = 0; ss < cnt; ss++) {
			sect = readSectionStart(si, 3, outLevel, subCnt, lc, rline);

			if (ss == 0) {
				if (outLevel != 3 || sect != "Common")
					throw MainDataInputException("section 'Common' expected", *si.pos);

				si.pos->addLines();
				readSectionCommon(si, subCnt);
			}
			else {
				si.pos->addLines();

				if (sect == "CmdLineOpts") {
					if (si.ioCmdLineOpts)
						readSectionCmdLineOpts(si, subCnt);
					else
						skipSection(si, outLevel, subCnt, lc);
				}
				else if (sect == "CodeParts") {
					if (si.ioModules || si.ioExtensions)
						readSectionCodeParts(si, subCnt);
					else
						skipSection(si, outLevel, subCnt, lc);
				}
				else if (sect == "Data") {
					readSectionData(si, subCnt);
				}
			}
		}
	}

	/**
	 * read common data from input stream
	 * @param si		serialization info
	 * @param cnt		count of elements within the section
	 */
	void MainData::readSectionCommon(SerializeInfo& si, int cnt)
	{
		int outLevel, subCnt, lc;
		string sect, rline;

		// strings
		sect = readSectionStart(si, 2, outLevel, subCnt, lc, rline);
		if (outLevel != 1 || sect != "Strings")
			throw MainDataInputException("section 'Strings' expected", *si.pos);

		si.pos->addLines();
		readStringsData(si, subCnt);

		// locations
		sect = readSectionStart(si, 2, outLevel, subCnt, lc, rline);
		if (outLevel != 1 || sect != "Locations")
			throw MainDataInputException("section 'Locations' expected", *si.pos);

		si.pos->addLines();
		readLocationsData(si, subCnt);
	
		// MainControl data
		sect = readSectionStart(si, 2, outLevel, subCnt, lc, rline);
		if (outLevel != 2 || sect != "MainControl")
			throw MainDataInputException("section 'MainControl' expected", *si.pos);

		si.pos->addLines();
		readSectionMainControl(si, subCnt);
	}

	/**
	 * read data lines of Strings section from input stream
	 * @param si		serialization info
	 * @param cnt		count of elements within the section
	 */
	void MainData::readStringsData(SerializeInfo& si, int cnt)
	{
		char *buf = NULL;
		size_t cap = 0;

		si.mapStrings = new unsigned[cnt];
		unsigned *ip = si.mapStrings;

		for (int i = 0; i < cnt; i++, ip++) {
			readLenCodedDataLine(si, &buf, cap);
			*ip = _globStrings->storeInd(buf);
		}

		if (buf)
			delete buf;
	}

	/**
	 * read data lines of Locations section from input stream
	 * @param si		serialization info
	 * @param cnt		count of elements within the section
	 */
	void MainData::readLocationsData(SerializeInfo& si, int cnt)
	{
		string line;
		int t, n, il;
		unsigned l, c;
		PositionInfo pb, pe;
		PositionInfo *p;

        vector<int> iln(cnt*2);
        si.mapLocsFrom.clear();
        si.mapLocsFrom.resize(cnt);

        for (int i = 0; i < cnt; i++) {
			for (int j = 0; j < 2; j++) {
				getline(*si.istr, line);
				if (!si.istr->good())
					throw MainDataInputException("unexpected end of file or read error", *si.pos);
				
				if (si.lineNums)
					getNextLong(line, si.pos);

				t = getNextLong(line, si.pos);
				n = getNextLong(line, si.pos);
				l = getNextLong(line, si.pos);
				c = getNextLong(line, si.pos);
				il = getNextLong(line, si.pos);

				p = (j==0 ? &pb : &pe);
				*p = PositionInfo(t, (*_globStrings)[n]);

				p->setLine(l);
				p->setCol(c);
                iln[2*i + j] = il;

				si.pos->addLines();
			}

            si.mapLocsFrom[i] = new LocationInfo(pb, pe);
		}

        for (int i = 0; i < cnt; i++) {
            LocationInfo *loc = si.mapLocsFrom[i];

            for (int j = 0; j < 2; j++) {
                if (iln[2*i + j] >= 0) {
                    p = const_cast<PositionInfo *>(j==0 ? &(loc->begin()) : &(loc->end()));  // location info is not in use yet, so discarding const is safe
                    p->setInLoc(si.mapLocsFrom[iln[2*i + j]]);
                }
            }

            unsigned long key = loc->key();
            vector<LocationInfo *>& vl = (*_globLocs)[key];
            vl.push_back(loc);
        }
    }

	/**
	 * read section for MainControl data from input stream
	 * @param si		serialization info
	 * @param cnt		count of elements within the section
	 */
	void MainData::readSectionMainControl(SerializeInfo& si, int cnt)
	{
		int outLevel, subCnt, lc;
		string sect, rline, line;
		int iSect;
		int i, j;
		long n1, n2, n3, n4, n5, n6, n7;
		set<string> mnames;
		string mname;

		for (i = 0; i < cnt; i++) {
			sect = readSectionStart(si, 1, outLevel, subCnt, lc, rline);
			iSect = (sect == "CmplFileBase" ? 1 : (sect == "ExecutedModules" ? 2 : (sect == "Errors" ? 3 : 0)));
			if (iSect == 0 || lc == 0)
				throw MainDataInputException("invalid subsection within section 'MainControl'", *si.pos);

			switch (iSect) {
				case 1:		// CmplFileBase
					_cmplFileBase = NULL;
					break;

				case 2:		// ExecutedModules
					if (_executedModules) {
						for (list<ExecutedModule>::iterator it = _executedModules->begin(); it != _executedModules->end(); it++)
							mnames.insert(string(it->module) + '\n' + it->instance);
					}
					else {
						_executedModules = new list<ExecutedModule>;
					}
					break;

				case 3:		// Errors
					_errorLevelMax = getNextLong(rline, si.pos);
					_errorCnt = getNextLong(rline, si.pos);
					_warnCnt = getNextLong(rline, si.pos);

					if (_errorList)
						_errorList->clear();
					else
						_errorList = new list<ErrorEntry>;
					break;
			}

			si.pos->addLines();

			for (j = 0; j < subCnt; j++) {
				getline(*si.istr, line);
				if (!si.istr->good())
					throw MainDataInputException("unexpected end of file or read error", *si.pos);
				
				if (si.lineNums)
					getNextLong(line, si.pos);

				switch (iSect) {
					case 1:		// CmplFileBase
						n1 = getNextLong(line, si.pos);
						_cmplFileBase = _globStrings->at(si.mapStrings[n1]);
						break;

					case 2:		// ExecutedModules
						n1 = getNextLong(line, si.pos);
						n2 = getNextLong(line, si.pos);
						n3 = getNextLong(line, si.pos);
						mname = string(_globStrings->at(si.mapStrings[n1])) + '\n' + _globStrings->at(si.mapStrings[n3]);
						if (mnames.count(mname) == 0) {
							// don't read info whether execution is finished, but treat all modules as finished
							_executedModules->push_back(ExecutedModule(_globStrings->at(si.mapStrings[n1]), (n2 >= 0 ? _globStrings->at(si.mapStrings[n2]) : NULL), _globStrings->at(si.mapStrings[n3]), true));
						}
						break;

					case 3:		// Errors
						n1 = getNextLong(line, si.pos);
						n2 = getNextLong(line, si.pos);
						n3 = getNextLong(line, si.pos);
						n4 = getNextLong(line, si.pos);
						n5 = getNextLong(line, si.pos);
						n6 = getNextLong(line, si.pos);
						n7 = getNextLong(line, si.pos);
						_errorList->push_back(ErrorEntry(n1, n2, _globStrings->at(si.mapStrings[n3]), (n4 >= 0 ? _globStrings->at(si.mapStrings[n4]) : NULL),
                                _globStrings->at(si.mapStrings[n5]), _globStrings->at(si.mapStrings[n6]), si.mapLocsFrom[n7]));
						break;
				}

				si.pos->addLines();
			}
		}
	}


	/**
	 * read common data from input stream
	 * @param si		serialization info
	 * @param cnt		count of elements within the section
	 */
	void MainData::readSectionCmdLineOpts(SerializeInfo& si, int cnt)
	{
		si.modp->ctrl()->deserializeCmdLineOptsFrom(si, this, cnt);
	}


	/**
	 * read common data from input stream
	 * @param si		serialization info
	 * @param cnt		count of elements within the section
	 */
	void MainData::readSectionCodeParts(SerializeInfo& si, int cnt)
	{
		int outLevel, subCnt, lc;
		string sect, rline;

		for (int ss = 0; ss < cnt; ss++) {
			sect = readSectionStart(si, 2, outLevel, subCnt, lc, rline);
			si.pos->addLines();

			if (sect == "Modules") {
				if (si.ioModules)
					si.modp->ctrl()->deserializeModulesFrom(si, this, subCnt);
				else
					skipSection(si, outLevel, subCnt, lc);
			}
			else if (sect == "Extensions") {
				if (si.ioModules)
					si.modp->ctrl()->deserializeExtensionsFrom(si, this, subCnt);
				else
					skipSection(si, outLevel, subCnt, lc);
			}
		}
	}


	/**
	 * read common data from input stream
	 * @param si		serialization info
	 * @param cnt		count of elements within the section
	 */
	void MainData::readSectionData(SerializeInfo& si, int cnt)
	{
		int outLevel, subCnt, lc;
		string dataName, rline;
		const char *modName;
		MainData *data = this;

		for (int ss = 0; ss < cnt; ss++) {
			dataName = readSectionStart(si, 2, outLevel, subCnt, lc, rline);
			modName = _globStrings->at(si.mapStrings[getNextLong(rline, si.pos)]);

			// construct registered data object
			MainData::DataBase *dataObj;
			if (0) { }
			#include "../CommonData/data.reg"
			else {
				// unknown data, skip it
				si.modp->ctrl()->errHandler().error(ERROR_LVL_WARN, si.modp->ctrl()->printBuffer("unknown data section '%s', skipped", dataName.c_str()), *si.pos);
				dataObj = NULL;
			}

			// check if data is to read
			if (dataObj && checkDataSerialize(si, dataObj, dataName)) {
				dataObj->deserializeFrom(si, this, subCnt, lc, rline);
				setData(dataName, dataObj);
			}
			else {
				si.pos->addLines();
				skipSection(si, outLevel, subCnt, lc);
				if (dataObj)
					delete dataObj;
			}
		}
	}

	

	/************** static utility functions for serialization **********/

	/**
	 * read start line of a section
	 * @param si		serialization info
	 * @param inLevel	level of expected section
	 * @param outLevel	return level of found section
	 * @param cnt		return count of elements within the section
	 * @param lc		return line count per element or 0 if elements are len coded
	 * @param rline		return remaining section start line
	 * @return			name of found section
	 */
	string MainData::readSectionStart(SerializeInfo& si, int inLevel, int& outLevel, int& cnt, int& lc, string &rline)
	{
		string line, sect;
		int i;

		getline(*si.istr, line);
		if (!si.istr->good())
			throw MainDataInputException("unexpected end of file or read error", *si.pos);

		for (i = 0; i < inLevel; i++) {
			if (i >= (int)(line.size()) || line[i] != '*')
				throw MainDataInputException(si.modp->ctrl()->printBuffer("invalid start for section level %d", inLevel), *si.pos);
		}

		size_t p = line.find(':');
		if (p == string::npos) {
			// section is pseudo section, continue with subsection
			sect = StringStore::lrTrim(line.substr(inLevel));

			si.pos->addLines();
			string sub = readSectionStart(si, inLevel-1, outLevel, cnt, lc, rline);
			if (inLevel == 1 || !sub.empty())
				throw MainDataInputException("invalid pseudo section start", *si.pos);
		}
		else {
			sect = StringStore::lrTrim(line.substr(inLevel, p - inLevel));
			rline = line.substr(p+1);
			si.pos->addCols(p+1);

			outLevel = inLevel;
			cnt = getNextLong(rline, si.pos);

			if (inLevel == 1)
				lc = getNextLong(rline, si.pos);
			else
				lc = 0;
		}

		return sect;
	}
	
	/**
	 * skip section with its data
	 * @param si		serialization info
	 * @param level		level of section
	 * @param cnt		count of elements within the section
	 * @param lc		line count per element or 0 if elements are len coded
	 */
	void MainData::skipSection(SerializeInfo& si, int level, int cnt, int lc)
	{
		string line;
		int i;

		if (level > 1) {
			// skip every subsection
			int outLevel, subCnt, lc;
			string rline;
				
			for (i = 0; i < cnt; i++) {
				readSectionStart(si, level-1, outLevel, subCnt, lc, rline);
				si.pos->addLines();

				skipSection(si, outLevel, subCnt, lc);
			}
		}

		else {
			// skip data lines
			if (lc == 0) {
				char *buf = NULL;
				size_t cap = 0;

				for (i = 0; i < cnt; i++) {
					readLenCodedDataLine(si, &buf, cap);
				}

				if (buf)
					delete buf;
			}
			else {
				for (i = 0; i < cnt*lc; i++) {
					getline(*si.istr, line);
					if (!si.istr->good())
						throw MainDataInputException("unexpected end of file or read error", *si.pos);

					si.pos->addLines();
				}
			}
		}
	}

	/**
	 * get int value from string, and set string and position to the next possible value.
	 * @param s			input string, returns string without parsed value
	 * @param pos		position of input string, returns iterated position
	 * @return			parsed value
	 */
	long MainData::getNextLong(string &s, PositionInfo *pos)
	{
		int res;
		size_t p;

		try {
			res = stol(s, &p);
		}
		catch (exception) {
			throw MainDataInputException("int value expected", *pos);
		}

		if (p < s.size())
			s = s.substr(p+1);
		else
			s.clear();

		pos->addCols(p+1);

		return res;
	}

    /**
     * get double value from string, and set string and position to the next possible value.
     * @param s			input string, returns string without parsed value
     * @param pos		position of input string, returns iterated position
     * @return			parsed value
     */
    double MainData::getNextDouble(string &s, PositionInfo *pos)
    {
        double res;
        size_t p;

        try {
            res = stod(s, &p);
        }
        catch (exception) {
            throw MainDataInputException("double value expected", *pos);
        }

        if (p < s.size())
            s = s.substr(p+1);
        else
            s.clear();

        pos->addCols(p+1);

        return res;
    }

	/**
	 * read len coded data line from serialized data
	 * @param si		serialization info
	 * @param buf		buffer to return data
	 * @param cap		current capacity of buf
	 * @return			length of data
	 */
	size_t MainData::readLenCodedDataLine(SerializeInfo& si, char **buf, size_t& cap)
	{
		// read length
		string ls;
		getline(*si.istr, ls, ':');

		if (si.lineNums)
			getNextLong(ls, si.pos);

		size_t len = getNextLong(ls, si.pos);

		// allocate buffer
		if (!(*buf) || len > cap) {
			if (*buf)
				delete *buf;

			cap = len;
			*buf = new char[cap+1];
		}

		// read data
		si.istr->read(*buf, len);
		(*buf)[len] = '\0';
		if (!si.istr->good())
			throw MainDataInputException("unexpected end of file or read error", *si.pos);

		// iterate position
		for (size_t j = 0; j < len; j++) {
			if ((*buf)[j] == '\n')
				si.pos->addLines();
		}

		// read new line after string
		getline(*si.istr, ls);
		if (!si.istr->good())
			throw MainDataInputException("unexpected end of file or read error", *si.pos);
			
		si.pos->addLines();
		return len;
	}
}


