#include<iostream>
#include<fstream>
#include<string>
#include<map>
#include<queue>
#include<time.h>
#include<iomanip>
using namespace std;

struct  HuffNode
{
	string value;//����
	int freq; //���ʳ���Ƶ��
	string huffcode;//���봮
	HuffNode* left;
	HuffNode* right;
};

struct cmpByFreq {

	cmpByFreq() {}
	bool operator()(const HuffNode *node1, const HuffNode *node2)const {
		return node1->freq > node2->freq;//�����ַ�Ƶ����Сֵ����
	}
};

//����Huffman 01���봮�ĺ���
void makeHuffmanCode(HuffNode *node, string codestr)
{
	node->huffcode += codestr;
	if (node->left != NULL)
		makeHuffmanCode(node->left, node->huffcode + "0");
	if (node->right != NULL)
		makeHuffmanCode(node->right, node->huffcode + "1");

}
//���Huffman��Ҷ�ڵ�ĺ���
void printLeaf(HuffNode * node)
{
	if (node) {
		printLeaf(node->left);
		printLeaf(node->right);
		if (node->left == NULL || node->right == NULL)
			cout << node->value << " : " << node->freq << " " << node->huffcode << endl;
	}
}
void printCharToBin(char tempchar)
{
	int T = 8;
	while (T--) //��������ƴ�
	{
		if ((tempchar&(1 << T)))
			cout << 1;
		else cout << 0;
	}
	cout << endl;
}
//�ͷ��ڴ�
void destroy(HuffNode *node)
{
	if (node) {
		destroy(node->left);
		destroy(node->right);
		delete node;
	}
}
//ѹ���ļ��ĺ���
void huffmanZipByWord(string filename, string zipname)
{
	ifstream fin(filename, ios::binary);
	if (!fin) {
		cout << "file open error!" << endl;
		exit(1);
	}
	//cout << "����ѹ���ļ�����" << endl;
	ofstream fout(zipname + ".wzip", ios::binary);
	long fbeg = fin.tellg();
	fin.seekg(0, ios::end);
	long fend = fin.tellg();
	long filelength = fend - fbeg;

	fin.seekg(0, ios::beg);
	//cout << fbeg << " " << fend << " " << filelength << endl;

	map<string, int> wordMap;
	char ch;
	string wordstr = "";
	string chstr = "";
	int wordsum = 0;
	for (long fp = fbeg; fp<fend; fp++)
	{
		fin.read(&ch, sizeof(char));
		//cout << ch;
		if ((ch >= 'a'&&ch <= 'z') || (ch >= 'A'&&ch <= 'Z'))
			wordstr += ch;
		else {
			//cout << "�ַ���" << ch << endl;
			if (wordstr != "") {
				if (wordMap.find(wordstr) == wordMap.end())
					wordMap[wordstr] = 1;
				else wordMap[wordstr]++;
				wordstr = "";
				wordsum++;
			}
			chstr = "";   //����ĸ������map
			chstr += ch;
			if (wordMap.find(chstr) == wordMap.end())
				wordMap[chstr] = 1;
			else wordMap[chstr]++;
			wordsum++;
		}
	}
	fin.close();
	//���һ���ַ��Ĵ���
	if ((ch >= 'a'&&ch <= 'z') || (ch >= 'A'&&ch <= 'Z')) //������һ���ַ�����ĸ
	{
		if (wordMap.find(wordstr) == wordMap.end())
			wordMap[wordstr] = 1;
		else wordMap[wordstr]++;
		wordstr = "";
		wordsum++;
	}

	fout.write((char*)&wordsum, sizeof(int)); //��ѹ���ļ�д�뵥������
	int wordNum = wordMap.size(); //��ȡ��������
	fout.write((char*)&wordNum, sizeof(int));//��ѹ���ļ�д�뵥��������

	char spacech = ','; //�ָ���
	//��ȡ��С���ȶ���
	map<string, HuffNode*> huffmanWordMap;
	map<string, int>::iterator iter;
	priority_queue<HuffNode*, vector<HuffNode*>, cmpByFreq> min_queue;
	for (iter = wordMap.begin(); iter != wordMap.end(); iter++)
	{
		HuffNode *huffNode = new HuffNode();
		huffNode->left = NULL; huffNode->right = NULL;
		huffNode->value = iter->first;
		huffNode->freq = iter->second;
		min_queue.push(huffNode);
		huffmanWordMap[huffNode->value] = huffNode;
		//cout << huffNode->value << " : " << huffNode->freq << endl;
		fout.write((char*)&huffNode->freq, sizeof(int)); //д�뵥��-Ƶ�ʶ�
		//fout.write((char*)&huffNode->value,huffNode->value.size());		//ע�ⲻ����sizeof(string)
		//fout.write(string)���ַ�ʽ���ַ����ܳ���ʱ��Ҳ�ᷢ���������
		fout << huffNode->value;
		fout.write((char*)&spacech, sizeof(char));//дһ��spacech��Ϊ�ָ���

	}

	//�����������
	HuffNode *node1, *node2, *root = NULL;
	for (int i = 0; i < wordNum - 1; i++) {	//n-1��
		HuffNode *newNode = new HuffNode();
		if (min_queue.size() == 2)	//Ϊ�˱���ÿ��ѭ����Ҫ�жϣ�������ѭ������n-2�Σ����һ���ٵ�������
			root = newNode;
		node1 = min_queue.top(); min_queue.pop();
		node2 = min_queue.top(); min_queue.pop();
		newNode->left = node1;
		newNode->right = node2;
		newNode->freq = node1->freq + node2->freq;//Ȩ��֮��
		//cout << node1->freq << " " << node2->freq <<" "<<newNode->freq << endl;
		min_queue.push(newNode);
	}

	makeHuffmanCode(root, ""); //���ɸ����ַ���Huffman���봮
	//printLeaf(root);

	//д��Huffman���뵽ѹ���ļ�
	wordstr = "";
	int length = 8; char tempchar = 0;
	ifstream infile(filename, ios::binary);
	string codestr;
	//while (!infile.eof())
	for (long fp = fbeg; fp<fend; fp++)
	{
		infile.read(&ch, sizeof(char));
		//cout << "�ַ���"<<ch << endl;
		if ((ch >= 'a'&&ch <= 'z') || (ch >= 'A'&&ch <= 'Z'))
			wordstr += ch;
		else {
			//cout << "�ַ���" << ch << endl;
			if (wordstr != ""){		
				codestr = huffmanWordMap[wordstr]->huffcode;
				//cout << wordstr<<" : "<<codestr << endl;
				for (int i = 0; i < codestr.size(); i++) {
					length--;
					if (codestr[i] == '0')
						tempchar = tempchar | (0 << length);
					else tempchar = tempchar | (1 << length);
					if (length == 0) {
						fout.write(&tempchar, sizeof(char));
						//д��Huffman���봮��ѹ���ļ�	
						//printCharToBin(tempchar); 
						length = 8;
						tempchar = 0;
					}
				}
				wordstr = "";
			}
			//����ĸ����
			chstr = ""; chstr += ch;
			codestr = huffmanWordMap[chstr]->huffcode;
			for (int i = 0; i < codestr.size(); i++) {
				length--;
				if (codestr[i] == '0')
					tempchar = tempchar | (0 << length);
				else tempchar = tempchar | (1 << length);
				if (length == 0) {
					fout.write(&tempchar, sizeof(char));
				//	printCharToBin(tempchar);
					//д��Huffman���봮��ѹ���ļ�	
					length = 8;
					tempchar = 0;
				}
			}
		}
	}
	//������һ���ַ�����ĸ
	if ((ch >= 'a'&&ch <= 'z') || (ch >= 'A'&&ch <= 'Z')) {
		codestr = huffmanWordMap[wordstr]->huffcode;
		for (int i = 0; i < codestr.size(); i++) {
			length--;
			if (codestr[i] == '0')
				tempchar = tempchar | (0 << length);
			else tempchar = tempchar | (1 << length);
			if (length == 0) {
				fout.write(&tempchar, sizeof(char));
				//д��Huffman���봮��ѹ���ļ�	
				//printCharToBin(tempchar);
				length = 8;
				tempchar = 0;
			}
		}
	}
	if (length != 0) //���һ���ֽڵĴ�����ֹд��ѹ���ļ���ʱ�򳤶�δд��8λ�ı���
	{
		//printCharToBin(tempchar);
		fout.write(&tempchar, sizeof(char));
	}
	//cout << "ѹ�����!" << endl;

	//�ͷ��ڴ棬���Huffman��ÿ���ڵ�
	//destroy(root);
	infile.close();
	fout.close();
}

