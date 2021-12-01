#include<iostream>
#include<fstream>//文件输入
#include<vector>//动态数组
#include<map>
#include<queue>
//#include<stack>
#include<set>//verctor 去重
#include <iomanip>//调整输出格式
#include <algorithm>//reverse()
using namespace std;

//获取第一个字符片段 如*AB 获取 *，T'A'c获取T', idAC获取 id。
string getFirstPart(string str);

//将读取的某一行 ，拆分为 开头 和 内容 如：E->anc 分为 {E,{anc}} E->A|a 分为{E,{A,a}}
pair<string, vector<string>> aPartString(string str);

//计算first
void getFirst(map<string, vector<string>>text, map<string, vector<string>>&first) {
	//1. 第一遍
	for (pair<string, vector<string>> cerPair : text) {//读取一类 E A B. 遍历所有非终结符
		
		//循环遍历所有pair给p赋值  
		pair<string, string> p;
		p.first = cerPair.first;
		//循环遍历某一类 E -> a b c ...  遍历 说有终结符
		// map特性 不需要判断是否记录过E
		first.insert({ p.first,{} });//占一个位置(初始化一下) 代码很丑但没有想到替代的办法
		for (auto it = cerPair.second.begin(); it != cerPair.second.end(); it++) {
			p.second = *it;//赋值

			//读取终结符 或者非终结符（first(A)=first(B)之类的）
			//E -> str ,first中是否记录过str
			
			//如果未记录p.second 则记录p.second
			if (find(first[p.first].begin(), first[p.first].end(), p.second) == first[p.first].end()) {
				first[p.first].push_back(getFirstPart(p.second));
			}
			else {//已记录则啥也不用干
				break;
			}
		}
	}

	//第二遍 进栈
	map<string, vector<string>>::iterator cerPair = first.begin();
	for (; cerPair != first.end();cerPair++) {//读取一类 E->a E->b E...
		queue<string> q;
		//全部进栈 并删除first中存的数据
		for (auto it = cerPair->second.begin(); it != cerPair->second.end(); it++) {
			q.push(*it);
		}
		cerPair->second.clear();

		//处理栈中数据，重新填入first map
		while (!q.empty()) {
			string str = q.front();
			q.pop();
			//非终结符 A 或者A' 两种情况，比较简单，直接写死
			if (str[0] >= 'A' && str[0] <= 'Z') {
				//遍历str对应的first
				for (string tmp : first[str]) {
					//检查是否含有该左部 如果没有
					if (find(cerPair->second.begin(), cerPair->second.end(), tmp) == cerPair->second.end()) {
						/*cerPair->second.push_back(tmp);*/
						q.push(tmp);
					}
				}
			}
			else {//终结符
				cerPair->second.push_back(str);
			}
		}
	}
}

//向follow中添加元素
void addToFollow(map<string, vector<string>>& follow, pair<string, vector<string>>addPair) {\

	pair<string, string> p;
	p.first = addPair.first;
	// map特性 不需要判断是否记录过E
	follow.insert({ p.first,{} });//占一个位置(初始化一下) 代码很丑但没有想到替代的办法
	for (auto it = addPair.second.begin(); it != addPair.second.end(); it++) {
		if (*it == "#") {//忽略空串
			break;
		}
		else {
			//若follow中对应项未重复则加入
			if (find(follow[p.first].begin(), follow[p.first].end(), *it) == follow[p.first].end()) {
				follow[p.first].push_back(*it);
			}
		}
	}
}

