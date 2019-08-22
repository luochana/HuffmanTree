#include<iostream>
#include"myHuffman.h"
using namespace std;
int main()
{
	string filename;
	cout << "请输入需要压缩的文件名" << endl;
	cin >> filename;
	huffmanZipByWord(filename, filename);
	cout << "正在压缩.....";
	cout << endl;
	unzipHuffFileByWord(filename + ".wzip");
	compareFiles(filename, filename + ".wzip");

	system("pause");
	return 0;
}