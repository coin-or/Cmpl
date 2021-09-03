
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


#include "Serialize.hh"
#include "../../Control/MainControl.hh"


namespace cmpl
{
	/*********** module definition **********/

	// defines the module "serialize" that is implemented in class "Serialize".
	// to register this module you have to insert "MODULE_CLASS_REG(1, serialize)" to file "modules.reg"
	MODULE_CLASS( serialize, Serialize )



	/*********** initialization **********/

	/**
	 * initialize modul, to call immediately after the constructor.
	 * @param ctrl			main object
	 * @param data			data object
	 * @param name			module name (or alias)
	 */
	void Serialize::init(MainControl *ctrl, MainData *data, const char *name)
	{
		ModuleBase::init(ctrl, data, name);

		_ioCmdLineOpts = false;
		_ioToServer = false;
		_ioFromServer = false;

		_ioDataDefault = 0;
		_lineNumbers = false;

		_streamTo = NULL;
		_streamFrom = NULL;
	}


	/*********** handling of command line options **********/

#define OPTION_SERIALIZE_WRITE_FILE			10
#define OPTION_SERIALIZE_READ_FILE			11

	// options to control the parts of data to write or read
#define OPTION_SERIALIZE_CMD_LINE_OPTS		20
#define OPTION_SERIALIZE_MODULES			21
#define OPTION_SERIALIZE_EXTENSIONS			22

#define OPTION_SERIALIZE_DATA_TO_SERVER		30
#define OPTION_SERIALIZE_DATA_FROM_SERVER	31
#define OPTION_SERIALIZE_SERVER_STEP		32

#define OPTION_SERIALIZE_DATA_DEFAULT		40
#define OPTION_SERIALIZE_DATA_CLASS			41
#define OPTION_SERIALIZE_DATA_MODULE		42

#define OPTION_SERIALIZE_LINENUMBERS		50

	// pseudo options only for use when directly called from other modules
#define OPTION_SERIALIZE_STREAM_OUT			100
#define OPTION_SERIALIZE_STREAM_IN			101



	/**
	 * register command line options options for delivery to this module
	 * @param modOptReg		vector to register in
	 */
	void Serialize::regModOptions(vector<CmdLineOptList::RegOption> &modOptReg)
	{
		ModuleBase::regModOptions(modOptReg);

		REG_CMDL_OPTION( OPTION_SERIALIZE_WRITE_FILE, "writefile", 1, 1, CMDL_OPTION_NEG_NO_ARG, true );
		REG_CMDL_OPTION( OPTION_SERIALIZE_READ_FILE, "readfile", 1, 1, CMDL_OPTION_NEG_NO_ARG, true );

		REG_CMDL_OPTION( OPTION_SERIALIZE_CMD_LINE_OPTS, "io-cmdl-opts", 0, 0, CMDL_OPTION_NEG_DELIV, true );
		REG_CMDL_OPTION( OPTION_SERIALIZE_MODULES, "io-modules", 0, 0, CMDL_OPTION_NEG_DELIV, true );
		REG_CMDL_OPTION( OPTION_SERIALIZE_EXTENSIONS, "io-extensions", 0, 0, CMDL_OPTION_NEG_DELIV, true );

		REG_CMDL_OPTION( OPTION_SERIALIZE_DATA_TO_SERVER, "io-to-server", 0, 0, CMDL_OPTION_NEG_DELIV, true );
		REG_CMDL_OPTION( OPTION_SERIALIZE_DATA_FROM_SERVER, "io-from-server", 0, 0, CMDL_OPTION_NEG_DELIV, true );
		REG_CMDL_OPTION( OPTION_SERIALIZE_SERVER_STEP, "io-server-step", 1, 1, CMDL_OPTION_NEG_IGNORE, true );

		REG_CMDL_OPTION( OPTION_SERIALIZE_DATA_DEFAULT, "io-data-default", 0, 1, CMDL_OPTION_NEG_IGNORE, true );
		REG_CMDL_OPTION( OPTION_SERIALIZE_DATA_CLASS, "io-data-class", 1, -1, CMDL_OPTION_NEG_DELIV, false );
		REG_CMDL_OPTION( OPTION_SERIALIZE_DATA_MODULE, "io-data-module", 1, -1, CMDL_OPTION_NEG_DELIV, false );

		REG_CMDL_OPTION( OPTION_SERIALIZE_LINENUMBERS, "linenumbers", 0, 0, CMDL_OPTION_NEG_DELIV, true );

		REG_CMDL_OPTION( OPTION_SERIALIZE_STREAM_OUT, "*ostream", 1, 1, CMDL_OPTION_NEG_IGNORE, false );
		REG_CMDL_OPTION( OPTION_SERIALIZE_STREAM_IN, "*istream", 1, 1, CMDL_OPTION_NEG_IGNORE, false );
	}