//向table中添加元素 由addToFollow改编 follw用于 在E->AB first(A)中由空串的时候 table(E->AB) +=follow(E)
void addToTable(map<string, vector<string>>& table, map<string, vector<string>> follow, pair<string, vector<string>>addPair) {
	pair<string, string> p;
	p.first = addPair.first;
	// map特性 不需要判断是否记录过E
	table.insert({ p.first,{} });//占一个位置(初始化一下) 代码很丑但没有想到替代的办法
	for (auto it = addPair.second.begin(); it != addPair.second.end(); it++) {
		if (*it == "#") {//空串特殊处理
			// 在E->AB first(A)中由空串的时候 table(E->AB) +=follow(E)
			//此时 addPair.first 的数据 大概长这样： E->AB
			//利用获取左部的函数可以获取到
			string startCh = aPartString(addPair.first).first;//获取到E
			addToTable(table, follow, { addPair.first,follow[startCh] });
			continue;
		}
		
		//若table中对应项未重复则加入
		if (find(table[p.first].begin(), table[p.first].end(), *it) == table[p.first].end()) {
			table[p.first].push_back(*it);
		}
	//}
	}
}

//分解产生式右部 用于获取follow
//如 c*A')BB'c  ->{"A'",")","B","B'","c"}  abd->{}
vector<string> splitStringRight(string str) {
	vector<string> res;
	int left = 0;//记录当前处理进度
	for (int i = 0; str[i]; i++) {
		if (str[i] >= 'A' && str[i] <= 'Z') {
			//右侧非空
 			if (str[i + 1]) {
				if (str[i + 1] == '\'') {//' 转义字符 \'
					res.push_back(str.substr(i,2));
					i++;
				}
				//右测不是'
				else {
					res.push_back(str.substr(i, 1));
				}
			}
			//右侧为空
			else {
				res.push_back(str.substr(i, 1));
			}
		}
		 
		else if (i >= 1) {
			if (str[i - 1] == '\'' || (str[i - 1] >= 'A' && str[i - 1] <= 'Z')) {//前面为非终结符
				//遇到小写字母
				if (str[i] >= 'a' && str[i] <= 'z') {
					left = i;
					while (str[i] >= 'a' && str[i] <= 'z' && str[i])i++; //后面遇到大写字母 或结束符
					res.push_back(str.substr(left, i-left));
					i--;//i-1抵消掉循环的i++
				 }
				//其他长度为1的终结符
				else if (str[i] == '#' || str[i] == '(' || str[i] == ')' || str[i] == '[' || str[i] == ']' || str[i] == '*'
					|| str[i] == '+' || str[i] == '-' || str[i] =='$' || str[i] == ',') {
					res.push_back(str.substr(i, 1));
				}
				else {//其他未定义的字符 报错
					cout << "splitStringRight出错" << endl;
				}
			}
			else {//前面为终结符 则跳过
				continue;
			}
		}
		
	}
	return res;
}

//分解产生式右部 用于获取follow
//如 c*A')BB'c  ->{"A'",")","B","B'","c"}  abd->{abd} 由splitStringRight2转化来 用于模拟栈过程中获取产生式右部的子集
vector<string> splitStringRight2(string str) {
	vector<string> res;
	int left = 0;//记录当前处理进度
	for (int i = 0; str[i]; i++) {
		if (str[i] >= 'A' && str[i] <= 'Z') {
			//右侧非空
			if (str[i + 1]) {
				if (str[i + 1] == '\'') {//' 转义字符 \'
					res.push_back(str.substr(i, 2));
					i++;
				}
				//右测不是'
				else {
					res.push_back(str.substr(i, 1));
				}
			}
			//右侧为空
			else {
				res.push_back(str.substr(i, 1));
			}
		}

		//遇到小写字母
		else if (str[i] >= 'a' && str[i] <= 'z') {
			left = i;
			while (str[i] >= 'a' && str[i] <= 'z' && str[i])i++; //后面遇到大写字母 或结束符
			res.push_back(str.substr(left, i - left));
			i--;//i-1抵消掉循环的i++
		}
		//其他长度为1的终结符 
		else if (str[i] == '(' || str[i] == ')' || str[i] == '[' || str[i] == ']' || str[i] == '*'
			|| str[i] == '+' || str[i] == '-' || str[i] == '$' || str[i] == ',') {
			res.push_back(str.substr(i, 1));
		}
		//空值特殊处理
		else if (str[i] == '#') {
			continue;
		}
		else {//其他未定义的字符 报错
			cout << "splitStringRight2出错" << endl;
		}
	}
	return res;
}

