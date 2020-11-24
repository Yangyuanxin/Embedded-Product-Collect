#include "config.h"
#define BUF_SIZE 256

int iniConfigTest()
{
	const char *file ="config.ini";
	const char *section = "student";
	const char *name_key = "name";
	const char *age_key = "age";
	char name[BUF_SIZE]={0};
	int age;

	//write name key value pair
	if(!write_profile_string(section,name_key,"Tom",file))
	{
		printf("write name pair to ini file fail\n");
		return -1;
	}
	
	//write age key value pair
	if(!write_profile_string(section,age_key,"20",file))
	{
		printf("write age pair to ini file fail\n");
		return -1;
	}
	
	//printf("[%s]\n",section);
	//read string pair, test read string value
	if(!read_profile_string(section, name_key, name, BUF_SIZE,"",file))
	{
		printf("read ini file fail\n");
		return -1;
	}
	else
	{
		printf("%s=%s\n",name_key,name);
	}

	//read age pair, test read int value.
	//if read fail, return default value
	age = read_profile_int(section,age_key,0,file);
	printf("%s=%d\n",age_key,age);

	write_profile_int("CAT", "age", BUF_SIZE,file);
	if(!read_profile_string("CAT", "name", name, BUF_SIZE,"",file))
	{
		printf("read ini file fail\n");
		return -1;
	}

	printf("%s=%s\n",name_key,name);
	return 0;
}



void initMachineConf()
{
	//iniConfigTest();
}

void loadConfig()
{

}


