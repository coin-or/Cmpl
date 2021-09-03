
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


#include "TransferToServer.hh"
#include "../../Control/MainControl.hh"
#include "../../Control/ModulesConf.hh"
#include "../../CommonData/ExternDataInfo.hh"


namespace cmpl
{
	/*********** module definition **********/

	// defines the module "transferToServer" that is implemented in class "TransferToServer".
	// to register this module you have to insert "MODULE_CLASS_REG(<execution step>, transferToServer, <description>)" to file "modules.reg"
	MODULE_CLASS( transferToServer, TransferToServer )



	/*********** initialization **********/

	/**
	 * initialize modul, to call immediately after the constructor.
	 * @param ctrl			main object
	 * @param data			data object
	 * @param name			module name (or alias)
	 */
	void TransferToServer::init(MainControl *ctrl, MainData *data, const char *name)
	{
		ModuleBase::init(ctrl, data, name);

		_clientOnly = false;
	}


	/*********** handling of command line options **********/

#define OPTION_SERVER_URL		10
#define OPTION_CLIENT_ONLY		11

// beispielhafte Kommandozeilenoptionen, die für die Server-Steuerung bestimmt sind
#define OPTION_SERVER_EXAMPLE_BOOL		101
#define OPTION_SERVER_EXAMPLE_INT		102

	/**
	 * register command line options options for delivery to this module
	 * @param modOptReg		vector to register in
	 */
	void TransferToServer::regModOptions(vector<CmdLineOptList::RegOption> &modOptReg)
	{
		ModuleBase::regModOptions(modOptReg);

		REG_CMDL_OPTION( OPTION_SERVER_URL, "url", 1, 1, CMDL_OPTION_NEG_NO_ARG, true );	// Falls es mehrere URLs zur alternativ Verwendung durch den Client geben kann, ist hier entsprechend die Argumentmaximalanzahl statt mit 1 als beliebig anzugeben. (In parseOption() sind dann alle Argumente durchzugehen und die URLs z.B. in einem vector zu speichern.)
		REG_CMDL_OPTION( OPTION_CLIENT_ONLY, "client-only", 0, 0, CMDL_OPTION_NEG_DELIV, true );

		REG_CMDL_OPTION( OPTION_SERVER_EXAMPLE_BOOL, "example-bool-for-server", 0, 0, CMDL_OPTION_NEG_DELIV, true );	// nur beispielhaft für eine boolesche Kommandozeilenoption, die für die Server-Steuerung bestimmt ist
		REG_CMDL_OPTION( OPTION_SERVER_EXAMPLE_INT, "example-int-for-server", 1, 1, CMDL_OPTION_NEG_NO_ARG, true );		// nur beispielhaft für eine Kommandozeilenoption mit einem int-Argument, die für die Server-Steuerung bestimmt ist
	}

