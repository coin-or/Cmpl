
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


#include "TransferFromServer.hh"
#include "../../Control/MainControl.hh"


namespace cmpl
{
	/*********** module definition **********/

	// defines the module "transferFromServer" that is implemented in class "TransferFromServer".
	// to register this module you have to insert "MODULE_CLASS_REG(<execution step>, TransferFromServer, <description>)" to file "modules.reg"
	MODULE_CLASS( transferFromServer, TransferFromServer )



	/*********** initialization **********/

	/**
	 * initialize modul, to call immediately after the constructor.
	 * @param ctrl			main object
	 * @param data			data object
	 * @param name			module name (or alias)
	 */
	void TransferFromServer::init(MainControl *ctrl, MainData *data, const char *name)
	{
		ModuleBase::init(ctrl, data, name);

		_serverJobId = -1;
	}


	/*********** handling of command line options **********/

#define OPTION_SERVER_JOB_ID		10

	/**
	 * register command line options options for delivery to this module
	 * @param modOptReg		vector to register in
	 */
	void TransferFromServer::regModOptions(vector<CmdLineOptList::RegOption> &modOptReg)
	{
		ModuleBase::regModOptions(modOptReg);

		REG_CMDL_OPTION( OPTION_SERVER_JOB_ID, "server-job-id", 1, 1, CMDL_OPTION_NEG_ERROR, true );
	}

	/**
	 * parse single option from command line options, this function is called for every delivered option
	 * @param ref			reference number of option registration, should be used for discriminate the options
	 * @param prio			priority value of option
	 * @param opt			option
	 * @return				true if option is used by the module
	 */
	bool TransferFromServer::parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt)
	{
		if (ModuleBase::parseOption(ref, prio, opt))
			return true;

		switch (ref) {
			case OPTION_SERVER_JOB_ID:
				_serverJobId = opt->argAsInt(0, _ctrl);
				return true;
		}

		return false;
	}

	/**
	 * writes usage info for the module to stream
	 * @param s				stream to write to
	 */
	void TransferFromServer::usage(ostream& s)
	{
		ModuleBase::usage(s);

		// (Wenn Identifikation des Serverergebnisses irgendwie anders passiert, dann entsprechend andere Instanzvariablen und Kommandozeilenoptionen,
		//  und entsprechend anderer Aufruf dieses Moduls aus Modul TransferToServer)
		s << "  -server-job-id <id>           identification number of the server job" << endl;
	}

	/**
	 * run module function
	 */
	void TransferFromServer::run()
	{
		_ctrl->errHandler().setExecStep("run");

		PROTO_OUTL("Start run module " << moduleName());

		if (_serverJobId == -1)
			_ctrl->errHandler().internalError("identification number of the server job missing");
		else
			executeTransfer();

		PROTO_OUTL("End run module " << moduleName());
	}

	/**
	 * execute the transfer
	 */
	void TransferFromServer::executeTransfer()
	{
		PROTO_OUTL("wait for the server result for server job id " << _serverJobId);
		waitForServerResult();

		PROTO_OUTL("read server result for server job id " << _serverJobId);
		// Eingabestream bereitstellen/öffnen, über den die Ergebnisdaten für _serverJobId zu lesen sind; dann anstelle des hier nur beispielhaft stehenden "cin" verwenden
		PositionInfo pos(POSITION_TYPE_STREAM, "stdin");
		readDataFromServerCmpl(cin, pos);
	}

	/**
	 * wait for the result from the server cmpl
	 */
	void TransferFromServer::waitForServerResult()
	{
		// Prüft auf den Ausführungsstatus des Servers, und wartet auf die Bereitstellung der Ergebnisdaten.
		// Bis dahin Entgegennahme von stdout/stderr des Servers, um sie hier auf dem Client auszugeben.
	}

	/**
	 * read data transfered from the server cmpl
	 * @param s				stream to read from
	 * @param pos			start position in input stream
	 */
	void TransferFromServer::readDataFromServerCmpl(istream &s, PositionInfo& pos)
	{
		MainData::SerializeInfo si(this, &s, &pos);
		si.setIoControl(true, true, true);
		si.setIoServer(-1, _ctrl->getOrderForRunningModule(this));

		_data->deserializeFrom(si);
	}
}