	/**
	 * parse single option from command line options, this function is called for every delivered option
	 * @param ref			reference number of option registration, should be used for discriminate the options
	 * @param prio			priority value of option
	 * @param opt			option
	 * @return				true if option is used by the module
	 */
	bool Serialize::parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt)
	{
		if (ModuleBase::parseOption(ref, prio, opt))
			return true;

		void *p;

		switch (ref) {
			case OPTION_SERIALIZE_WRITE_FILE:
				if (opt->neg())
					_fileTo.clear();
				else {
					_fileTo = (*opt)[0];
					_fileToLoc = opt->loc(true);
				}
				return true;

			case OPTION_SERIALIZE_READ_FILE:
				if (opt->neg())
					_fileFrom.clear();
				else {
					_fileFrom = (*opt)[0];
					_fileFromLoc = opt->loc(true);
				}
				return true;

            case OPTION_SERIALIZE_CMD_LINE_OPTS:
				_ioCmdLineOpts = !opt->neg();
				return true;

			case OPTION_SERIALIZE_MODULES:
				_ioModules = !opt->neg();
				return true;

			case OPTION_SERIALIZE_EXTENSIONS:
				_ioExtensions = !opt->neg();
				return true;

            case OPTION_SERIALIZE_DATA_TO_SERVER:
				_ioToServer = !opt->neg();
				return true;

            case OPTION_SERIALIZE_DATA_FROM_SERVER:
				_ioFromServer = !opt->neg();
				return true;

			case OPTION_SERIALIZE_SERVER_STEP:
				_ioServerStep = opt->argAsInt(0, _ctrl);
				return true;

			case OPTION_SERIALIZE_DATA_DEFAULT:
				if (opt->size() == 0 || ((*opt)[0]) == "check")
					_ioDataDefault = 0;
				else if (((*opt)[0]) == "all")
					_ioDataDefault = 1;
				else if (((*opt)[0]) == "none")
					_ioDataDefault = -1;
				else
					_ctrl->errHandler().error(ERROR_LVL_NORMAL, ctrl()->printBuffer("unknown argument '%s' for command line option 'io-data-default'", ((*opt)[0]).c_str()), opt->loc(true));
				return true;

			case OPTION_SERIALIZE_DATA_CLASS:
				{
					set<string>& ioData = (opt->neg() ? _ioDataNoClasses : _ioDataClasses);
					for (unsigned i = 0; i < opt->size(); i++)
						ioData.insert((*opt)[i]);
				}
				return true;

			case OPTION_SERIALIZE_DATA_MODULE:
				{
					set<string>& ioData = (opt->neg() ? _ioDataNoModules : _ioDataModules);
					for (unsigned i = 0; i < opt->size(); i++)
						ioData.insert((*opt)[i]);
				}
				return true;

			case OPTION_SERIALIZE_LINENUMBERS:
				_lineNumbers = !opt->neg();
				return true;

			case OPTION_SERIALIZE_STREAM_OUT:
				sscanf((*opt)[0].c_str(), "%p", &p);
				_streamTo = (ostream *) p;
				return true;

			case OPTION_SERIALIZE_STREAM_IN:
				sscanf((*opt)[0].c_str(), "%p", &p);
				_streamFrom = (istream *) p;
				return true;
		}

		return false;
	}

	/**
	 * writes usage info for the module to stream
	 * @param s				stream to write to
	 */
	void Serialize::usage(ostream& s)
	{
		ModuleBase::usage(s);

		s << "  -writefile <file>             write current cmpl data state to <file>" << endl;
		s << "  -readfile <file>              read current cmpl data state from <file>" << endl;

		s << "  -io-cmdl-opts                 input/output also of the command line options" << endl;
		s << "  -io-modules                   input/output also the names of the modules to execute" << endl;
		s << "  -io-extensions                input/output also the extension classes" << endl;
		
		s << "  -io-to-server                 input/output only data for the transfer from client to server" << endl;
		s << "  -io-from-server               input/output only data for the transfer from server to client" << endl;
		s << "  -io-server-step <number>      use module order step <number> of transfer of data from/to server" << endl;

		s << "  -io-data-default [all | none] input/output all or none data as default (modified then by -io-data-class and -io-data-module)" << endl;
		s << "  -io-data-class name1 ...      include (when negated exclude) data of these data classes in the input/output" << endl;
		s << "  -io-data-module name1 ...     include (when negated exclude) data generated by these modules in the input/output" << endl;

		s << "  -linenumbers                  print line numbers to serialized data (only for better readibility)" << endl;

		// pseudo options: don't print as usage
		/*
		s << "  *ostream <streampointer>      write to the stream. <streampointer> is the pointer to the stream as printed with %p" << endl;
		s << "  *istream <streampointer>      read from the stream. <streampointer> is the pointer to the stream as printed with %p" << endl;
		*/
	}

	/**
	 * run module function
	 */
	void Serialize::run()
	{
		_ctrl->errHandler().setExecStep("run");

		PROTO_OUTL("Start run serialize module " << moduleName());

		if (!_fileTo.empty())
			serializeToFile(_fileTo, _fileToLoc);

		if (!_fileFrom.empty())
			deserializeFromFile(_fileFrom, _fileFromLoc);

		if (_streamTo)
			serializeTo(_streamTo);

		if (_streamFrom) {
			PositionInfo pos(POSITION_TYPE_DESCR, "(internal)");
			deserializeFrom(_streamFrom, pos);
		}

		PROTO_OUTL("End run serialize module " << moduleName());
	}

