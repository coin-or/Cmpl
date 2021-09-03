
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


#include "Start.hh"
#include "Control/MainControl.hh"
#include "Control/MainData.hh"
#include "Control/CmdLineOptList.hh"

#if STANDALONE
	/**
	 * start of cmpl
	 */
	int main(int argc, char *argv[])
	{
		int errCode = -1;

		try {
			// get necessary objects
			cmpl::MainData *data = cmpl::Start::getNewEmptyMainData();
			cmpl::CmdLineOptList *opts = cmpl::Start::getNewEmptyCmdLineOptList();
			opts->fillFromMain(argc, argv);

			// run cmpl
			errCode = cmpl::Start::run(opts, data);

			// delete objects
			cmpl::Start::dispose(opts);
			cmpl::Start::dispose(data);
		}
		catch (exception& e) {
			cerr << "internal cmpl error: " << e.what() << endl;
			errCode = -1;
		}

		return errCode;
	}
#endif //STANDALONE


namespace cmpl
{
	/**
	 * gets a new <code>CmdLineOptList</code> object
	 */
	CmdLineOptList * Start::getNewEmptyCmdLineOptList()
	{
		return new CmdLineOptList();
	}

	/**
	 * free an <code>CmdLineOptList</code> object
	 * @param opts		CmdLineOptList to free, pointer is after that no longer usable
	 */
	void Start::dispose(CmdLineOptList *opts)
	{
		opts->cleanUp();
		delete opts;
	}


	/**
	 * gets a new <code>MainData</code> object
	 */
	MainData * Start::getNewEmptyMainData()
	{
		return new MainData();
	}

	/**
	 * free an <code>MainData</code> object
	 * @param args		MainData to free, pointer is after that no longer usable
	 */
	void Start::dispose(MainData *data)
	{
		data->cleanUp();
		delete data;
	}


	/**
	 * run cmpl
	 * @param opts			command line options
	 * @param data          object for input data and output data
	 * @return              exit code
	 */
	int Start::run(CmdLineOptList *opts, MainData *data)
	{
		// if no data object then alloc here one
		bool blnHasData = (data != NULL);
		if (!blnHasData)
			data = getNewEmptyMainData();

		// construct main control object and run it
		MainControl ctrl;
		ctrl.init(&ctrl, data, ctrl.moduleName());
		int res = ctrl.run(opts);

		// release data if allocated here
		if (!blnHasData)
			dispose(data);

		return res;
	}
}

