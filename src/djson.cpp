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

#include "djson.h"

static bool is_number(char c)
{
	char num[]={'0','1','2','3','4','5','6','7','8','9','.','+','-','e','E'};
	int i;
	for (i=0;i<sizeof(num)/sizeof(num[0]);i++)
	{
		if (c==num[i])
			return true;
	}
	return false;
}

void djson::freeitems()
{
	djitem *pitem;
	while (itempool.size())
	{
		pitem = itempool.back();
		delete (pitem);
		itempool.pop_back();
	}
}

void djson::init()
{
	freeitems();
	jsontext="";
	line=0;
	root.type = JROOT;
	root.parent = NULL;
	root.sibling = NULL;
	root.child = NULL;
	root.level=0;
	error=JE_SUCCESS;
}

djson::djson()
{
	init();
}

void djson::settext(const char* str,int len)
{
	init();
	jsontext = str;
	jsontextlen = len;
	root.type = JROOT;
	parse();
}

int djson::skip(int startpos)
{
	char c;
	int pos=startpos;
	while (pos<jsontextlen)
	{
		c = jsontext[pos];
		if ( (c<=32) || (c=='\\') )
			pos++;
		else
			break;
	}
	return pos-startpos;
}

djitem* djson::newitem(djitem* parent,djson_type type,string key,string value)
{
	djitem* pnew = new djitem;
	if (pnew==NULL)
		return NULL;
	pnew->type = type;
	pnew->parent = parent;
	pnew->skey = key;
	pnew->svalue = value;
	itempool.push_back(pnew);
	while (parent)
	{
		pnew->level++;
		parent = parent->parent;
	}
	return pnew;
}

void djson::parse()
{
	djitem *jo;
	djitem *sub;
	std::string s,key;
	char c;
	int pos=0,ret;
	stack<djitem*> stk;

	stk.push(&root);
	pos += skip(pos);
	c = jsontext[pos];
	if ( (c!='{') && (c!='[') )
		error = JE_PREFIXSYMBOL;
	while ( true )
	{
		if (error!=JE_SUCCESS)
			break;

		if (stk.empty())
		{
			//error happened. maybe {[]} is not as pairs
			error = JE_PAIR;
			break;
		}

		jo = stk.top();	
		pos += skip(pos);
		if (pos>=jsontextlen)
			break;
		c = jsontext[pos];

		switch (c)
		{
		case ':':
		case ',':
			break;
		case '[':
		case '{':
			sub = newitem(jo,(c=='[')?JARRAY:JOBJECT,key,"");
			//sub->level = stk.size()-1;			
			key="";
			jo->append(sub);
			stk.push(sub);
			break;
		case '}':
		case ']':
			if (!stk.empty())
				stk.pop();
			break;
		case '\"':
			pos++; //skip the \"
			ret = parse_string(pos,s);
			if (ret==0)
			{
				error = JE_STRING;
				break;
			}
			pos += ret;
			//check the next useable char 
			pos+=skip(pos);
			if (pos>=jsontextlen)
				continue;
			else if (jsontext[pos+1]==':')
			{
				key = s;
				pos++;
				continue;
				break;
			}
			sub = newitem(jo,JSTRING,key,s);
			//sub->level = stk.size()-1;
			key = "";
			jo->append(sub);
			break;
		//number,true,false,null: need go back 1 byte
		case '+': case '-': case '.':
		case '0': case '1':	case '2': 
		case '3': case '4': case '5':
		case '6': case '7': case '8': 
		case '9':
			ret = parse_number(pos,s);
			if (ret==0)
			{
				error = JE_NUMBER;
				break;
			}
			pos += ret;
			sub = newitem(jo,JNUMBER,key,s);
			//sub->level = stk.size()-1;
			key = "";
			jo->append(sub);
			pos--;
			break;
		case 't':
			ret = parse_string_want(pos,"true",s);
			if (ret!=4)
			{
				error = JE_BOOLEAN;
				break;
			}
			pos += ret;
			sub = newitem(jo,JTRUE,key,s);
			//sub->level = stk.size()-1;
			key = "";
			jo->append(sub);
			pos--;
			break;
		case 'f':
			ret = parse_string_want(pos,"false",s);
			if (ret!=5)
			{
				error = JE_BOOLEAN;
				break;
			}
			pos+=ret;
			sub = newitem(jo,JFALSE,key,s);
			//sub->level = stk.size()-1;
			key = "";
			jo->append(sub);
			pos--;
			break;
		case 'n':
			ret = parse_string_want(pos,"null",s);
			if (ret!=4)
			{
				error = JE_NULL;
			}
			pos += ret;
			sub = newitem(jo,JNULL,key,s);
			//sub->level = stk.size()-1;
			key = "";
			jo->append(sub);
			pos--;
			break;
		default:
			error = JE_INVALIDCHARACTER;
			continue;
			break;
		}
		pos++;
	}
	
	if (pos<jsontextlen && error==JE_SUCCESS)
		error = JE_UNFINISH;

	if (error!=JE_SUCCESS)
		printf("eror:%d %s, pos:%d\n",error,geterrorstring().c_str(),pos);
}

int djson::parse_string(int startpos,string &str)
{
	char c;
	int pos=startpos;
	str = "";
	while (pos<jsontextlen)
	{
		c = jsontext[pos];

		if (c == '\"')
		{
			break;
		}
		
		//The escape characters
		if (c == '\\')
		{
			pos++;
			continue;
		}

		str += (char)c;
		pos++;
	}
	
	return pos-startpos;
}

int djson::parse_number(int startpos,string &str)
{
	char c;
	int pos=startpos;
	str = "";
	while (pos<jsontextlen)
	{	
		c = jsontext[pos];

		//The escape characters
		if (c == '\\')
		{
			pos++;
			continue;
		}

		if (!is_number(c))
			break;

		str += c;		
		pos++;
	}
	return pos-startpos;
}
	
int djson::parse_string_want(int startpos,string swant,string &str)
{
	char c,w,t;
	int i,j,pos=startpos;
	str = "";
	for (i=0,j=0; (i<swant.length()) && (pos<jsontextlen); i++)
	{
		c = jsontext[pos];

		//The escape characters
		if (c == '\\')
		{
			pos++;
			continue;
		}
		w = swant.at(j++);
		t = w;
		c = tolower(c);
		w = tolower(w);
		if (c!=w)
			break;
		pos++;
		str += t;
	}	
    return pos-startpos;
}