#if 0
	/**
	 * check whether a data object has to be serialized
	 * @param si			serialization info
	 * @param data			main data object
	 * @param d				data object
	 * @param nc			name of data object
	 * @return				true if data object has to be serialized
	 */
	bool Serialize::ioDataSerialize(MainData::SerializeInfo& si, const MainData *data, MainData::DataBase *d, const string& nc)
	{
		bool r = (_ioDataDefault > 0 || (_ioDataDefault == 0 && d->checkSerialize(si, data)));

		string m = d->modName();

		if (!r && _ioDataModules.count(m) != 0)
			r = true;

		if (r && _ioDataNoModules.count(m) != 0)
			r = false;

		if (!r && _ioDataClasses.count(nc) != 0)
			r = true;

		if (r && _ioDataNoClasses.count(nc) != 0)
			r = false;

		return r;
	}
#endif


	/**
	 * write content of <code>_data</code> to file
	 * @param fn            filename
	 * @param loc			location of filename in command line options
	 */
	void Serialize::serializeToFile(string& fn, LocationInfo& loc)
	{
		FileOutput file;
		file.setFile(_data, IO_MODE_FILE, &fn, NULL);

		ostream *str;
		try {
			str = file.open();
		}
		catch (exception& ex) {
			_ctrl->errHandler().error(ERROR_LVL_FATAL, "serialization write file error", loc, &ex);
		}

		serializeTo(str, &loc);

		file.close();
	}

	/**
	 * write content of <code>_data</code> to stream
	 * @param str			stream
	 * @param loc			location for error message / NULL: internal
	 */
	void Serialize::serializeTo(ostream *str, LocationInfo *loc)
	{
		try {
			MainData::SerializeInfo si(this, str, _data->version(), _data->globStrings()->storeInd(_ctrl->instanceName()), _lineNumbers);
			si.setIoControl(_ioCmdLineOpts, _ioModules, _ioExtensions);

			if (_ioToServer || _ioFromServer)
				si.setIoServer(_ioToServer ? 1 : (_ioFromServer ? -1 : 0), _ioServerStep);

			si.setIoData(_ioDataDefault, &_ioDataClasses, &_ioDataNoClasses, &_ioDataModules, &_ioDataNoModules);

			_data->serializeTo(si);
		}
		catch (exception& ex) {
			if (loc)
				_ctrl->errHandler().error(ERROR_LVL_FATAL, "serialization write error", *loc, &ex);
			else
				_ctrl->errHandler().internalError("serialization write error", &ex);
		}
	}


	/**
	 * read content for <code>_data</code> from file
	 * @param fn            filename
	 * @param loc			location of filename in command line options
	 */
	void Serialize::deserializeFromFile(string& fn, LocationInfo& loc)
	{
		FileInput file;
		file.setFile(_data, IO_MODE_FILE, &fn, NULL);

		PositionInfo pos(&loc, POSITION_TYPE_FILE, fn.c_str());

		istream *str;
		try {
			str = file.open();
		}
		catch (exception& ex) {
			_ctrl->errHandler().error(ERROR_LVL_FATAL, "serialization read file error", loc, &ex);
		}

		deserializeFrom(str, pos, &loc);

		file.close();
	}

	/**
	 * read content of <code>_data</code> from stream
	 * @param str			stream
	 * @param pos			start position in input stream
	 * @param loc			location for error message / NULL: internal
	 */
	void Serialize::deserializeFrom(istream *str, PositionInfo& pos, LocationInfo *loc)
	{
		try {
			MainData::SerializeInfo si(this, str, &pos);
			si.setIoControl(_ioCmdLineOpts, _ioModules, _ioExtensions);

			if (_ioToServer || _ioFromServer)
				si.setIoServer(_ioToServer ? 1 : (_ioFromServer ? -1 : 0), _ioServerStep);

			si.setIoData(_ioDataDefault, &_ioDataClasses, &_ioDataNoClasses, &_ioDataModules, &_ioDataNoModules);

			_data->deserializeFrom(si);
		}
		catch (exception& ex) {
			if (loc)
				_ctrl->errHandler().error(ERROR_LVL_FATAL, "serialization read error", *loc, &ex);
			else
				_ctrl->errHandler().internalError("serialization read error", &ex);
		}
	}

}