//计算follow
void getFollow(map<string, vector<string>>text, map<string, vector<string>>first, map<string, vector<string>>& follow,string startCh) {
	//方法
	/*
	记录
	1.follow (S) += $
	2.若发现连续的终结符 AB 则follow(A)+=first(B)  (去除first(B)中的#)	
	3.S->...ACB   有follow(B)+=follow(S)
		同时：若first(B)包含# 则有follow(C)+=follow(S)  
		此基础上,若C包含#，则有follow(A)+=follow(S)  
	4.有...E).. 则将)加入follow(E) 
	5.应该可以不管重复元素，利用vector特性去重
	步骤
	1.对每一条生产式执行记录1-4 两遍
	2.所有
	*/

	//给follow(startCh)加入 $符号
	addToFollow(follow, { startCh,{"$"} });
	

	//1. 第一遍处理
	for (auto cerPair = text.begin(); cerPair != text.end();cerPair++) {//读取一个单独一类产生式（如产生式开头都为E） E A B..

		//循环遍历所有pair给p赋值  
		pair<string, string> p;
		p.first = cerPair->first;

		//if (cerPair == text.begin()) {//给第一条产生式加入 $符号
		//	addToFollow(follow, { p.first,{"$"} });
		//}
		
		//遍历到单独的一句产生式 如E->a  E->id
		for (auto it = cerPair->second.begin(); it != cerPair->second.end(); it++) {
			//将产生式右部分解  如 c*A')BB'c  ->{"A'",")","B","B'","c"}
			vector<string>tmp = splitStringRight(*it);
			//遍历分解的因子
			for (int i = 0; i<tmp.size(); i++) {
				if (tmp[i][0] <= 'A' || tmp[i][0] >= 'Z') {//终结符 则将终结符放入前一个非终结符的follow
					//此处不用担心越界问题,splitStringRight得到的string数组第一个不可能为终结符
					addToFollow(follow, { tmp[i - 1],{tmp[i]} });
				}
				else if (tmp[i][0] >= 'A' && tmp[i][0] <= 'Z') {//非终结符 
					if (i >= 1) {
						//if (tmp[i - 1][0] <= 'A' && tmp[i - 1][0] >= 'Z')continue;//前一个是终结符
						if (tmp[i - 1][0] <= 'A' || tmp[i - 1][0] >= 'Z')continue;//前一个是终结符
						else if (tmp[i - 1][0] >= 'A' && tmp[i - 1][0] <= 'Z') {//前一个是非终结符

							//AB follow(A)+=first(B)
							addToFollow(follow, { tmp[i - 1],first[tmp[i]] });

						}
					}
					if (i == tmp.size() - 1 && i >= 1) {
						//特殊情况 E->..A follow(A)+=follow(E)
						addToFollow(follow, { tmp[i],follow[p.first] });

						//E->AB 如果前一个终结符A first(B)中包含空值，则有follow(A)+=follow(E)
						if (find(first[tmp[i]].begin(), first[tmp[i]].end(), "#") != first[tmp[i]].end()) {
							addToFollow(follow, { tmp[i - 1],follow[p.first] });
						}
					}
				}

			}
		}
	}

	//第二遍编译
	for (auto cerPair = text.begin(); cerPair != text.end(); cerPair++) {//读取一个单独一类产生式（如产生式开头都为E） E->a E->b E...

		//循环遍历所有pair给p赋值  
		pair<string, string> p;
		p.first = cerPair->first;

		//if (cerPair == text.begin()) {//给第一条产生式加入 $符号
		//	addToFollow(follow, { p.first,{"$"} });
		//}

		//遍历到单独的一句产生式 如E->a  E->id
		for (auto it = cerPair->second.begin(); it != cerPair->second.end(); it++) {
			//将产生式右部分解  如 c*A')BB'c  ->{"A'",")","B","B'","c"}
			vector<string>tmp = splitStringRight(*it);
			//遍历分解的因子
			for (int i = 0; i < tmp.size(); i++) {
				if (tmp[i][0] <= 'A' || tmp[i][0] >= 'Z') {//终结符 则将终结符放入前一个非终结符的follow
					//此处不用担心越界问题,splitStringRight得到的string数组第一个不可能为终结符
					addToFollow(follow, { tmp[i - 1],{tmp[i]} });
				}
				else if (tmp[i][0] >= 'A' && tmp[i][0] <= 'Z') {//非终结符 
					if (i >= 1) {
						if (tmp[i - 1][0] <= 'A' && tmp[i - 1][0] >= 'Z')continue;//前一个是终结符
						else if (tmp[i - 1][0] >= 'A' && tmp[i - 1][0] <= 'Z') {//前一个是非终结符

							//AB follow(A)+=first(B)
							addToFollow(follow, { tmp[i - 1],first[tmp[i]] });

						}
					}
					if (i == tmp.size() - 1 && i >= 1) {
						//特殊情况 E->..A follow(A)+=follow(E)
						addToFollow(follow, { tmp[i],follow[p.first] });

						//E->AB 如果前一个终结符A first(B)中包含空值，则有follow(A)+=follow(E)
						if (find(first[tmp[i]].begin(), first[tmp[i]].end(), "#") != first[tmp[i]].end()) {
							addToFollow(follow, { tmp[i - 1],follow[p.first] });
						}
					}
				}

			}
		}
	}
}

