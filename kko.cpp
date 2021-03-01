#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <iostream> 
#include <string> // for string class 
#include <vector>
#include <algorithm>
#include <queue>
#include <fstream>
#include <chrono>
#include <cmath>
using namespace std;

#define N 262144

struct BVSnode{
	int character;

	BVSnode *lptr;
	BVSnode *rptr;
};

struct BVSnode* create(int character){
	struct BVSnode* node = (struct BVSnode*)malloc(sizeof(struct BVSnode));
	node->character = character;

	node->lptr = NULL;
	node->rptr = NULL;
	return node;
}
/*
struct BVSnode* insert(struct BVSnode* node, int character, int codeI){
	if(node == NULL) return create(character, codeI);
	if(codeI < node->codeI) node->lptr = insert(node->lptr, character, codeI);
	else if(codeI > node->codeI) node->rptr = insert(node->rptr, character, codeI);
	return node;
}

int search(struct BVSnode* node, int val){
	if(node == NULL) return 999;
	if(node->codeI > val) return search(node->lptr, val);
	else if(node->codeI < val) return search(node->rptr, val);
	else return node->character;
}
*/
struct coding{
	int character;
	string code;
};

/*
	struktura pro nalezeni odpovidajiho znaku
*/
struct find_character{
	int character;
	find_character(int character) : character(character){}
	bool operator() (const coding& a) const{
		return a.character == character;
	}
};

/*
	struktura pro nalezeni odpovidajiho kodoveho slova 
*/
struct find_code{
	string code;
	find_code(string code) : code(code){}
	bool operator() (const coding& a) const{
		return a.code == code;
	}
};

/*
	vektor uchovava aktualni huffmanovo kodovani
*/
vector<coding> v;

/*
	struktura reprezentujici uzel stromu
*/
struct code{
	int character;
	double probability;
	code *lptr = NULL;
	code *rptr = NULL;

	code(int character, double probability){
		this->character = character;
		this->probability = probability;
		lptr = NULL;
		rptr = NULL;
	}
};

/*
	struktura ktera se pouziva pri vkladani do stromu
*/
struct compare{
	bool operator()(code* a, code* b){
		return a->probability > b->probability;
	}
};

bool cmpSize(const coding& a, const coding& b)
{
    return a.code.length() < b.code.length();
}

/*
	pruchod stromem a ziskani kodovych slov pro vsechny znaky
*/
void treeTraverse(struct code* top, string code, int size){
	if(top == NULL){
		return;
	}
	if(top->character != -1){
		coding node;
		node.character = top->character;
		if(size == 1){
			code = code + "1";
			node.code = code;
			v.push_back(node);
			return;
		}
		node.code = code;
		v.push_back(node);
	}
	treeTraverse(top->rptr, code + "1", size);
	treeTraverse(top->lptr, code + "0", size);
	
}

/*
	vytvoreni stromu
*/
void huffmanCoding(int *abeceda, int idxA, double *probabilities){
	code *parent, *lowest1, *lowest2;
	priority_queue<code*, vector<code*>, compare> tree;

	for(int i = 0; i < idxA; i++){
		tree.push(new code(abeceda[i], probabilities[i]));
	}
	
	int treeSize = tree.size();
	while(tree.size() != 1){
		lowest1 = tree.top();
		tree.pop();
		lowest2 = tree.top();
		tree.pop();
		parent = new code(-1, lowest1->probability + lowest2->probability);
		parent->lptr = lowest1;
		parent->rptr = lowest2;
		tree.push(parent);
	}
	treeTraverse(tree.top(), "", treeSize);

}

/*
	kodovani znaku pokdle prislusnych kodovych slov
	int *data - blok dat
	int length - velikost bloku dat
*/
string encode(int *data, int length){
	vector<coding>::iterator it;
	int len = 0;
	string part = "";
	for (int i = 0; i < length; i++){
			it = find_if(v.begin(), v.end(), find_character(data[i]));
			if(it == v.end()){
				cout << data[i]  << " halo" << endl;
			}
			part += (*it).code;
	}
	return part;
}


string intToBin(int number){
	string binary = "";

	if(number<0){
		number = 256+number;
	}
	for(int i = 7; i >= 0; i--){
		if(number%2 == 0){
			binary = "0" + binary;
		}
		else{
			binary = "1" + binary;
		}
		number /= 2;
	}
	return binary;

}

unsigned char makeChar(string binary){
	int number = 0;
	int exponent = 0;
	for(int i = binary.length()-1; i >= 0; i--){
		if(binary[i] == '1'){
			number += pow(2, exponent);
		}
		exponent++;
	}
	return (unsigned char)number;
}


