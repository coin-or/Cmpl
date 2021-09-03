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


#ifndef CMPLSOURCEINPUT_HH
#define CMPLSOURCEINPUT_HH

#include <string>
#include "../Control/MainData.hh"

using namespace std;


namespace cmpl
{
	/**
	 * <code>CmplSourceInput</code> contains the input for the compiler module, already processed by the precompiler module
	 */
	class CmplSourceInput : public MainData::DataBase
	{
	private:
		string _contents;			///< cmpl input for the compiler module


	public:
		/**
		 * get cmpl input for the compiler module
		 */
		string& contents()					{ return _contents; }

		/**
		 * set cmpl input for the compiler module
		 */
		void setContents(string& s)			{ _contents = s; }


		/************** implementation for MainData::DataBase methods **********/

	public:
		/**
		 * constructor
		 * @param m         module name
		 * @param data      main data object
		 */
		CmplSourceInput(const char *m, MainData *data): MainData::DataBase(m, data)				{ }

		/**
		 * delete all data
		 */
		virtual void cleanUp()																	{ _contents.clear(); }

		/**
		 * get count of sub data objects
		 */
		virtual int subCnt()																	{ return (_contents.empty() ? 0 : 1); }

		/**
		 * if sub data is splitted up to different parts for serialization then get count of these parts, otherwise 0
		 */
		virtual int subPartCount()																{ return 0; }

		/**
		 * get serialization lines per data element, or 0 if data elements can have multiline contents for which a len coded serialization is necessary
		 */
		virtual int linesPerData()																{ return 0; }

		/**
		 * check if serialization to or from stream should be done for the data object
		 * @param si		serialization info
		 * @param data		main data object
		 * @return			true if serialization should be done
		 */
		virtual bool checkSerialize(MainData::SerializeInfo& si, const MainData *data);

		/**
		 * write data object to output stream
		 * @param si		serialization info
		 * @param data		main data object
		 */
		virtual void serializeTo(MainData::SerializeInfo& si, const MainData *data);

		/**
		 * fill data object from input stream
		 * @param si		serialization info
		 * @param data		main data object
		 * @param pos		position of input string, returns iterated position
		 * @param subCnt	count of sub data objects
		 * @param lc		line count per element or 0 if elements are len coded
		 * @param rline		remaining section start line
		 */
		virtual void deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline);
	};
}

#endif // CMPLSOURCEINPUT_HH