//获取第一个字符片段 如*AB 获取 *，T'A'c获取T', idAC获取 id。
string getFirstPart(string str) {
	int i = 0;
	string res = "";
	if (str[i] >= 'A' && str[i] <= 'Z') {
		if (i+1 < str.size()) {//后面可能有'
			i++;
			if (str[i] != '\'') {//转义字符 \' = '
				i--;
			}
		}
		res = str.substr(0, i+1);
	}
	else if (str[i] >= 'a' && str[i] <= 'z') {
		while (str[i] >= 'a' && str[i] <= 'z' && str[i])i++; //后面遇到大写字母 或结束符
		res = str.substr(0, i);
	}
	//其他长度为1的终结符
	else if(str[i] == '#' || str[i] == '(' || str[i] == ')' || str[i] == '[' || str[i] == ']' || str[i] == '*'
		|| str[i] == '+'|| str[i] == '-' || str[i] == ',') {
		res = str.substr(0, 1);
	}
	else {//出现为考虑到的情况 报错
		cout << "getFirstPart()函数出错" << endl;
	}
	return res;
}

//测试函数
void test() {
	//验证getFirstPart函数
	/*cout << getFirstPart("A'B") << endl;;
	cout << getFirstPart("acbABD") << endl;
	cout << getFirstPart("acbb") << endl;
	cout << getFirstPart("ABB") << endl;
	cout << getFirstPart("A'''B") << endl;*/

	vector<string>res = splitStringRight("A')BCAabcB");
	vector<string>res2 = splitStringRight("abc");

	
}

//将读取的某一行 ，拆分为 开头 和 内容 如：E->anc 分为 {E,{anc}} E->A|a 分为{E,{A,a}}
pair<string, vector<string>> aPartString(string str) {
	int i = 0;
 	int left = 0, right = 0;//left指向左部最后一个字母,right指向右部第一个字母
	pair<string, vector<string>> inPair;
 	for (i = 0; str[i] && str[i] != '>'; i++);// >为边界
	left = i - 2;
	right = i + 1;
	inPair.first = str.substr(0, left + 1);//获取左部
	left = right;//左部后移 方柏霓进行下面的操作
	
	//切片由右部 处理 |
	for (i=right; str[i] ; i++) {
		if (str[i] == '|') {
			inPair.second.push_back(str.substr(left,i-left)) ;
			right = i + 1;
			left = right;
		}
	}
	inPair.second.push_back(str.substr(right, i));
	return inPair;
}