int binToChars(string binary, ofstream& f){
	if(!f){
		cerr << "file error" << endl;
		return 0;
	}
	string bin = "";
	int idx = 0;
	for(int i = 0; i < binary.length(); i++){
		bin += binary[i];
		if(idx++ == 7){
			f << makeChar(bin);
			idx = 0;
			bin = "";
		}
	}
	return 0;
}


void inorder(struct BVSnode *root) 
{ 
    if (root != NULL) 
    { 
        inorder(root->lptr); 
        printf("%c \n", root->character); 
        inorder(root->rptr); 
    } 
} 

int comprime(int flagM, string input, string output, int huf, int width){
	long size;
	auto start = chrono::system_clock::now();

	int iter;
	string str = "";
	ofstream f;
	f.open(&output[0]);

	FILE *mFile = fopen(&input[0], "r");

	if(!mFile){
		cerr << "File erro" << endl;
	}

	/*ziskani delky souboru*/
	fseek(mFile, 0, SEEK_END);
	size = ftell(mFile);
	rewind(mFile);
	width = size;
	int data[width];
	/*vytvoreni mista v pameti pro nacteni celeho souboru*/
	unsigned char *buffer = (unsigned char *) malloc(sizeof(char) * size);
	if(buffer == NULL){
		cerr << "memory error" << endl;
	}

	size = fread(buffer,1 , size, mFile);
	if(!mFile){
		cerr <<"erro reading file" << endl;
	}

	/*pocet bloku dat*/
	iter = size / width;

	double *arr = new double[256];

	/*pokud pocet symbolu v souboru neni delitelny width*/
	if(iter*width < size) iter++;

	/*po kazdem nacteni bloku dat o delce width vytvoreni huffmanova stromu*/
	for(int x = 0; x < iter ; x++){
		int idx = 0;
		int idxA = 0;
		int del = width;
		int abeceda[256];
		fill(begin(abeceda), end(abeceda), -1);
		/*nacteni bloku dat*/
		for(int i = x*width; i < (x*width)+width; i++){
			if(i >= size) break;
			data[idx++] = buffer[i];
			//cout << (int)buffer[i] << " ";
			if(find(abeceda, abeceda+256, buffer[i]) == abeceda+256){
				abeceda[idxA++] = buffer[i];
				//cout << (int)buffer[i] << " ";
			}
		}
		/*ziskani frekvence vyskytu kazdeho symbolu z abecedy v nactenem bloku dat*/
		for(int i = 0; i < idxA; i++){
			int counter = 0;
			for(int j = x*width; j < (x*width)+width; j++){
				if(j >= size){
					
					del = size - x*width;
					break;
				}
				int number = (int)buffer[j];
				if(abeceda[i] == number) counter++;
			}

			arr[i] = counter/(double)del;
		}
		if(idx > 0){
			/*vytvoreni huffmanova stromu*/
			huffmanCoding(abeceda, idxA, arr);
			int offset = width % 8;
			if(offset > 0){
				offset = 8 - offset;
			}
			//cout << "offset:" << offset << endl;
			/*vytvoreni hlavicky pro dekoder - ulozeni huffmanova stromu do souboru*/
			for(int i = 0; i < v.size(); i++){
				f << static_cast<char>(v[i].character) << ":" << v[i].code << " ";
			}
			f  << "e-" << offset << " " << endl;

			/*zakodovani bloku dat pomoci huffmanova stromu*/
			str = encode(data, del);

			/*pridani nekolika nul, aby byla delka str delitelna 8 - do souboru lze zapisovat jen po bytech*/
			/*str reprezentuje binarku*/
			int g = 0;
			while(str.length() % 8 != 0){
				//cout << str.length() << " " <<g++ << endl;
				str +=  "0";
			}

			/*po 8mi bitech se vytvari znak*/
			binToChars(str, f);
		}
		v.clear();
	}
	
	
	
	free(buffer);
	free(arr);
	fclose(mFile);
	
	auto end = chrono::system_clock::now();
	chrono::duration<double> elapsed = end - start;
	cout << elapsed.count() << endl;

	return 2;
}

