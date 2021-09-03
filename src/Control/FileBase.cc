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


#include "FileBase.hh"
#include "MainData.hh"


#include <cstdlib>
#include <ctime>


namespace cmpl
{
	/***********************************************************************
	 * FileBase
	 **********************************************************************/

	/**
	 * Returns the file name, with replacement of "{}" by the base file name
	 * @param src	source file name
	 * @return  file name
	 */
	string FileBase::fileNameReplaced(string& src)
	{
		string ret = src;
		size_t p;
		int i = 0;

		// replace {}
		while ((p = ret.find("{}")) != string::npos && i++ < 100) {
			ret = ret.replace(p, 2, _data->cmplFileBase());
		}

		if (_aliases && !(_aliases->empty())) {
			// replace full file name
			map<string,string>::iterator it = _aliases->find(ret);
			if (it != _aliases->end()) {
				ret = it->second;
			}

			// replace file name without extension
			else {
				size_t s = ret.find_last_of('.');
				if (s != string::npos) {
					string f = ret.substr(0, s);
					string e = ret.substr(s);

					it = _aliases->find(f);
					if (it != _aliases->end()) {
						ret = it->second + e;
					}
				}
			}
		}

		return ret;
	}

	/**
	 * get whether the name has an absolute dir path
	 * @param ps			path separator char
	 * @return				true if absolute dir path
	 */
	bool FileBase::hasNameAbsPath(char ps)
	{
		// if name is empty or start with "{}" it is treated like a absolute dir path
		if (_fileName.empty() || (_fileName.size() >= 2 && _fileName[0] == '{' && _fileName[1] == '}'))
			return true;

		const string& fn = (_aliases && !(_aliases->empty()) ? fileNameReplaced() : _fileName);

		if (fn.empty() || fn[0] == ps)
			return true;

		// test windows drive letter
		if (ps == '\\' && fn.size() >= 2 && fn[1] == ':')
			return true;

		return false;
	}

	/**
	 * creates new position for this file
	 * @param name			file name
	 * @param loc			source location
	 * @return				new position object
	 */
	PositionInfo* FileBase::newPositionInfo(const char *name, LocationInfo *loc)
	{
		int t = (_mode==IO_MODE_STRING ? POSITION_TYPE_STRING : (_mode==IO_MODE_STD_IO || _mode==IO_MODE_STD_CERR ? POSITION_TYPE_STREAM : POSITION_TYPE_FILE));
		return new PositionInfo(loc, t, name);
	}

	/**
	 * creates new position for this file
	 * @param name			file name
	 * @param loc			source location
	 * @return				new position object
	 */
	PositionInfo* FileBase::newPositionInfo(string name, LocationInfo *loc)
	{
		if (!_data)
			throw FileException("internal error: _data object is not set", _fileName.c_str());

		return newPositionInfo(_data->globStrings()->store(name), loc);
	}

    /**
     * get path separator char
     * @param path          path
     * @param psDef         default path separator char
     * @return              path separator char or '\0' if no path
     */
    char FileBase::getDirSepChar(const char *path, const char psDef)
    {
        if (path && *path) {
            // path separator can be '/' or '\'
            for (int i = 0; path[i] != '\0'; i++) {
                if (path[i] == '/' || path[i] == '\\')
                    return path[i];
            }
        }

        return psDef;
    }

    /**
     * replace last component of path by another file name
     * @param path          path
     * @param file          other file name
     * @param ps            path separator char
     * @return              replaced file name
     */
    string FileBase::replFileInPath(string *path, string *file, char ps)
    {
        if (!path || path->empty()) {
            if (file)
                return string(*file);
            else
                return string();
        }

        size_t p = path->find_last_of(ps);
        if (p == string::npos) {
            string res(*path);
            res += ps;
            if (file)
                res += *file;
            return res;
        }
        else {
            string res(*path, 0, p+1);
            if (file)
                res += *file;
            return res;
        }
    }


    string FileBase::getTmpPath() {

        char *tmpP;
        string tmpPath;

        tmpP=getenv("TMPDIR");

        if (tmpP==NULL) {
            tmpP=getenv("TEMP");
            if (tmpP==NULL) {
                tmpP=getenv("TMP");
                if (tmpP==NULL) {
                    tmpP=getenv("TEMPDIR");
                    if (tmpP==NULL) {
                         tmpP=getenv("XDG_RUNTIME_DIR");
                    }
                }
            }
        }

        if (tmpP==NULL)
             tmpPath="";
        else {
            if (StringStore::contains(tmpP,"/") && !StringStore::stringEndsWith(tmpP,"/"))
                tmpPath=string(tmpP)+"/";
            else {
                if (StringStore::contains(tmpP,"\\") && !StringStore::stringEndsWith(tmpP,"\\"))
                    tmpPath=string(tmpP)+"\\";
                else
                    tmpPath=tmpP;
            }
        }

        return tmpPath;

    }