//pair<string,string>加入到map<string, vector<string>>类型的数据中
//需要输入&text 而非text 否则不能保存对text的修改
void insertInMap(map<string, vector<string>>&text, pair<string, string> pair) {
	//网上说如果map[key]=value 中的key未定义，则会新建一个map[key] 故不需要判断map是否已有该元素
	//经过测试确实如此
	text[pair.first].push_back(pair.second);
}

//输出读取到的text的数据
void coutTextMap(map<string, vector<string>> map) {
	for (pair<string, vector<string>> cerPair : map) {//读取一类 E->a E->b E...
		//循环遍历所有pair给p赋值  
		pair<string, string> p;
		p.first = cerPair.first;
		//循环遍历某一类 E -> a b c ...
		for (auto it = cerPair.second.begin(); it != cerPair.second.end(); it++) {
			p.second = *it;//赋值
			cout << p.first + ": " << p.second << endl;
		}
	}
}

//输出读取到的table的数据
void coutTableMap(map<string,string> map, vector<string> terminator,vector<string> startChs) {
	int width = 15;
	int toaWidth = (width+1) * (terminator.size()+1);

	//横线
	cout << endl;
	for (int i = 0; i < toaWidth; i++) {
		cout << '-';
	}
	cout << endl;

	cout.width(width);
	cout << setiosflags(ios::left);
	cout << "|预测分析表";
	for (string s : terminator) {
		cout.width(width);
		cout << setiosflags(ios::left);
		cout << '|'+ s;
	}

	//横线
	cout << endl;
	for (int i = 0; i < toaWidth; i++) {
		cout << '-';
	}
	cout << endl;

	for (string start : startChs) {//读取某一类  E E' T T'.
		//先输出开头 
		cout.width(width);
		cout << setiosflags(ios::left);
		cout << '|' + start;
		

		//循环遍历所有终结符 id * ( ...
		for (string end : terminator) {
			string tmp;
			tmp = "" + start + " " + end;
			if (map.find(tmp) != map.end()) {//存在则输出
				cout.width(width);
				cout << setiosflags(ios::left);
				cout << '|' + map[tmp];
			}
			else {//不存在则输出空值 为了格式整齐
				cout.width(width);
				cout << setiosflags(ios::left);
				cout << '|';
			}

		}


		//横线
		cout << endl;
		for (int i = 0; i < toaWidth; i++) {
			cout << '-';
		}
		cout << endl;
	
	}
}

//输出first中的数据
void coutFirstMap(map<string, vector<string>> map) {
	for (pair<string, vector<string>> cerPair : map) {//读取一类 E->a E->b E...
		//循环遍历所有pair给p赋值  
		pair<string, string> p;
		p.first = cerPair.first;
		cout << p.first + ":";
		//循环遍历某一类 E -> a b c ...
		for (auto it = cerPair.second.begin(); it != cerPair.second.end(); it++) {
			p.second = *it;//赋值
			cout << " "+ p.second;
		}
		cout << endl;
	}
}

int readFile(map<string, vector<string>>&text,string &start) {
	
	ifstream input_file("files/input.txt");
	if (!input_file) {
		cout << "false to open input.txt" << endl;
		return -1;
	}
	// 输入格式 每一个文法语句末尾接一个换行，空值用#代替
	string str;//辅助变量
	cout << "这里是readFile函数：" << endl;
	cout << "文件中的实际内容:" << endl;

	int cnt = 0;//辅助记录产生式开始符号start
	while (input_file >> str) {
		//将读取的某一行 ，拆分为 开头 和 内容 如：E->anc 分为 {E,anc}
		cout << str << endl;
		pair<string, vector<string>> getPair = aPartString(str);

		//求start
		cnt++;
		if (cnt == 1) {
			start = getPair.first;
		}

		for (string str : getPair.second) {
			insertInMap(text, { getPair.first ,str});
		}
		
	}
	cout << endl;
	return 0;
}