void unzipHuffFileByWord(string filename)
{
	cout << "���ڽ�ѹ����" << endl;
	ifstream fin(filename, ios::binary);
	ofstream fout(filename + ".unzip", ios::binary);
	long fbeg, fend;
	fbeg = fin.tellg();
	int wordsum = 0;
	fin.read((char*)&wordsum, sizeof(int)); 
	int wordNum = 0;
	fin.read((char*)&wordNum, sizeof(int)); 
	string wordstr; int freq; char ch; int spaceflag = 0;
	//��С���ȶ��� priority_queue
	priority_queue<HuffNode*, vector<HuffNode*>, cmpByFreq> min_queue;
	for (int i = 0; i < wordNum; i++)
	{
		fin.read((char*)&freq, sizeof(int));
		wordstr = "";
		spaceflag = 0;
		while (true)
		{
			fin.read(&ch, sizeof(char));
			if ((ch >= 'a'&&ch <= 'z') || (ch >= 'A'&&ch <= 'Z')) {
				wordstr += ch;
			}
			else {
				if (wordstr == "") {
					wordstr += ch;
					continue;
				}
				if (wordstr != ""&&ch == ',') //�����ָ��������ţ�//
					break;
			}
		}
		//cout << wordstr << " : " << freq << endl;
		HuffNode *huffnode = new HuffNode();
		huffnode->left = NULL; huffnode->right = NULL;
		huffnode->value = wordstr;
		huffnode->freq = freq;
		//�����
		min_queue.push(huffnode);
	}

	fend = fin.tellg();
	cout << "����������С��" << (fend - fbeg)*1.0 / 1024 << "kB" << endl;
	fbeg = fend;
	fin.seekg(0, ios::end);
	fend = fin.tellg();
	fin.seekg(fbeg, ios::beg);
	cout << "Huffman�����С��" << (fend - fbeg)*1.0 / 1024 << "kB" << endl;

	//�����������
	HuffNode *node1, *node2, *root = NULL;
	for (int i = 0; i < wordNum - 1; i++) {	//n-1��
		HuffNode *newNode = new HuffNode();
		if (min_queue.size() == 2)	//Ϊ�˱���ÿ��ѭ����Ҫ�жϣ�������ѭ������n-2�Σ����һ���ٵ�������
			root = newNode; //
		node1 = min_queue.top(); min_queue.pop();
		node2 = min_queue.top(); min_queue.pop();
		newNode->left = node1;
		newNode->right = node2;
		newNode->freq = node1->freq + node2->freq;
		
		min_queue.push(newNode);
	}
	if (root == NULL)
		return;
	makeHuffmanCode(root, ""); 
	//printLeaf(root);
	//��ʼ����
	char tempchar = 0;
	HuffNode *tempNode = root; 
	while (!fin.eof())
	{
		fin.read(&ch, sizeof(char));
		for (int i = sizeof(char)* 8 - 1; i >= 0; i--)
		{
			if (tempNode->left != NULL)
			{
				if ((ch >> i) & 1)
					tempNode = tempNode->right;
				else tempNode = tempNode->left;
			}
			if (tempNode->left == NULL) //�ҵ�Ҷ�ڵ��򽫵���д���ļ�
			{
				if (wordsum == 0)
					break;
				wordsum--;
				wordstr = tempNode->value;
				fout << wordstr;
				//cout <<wordstr;
				tempNode = root; //��tempNode����ָ����ڵ�
			}

		}
	}

	cout << "��ѹ��ɣ�" << endl;
	//�ͷ��ڴ棬�ͷ�Huffman����ÿһ���ڵ��ڴ�
	//destroy(root);
	fin.close();
	fout.close();
}

