
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


#include "TransferFromClient.hh"
#include "../../Control/MainControl.hh"


namespace cmpl
{
	/*********** module definition **********/

	// defines the module "transferFromClient" that is implemented in class "TransferFromClient".
	// to register this module you have to insert "MODULE_CLASS_REG(<execution step>, transferFromClient, <description>)" to file "modules.reg"
	MODULE_CLASS( transferFromClient, TransferFromClient )



	/*********** initialization **********/

	/**
	 * initialize modul, to call immediately after the constructor.
	 * @param ctrl			main object
	 * @param data			data object
	 * @param name			module name (or alias)
	 */
	void TransferFromClient::init(MainControl *ctrl, MainData *data, const char *name)
	{
		ModuleBase::init(ctrl, data, name);
	}


	/*********** handling of command line options **********/

#define OPTION_CLIENT_DATAFILE		10

	/**
	 * register command line options options for delivery to this module
	 * @param modOptReg		vector to register in
	 */
	void TransferFromClient::regModOptions(vector<CmdLineOptList::RegOption> &modOptReg)
	{
		ModuleBase::regModOptions(modOptReg);

		REG_CMDL_OPTION( OPTION_CLIENT_DATAFILE, "datafile-from-client", 1, 1, CMDL_OPTION_NEG_ERROR, true );
	}

	/**
	 * parse single option from command line options, this function is called for every delivered option
	 * @param ref			reference number of option registration, should be used for discriminate the options
	 * @param prio			priority value of option
	 * @param opt			option
	 * @return				true if option is used by the module
	 */
	bool TransferFromClient::parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt)
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
	void TransferFromClient::usage(ostream& s)
	{
		ModuleBase::usage(s);

		// Die Serversteuerung muss das Server-Cmpl mit dieser Kommandozeilenoption aufrufen
		// (Wenn die vom Client gelieferten Daten doch keine eigene Datei sein sollen, sondern als CDATA-Block in die XML-Datei mit der Anforderung an den Server
		//  eingebettet sein sollen, dann könnte man hier als weiteres Argument zu diesem Parameter die Startposition der Daten in der XML-Datei aufnehmen.
		//  Vor dem Aufruf des eigentlichen Einlesens müsste dann der Input-Stream auf diese Position gesetzt werden.
		//  Eine andere Alternative wäre, dass die Server-Steuerung den Inhalt des CDATA-Blocks dem Server-Cmpls als Standardeingabe zur Verfügung stellt,
		//  dann müsste sie als Pseudo-Dateiname 0 übergeben.)
		s << "  -datafile-from-client <file>  name of the file with the data delivered from the client" << endl;
	}

	/**
	 * run module function
	 */
	void TransferFromClient::run()
	{
		_ctrl->errHandler().setExecStep("run");

		PROTO_OUTL("Start run module " << moduleName());

		if (_dataFile.empty())
			_ctrl->errHandler().internalError("name of data file delivered from the client missing");
		else
			executeTransfer();

		PROTO_OUTL("End run module " << moduleName());
	}

	/**
	 * execute the transfer
	 */
	void TransferFromClient::executeTransfer()
	{
        FileInput file;
        file.setFile(_data, IO_MODE_FILE, &_dataFile, NULL);

		istream *strData;
		try {
			strData = file.open();
		}
		catch (exception& ex) {
			_ctrl->errHandler().internalError("open file error", &ex);
		}

		try {
			PROTO_OUTL("read data file '" << _dataFile << "' delivered from client cmpl");
			PositionInfo pos(POSITION_TYPE_FILE, file.fileName().c_str());
			readDataFromClientCmpl(*strData, pos);
		}
		catch (exception& ex) {
			_ctrl->errHandler().internalError("read data from client cmpl error", &ex);
		}

		file.close();
	}

	/**
	 * read data transfered from the client cmpl
	 * @param s				stream to read from
	 * @param pos			start position in input stream
	 */
	void TransferFromClient::readDataFromClientCmpl(istream &s, PositionInfo& pos)
	{
		MainData::SerializeInfo si(this, &s, &pos);
		si.setIoControl(true, true, true);
		si.setIoServer(1, _ctrl->getOrderForRunningModule(this));

		_data->deserializeFrom(si);
	}
}

