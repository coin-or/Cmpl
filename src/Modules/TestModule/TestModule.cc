
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


#include "TestModule.hh"
#include "../../Control/MainControl.hh"


namespace cmpl
{
	/*********** module definition **********/

	// defines the module "test" that is implemented in class "TestModule".
	// to register this module you have to insert "MODULE_CLASS_REG(1, test)" to file "modules.reg"
	MODULE_CLASS( test, TestModule )



	/*********** initialization **********/

	/**
	 * initialize modul, to call immediately after the constructor.
	 * @param ctrl			main object
	 * @param data			data object
	 * @param name			module name (or alias)
	 */
	void TestModule::init(MainControl *ctrl, MainData *data, const char *name)
	{
		ModuleBase::init(ctrl, data, name);

		_testA = false;
		_testB = -1;
	}


	/*********** handling of command line options **********/

#define OPTION_TEST_A		10
#define OPTION_TEST_B		11
#define OPTION_TEST_C		12
#define OPTION_FILE_OUT		20

	/**
	 * register command line options options for delivery to this module
	 * @param modOptReg		vector to register in
	 */
	void TestModule::regModOptions(vector<CmdLineOptList::RegOption> &modOptReg)
	{
		ModuleBase::regModOptions(modOptReg);

		REG_CMDL_OPTION( OPTION_TEST_A, "a", 0, 0, CMDL_OPTION_NEG_DELIV, true );
		REG_CMDL_OPTION( OPTION_TEST_B, "b", 1, 1, CMDL_OPTION_NEG_NO_ARG, true );
		REG_CMDL_OPTION( OPTION_TEST_C, "c", 0, -1, CMDL_OPTION_NEG_ERROR, false );

		REG_CMDL_OPTION( OPTION_FILE_OUT, "o", 1, 1, CMDL_OPTION_NEG_ERROR, true );
	}

	/**
	 * parse single option from command line options, this function is called for every delivered option
	 * @param ref			reference number of option registration, should be used for discriminate the options
	 * @param prio			priority value of option
	 * @param opt			option
	 * @return				true if option is used by the module
	 */
	bool TestModule::parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt)
	{
		if (ModuleBase::parseOption(ref, prio, opt))
			return true;

		switch (ref) {
			case OPTION_TEST_A:
				_testA = !opt->neg();
				return true;

			case OPTION_TEST_B:
				_testB = (opt->neg() ? -1 : opt->argAsInt(0, _ctrl));
				return true;

			case OPTION_TEST_C:
				for (unsigned i = 0; i < opt->size(); i++)
					_testC.push_back((*opt)[i]);
				return true;

			case OPTION_FILE_OUT:
				_outFile = (*opt)[0];
				return true;
		}

		return false;
	}

	/**
	 * writes usage info for the module to stream
	 * @param s				stream to write to
	 */
	void TestModule::usage(ostream& s)
	{
		ModuleBase::usage(s);

		s << "  -a                            enable test option a" << endl;
		s << "  -b <int>                      set test option b to integer number <int>" << endl;
		s << "  -c <name1> ...                add names to collection for test option c" << endl;
		s << "  -o <file>                     output of test options to <file> instead of stdout" << endl;
	}

	/**
	 * run module function
	 */
	void TestModule::run()
	{
		_ctrl->errHandler().setExecStep("run");

		PROTO_OUTL("Start run test module " << moduleName());

		// open output file
		FileOutput of;
		of.setFile(_data, (_outFile.empty() ? IO_MODE_STD_IO : IO_MODE_FILE), &_outFile, NULL, true);
		ostream *o = of.open();

		// output test options
		*o << "TestModule '" << moduleName() << "' test options:" << endl;
		*o << "  a: " << (_testA ? "true" : "false") << endl;

		*o << "  b: " << _testB << endl;

		*o << "  c:";
		for (unsigned i = 0; i < _testC.size(); i++)
			*o << " " << _testC[i];
		*o << endl;

		// close output file
		of.close();

		PROTO_OUTL("End run test module " << moduleName());
	}
}