void compareFiles(string srcfile, string zipfile)
{

	ifstream srcfin(srcfile);
	ifstream zipfin(zipfile);
	long fbeg1 = srcfin.tellg();
	srcfin.seekg(0, ios::end);
	long fend1 = srcfin.tellg();

	long fbeg2 = zipfin.tellg();
	zipfin.seekg(0, ios::end);
	long fend2 = zipfin.tellg();

	cout << srcfile << "��С��" << (fend1 - fbeg1)*1.0 / 1024 << "kb" << endl;
	cout << zipfile << "��С��" << (fend2 - fbeg2)*1.0 / 1024 << "kb" << endl;
	cout << setiosflags(ios::fixed) << setprecision(2);
	cout << "ѹ���ʣ�" << (fend2 - fbeg2) *1.0 / (fend1 - fbeg1) * 100 << "%" << endl;

	srcfin.close();
	zipfin.close();
}

void testWriteStrToFile(string filename) //�������ļ�д�볤�ַ����Ƿ����������
{
	ofstream fout(filename, ios::binary);
	string str = "abcabctrationfor";
	//string str = "hello";
	cout << "size:" << str.size() << endl;
	//fout.write((char*)&str,str.size()); //����ַ������ȹ��������ַ�ʽд���ļ���ᷢ������
	fout << str; //���ַ�ʽ����ȷ��
	fout.close();
}
