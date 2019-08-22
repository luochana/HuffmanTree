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
	string value;//单词
	int freq; //单词出现频率
	string huffcode;//编码串
	HuffNode* left;
	HuffNode* right;
};

struct cmpByFreq {

	cmpByFreq() {}
	bool operator()(const HuffNode *node1, const HuffNode *node2)const {
		return node1->freq > node2->freq;//根据字符频率最小值优先
	}
};

//生成Huffman 01编码串的函数
void makeHuffmanCode(HuffNode *node, string codestr)
{
	node->huffcode += codestr;
	if (node->left != NULL)
		makeHuffmanCode(node->left, node->huffcode + "0");
	if (node->right != NULL)
		makeHuffmanCode(node->right, node->huffcode + "1");

}
//输出Huffman树叶节点的函数
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
	while (T--) //输出二进制串
	{
		if ((tempchar&(1 << T)))
			cout << 1;
		else cout << 0;
	}
	cout << endl;
}
//释放内存
void destroy(HuffNode *node)
{
	if (node) {
		destroy(node->left);
		destroy(node->right);
		delete node;
	}
}
//压缩文件的函数
void huffmanZipByWord(string filename, string zipname)
{
	ifstream fin(filename, ios::binary);
	if (!fin) {
		cout << "file open error!" << endl;
		exit(1);
	}
	//cout << "正在压缩文件……" << endl;
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
			//cout << "字符：" << ch << endl;
			if (wordstr != "") {
				if (wordMap.find(wordstr) == wordMap.end())
					wordMap[wordstr] = 1;
				else wordMap[wordstr]++;
				wordstr = "";
				wordsum++;
			}
			chstr = "";   //非字母类型入map
			chstr += ch;
			if (wordMap.find(chstr) == wordMap.end())
				wordMap[chstr] = 1;
			else wordMap[chstr]++;
			wordsum++;
		}
	}
	fin.close();
	//最后一个字符的处理
	if ((ch >= 'a'&&ch <= 'z') || (ch >= 'A'&&ch <= 'Z')) //如果最后一个字符是字母
	{
		if (wordMap.find(wordstr) == wordMap.end())
			wordMap[wordstr] = 1;
		else wordMap[wordstr]++;
		wordstr = "";
		wordsum++;
	}

	fout.write((char*)&wordsum, sizeof(int)); //向压缩文件写入单词总数
	int wordNum = wordMap.size(); //获取单词种类
	fout.write((char*)&wordNum, sizeof(int));//向压缩文件写入单词种类数

	char spacech = ','; //分隔符
	//获取最小优先队列
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
		fout.write((char*)&huffNode->freq, sizeof(int)); //写入单词-频率对
		//fout.write((char*)&huffNode->value,huffNode->value.size());		//注意不能用sizeof(string)
		//fout.write(string)这种方式当字符串很长的时候也会发生乱码错误
		fout << huffNode->value;
		fout.write((char*)&spacech, sizeof(char));//写一个spacech作为分隔符

	}

	//构造哈夫曼树
	HuffNode *node1, *node2, *root = NULL;
	for (int i = 0; i < wordNum - 1; i++) {	//n-1次
		HuffNode *newNode = new HuffNode();
		if (min_queue.size() == 2)	//为了避免每次循环都要判断，可以先循环处理n-2次，最后一次再单独处理
			root = newNode;
		node1 = min_queue.top(); min_queue.pop();
		node2 = min_queue.top(); min_queue.pop();
		newNode->left = node1;
		newNode->right = node2;
		newNode->freq = node1->freq + node2->freq;//权重之和
		//cout << node1->freq << " " << node2->freq <<" "<<newNode->freq << endl;
		min_queue.push(newNode);
	}

	makeHuffmanCode(root, ""); //生成各个字符的Huffman编码串
	//printLeaf(root);

	//写入Huffman编码到压缩文件
	wordstr = "";
	int length = 8; char tempchar = 0;
	ifstream infile(filename, ios::binary);
	string codestr;
	//while (!infile.eof())
	for (long fp = fbeg; fp<fend; fp++)
	{
		infile.read(&ch, sizeof(char));
		//cout << "字符："<<ch << endl;
		if ((ch >= 'a'&&ch <= 'z') || (ch >= 'A'&&ch <= 'Z'))
			wordstr += ch;
		else {
			//cout << "字符：" << ch << endl;
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
						//写入Huffman编码串到压缩文件	
						//printCharToBin(tempchar); 
						length = 8;
						tempchar = 0;
					}
				}
				wordstr = "";
			}
			//非字母类型
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
					//写入Huffman编码串到压缩文件	
					length = 8;
					tempchar = 0;
				}
			}
		}
	}
	//如果最后一个字符是字母
	if ((ch >= 'a'&&ch <= 'z') || (ch >= 'A'&&ch <= 'Z')) {
		codestr = huffmanWordMap[wordstr]->huffcode;
		for (int i = 0; i < codestr.size(); i++) {
			length--;
			if (codestr[i] == '0')
				tempchar = tempchar | (0 << length);
			else tempchar = tempchar | (1 << length);
			if (length == 0) {
				fout.write(&tempchar, sizeof(char));
				//写入Huffman编码串到压缩文件	
				//printCharToBin(tempchar);
				length = 8;
				tempchar = 0;
			}
		}
	}
	if (length != 0) //最后一个字节的处理，防止写入压缩文件的时候长度未写满8位的倍数
	{
		//printCharToBin(tempchar);
		fout.write(&tempchar, sizeof(char));
	}
	//cout << "压缩完成!" << endl;

	//释放内存，清除Huffman树每个节点
	//destroy(root);
	infile.close();
	fout.close();
}

