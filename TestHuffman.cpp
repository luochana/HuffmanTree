#include<iostream>
#include"myHuffman.h"
using namespace std;
int main()
{
	string filename;
	cout << "��������Ҫѹ�����ļ���" << endl;
	cin >> filename;
	huffmanZipByWord(filename, filename);
	cout << "����ѹ��.....";
	cout << endl;
	unzipHuffFileByWord(filename + ".wzip");
	compareFiles(filename, filename + ".wzip");

	system("pause");
	return 0;
}