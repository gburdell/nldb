//The MIT License
//
//xyzzy - c++ library
//Copyright (c) 2007-2010  Karl W. Pfalzer
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.

#include <fstream>
#include "xyzzy/assert.hxx"
#include "xyzzy/stringtmpl.hxx"

namespace xyzzy
{
	using namespace std;

	static const string cEoln = "\n";

	TStringTmpl::TStringTmpl()
	{}

	TStringTmpl::TStringTmpl(string fname)
	{
		init(fname);
	}

	void
	TStringTmpl::init(string fname)
	{
		typedef string::size_type size_t;
		static const unsigned cMax = 1024;
		static const string cMrk = "!!";
		char *const buf = new char[cMax];
		ifstream ifs(fname.c_str());
		bool waitForBegin = true;
		string building = "";
		string tmplName = "";
		while (ifs)
		{
			ifs.getline(buf, cMax);
			if (!ifs)
				break;
			string s(buf);
			size_t ix[2] = {s.find(cMrk), 0};
			if (0 == ix[0])
			{
				if (waitForBegin)
				{
					ix[1] = s.find(cMrk, ix[0]+1);
					ASSERT_TRUE(string::npos != ix[1]);
					ix[0] += cMrk.size();
					ix[1] -= ix[0];
					tmplName = s.substr(ix[0], ix[1]);
					waitForBegin = false;
				}
				else
				{
					m_tmpls[tmplName] = building;
					tmplName = "";
					building = "";
					waitForBegin = true;
				}
			}
			else if (false == waitForBegin)
			{
				building += s;
				building += cEoln;
			}
		}
		ifs.close();
		delete buf;
	}

	void 
	TStringTmpl::addTmpl(string ix, string tmpl)
	{
		m_tmpls[ix] = tmpl;
	}

	void
	TStringTmpl::setParm(string ix, string val)
	{
		m_parmVals[ix] = val;
	}

	string 
	TStringTmpl::emit(string ix, int indentBy)
	{
		typedef string::size_type size_t;
		static const string cParmMrk = ",,";
		string rval = "";
		const string curr = m_tmpls[ix];
		size_t currIx = 0;
		while (currIx < curr.size())
		{
			// Extract next line
			string line;
			{
				size_t eolnIx = curr.find(cEoln, currIx);
				size_t cnt;
				if (string::npos == eolnIx)
				{
					cnt = curr.size() - currIx;
				}
				else
				{
					cnt = cEoln.size() + eolnIx - currIx;
				}
				line = curr.substr(currIx, cnt);
				currIx += cnt + 1;
			}

			// Substitute parms
			size_t mrks[2];	//0=start, 1=end
			size_t lineIx = 0;
			while (lineIx < line.size())
			{
				mrks[0] = line.find(cParmMrk, lineIx);
				if (string::npos == mrks[0])
					break;	//while
				mrks[1] = line.find(cParmMrk, mrks[0] + cParmMrk.size());
				ASSERT_TRUE(string::npos != mrks[1]);
				mrks[0] += cParmMrk.size();	//start of nm
				mrks[1] -= mrks[0];	//cnt
				string pnm = line.substr(mrks[0], mrks[1]);
				string pval = m_parmVals[pnm];
				mrks[0] -= cParmMrk.size();	//start of parm
				mrks[1] += (2 * cParmMrk.size());
				line.replace(mrks[0], mrks[1], pval);
				lineIx = mrks[0];
			}
			for (int i = 0; i < indentBy; i++)
				rval += " ";
			rval += line;
		}
		return rval;
	}

	ostream& 
	TStringTmpl::iterateOver(ostream &os, string ix, string parm, 
							 PTRcArray<string> vals)
	{
		for (int i = 0; i < vals.length(); i++)
		{
			setParm(parm, vals[i]);
			os << emit(ix);
		}
		return os;
	}

	void 
	TStringTmpl::resetParms(PTRcArray<string> parms)
	{
		static const string cNull;
		for (int i = 0; i < parms.length(); i++)
		{
			setParm(parms[i], cNull);
		}
	}
};
