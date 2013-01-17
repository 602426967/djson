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
	djitem *sub1,*sub2,*sub3;

	dj.root.append(sub1=dj.newitem(&dj.root,JOBJECT,"",""));
	sub1->append(dj.newitem(sub1,JSTRING,"key1","111"));
	sub1->append(dj.newitem(sub1,JNUMBER,"key2","222"));

	//if you insert a sub item
	//the sub must be JOBJECT or JARRAY
	sub1->append(sub2=dj.newitem(sub1,JOBJECT,"",""));
		sub2->append(dj.newitem(sub2,JTRUE,"sub1","true"));
		sub2->append(dj.newitem(sub2,JFALSE,"sub2","false"));
		sub2->append(sub3=dj.newitem(sub2,JARRAY,"",""));
			sub3->append(dj.newitem(sub3,JNUMBER,"subsub1","1.234"));
			sub3->append(dj.newitem(sub3,JTRUE,"subsub2","true"));
			sub3->append(dj.newitem(sub3,JSTRING,"subsub3","abcdefg"));
		sub2->append(dj.newitem(sub2,JNULL,"sub3","null"));
		sub2->append(dj.newitem(sub2,JNUMBER,"sub4","0.1234"));

	sub1->append(dj.newitem(sub1,JFALSE,"key3","false"));
	sub1->append(dj.newitem(sub1,JNULL,"key4","null"));

	cout << "---------- show the tree -------" << endl;
	dj.show();
	cout << "------- out to string -----------" << endl;
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