void unzipHuffFileByWord(string filename)
{
	cout << "正在解压……" << endl;
	ifstream fin(filename, ios::binary);
	ofstream fout(filename + ".unzip", ios::binary);
	long fbeg, fend;
	fbeg = fin.tellg();
	int wordsum = 0;
	fin.read((char*)&wordsum, sizeof(int)); 
	int wordNum = 0;
	fin.read((char*)&wordNum, sizeof(int)); 
	string wordstr; int freq; char ch; int spaceflag = 0;
	//最小优先队列 priority_queue
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
				if (wordstr != ""&&ch == ',') //读到分隔符（逗号）//
					break;
			}
		}
		//cout << wordstr << " : " << freq << endl;
		HuffNode *huffnode = new HuffNode();
		huffnode->left = NULL; huffnode->right = NULL;
		huffnode->value = wordstr;
		huffnode->freq = freq;
		//入队列
		min_queue.push(huffnode);
	}

	fend = fin.tellg();
	cout << "哈夫曼树大小：" << (fend - fbeg)*1.0 / 1024 << "kB" << endl;
	fbeg = fend;
	fin.seekg(0, ios::end);
	fend = fin.tellg();
	fin.seekg(fbeg, ios::beg);
	cout << "Huffman编码大小：" << (fend - fbeg)*1.0 / 1024 << "kB" << endl;

	//构造哈夫曼树
	HuffNode *node1, *node2, *root = NULL;
	for (int i = 0; i < wordNum - 1; i++) {	//n-1次
		HuffNode *newNode = new HuffNode();
		if (min_queue.size() == 2)	//为了避免每次循环都要判断，可以先循环处理n-2次，最后一次再单独处理
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
	//开始解码
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
			if (tempNode->left == NULL) //找到叶节点则将单词写入文件
			{
				if (wordsum == 0)
					break;
				wordsum--;
				wordstr = tempNode->value;
				fout << wordstr;
				//cout <<wordstr;
				tempNode = root; //将tempNode重新指向根节点
			}

		}
	}

	cout << "解压完成！" << endl;
	//释放内存，释放Huffman树的每一个节点内存
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

	cout << srcfile << "大小：" << (fend1 - fbeg1)*1.0 / 1024 << "kb" << endl;
	cout << zipfile << "大小：" << (fend2 - fbeg2)*1.0 / 1024 << "kb" << endl;
	cout << setiosflags(ios::fixed) << setprecision(2);
	cout << "压缩率：" << (fend2 - fbeg2) *1.0 / (fend1 - fbeg1) * 100 << "%" << endl;

	srcfin.close();
	zipfin.close();
}

void testWriteStrToFile(string filename) //测试向文件写入长字符串是否有乱码错误
{
	ofstream fout(filename, ios::binary);
	string str = "abcabctrationfor";
	//string str = "hello";
	cout << "size:" << str.size() << endl;
	//fout.write((char*)&str,str.size()); //如果字符串长度过长，这种方式写入文件则会发生错误
	fout << str; //这种方式是正确的
	fout.close();
}
