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
/// \date       2012/12/30
/// \version    1.0.1
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

#define UPPERCASE(s) std::transform(s.begin(),s.end(),s.begin(),::toupper)
#define LOWERCASE(s) std::transform(s.begin(),s.end(),s.begin(),::tolower)

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
	djitem(string key,int v)
	{
		char buf[16];
		djitem();
		sprintf(buf,"%d",v);
		type = JNUMBER;
		svalue = buf;
		skey = key;
	}
	djitem(string key,double v)
	{
		char buf[16];
		djitem();
		sprintf(buf,"%lf",v);
		type = JNUMBER;
		svalue = buf;
		skey = key;
	}
	djitem(string key,float v)
	{
		char buf[16];
		djitem();
		sprintf(buf,"%f",v);
		type = JNUMBER;
		svalue = buf;
		skey = key;
	}
	djitem(string key,char* s)
	{
		djitem();
		type = JSTRING;
		svalue = s;
		skey = key;
	}
	djitem(string key,string s)
	{
		djitem();
		type = JSTRING;
		svalue = s;
		skey = key;
	}
	djitem(string key,bool v)
	{
		djitem();
		if (v)
		{
			type = JTRUE;
			svalue = "true";
		}
		else
		{
			type = JFALSE;
			svalue = "false";
		}
		skey = key;
	}
	djitem()
	{
		type = JUNKNOW;
		parent = NULL;
		sibling = NULL;
		child = NULL;
		level = 0;
	}
	virtual ~djitem(void) 
	{

	}
	virtual void show()
	{
 		int i;
  		for (i=0;i<level;i++)
  			printf("\t");
  		printf("[%d - %s] %s:%s\n",
 			level,gettypename().c_str(),skey.c_str(),svalue.c_str());
 		//this,parent,sibling,child,level);
		if (child!=NULL)
			child->show();
 		if ( (type==JARRAY) || (type==JOBJECT) )
 		{
 			for (i=0;i<level;i++)
 				printf("\t");
 			printf("[/%s]\n",gettypename().c_str());
 		}

		if (sibling)
			sibling->show();
	}

	virtual void out(string &str)
	{
		if ( (type==JOBJECT) || (type==JARRAY) )
		{
			if (skey.length())
			{
				str += "\"" + skey + "\":";
			}
		}

		switch (type)
		{
		case JOBJECT:
			str += "{";
			break;
		case JARRAY:
			str += "[";
			break;
		case JNUMBER:
		case JTRUE:
		case JFALSE:
		case JNULL:
			if (skey.length()==0)
				str += svalue.c_str();
			else
				str += ("\"" + skey + "\":" + svalue);
			break;
		case JSTRING:
			if (skey.length()==0)
				str += ("\"" + svalue + "\"");
			else
				str += ("\"" + skey + "\":\"" + svalue + "\"");
			break;
		}

		if (child)
		{
			//recursive call for child
			child->out(str);
		}

		if (type==JOBJECT)
			str += "}";
		if (type==JARRAY)
			str += "]";

		if (sibling)
		{
			str += ",";
			//recursive call for sibling
			sibling->out(str);
		}
	}
	
	int as_int()
	{
		return atoi(svalue.c_str());
	}
	unsigned int as_uint()
	{
		return (unsigned int)as_int();
	}
	double as_double()
	{
		double f;
		if (1==sscanf(svalue.c_str(),"%lf",&f))
			return f;
		return 0.0;
	}
	float as_float()
	{
		float f;
		if (1==sscanf(svalue.c_str(),"%f",&f))
			return f;
		return 0.0f;
	}
	std::string as_string()
	{
		return svalue;
	}
	bool as_bool()
	{
		if (svalue=="true")
			return true;
		else
			return false;
	}
	std::string gettypename()
	{
		char *s=NULL;
		char buf[32];
		switch (type)
		{
		case JROOT:   s="Root";   break;
		case JUNKNOW: s="Unknow"; break;
		case JSTRING: s="String"; break;
		case JNUMBER: s="Number"; break;
		case JOBJECT: s="Object"; break;
		case JARRAY:  s="Array";  break;
		case JTRUE:   s="True";   break;
		case JFALSE:  s="False";  break;
		case JNULL:   s="Null";   break;
		}
		if (!s)
			sprintf(buf,"Name:%d",(int)type);
		else
			sprintf(buf,"%s",s);
		return std::string(buf);
	}
	djitem* getlast()
	{
		djitem* temp=this;
		while (temp->sibling!=NULL)
		{
			temp = temp->sibling;
		}
		return temp;
	}
	void append(djitem* sub)
	{
		if (child)
			child->getlast()->sibling = sub;
		else
		{
			if (getlast()==this) //is first
				child = sub;
			else
				getlast()->sibling = sub;
		}
	}
	
	int level;
	djson_type type;
	string svalue;
	string skey;

	djitem* sibling;
	djitem* child;
	djitem* parent;
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
	
    ~djson() 
	{
		freeitems();
	}

	std::string geterrorstring()
	{
		switch (error)
		{
		case JE_SUCCESS:
			return "sucess";
		case JE_PREFIXSYMBOL:
			return "bad prefix symbol";
		case JE_PAIR:
			return "{} or [] pair error";
		case JE_INVALIDCHARACTER:
			return "invalid character";
		case JE_STRING:
			return "bad string";
		case JE_NUMBER:
			return "bad number character";
		case JE_BOOLEAN:
			return "bad boolean character";
		case JE_NULL:
			return "bad null";
		case JE_UNFINISH:
			return "json unfinished";
		default:
			break;
		}
		return "unknow error";
	}

/// public data
public:
	djson_error    error;
	djitem       root;			//the decode or encode result here

	void settext(const char* str,int len);

	void show()
	{
		root.show();
	}

	void out(string &str)
	{
		root.out(str);
	}

	djitem* newitem(djitem* parent,djson_type type,string key,string value);
	djitem* finditem(djitem* parent,string keyname);
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
	vector<djitem*> itempool;
	const char* jsontext;        ///< store the json text pointer here
	int         jsontextlen;     ///< store the json text length

    int line;                   ///< The line number start=0
};


#endif // djson
