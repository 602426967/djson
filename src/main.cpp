#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>

#include "djson.h"

void test_read()
{
	djson dj;
	string s;
	ifstream ifs;
	stringstream ss;

	ifs.open("test.txt");
	if (!ifs.good())
	{
		cout << "test.txt not find." << endl;
		return;
	}
	
	ss << ifs.rdbuf();
	s = ss.str();
		
	dj.settext(s.c_str(),s.length());

	cout << "---------- show the tree -------" << endl;
	dj.show();

	cout << "------- out to string -----------" << endl;
	s = "";
	dj.out(s);
	
	cout << s << endl;
}

void test_write(djson &dj)
{
	string s;
	djitem *it,*sub,*sub2;
	it = dj.newitem(&dj.root,JARRAY,"","");
	dj.root.child = it;
	it->append(dj.newitem(it,JSTRING,"key1","111"));
	it->append(dj.newitem(it,JNUMBER,"key2","222"));
	it->append(dj.newitem(it,JFALSE,"key3","false"));
	it->append(dj.newitem(it,JNULL,"key4","null"));

	//if you insert a sub item
	//the sub must be JOBJECT or JARRAY
	sub = dj.newitem(it,JOBJECT,"","");
	it->append(sub);
	sub->append(dj.newitem(sub,JTRUE,"sub1","true"));
	sub->append(dj.newitem(sub,JFALSE,"sub2","false"));
	  sub2 = dj.newitem(sub,JARRAY,"","");
	  sub->append(sub2);
	  sub2->append(dj.newitem(sub2,JNULL,"subsub1","null"));
	  sub2->append(dj.newitem(sub2,JNULL,"subsub2","null"));
	  sub2->append(dj.newitem(sub2,JNULL,"subsub3","null"));
	sub->append(dj.newitem(sub,JNULL,"sub3","null"));
	sub->append(dj.newitem(sub,JNUMBER,"sub4","0.1234"));
	dj.show();
	dj.out(s);
	cout << s << endl;
}

void test_access(djson &dj)
{
	djitem *it;
	//TODO
}

int main(int argc,char* argv[])
{
	djson dj;
	cout << "=============== read from 'test.txt' =============== " << endl;
	test_read();
	cout << "=============== write to json object =============== " << endl;
	test_write(dj);
	cout << "=============== access a json object =============== " << endl;
	test_access(dj);
	cout << "press ctrl+c to exit" << endl;
	cin >> argc;
	return 0;
}