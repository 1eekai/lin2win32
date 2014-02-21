#pragma once

#include	<fstream>
#include	<cstring>
#include	<string>
#include	<regex>

#define		CANNOT_READ_ANYMORE -65535

using namespace	std;

bool	regex_fit_easily(string str, char* regex_line){
	bool	finalresult;
	string regexBuff = ".*"+(string)regex_line+".*";
	regex	pattern(regexBuff);
	finalresult = std::regex_match(str,pattern);
	return	finalresult;
}

int	fuck_r(char* str){
	int	counter;
	if (str[0]!='\r')	return -1;
	for (counter=0;counter<sizeof(str);counter++){
		str[counter]=str[counter+1];
	}
	return 0;
}

/*
void show_hex(char* str){
	int counter;
	int tmp;
	for (counter=0;counter<strlen(str);counter++){
		tmp=str[counter];
		cout<<tmp<<" ";
	}
}
*/

class	_file{
	public:
		int	getLength(void){
			if (ifFileexistd==false) return 0;
			else return	file_end-file_begin;
		}
		_file(char* filename, int mode){
			this->filename=filename;
			this->mode=mode;
			ifreadLineCalled = false;
			errBuff=new char[255];
			memset(errBuff,'\0',sizeof(errBuff));
			if (filename==0){
				ifFileexistd=false;
				strcpy(errBuff,"ERR: File name must be specified.");
			}else{
				fp.open(filename,mode);
				if (!fp){        // 如果打开文件失败
					ifFileexistd=false;
					strcpy(errBuff,"ERR: Open file failed. Bad inode or corrupt file.");
				}else{
					file_begin=fp.tellg(); 
					fp.seekg(0,ios::end);	// 计算文件长度
					file_end=fp.tellg();
					fp.seekg(0,ios::beg);  //将文件指针移回文件开始
				}
			}
		}
		void	closefile(void){      // 关闭文件并回收缓冲区
			delete [] errBuff;
			delete [] dymBuff;   
			fp.close();
		}
		char*	countChar_cstr(void){
			return	dymBuff;   // 返回指向缓冲区字符数组的指针，可以通过<cstring>定义的C标准库来操作C字符串。
		}
		string	countChar_str(void){
			return	dymBuff; // 返回string。可以使用string字符串相关来操作。
		}
		char	countChar_singlechar(int _count){
			return	dymBuff[_count];  // 返回缓冲区第 _count 个字符串，这个绝对有用。
		}
		void	readLine(int line_num){
			if (ifreadLineCalled==true){
				memset(dymBuff,'\0',sizeof(dymBuff)); // 如果一个打开的 _file 类使用过readLine()，则不再申请内存，但是必须清空缓冲区。
			}else{
				dymBuff=new char[255];
				memset(dymBuff,'\0',sizeof(dymBuff));
			}
			ifreadLineCalled=true;
			fp.seekg(0,ios::beg);
			counter1=0;  //行计数器
			counter2=0;  //计算_line_num行的长度。
			for (counter_forReadLine=0;counter_forReadLine<getLength();counter_forReadLine++){	//一个一个字符读，并存入dymBuff缓冲区中。
				ching=fp.get();
				if (ching==10)	continue;    // DOS文本中的换行符为\r\n，此举是为了兼容DOS文本（忽略\r，不将其读入hymBuff。 ）
				if(counter1+1==line_num){
					if (ching != 13)	dymBuff[counter2]=ching;
					counter2++; //在读到line_num行的时候，每过一个循环 counter2++ ,最后可以得到这一行的长度。
				}
				if (ching==13){
					counter1++;
					continue;
				}
			}
			dymBuff[counter2+1]='\0';
			fp.seekg(0,ios::beg);  // 恢复文件指针。
			fuck_r(dymBuff);
		}
		int	findkeyWordsAndReadLine(char* keyWords){
			counter3=1;
			while(!fp.eof()){
				ifAloopTrue=true;
				readLine(counter3); //进入循环，并从第一行开始读取文件中的内容。
				ifAloopTrue = regex_fit_easily(dymBuff, keyWords);
				if (ifAloopTrue==true)	return counter3;
				else{
					counter3++;
				}
			}
			strcpy(errBuff,"ERR, Cannot find anymore fit strings.");
			return	CANNOT_READ_ANYMORE;  //错误处理。
		}
		void	errMsgOutput(void){	cout<<errBuff<<endl;	}
		bool	ifFileexistd;
	
	private:
		char*	dymBuff;
		char*	filename;
		char*	errBuff;
		char	ching;
		fstream	fp;
		int	mode;
		int	access;
		int	file_begin;
		int	file_end;
		int	counter_forReadLine;
		int	counter1;
		int	counter2;
		int	counter3;
		bool	ifAloopTrue;
		bool	ifreadLineCalled;
};	
