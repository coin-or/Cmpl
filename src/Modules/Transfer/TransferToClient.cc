
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


#include "TransferToClient.hh"
#include "../../Control/MainControl.hh"


namespace cmpl
{
	/*********** module definition **********/

	// defines the module "transferToClient" that is implemented in class "TransferToClient".
	// to register this module you have to insert "MODULE_CLASS_REG(<execution step>, transferToClient, <description>)" to file "modules.reg"
	MODULE_CLASS( transferToClient, TransferToClient )



	/*********** initialization **********/

	/**
	 * initialize modul, to call immediately after the constructor.
	 * @param ctrl			main object
	 * @param data			data object
	 * @param name			module name (or alias)
	 */
	void TransferToClient::init(MainControl *ctrl, MainData *data, const char *name)
	{
		ModuleBase::init(ctrl, data, name);
	}


	/*********** handling of command line options **********/

#define OPTION_CLIENT_DATAFILE		10

	/**
	 * register command line options options for delivery to this module
	 * @param modOptReg		vector to register in
	 */
	void TransferToClient::regModOptions(vector<CmdLineOptList::RegOption> &modOptReg)
	{
		ModuleBase::regModOptions(modOptReg);

		REG_CMDL_OPTION( OPTION_CLIENT_DATAFILE, "datafile-to-client", 1, 1, CMDL_OPTION_NEG_ERROR, true );
	}

	/**
	 * parse single option from command line options, this function is called for every delivered option
	 * @param ref			reference number of option registration, should be used for discriminate the options
	 * @param prio			priority value of option
	 * @param opt			option
	 * @return				true if option is used by the module
	 */
	bool TransferToClient::parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt)
	{
		if (ModuleBase::parseOption(ref, prio, opt))
			return true;

		switch (ref) {
			case OPTION_CLIENT_DATAFILE:
				_dataFile = (*opt)[0];
				return true;
		}

		return false;
	}

	/**
	 * writes usage info for the module to stream
	 * @param s				stream to write to
	 */
	void TransferToClient::usage(ostream& s)
	{
		ModuleBase::usage(s);

		// Die Serversteuerung muss das Server-Cmpl mit dieser Kommandozeilenoption aufrufen
		// (bzw. wenn die Daten irgendwie anders geschrieben werden müssen, dann mit entsprechend anderen Kommandozeilenoptionen)
		s << "  -datafile-to-client <file>    name of the file with the data delivered from the client" << endl;
	}

	/**
	 * run module function
	 */
	void TransferToClient::run()
	{
		_ctrl->errHandler().setExecStep("run");

		PROTO_OUTL("Start run module " << moduleName());

		if (_dataFile.empty())
			_ctrl->errHandler().internalError("name of data file to deliver to the client missing");
		else
			executeTransfer();

		PROTO_OUTL("End run module " << moduleName());
	}

	/**
	 * execute the transfer
	 */
	void TransferToClient::executeTransfer()
	{
        FileOutput file;
        file.setFile(_data, IO_MODE_FILE, &_dataFile, NULL);

		ostream *strData;
		try {
			strData = file.open();
		}
		catch (exception& ex) {
			_ctrl->errHandler().internalError("open file error", &ex);
		}

		try {
			PROTO_OUTL("write data file '" << _dataFile << "' to deliver to client cmpl");
			writeDataForClientCmpl(*strData);
		}
		catch (exception& ex) {
			_ctrl->errHandler().internalError("write data to client cmpl error", &ex);
		}

		file.close();

		// Mit der Ergebnisrückgabe an den Client ist die Ausführung des server cmpl jedenfalls beendet, falls doch noch Module danach folgen sollten,
		// werden sie deshalb hier deaktiviert
		_ctrl->delModules(_ctrl->getOrderForRunningModule(this), 9999);
	}

	/**
	 * write data to transfer to the client cmpl
	 * @param s				stream to write to
	 */
	void TransferToClient::writeDataForClientCmpl(ostream &s)
	{
		MainData::SerializeInfo si(this, &s, _data->version(), _data->globStrings()->storeInd(_ctrl->instanceName()), false);
		si.setIoControl(true, true, true);
		si.setIoServer(-1, _ctrl->getOrderForRunningModule(this));

		_data->serializeTo(si);
	}
}

