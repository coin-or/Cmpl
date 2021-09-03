
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


#ifndef SERIALIZE_HH
#define SERIALIZE_HH

#include <string>
#include <set>
#include <iostream>

#include "../../Control/ModuleBase.hh"
#include "../../Control/MainData.hh"


using namespace std;



namespace cmpl
{
    /**
	 * <code>Serialize</code> is used for write the cmpl data to a file or to read it from a file
	 */
	class Serialize : public ModuleBase
	{

		/************** command line options **********/

	private:
		string _fileTo;							///< file name to write to
		LocationInfo _fileToLoc;				///< location of command line option for file name to write to

		string _fileFrom;						///< file name to read from 
		LocationInfo _fileFromLoc;				///< location of command line option for file name to read from

		bool _ioCmdLineOpts;					///< input/output also of the command line options
		bool _ioModules;						///< input/output also the names of the modules to execute
		bool _ioExtensions;						///< input/output also extension classes

		bool _ioToServer;						///< input/output only data for the transfer from client to server
		bool _ioFromServer;						///< input/output only data for the transfer from server to client
		int _ioServerStep;						///< module order step of transfer of data from/to server

		int _ioDataDefault;						///< default for serialization of data classes: -1: none / 0: call checkSerialize() / 1: all
		set<string> _ioDataClasses;				///< names of data classes to serialize
		set<string> _ioDataNoClasses;			///< names of data classes not to serialize
		set<string> _ioDataModules;				///< names of modules whose data classes to serialize
		set<string> _ioDataNoModules;			///< names of modules whose data classes not to serialize

		bool _lineNumbers;						///< print line numbers in the serialized data (only for output)

		ostream *_streamTo;						///< write to this stream
		istream *_streamFrom;					///< read from this stream


		/************** overwritten methods of <code>ModuleBase</code> **********/

	public:
		/**
		 * initialize modul, to call immediately after the constructor.
		 * @param ctrl			main object
		 * @param data			data object
		 * @param name			module name (or alias)
		 */
		virtual void init(MainControl *ctrl, MainData *data, const char *name);

		/**
		 * run module function
		 */
		virtual void run();

	protected:
		/**
		 * register command line options options for delivery to this module
		 * @param modOptReg		vector to register in
		 */
		virtual void regModOptions(vector<CmdLineOptList::RegOption> &modOptReg);

		/**
		 * parse single option from command line options, this function is called for every delivered option
		 * @param ref			reference number of option registration, should be used for discriminate the options
		 * @param prio			priority value of option
		 * @param opt			option
		 * @return				true if option is used by the module
		 */
		virtual bool parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt);

		/**
		 * writes usage info for the module to stream
		 * @param s				stream to write to
		 */
		virtual void usage(ostream& s);


		/************** serialization **********/

	public:
		/**
		 * get whether input/output also of the command line options
		 */
		inline bool ioCmdLineOpts()										{ return _ioCmdLineOpts; }

		/**
		 * get whether input/output also the names of the modules to execute
		 */
		inline bool ioModules()											{ return _ioModules; }

		/**
		 * get whether input/output also extension classes
		 */
		inline bool ioExtensions()										{ return _ioExtensions; }

		/**
		 * get whether input/output only data for the transfer from client to server
		 */
		inline bool ioToServer()										{ return _ioToServer; }

		/**
		 * get whether input/output only data for the transfer from server to client
		 */
		inline bool ioFromServer()										{ return _ioFromServer; }

		/**
		 * get module order step of transfer of data from/to server
		 */
		inline int ioServerStep()										{ return _ioServerStep; }

		/**
		 * check whether a data object has to be serialized
		 * @param si			serialization info
		 * @param data			main data object
		 * @param d				data object
		 * @param nc			name of data object
		 * @return				true if data object has to be serialized
		 */
		bool ioDataSerialize(MainData::SerializeInfo& si, const MainData *data, MainData::DataBase *d, const string& nc);

		/**
		 * get whether to print line numbers in the serialized data (only for output)
		 */
		inline bool lineNumbers()										{ return _lineNumbers; }

	private:
		/**
		 * write content of <code>_data</code> to file
		 * @param fn			filename
		 * @param loc			location of filename in command line options
		 */
		void serializeToFile(string& fn, LocationInfo& loc);

		/**
		 * write content of <code>_data</code> to stream
		 * @param str           stream
		 * @param loc           location for error message / NULL: internal
		 */
		void serializeTo(ostream *str, LocationInfo *loc = NULL);

		/**
		 * read content for <code>_data</code> from file
		 * @param fn			filename
		 * @param loc			location of filename in command line options
		 */
		void deserializeFromFile(string& fn, LocationInfo& loc);

		/**
		 * read content of <code>_data</code> from stream
		 * @param str           stream
		 * @param pos           start position in input stream
		 * @param loc           location for error message / NULL: internal
		 */
		void deserializeFrom(istream *str, PositionInfo& pos, LocationInfo *loc = NULL);
	};
}

#endif // SERIALIZE_HH