	/**
	 * parse single option from command line options, this function is called for every delivered option
	 * @param ref			reference number of option registration, should be used for discriminate the options
	 * @param prio			priority value of option
	 * @param opt			option
	 * @return				true if option is used by the module
	 */
	bool TransferToServer::parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt)
	{
		if (ModuleBase::parseOption(ref, prio, opt))
			return true;

		switch (ref) {
			case OPTION_SERVER_URL:
				if (opt->neg())
					_url.clear();		// dadurch kann mit "-no-url" als Kommandozeilenoption (bzw "%arg no-url" in cmpl-Header) die lokale Ausführung erzwungen werden, auch wenn in der Konfigurationsdatei ein Server vorgesehen ist 
				else
					_url = (*opt)[0];	// Für die Kommandozeilenoption "url" ist hier genau ein Argument vorgesehen (siehe REG_CMDL_OPTION dafür), dies wird hier mit Index 0 angesprochen
				_urlLoc = opt->loc(true);
				return true;

			case OPTION_CLIENT_ONLY:
				_clientOnly = !opt->neg();		// Kommandozeilenoption "client-only" hat kein Argument (siehe REG_CMDL_OPTION dafür), es wird nur unterschieden, ob sie negiert angegeben ist
				_clientOnlyLoc = opt->loc(true);
				return true;

			// nur beispielhaft für eine boolesche Kommandozeilenoption, die für die Server-Steuerung bestimmt ist
			case OPTION_SERVER_EXAMPLE_BOOL:
				_exampleForServerBool = !opt->neg();
				return true;

			// nur beispielhaft für eine Kommandozeilenoption mit einem int-Argument, die für die Server-Steuerung bestimmt ist
			case OPTION_SERVER_EXAMPLE_INT:
				_exampleForServerInt = (opt->neg() ? -1 : opt->argAsInt(0, _ctrl));
				return true;
		}

		return false;
	}

	/**
	 * writes usage info for the module to stream
	 * @param s				stream to write to
	 */
	void TransferToServer::usage(ostream& s)
	{
		ModuleBase::usage(s);

		s << "  -url <server-url>             request transfer of execution to the given cmpl server" << endl;
		s << "  -client-only                  mark this cmpl as client only, that means it cannot run without a cmpl server" << endl;

		s << "  -example-bool-for-server      example bool option for server" << endl;		// nur beispielhaft für eine boolesche Kommandozeilenoption, die für die Server-Steuerung bestimmt ist
		s << "  -example-int-for-server <i>   example int option for server" << endl;		// nur beispielhaft für eine Kommandozeilenoption mit einem int-Argument, die für die Server-Steuerung bestimmt ist
	}

	/**
	 * run module function
	 */
	void TransferToServer::run()
	{
		_ctrl->errHandler().setExecStep("run");

		PROTO_OUTL("Start run module " << moduleName());

		// check if transfer to server is requested
		if (!_url.empty()) {
			executeTransfer();
		}
		else if (_clientOnly) {
			_ctrl->errHandler().error(ERROR_LVL_NORMAL, "server url missing for a client only cmpl", _clientOnlyLoc);
		}

		PROTO_OUTL("End run module " << moduleName());
	}

	/**
	 * execute the transfer
	 */
	void TransferToServer::executeTransfer()
	{
		//--------------------------------------------------------------------------
		// XML-Datei für die Anforderung an den Server schreiben.
		// Diese Datei ist für die Serversteuerung gedacht und muss auf Seite des Servers von der Serversteuerung
		// eingelesen und verarbeitet werden.
		// (Vielleicht muss es auf der Client-Seite gar nicht erst als Datei geschrieben werden, sondern kann
		// direkt an den Server gehen, dann entsprechend anzupassen (das Schreiben erfolgt sowieso in einen Stream).
		//--------------------------------------------------------------------------
		string filenameServerRequest = "TODO-ServerRequest.xml";	// falls in eine Datei geschrieben wird, muss hier ein eindeutiger Name dafür bereitgestellt werden
        FileOutput fileServerRequest;
        fileServerRequest.setFile(_data, IO_MODE_FILE, &filenameServerRequest, NULL);

		ostream *strServerRequest;
		try {
			strServerRequest = fileServerRequest.open();
		}
		catch (exception& ex) {
			_ctrl->errHandler().error(ERROR_LVL_FATAL, "write file error", _urlLoc, &ex);	// Als fehlerverursachende Location wird hier die der URL-Angabe genommen, weil die URL-Angabe letztlich dafür sorgt, dass hier die Datei zu schreiben ist
		}

		try {
			PROTO_OUTL("write xml file '" << filenameServerRequest << "' with server request");
			writeServerRequest(*strServerRequest);
		}
		catch (exception& ex) {
			_ctrl->errHandler().error(ERROR_LVL_FATAL, "write server request error", _urlLoc, &ex);
		}

		fileServerRequest.close();


		//--------------------------------------------------------------------------
		// Datei mit den Daten für das Server-Cmpl schreiben.
		// Hier wird beispielhaft in eine Datei geschrieben, aber da das eigentliche Schreiben in einen Stream erfolgt,
		// könnte es zum Beispiel auch als Teil der XML-Datei mit der Anforderung an den Server geschrieben werden,
		// dann sinnvollerweise als CDATA-Block.
		// (Von der Performance her scheint es mir aber besser, es gleich von der XML-Datei zu trennen, denn ansonsten muss
		// die Serversteuerung es doch nur aus der XML-Datei holen und auf dem Server als separate Datei speichern.)
		//
		// Die Daten hier für das Server-Cmpl sind selbst kein XML.
		// Denn sowohl Sender (dieses Modul) als auch Empfänger (Modul TransferFromClient) sind Module in Cmpl, so dass kein Bedarf
		// für ein allgemein lesbares Format besteht. (Die Serversteuerung braucht und soll jedenfalls nichts aus diesen Daten lesen.)
		// Deshalb hier ein einfaches Textformat, das ich in Hinblick auf möglichst kompakte Darstellung und einfache Einlesbarkeit gewählt habe.
		//--------------------------------------------------------------------------
		string filenameMainData = "TODO-Data";	// falls in eine Datei geschrieben wird, muss hier ein eindeutiger Name dafür bereitgestellt werden
        FileOutput fileMainData;
        fileMainData.setFile(_data, IO_MODE_FILE, &filenameMainData, NULL);

		ostream *strMainData;
		try {
			strMainData = fileMainData.open();
		}
		catch (exception& ex) {
			_ctrl->errHandler().error(ERROR_LVL_FATAL, "write file error", _urlLoc, &ex);	// Als fehlerverursachende Location wird hier die der URL-Angabe genommen, weil die URL-Angabe letztlich dafür sorgt, dass hier die Datei zu schreiben ist
		}

		try {
			PROTO_OUTL("write data file '" << filenameServerRequest << "' to deliver to server cmpl");
			writeDataForServerCmpl(*strMainData);
		}
		catch (exception& ex) {
			_ctrl->errHandler().error(ERROR_LVL_FATAL, "write data for cmpl server error", _urlLoc, &ex);
		}

		fileMainData.close();


		//--------------------------------------------------------------------------
		// Eigentliche Serveransteuerung, meinem Verständnis nach:
		// - Übertragung der Datei zur Server-Anforderung an Server bzw. Scheduler
		// - Wenn Scheduler, dann nach der Antwort Übertragung der Datei zur Server-Anforderung an den zugeteilten Server
		// - Übertragung der Datei für das Server-Cmpl an den Server, ebenfalls aller benötigten cdat-Dateien
		// - Hier erzeugte Übertragungsdateien Client-seitig wieder löschen
		// - (Serversteuerung sorgt auf dem Server für den Start des Server-Cmpl)
		// 
		// (Sollte kein Server zur Ausführung verfügbar sein, könnte hier dieses Modul immer noch abgebrochen werden,
		// und ersatzweise mit der lokalen Ausführung fortgefahren werden.)
		//--------------------------------------------------------------------------
		int serverJobId = 0; //TODO: Ich nehme an, es gibt eine Art Identifikation für die Ausführung auf dem Server, mit der dann das zurückkommende Ergebnis zugeordnet werden kann


		// de-activate all modules here, which are now executed in the server cmpl
		int delMod = _ctrl->delModules(_ctrl->getOrderForRunningModule(this), MODULE_CONF_ORDER_FROM_SERVER);
		PROTO_OUTL("de-activate " << delMod << " modules on client side");

		// execute the module for return data from server to client as next module; with serverJobId as command line option
		// (Ich weiß nicht, wie genau das Ergebnis zurückkommt und vom Client erkannt wird. Hier wird einfach angenommen, dass das über eine serverJobId geht)
		PROTO_OUTL("execute 'transferFromServer' for server job " << serverJobId);
		CmdLineOptList clOpts;
		PositionInfo pos(POSITION_TYPE_DESCR, "(internal)");
		string opts = _ctrl->printBuffer("-server-job-id %d", serverJobId);

		clOpts.addOption(opts, pos);
		_ctrl->addModule("transferFromServer", MODULE_CONF_ORDER_FROM_SERVER, &clOpts);
	}

	/**
	 * write XML for server request
	 * @param s				stream to write to
	 */
	void TransferToServer::writeServerRequest(ostream &s)
	{
		// Schreibt die XML-Datei zur Anforderung der Ausführung auf dem Server.
		// Prinzipiell wie die bisherige Datei CmplInstance, enthält nun aber nicht mehr:
		// 	- Kommandozeilenoptionen für cmpl (sondern nur noch die speziell für die Serversteuerung gedachten Kommandozeilenoptionen)
		// 	- cmpl-Eingabedatei

		// Hier nur ausschnittsweise das Schreiben einiger Daten:
		// (die Benennung und Struktur der XML-Tags nur beispielhaft)
		
		// Schreiben Kommandozeilenoptionen für die Serversteuerung
		// (hier nur ganz beispielhaft)
		s << "<options>" << endl;
		s << "  <opt name='example-bool-for-server'>" << (_exampleForServerBool ? 1 : 0) << "</opt>" << endl;
		s << "  <opt name='example-int-for-server'>" << _exampleForServerInt << "</opt>" << endl;
		s << "</options>" << endl;

		// Schreiben Namen der cdat-Dateien
		// (Die Serversteuerung muss diese Dateien auf den Server kopieren, so dass sie dort für das Server-Cmpl zugreifbar sind.
		// Soweit die Dateien auf dem Server andere Namen erhalten (bzw. in anderen Verzeichnissen liegen), muss die Serversteuerung
		// dies dem Server-Cmpl beim Start per Kommandozeilenoption "-filealias <from1>=<to1> ..." mitteilen.)
		GET_DATA(ExternDataInfo, ed);
		if (ed) {
			set<const char *> filenames;
            ed->getDataFileNames(_data, filenames);

			if (!filenames.empty()) {
				s << "<datafiles>" << endl;
				for (set<const char *>::iterator it = filenames.begin(); it != filenames.end(); it++)
					s << "  <file>" << *it << "</file>" << endl;
				s << "</datafiles>" << endl;
			}
		}
	}

	/**
	 * write data for transfer to the server cmpl
	 * @param s				stream to write to
	 */
	void TransferToServer::writeDataForServerCmpl(ostream &s)
	{
		MainData::SerializeInfo si(this, &s, _data->version(), _data->globStrings()->storeInd(_ctrl->instanceName()), false);
		si.setIoControl(true, true, true);
		si.setIoServer(1, _ctrl->getOrderForRunningModule(this));

		_data->serializeTo(si);
	}
}

