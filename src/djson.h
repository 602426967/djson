///////////////////////////////////////////////////////////////////////////////
///  Copyright 2012 Garnel
///
///  Licensed under the Apache License, Version 2.0 (the "License");
///  you may not use this file except in compliance with the License.
///  You may obtain a copy of the License at
///
///    http://www.apache.org/licenses/LICENSE-2.0
///
///  Unless required by applicable law or agreed to in writing, software
///  distributed under the License is distributed on an "AS IS" BASIS,
///  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///////////////////////////////////////////////////////////////////////////////

///
/// \file       djson.h
/// \date       2013/01/18
/// \version    1.0.2
/// \author     dege
/// \mail       602426967@qq.com
/// \copyright  Apache License, Version 2.0
///

#ifndef DJSON
#define DJSON

//disable warning 4786 in msvc++
#pragma warning(disable:4786)

#include <iostream>
#include <string>
#include <cassert>
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <cmath>
#include <vector>
#include <map>
#include <stack>

class djitem;
class djson;

using namespace std;

///
/// \enum   json_type
/// \brief  The types of json element
///
enum djson_type
{
	JUNKNOW,
	JROOT,	
	JSTRING,
	JNUMBER,
	JOBJECT,
	JARRAY,
	JTRUE,
	JFALSE,
	JNULL
};

///
/// \enum   djson_error 
/// \brief  The errors when parse the string
///
enum djson_error
{
	JE_SUCCESS,			//this means json decode success, others are failure.
	JE_PREFIXSYMBOL,	//first symbol is not {[
	JE_PAIR,            // {} or [] is not pairs

	JE_INVALIDCHARACTER, //invalid char when parse

	JE_STRING,           //parse string fail
	JE_NUMBER,           //parse number fail
	JE_BOOLEAN,          //
	JE_NULL,

	JE_UNFINISH          //have characters left
};

///
/// \class   djitem 
/// \brief  The djitem class is include in djson
///
class djitem
{
public:

	djitem();
	virtual ~djitem(void);

	virtual void show();
	virtual void out(string &str);

	int              as_int();
	unsigned int     as_uint();
	double           as_double();
	float            as_float();
	string           as_string();
	bool             as_bool();

	string gettypename();
	djitem* getlast();
	void append(djitem* sub);

	int level;
	djson_type type;
	string svalue;
	string skey;

	djitem* parent;
	djitem* sibling;
	djitem* child;

};


///////////////////////////////////////////////////////////////////////////
///json parser
///////////////////////////////////////////////////////////////////////////

///
/// \class  djson
/// \brief  The json parser 
///
class djson
{
/// public functions
public:

	djson();	
    ~djson();

	std::string geterrorstring();

/// public data
public:
	djson_error    error;
	djitem         root;			//the root of all items

	void settext(const char* str,int len);
	void show();
	void out(string &str);

	djitem* trueitem(djitem* parent);
	djitem* newitem(djitem* parent,djson_type type,string key="",string value="");
	djitem* finditem(djitem* parent,string keyname);
	djitem* finditembyindex(djitem* parent,int index); //start at 0
	djitem* finditembytype(djitem* parent,int index,djson_type type);
	djitem* insertitem(djitem* psrc,djitem* pdst);  //insert after dst

	//TODO: delete
	bool deleteitem(djitem* pitem);

/// private functions
private:
	void init();
	void parse();
	void freeitems();
	
	int skip(int startpos);
	int parse_string(int startpos,string &str);
	int parse_number(int startpos,string &str);
	int parse_string_want(int startpos,std::string swant, string &str);

/// private data
private:
	vector<djitem*> itempool;    /// memory poll of all items allocated
	const char* jsontext;        ///< store the json text pointer here
	int         jsontextlen;     ///< store the json text length

    int line;                   ///< The line number start=0
};


#endif // djson