void getTableAndTerminator(map<string, vector<string>>text, map<string, vector<string>>first, map<string, vector<string>>follow, map<string, vector<string>>&table,vector<string>&chs ) {
	//求seletct方法
	//A->*B  select(A)=*
	//A->BC  select(A) = first(B)
	//A->#  select(A) = follow(B)

	for (pair<string, vector<string>> cerPair : text) {//读取一类 E->a E->b E...
		//循环遍历所有pair给p赋值  
		pair<string, string> p;
		p.first = "" + cerPair.first + "->";

		//循环遍历某一类 E -> a b c ...
		for (auto it = cerPair.second.begin(); it != cerPair.second.end(); it++) {
			p.second = *it;//赋值
			string head = getFirstPart(*it);//获取头部
			if (head[0] == '#') {
				addToTable(table, follow,{ "" + p.first + *it,follow[cerPair.first]});
				//记录select中的终结符
				for (auto item = follow[cerPair.first].begin(); item != follow[cerPair.first].end(); item++) {
					chs.push_back(*item);
				}
			}
			else if (head[0] >= 'A' && head[0] <= 'Z') {
				addToTable(table, follow, { "" + p.first + *it,first[head] });
				//记录select中的终结符
				for (auto item = first[head].begin(); item != first[head].end(); item++) {
					chs.push_back(*item);
				}
			}
			else if (head[0] <= 'A' || head[0] >= 'Z') {
				addToTable(table, follow, { "" + p.first + *it,{head} });
				//记录select中的终结符
				chs.push_back(head);
			}
		}
	}

	//chs去重
	set<string>s(chs.begin(), chs.end());
	chs.assign(s.begin(), s.end());
 }

void getSelect(map<string, vector<string>>table, vector<string> chs, map<string, string>&select) 
{
	for (pair<string, vector<string>> cerPair : table) {//读取一类 E->a E->b E...
		//循环遍历所有pair给p赋值  
		pair<string, string> p;
		p.first = getFirstPart(cerPair.first);
		//循环遍历某一类 E -> a b c ...
		for (auto it = cerPair.second.begin(); it != cerPair.second.end(); it++) {
			pair<string, string> tmp;
			tmp.first = "" + p.first +" "+ * it;//赋值
			tmp.second = cerPair.first;
			select.insert(tmp);
		}
	}
}
void getStartChs(map<string, vector<string>>text, vector<string>&startChs) {
	for (pair<string, vector<string>> str : text) {
		startChs.push_back(str.first);
	}

	//去重
	set<string>s(startChs.begin(), startChs.end());
	startChs.assign(s.begin(), s.end());
}


//输出select中的数据
void coutSelect(map<string, string>select) {
	for (auto it = select.begin(); it != select.end(); it++) {
		cout.width(5);
		cout << setiosflags(ios::left);
		cout << it->first << " " << it->second << endl;
	}
}

//将vector<string> 组合为string 方便输出
string joinString(vector<string> strs) {
	string res = "";
	for (string ch : strs) {
		res += ch;
	}
	return res;
}

vector<string> splitTerminator(string str) {
	vector<string> res;
	int left = 0;//记录当前处理进度
	for (int i = 0; str[i]; i++) {
		if (str[i] >= 'A' && str[i] <= 'Z') {
			cout << "splitTeminator()出错了: 出现了大写字母" << endl;
		}

		//遇到小写字母
		else if (str[i] >= 'a' && str[i] <= 'z') {
			left = i;
			while (str[i] >= 'a' && str[i] <= 'z' && str[i])i++; //后面遇到大写字母 或结束符
			res.push_back(str.substr(left, i - left));
			i--;//i-1抵消掉循环的i++
		}
		//其他长度为1的终结符
		else if (str[i] == '#' || str[i] == '(' || str[i] == ')' || str[i] == '[' || str[i] == ']' || str[i] == '*'
			|| str[i] == '+' || str[i] == '-' || str[i] == '$'|| str[i] == ',') {
			res.push_back(str.substr(i, 1));
		}
		else {//其他未定义的字符 报错
			cout << "splitStringRight出错" << endl;
		}
	}
	return res;
}