    string FileBase::getTmpFileName(const string& startStr, const int maxNr)  {
        string tmpStr=startStr;

        srand(time(0));
        int i=rand()%maxNr+1;
        tmpStr+=to_string(i);

        return tmpStr;
    }


    bool FileBase::exists(const string& fileName) {
        ifstream f(fileName.c_str());
        return f.good();
    }


	/***********************************************************************
	 * FileOutput
	 **********************************************************************/

	/**
	 * set file for output
	 * @param data			data object to derive filenames
	 * @param mode			mode
	 * @param fn			pointer to filename
	 * @param std			pointer to standard filename to use if fn is empty
	 * @param overwrite		if already set, then overwrite it
	 * @return				file was already set
	 */
	bool FileOutput::setFile(MainData *data, int mode, const string *fn, const char *std, bool overwrite)
	{
		bool ret = _enabled;

		if (!_enabled || overwrite) {
			_data = data;
			_mode = mode;
			_enabled = (mode != 0);

			if (fn)
				_fileName = *fn;
			else
				_fileName.clear();

			if ((fn && *fn == "1") || mode == IO_MODE_STD_IO) {
				_mode = IO_MODE_STD_IO;
				if (!fn || *fn == "1")
					_fileName = "(stdout)";
			}
			else if ((fn && *fn == "2") || mode == IO_MODE_STD_CERR) {
				_mode = IO_MODE_STD_CERR;
				if (!fn || *fn == "2")
					_fileName = "(stderr)";
			}
			else if (mode == IO_MODE_STRING) {
				if (!fn)
					_fileName = "(string)";
			}
			else if (mode != IO_MODE_FILE) {
				throw FileException("internal error: invalid file mode for output", _fileName.c_str());
			}
			else if (!fn || fn->empty() || *fn == "-" || *fn == "--")
			{
				_fileName = std;
			}
		}

		return ret;
	}


	/**
	 * opens the output file
	 * @return		stream for writing to the file
	 */
    ostream* FileOutput::open(ios_base::openmode mode )
	{
		// if already open, just return stream
		if (_oOut)
			return _oOut;

		if (_enabled) {
			string filename = fileNameReplaced();
			string orgExc;

			try {
				if (_mode == IO_MODE_STD_IO || _mode == IO_MODE_STD_CERR) {
					_oOut = (_mode == IO_MODE_STD_IO ? &cout : &cerr);
				}
				else if (_mode == IO_MODE_STRING) {
					_oOut = new ostringstream();
				}
				else {
                    _oOut = new ofstream(filename.c_str(), mode);
				}
			}
			catch (exception& e) {
				orgExc = e.what();

				if (_oOut) {
					try {
						delete _oOut;
					}
					catch (exception) { }
				}

				_oOut = NULL;
			}

			if (!_oOut || !(*_oOut))
				throw FileException("file error: cannot open file for output", filename.c_str(), orgExc.c_str());
		}

        return _oOut;
    }


	/**
	 * close the file
	 */
    void FileOutput::close()
	{
		if (_oOut) {
			if (_mode == IO_MODE_STD_IO || _mode == IO_MODE_STD_CERR) {
				// only flush, dont close
				_oOut->flush();
			}
			else if (_mode == IO_MODE_STRING) {
				_ioString = ((ostringstream *)_oOut)->str();
				delete _oOut;
			}
			else {
				((ofstream *)_oOut)->close();
				delete _oOut;
			}

			_oOut = NULL;
		}
	}



	/***********************************************************************
	 * FileInput
	 **********************************************************************/

