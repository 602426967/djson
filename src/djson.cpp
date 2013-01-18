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

#include "djson.h"

//------------------------- static functions -------------------------------------

// replace a sub string in a string
static string& replace_all(string& str,const string&old_value,const string& new_value)    
{    
	string::size_type pos;
	if (str.length()<1)
		return str;
    for(pos=0; pos!=string::npos; pos+=new_value.length()) 
	{    
        if(   (pos=str.find(old_value,pos))!=string::npos   )   
		{
            str.replace(pos,old_value.length(),new_value);    
		}
        else   
			break;    
    }    
    return str;    
} 

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

//--------------------------- djitem member functions -----------------------------
djitem::djitem()
{
	type = JUNKNOW;
	parent = NULL;
	sibling = NULL;
	child = NULL;
	level = 0;
}

djitem::~djitem(void) 
{

}

void djitem::show()
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

void djitem::out(string &str)
{
	string k,v;
	k = skey;
	v = svalue;

	//replace the escape char
	replace_all(k,"\\","\\\\");
	replace_all(k,"\"","\\\"");
	if (type==JSTRING)
	{
		replace_all(v,"\\","\\\\");
		replace_all(v,"\"","\\\"");
	}

	if ( (type==JOBJECT) || (type==JARRAY) )
	{
		if (k.length())
		{
			str += "\"" + k + "\":";
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
		if (k.length()==0)
			str += v;
		else
			str += ("\"" + k + "\":" + v);
		break;
	case JSTRING:
		if (skey.length()==0)
			str += ("\"" + v + "\"");
		else
			str += ("\"" + k + "\":\"" + v + "\"");
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

int djitem::as_int()
{
	return atoi(svalue.c_str());
}

unsigned int djitem::as_uint()
{
	return (unsigned int)as_int();
}

double djitem::as_double()
{
	double f;
	if (1==sscanf(svalue.c_str(),"%lf",&f))
		return f;
	return 0.0;
}

float djitem::as_float()
{
	float f;
	if (1==sscanf(svalue.c_str(),"%f",&f))
		return f;
	return 0.0f;
}

std::string djitem::as_string()
{
	return svalue;
}

bool djitem::as_bool()
{
	if (svalue=="true")
		return true;
	else
		return false;
}

std::string djitem::gettypename()
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

djitem* djitem::getlast()
{
	djitem* temp=this;
	while (temp->sibling!=NULL)
	{
		temp = temp->sibling;
	}
	return temp;
}

void djitem::append(djitem* sub)
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



//------------------------------- djson member functions ---------------------------

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

djson::~djson()
{
	init();
}

void djson::show()
{
	root.show();
}

void djson::out(string &str)
{
	root.out(str);
}

string djson::geterrorstring()
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
	switch (type)
	{
	case JTRUE:
		pnew->svalue = "true"; 
		break;
	case JFALSE:
		pnew->svalue = "false"; 
		break;
	case JNULL:
		pnew->svalue = "null"; 
		break;
	default:
		pnew->svalue = value;
		break;
	}
	pnew->skey = key;
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
			key = "";
			jo->append(sub);
			pos--;
			break;
		case 'n':
			ret = parse_string_want(pos,"null",s);
			if (ret!=4)
			{
				error = JE_NULL;
				break;
			}
			pos += ret;
			sub = newitem(jo,JNULL,key,s);
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
		if ( (c == '\\') && (pos+1<jsontextlen) )
		{
			if (jsontext[pos+1]=='\\')
			{
				str += '\\';
				pos+=2;
				continue;
			}
			
			if (jsontext[pos+1]=='\"')
			{
				str += '\"';
				pos+=2;
				continue;
			}
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

djitem* djson::trueitem(djitem* parent)
{
	if (parent->type==JROOT)
		parent = parent->child;
	if (!parent)
		return NULL;
	if ( (parent->type==JARRAY) ||
		(parent->type==JOBJECT) )
		return parent->child;
	return parent; //else NULL?
}

djitem* djson::finditem(djitem* parent,string keyname)
{
	djitem* it;
	it = trueitem(parent);
	while (it)
	{
		if (it->skey==keyname)
			return it;
		it = it->sibling;
	}
	return NULL;
}

djitem* djson::finditembyindex(djitem* parent,int index)
{
	djitem* it = trueitem(parent);
	int i=0;
	while (it)
	{
		if (i==index)
			return it;
		it = it->sibling;
		i++;
	}
	return NULL;
}

djitem* djson::finditembytype(djitem* parent,int index,djson_type type)
{
	djitem* it = trueitem(parent);
	int i=0;
	while (it)
	{
		if (it->type==type)
		{
			if (i==index)
				return it;
			i++;
		}
		it = it->sibling;
	}
	return NULL;
}

bool djson::deleteitem(djitem* pitem)
{
	printf("delete is unfinish.\n");
	return false;
}

//insert after dst
djitem* djson::insertitem(djitem* psrc,djitem* pdst)
{
	printf("insert is unfinish.\n");
	return NULL;
}