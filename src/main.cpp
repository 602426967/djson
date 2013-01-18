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

	dj.root.append(sub1=dj.newitem(&dj.root,JOBJECT));
	sub1->append(dj.newitem(sub1,JSTRING,"key1","111"));
	sub1->append(dj.newitem(sub1,JNUMBER,"key2","222"));
	sub1->append(dj.newitem(sub1,JNUMBER,"key3","333"));

	//if you insert a sub item
	//the sub must be JOBJECT or JARRAY
	sub1->append(sub2=dj.newitem(sub1,JOBJECT));
		sub2->append(dj.newitem(sub2,JTRUE,"sub1"));
		sub2->append(dj.newitem(sub2,JFALSE,"sub2"));
		sub2->append(sub3=dj.newitem(sub2,JARRAY));
			sub3->append(dj.newitem(sub3,JNUMBER,"subsub1","1.234"));
			sub3->append(dj.newitem(sub3,JTRUE,"subsub2"));
			sub3->append(dj.newitem(sub3,JSTRING,"subsub3","abcdefg"));
		sub2->append(dj.newitem(sub2,JNULL,"sub3"));
		sub2->append(dj.newitem(sub2,JNUMBER,"sub4","0.1234"));

	sub1->append(dj.newitem(sub1,JFALSE,"key4","false"));
	sub1->append(dj.newitem(sub1,JNULL,"key5"));

	cout << "---------- show the tree -------" << endl;
	dj.show();
	cout << "------- out to string -----------" << endl;
	dj.out(s);
	cout << s << endl;
}

void test_access(djson &dj)
{
	djitem *it;

	it = dj.finditem(&dj.root,"key2");
	if (it) 
		cout << "find by name key2:" << it->skey << ",value:" << it->svalue <<
		",type:" << it->gettypename() << endl;
	it = dj.finditem(&dj.root,"key4");
	if (it) 
		cout << "find by name key4:" << it->skey << ",value:" << it->svalue <<
		",type:" << it->gettypename() << endl;

	it = dj.finditem(&dj.root,"key9");
	if (it) 
		cout << "find by name key9:" << it->skey << ",value:" << it->svalue <<
		",type:" << it->gettypename() << endl;
	else
		cout << "find by name key9 failure" << endl;

	it = dj.finditembyindex(&dj.root,2);
	if (it) 
		cout << "find by index=2:" << it->skey << ",value:" << it->svalue <<
		",type:" << it->gettypename() << endl;

	it = dj.finditembyindex(&dj.root,6);
	if (it) 
		cout << "find by index=6:" << it->skey << ",value:" << it->svalue <<
		",type:" << it->gettypename() << endl;
	else
		cout << "find by index=6 failure.\n";

	it = dj.finditembytype(&dj.root,1,JNUMBER);
	if (it) 
		cout << "find the 2th number:" << it->skey << ",value:" << it->svalue <<
		",type:" << it->gettypename() << endl;
	else
		cout << "find by 2th number failure.\n";
}

void test_modify(djson &dj)
{
	cout << "nofinish now" << endl;
	//delete
	//insert
	//modify
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
	cout << "=============== modify a json object =============== " << endl;
	test_modify(dj);
	cout << "press ctrl+c to exit" << endl;
	cin >> argc;
	return 0;
}