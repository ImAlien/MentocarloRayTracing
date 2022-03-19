
#include "Log.h"
#include <fstream>
#include <ctime>
using namespace std;

std::string curTime()
{
	// 获取当前时间，并规范表示
	char tmp[64];
	time_t ptime;
	time(&ptime);  // time_t time (time_t* timer);
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&ptime));
	return tmp;
}

void LOG(std::string strMsg){
	std::ofstream outfile;
	outfile.open("Log.txt", ios_base::app | ios_base::out); //存放数据的文件名  
	cout << strMsg << endl;
	if (outfile.is_open())
	{
		outfile << curTime() << "   " << strMsg << endl;
		outfile.close();
	}
	else
	{
		std::cout << "不能打开文件!" << std::endl;
	}
}