int decomprime(int flagM, string input, string output, int huf){
	//cout << flagM << ", " << input << ", " << output << ", " << huf << endl;
	int critical;
	ifstream f;
	ofstream o;
	string str = "";
	string binary = "";
	int offset = 0;
	f.open(&input[0]);
	o.open(&output[0]);
	auto start = chrono::system_clock::now();

	f.seekg(0, ios::end);
	str.reserve(f.tellg());
	f.seekg(0, ios::beg);
	str.assign(istreambuf_iterator<char>(f), istreambuf_iterator<char>());
	//cout << str.substr(1,1) << ":" << endl;

	/*init tree*/
	struct BVSnode *root = create(' ');

	int index = 0;
	while(1){
		if(str.substr(1,1) == ":"){
			BVSnode *tmp;

			char name = str[0];
			str.erase(0, 1);
			int mezera = str.find(" ");
			string code = str.substr(1, mezera-1);

			str.erase(0, mezera+1);

			/*vytvoreni huffmanova stromu pro dekoder*/
			tmp = root;
			for (int i = 0; i < code.length(); i++){
				if(code[i] == '0'){
					if(tmp->lptr == NULL) tmp->lptr = create(name);
					tmp = tmp->lptr;
				}
				else{
					if(tmp->rptr == NULL) tmp->rptr = create(name);
					tmp = tmp->rptr;
				}
			}

		}
		else if(str.substr(1,1) == "-"){
			int mezera = str.find(" ");
			offset = stoi(str.substr(2, mezera-1));
			str.erase(0, mezera+2);
		}
		else{
			break;
		}
	}
	/*vytvoreni binarky po odstraneni hlavicky*/
	for(int i = 0; i < str.size(); i++){
		binary += intToBin((int)str[i]);
	}

	
	BVSnode *tmp = root;
	for (int i = 0; i < binary.length()-offset; i++){
		
		if(binary[i] == '0'){
			tmp = tmp->lptr;
		}else{
			tmp = tmp->rptr;
		}
		if(tmp->lptr == NULL && tmp->rptr == NULL){
			o << static_cast<char>(tmp->character);
			tmp = root;
		}
	}

	auto end = chrono::system_clock::now();
	chrono::duration<double> elapsed = end - start;
	cout << elapsed.count() << endl;
	return 3;
}

int main(int argc, char **argv){
	int counter = 0;
	int flagC = 0;
	int flagD = 0;
	int flagH = 0;
	int flagM = 0;
	int flagI = 0;
	int flagO = 0;
	int flagW = 0;
	int huf = 0;
	int c;
	int help = 0;
	int go = 0;
	string input = "";
	string output = "";
	int width = 0;
	while((c = getopt(argc, argv, ":h:cdmi:o:w:"))){
		if(go){
			break;
		}
		switch(c){
			case 'c':
				flagC++;
				counter++;
				//cout << "comprime" << endl;
				break;
			case 'd':
				flagD++;
				counter++;
				//cout << "decomprime" << endl;
				break;
			case 'h':
				flagH++;
				counter++;
				if(string(optarg) == "static"){
					huf = 1;
					//cout << "static" << endl;
				}
				else if(string(optarg) == "adaptive"){
					huf = 2;
					//cout << "adaptive" << endl;
				}
				else{
					cerr << "error: wrong huffman method" << endl;
					return 1;
				}
				break;
			case ':':
				switch(optopt){
					case 'h':
						flagH++;
						help = 1;
						//cout << "help" << endl;
						break;
					default:
						cerr << "Argument is missing" << endl;
						return 1;
				}
				break;
			case 'm':
				flagM++;
				counter++;
				//cout << "model" << endl;
				break;
			case 'i':
				flagI++;
				counter++;
				input = optarg;
				//cout << "input: "<< input << endl;
				break;
			case 'o':
				flagO++;
				counter++;
				output = optarg;
				//cout << "output: "<< output << endl;
				break;
			case 'w':
				flagW++;
				counter++;
				width = atoi(optarg);
				//cout << "width: "<< width << endl;
				break;
			default:
				go = 1;
				break;
		}
	}
	if(flagC > 1 || flagD > 1 || flagH > 1 || flagM > 1 || flagI > 1 || flagO > 1){
		cerr << "duplicitni pocet argumentu" << endl;
		return 1;
	}
	if(help && counter == 0){
		cout << "napoveda" << endl;
		return 0;
	}

	if(huf == 0){
		cerr << "Nedefinovana huffmanova metoda" << endl;
		return 1;
	}

	if(flagI && flagO);
	else{
		cerr << "Nedefinovany jeden z I/O souboru" << endl;
		return 1;
	}
	if(flagC && flagD == 0){
		comprime(flagM, input, output, huf, width);
	}
	else if(flagD && flagC == 0){
		decomprime(flagM, input, output, huf);
	}
	else{
		cerr << "Chyba v prepinaci -c nebo -d" << endl;
		return 1;
	}
	return 0;
}