//将b中的数据逆序加入到A中 用于模拟栈的过程中
void joinVectorA(vector<string>&a, vector<string>b) {
	for (auto it = b.rbegin(); it != b.rend(); it++) {
		a.push_back(*it);
	}
}

string getStringRight(string str) {
	string res = "";
	for (int i = 0; str[i]; i++) {
		if (str[i] == '>'&& (i+1) < str.size()) {
			res = str.substr(i+1);
			return res;
		}
	}
	cout << "getStringRight 出错了" << endl;
	return res;
}

//模拟的运行过程并输出 
//此处并没有用stack 而是用vector<string>保存数据，因为更方便cout
//start为 开始符号
void getAndCoutStack(string input,string start, map<string, string>select) {
	int width = 25;
	int tol_width = (width + 1) * 3;
	vector<string> startChs;
	vector<string> terminator;

	startChs.push_back("$");
	startChs.push_back(start);//初始化保存非终结符的字符串

	input += "$";
	terminator = splitTerminator(input);//初始化输入字符串

	//横线
	for (int i = 0; i < tol_width; i++) {
		cout << '-';
	}
	cout << endl;

	cout.width(width);
	cout << setiosflags(ios::left);
	cout << "|栈";
	cout.width(width);
	cout << setiosflags(ios::left);
	cout << "|输入";
	cout.width(width);
	cout << setiosflags(ios::left);
	cout << "|动作"<<endl;

	//横线
	for (int i = 0; i < tol_width; i++) {
		cout << '-';
	}
	cout << endl;

	//第一行特殊处理(没有动作)
	cout.width(width);
	cout << setiosflags(ios::left);
	cout <<'|'+ joinString(startChs);
	cout.width(width);
	cout << setiosflags(ios::left);
	cout <<'|' + joinString(terminator);
	cout.width(width);
	cout << setiosflags(ios::left);
	cout << '|';
	cout << endl;

	//横线
	for (int i = 0; i < tol_width; i++) {
		cout << '-';
	}
	cout << endl;

	//循环模拟栈(最后生一个$)
	while (startChs.size()) {
		string lastChs = startChs[startChs.size() - 1];
		string beginTerminator = terminator[0];
		string getProString;//产生式
		if (lastChs[0] <= 'Z' && lastChs[0] >= 'A') {//非终结符
			if (select.find(lastChs + " " + beginTerminator) != select.end()) {//如果有对应的产生式
				//获取到 对应的产生式 
				string getProString = select[lastChs + " " + beginTerminator];
				/*cout << "grtProString" << endl;
				cout << getProString << endl;*/

				//非终结符更新
				startChs.erase(startChs.end()-1);//去除末尾
				string getProRight = getStringRight(getProString);//获取产生式右部
				//cout << "产生式右部： " << getProRight << endl;
				joinVectorA(startChs, splitStringRight2(getProRight));// 产生式右部分解成vector 再反向加入starChs

				//输出更新后的非终结符
				cout.width(width);
				cout << setiosflags(ios::left);
				cout << '|' + joinString(startChs);

				//输出更新后的terminate 终结符
				cout.width(width);
				cout << setiosflags(ios::left);
				cout << '|' + joinString(terminator);

				//输出说明
				cout.width(width);
				cout << setiosflags(ios::left);
				cout << "|输出 " + getProString;
				cout << endl;

				//横线
				for (int i = 0; i < tol_width; i++) {
					cout << '-';
				}
				cout << endl;
			}
			else {//输出错误信息
				cout << "出错啦：预测分析表中"+lastChs +" "+ beginTerminator+ "没有对应的值" << endl;
				return;
			}
		}
		else {//非终结符
			if (lastChs == beginTerminator) { //两个终结符对应
				//更新 startChs 和 terminator
				startChs.erase(startChs.end() - 1);//去除末尾
				terminator.erase(terminator.begin());//去除头部
				
				//输出更新后的非终结符
				cout.width(width);
				cout << setiosflags(ios::left);
				cout << '|' + joinString(startChs);

				//输出更新后的terminate 终结符
				cout.width(width);
				cout << setiosflags(ios::left);
				cout << '|' + joinString(terminator);

				//输出说明
				cout.width(width);
				cout << setiosflags(ios::left);
				cout << "|匹配 " + lastChs;
				cout << endl;

				//横线
				for (int i = 0; i < tol_width; i++) {
					cout << '-';
				}
				cout << endl;
			}
		}
	}
}