	/**
	 * set file for input
	 * @param data			data object to derive filenames
	 * @param mode			mode
	 * @param fn			pointer to filename or NULL if output filename is to derive from input filename
	 * @param std			pointer to standard filename to use if fn is empty
	 * @param overwrite		if already set, then overwrite it
	 * @return				file was already set
	 */
	bool FileInput::setFile(MainData *data, int mode, const string *fn, const char *std, bool overwrite)
	{
		bool ret = _enabled;

		if (!_enabled || overwrite) {
			_data = data;
			_mode = mode;
			_enabled = (mode != 0);

			if (fn)
				_fileName = *fn;
			else
				_fileName.clear();

			if ((fn && *fn == "0") || mode == IO_MODE_STD_IO) {
				_mode = IO_MODE_STD_IO;
				if (!fn || *fn == "0")
					_fileName = "(stdin)";
			}
			else if (mode == IO_MODE_STRING) {
				if (!fn)
					_fileName = "(string)";
			}
			else if (mode != IO_MODE_FILE) {
				throw FileException("internal error: invalid file mode for input", _fileName.c_str());
			}
			else if (!fn || fn->empty() || *fn == "-" || *fn == "--")
			{
				_fileName = std;
			}
		}

		return ret;
	}


	/**
	 * set string for input
	 * @param data			data object to derive filenames
	 * @param fn			pseudo filename
	 * @param input			input data
	 * @param overwrite		if already set, then overwrite it
	 * @return				whether if error (filename is already set and not overwrite)
	 */
	bool FileInput::setInputString(MainData *data, const string *fn, const string *input, bool overwrite)
	{
		if (_enabled && !overwrite)
			return false;

		_data = data;
		_mode = IO_MODE_STRING;
		_enabled = true;

		if (fn)
			_fileName = *fn;
		else
			_fileName = "(string)";

		if (input)
			_ioString = *input;
		else
			_ioString.clear();

		return true;
	}


	/**
	 * opens the input file
	 * @return		stream for reading from the file
	 */
	istream* FileInput::open()
	{
		// must not already opened as file
		if (_iFp)
			throw FileException("internal error: file already opened as file", fileNameReplaced().c_str());

		// if already open, just return stream
		if (_iIn)
			return _iIn;

		if (_enabled) {
			string filename = fileNameReplaced();
			string orgExc;

			try {
				if (_mode == IO_MODE_STD_IO) {
					_iIn = &cin;
				}
				else if (_mode == IO_MODE_STRING) {
					_iIn = new istringstream(_ioString);
				}
				else {
					_iIn = new ifstream(filename.c_str());
				}
			}
			catch (exception& e) {
				orgExc = e.what();

				if (_iIn) {
					try {
						delete _iIn;
					}
					catch (exception) { }
				}

				_iIn = NULL;
			}

			if (!_iIn || !(*_iIn))
				throw FileException("file error: cannot open file for input", filename.c_str(), orgExc.c_str());
		}

        return _iIn;
	}


	/**
	 * open and get the output file or string
	 * @param input			return string with input data, or NULL if input file
	 * @return				file pointer to opened input file, or NULL if input string data			
	 */
	FILE* FileInput::openFile(string **input)
	{
		// must not already opened as stream
		if (_iIn)
			throw FileException("internal error: file already opened as stream", fileNameReplaced().c_str());

		// if already open, just return file pointer
		if (_iFp)
			return _iFp;

		*input = NULL;

		if (_enabled) {
			string filename = fileNameReplaced();

			if (_mode == IO_MODE_STRING) {
				*input = &_ioString;
				_iFp = NULL;
			}
			else if (_mode == IO_MODE_STD_IO) {
				_iFp = stdin;
			}
			else if (_mode == IO_MODE_FILE) {
				_iFp = fopen(filename.c_str(), "r");
				if (!_iFp)
					throw FileException("file error: cannot open file for input", filename.c_str());
			}
			else {
				throw FileException("internal error: invalid file mode for input", filename.c_str());
			}
		}

		return _iFp;
	}


	/**
	 * close the file
	 */
    void FileInput::close()
	{
		if (_iIn) {
			if (_mode == IO_MODE_STRING) {
				delete _iIn;
			}
			else if (_mode != IO_MODE_STD_IO) {
				((ifstream *)_iIn)->close();
				delete _iIn;
			}

			_iIn = NULL;
		}
		else if (_iFp) {
			if (_mode == IO_MODE_FILE)
				fclose(_iFp);

			_iFp = NULL;
		}
	}



	/***********************************************************************
	 * FileException
	 **********************************************************************/

	/**
	 * constructor
	 * @param msg				exception message
	 * @param filename			name of file
	 * @param orgExc            what() from original exception
	 */
	FileException::FileException(const char *msg, const char *filename, const char *orgExc)
	{
		_what = string(msg);

		if (orgExc && *orgExc)
			_what = _what + " (" + orgExc + ")";
	
		_filename = filename;
        _line = 0;
	}

}
