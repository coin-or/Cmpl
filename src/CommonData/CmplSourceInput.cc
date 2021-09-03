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


#include "CmplSourceInput.hh"
#include "../Control/ModulesConf.hh"

using namespace std;


namespace cmpl
{
	// defines data class
	DATA_CLASS( CmplSourceInput );


	/**
	 * check if serialization to or from stream should be done for the data object
	 * @param si		serialization info
	 * @param data		main data object
	 * @return			true if serialization should be done
	 */
	bool CmplSourceInput::checkSerialize(MainData::SerializeInfo& si, const MainData *data)
	{
		return (!si.toFromServer || (si.toFromServer == 1 && (si.serverStep < MODULE_CONF_ORDER_CMPL_PARSE || !HAS_DATA(data, IntCode))));
	}

	/**
	 * write data object to output stream
	 * @param si		serialization info
	 * @param data		main data object
	 */
	void CmplSourceInput::serializeTo(MainData::SerializeInfo& si, const MainData *data)
	{
		(*si.ostr) << endl;

		if (!_contents.empty()) {
			if (si.lineNums)
				(*si.ostr) << "0#";

			(*si.ostr) << _contents.size() << ':' << _contents << endl;
		}
	}

	/**
	 * fill data object from input stream
	 * @param si		serialization info
	 * @param data		main data object
	 * @param pos		position of input string, returns iterated position
	 * @param subCnt	count of sub data objects
	 * @param lc		line count per element or 0 if elements are len coded
	 * @param rline		remaining section start line
	 */
	void CmplSourceInput::deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline)
	{
		si.pos->addLines();

		_contents.clear();

		if (subCnt == 1) {
			char *buf = NULL;
			size_t cap = 0;

			MainData::readLenCodedDataLine(si, &buf, cap);
			_contents = buf;

			if (buf)
				delete buf;
		}
	}
}