int main()
{
	map<string, vector<string>>text = {};//储存读取到的输入信息
	string startCh;//记录第一条生产时的开头，用于getFollow时添加"$"
	map<string, vector<string>>first = {};//储存first
	map<string, vector<string>>follow = {};//储存follow

	map<string, vector<string>>table = {};//预测分析表
	vector<string> terminator;//记录select表中出现的终结符
	vector<string> startChs;//记录select表中出现的非终结符
	map<string, string>select;//记录预测分析表终结符与非终结符的对应关系 如E->TE' 的select集中含有 id 则保存为{"E id","E->TE'"}
	//1 读取输入文法并保存信息
	readFile(text,startCh);

	//测试函数 
	//test();

	//1.1 输出读取到的text
	cout << "将文件中的数据储存搭到text,并且记录开始符号" << endl;
	cout << "开始符号: " << startCh << endl;
	cout << endl;
	cout << "text中的信息:" << endl;
	coutTextMap(text);
	

	//2 根据text计算first
	cout << "根据text计算first" << endl;
	getFirst(text, first);
	//2.1 输出计算后的first
	
	cout << "最终得到的first的信息:" << endl;
	coutFirstMap(first);
	cout << endl;

	//3 根据first计算follow
	cout << "根据first计算follow" << endl;
	getFollow(text,first, follow,startCh);
	//3.1 输出计算后的follow
	cout << "最终得到的follow的信息:" << endl;
	coutFirstMap(follow);
	cout << endl;

	
	//根据first follow ,text构造table 和 terminator(记录终结符)
	cout << "根据first，follow和text信息 构造table和terminator" << endl;
	getTableAndTerminator(text, first, follow, table,terminator);
	cout << "最终得到table中的数据" << endl;
	coutTextMap(table);
	cout << endl;

	//获取辅助变量 startChs
	cout << "获取辅助变量 startChs" << endl;
	cout << endl;
	getStartChs(text, startChs);

	//获取select
	cout << "获取select" << endl;
	getSelect(table, terminator, select);
	cout << "select中保存的信息" << endl;
	coutSelect(select);
	cout << endl;

	cout << "将select中的数据按预测分析表的格式输出" << endl;
	coutTableMap(select, terminator,startChs);
	cout << endl;

	cout << "根据预测分析表完成预测分析程序并输出过程" << endl;
	 
	/*cout << "输入1：" << endl;
	* cout << input1 <<endl;
	cin >> "输入1：" >> endl;
	cout << "输入1：" << endl;*/

	//案例1 课本范例
	//string input1 = "id*id+id";
	//getAndCoutStack(input1, *startChs.begin(), select);//此处startChs用于获取 程序可能出现的非终结符信息

	//案例2 习题二
	//string input2 = "(a,(a,a))";
	//getAndCoutStack(input2,startCh,select);//此处startChs用于获取 程序可能出现的非终结符信息

	//案例3 习题二
	//string input2 = "(a,(b,a))";
	//getAndCoutStack(input2, startCh, select);//此处startChs用于获取 程序可能出现的非终结符信息

	//案例4 习题二
	//string input2 = "(a,(aa,a))";
	//getAndCoutStack(input2, startCh, select);//此处startChs用于获取 程序可能出现的非终结符信息

	//案例5 习题三
	string input2 = "id*id+id";
	getAndCoutStack(input2, startCh, select);//此处startChs用于获取 程序可能出现的非终结符信息
	
	
	return 0;
}