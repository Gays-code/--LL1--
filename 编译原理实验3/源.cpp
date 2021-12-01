#include<iostream>
#include<fstream>//�ļ�����
#include<vector>//��̬����
#include<map>
#include<queue>
//#include<stack>
#include<set>//verctor ȥ��
#include <iomanip>//���������ʽ
#include <algorithm>//reverse()
using namespace std;

//��ȡ��һ���ַ�Ƭ�� ��*AB ��ȡ *��T'A'c��ȡT', idAC��ȡ id��
string getFirstPart(string str);

//����ȡ��ĳһ�� �����Ϊ ��ͷ �� ���� �磺E->anc ��Ϊ {E,{anc}} E->A|a ��Ϊ{E,{A,a}}
pair<string, vector<string>> aPartString(string str);

//����first
void getFirst(map<string, vector<string>>text, map<string, vector<string>>&first) {
	//1. ��һ��
	for (pair<string, vector<string>> cerPair : text) {//��ȡһ�� E A B. �������з��ս��
		
		//ѭ����������pair��p��ֵ  
		pair<string, string> p;
		p.first = cerPair.first;
		//ѭ������ĳһ�� E -> a b c ...  ���� ˵���ս��
		// map���� ����Ҫ�ж��Ƿ��¼��E
		first.insert({ p.first,{} });//ռһ��λ��(��ʼ��һ��) ����ܳ�û���뵽����İ취
		for (auto it = cerPair.second.begin(); it != cerPair.second.end(); it++) {
			p.second = *it;//��ֵ

			//��ȡ�ս�� ���߷��ս����first(A)=first(B)֮��ģ�
			//E -> str ,first���Ƿ��¼��str
			
			//���δ��¼p.second ���¼p.second
			if (find(first[p.first].begin(), first[p.first].end(), p.second) == first[p.first].end()) {
				first[p.first].push_back(getFirstPart(p.second));
			}
			else {//�Ѽ�¼��ɶҲ���ø�
				break;
			}
		}
	}

	//�ڶ��� ��ջ
	map<string, vector<string>>::iterator cerPair = first.begin();
	for (; cerPair != first.end();cerPair++) {//��ȡһ�� E->a E->b E...
		queue<string> q;
		//ȫ����ջ ��ɾ��first�д������
		for (auto it = cerPair->second.begin(); it != cerPair->second.end(); it++) {
			q.push(*it);
		}
		cerPair->second.clear();

		//����ջ�����ݣ���������first map
		while (!q.empty()) {
			string str = q.front();
			q.pop();
			//���ս�� A ����A' ����������Ƚϼ򵥣�ֱ��д��
			if (str[0] >= 'A' && str[0] <= 'Z') {
				//����str��Ӧ��first
				for (string tmp : first[str]) {
					//����Ƿ��и��� ���û��
					if (find(cerPair->second.begin(), cerPair->second.end(), tmp) == cerPair->second.end()) {
						/*cerPair->second.push_back(tmp);*/
						q.push(tmp);
					}
				}
			}
			else {//�ս��
				cerPair->second.push_back(str);
			}
		}
	}
}

//��follow�����Ԫ��
void addToFollow(map<string, vector<string>>& follow, pair<string, vector<string>>addPair) {\

	pair<string, string> p;
	p.first = addPair.first;
	// map���� ����Ҫ�ж��Ƿ��¼��E
	follow.insert({ p.first,{} });//ռһ��λ��(��ʼ��һ��) ����ܳ�û���뵽����İ취
	for (auto it = addPair.second.begin(); it != addPair.second.end(); it++) {
		if (*it == "#") {//���Կմ�
			break;
		}
		else {
			//��follow�ж�Ӧ��δ�ظ������
			if (find(follow[p.first].begin(), follow[p.first].end(), *it) == follow[p.first].end()) {
				follow[p.first].push_back(*it);
			}
		}
	}
}

//��table�����Ԫ�� ��addToFollow�ı� follw���� ��E->AB first(A)���ɿմ���ʱ�� table(E->AB) +=follow(E)
void addToTable(map<string, vector<string>>& table, map<string, vector<string>> follow, pair<string, vector<string>>addPair) {
	pair<string, string> p;
	p.first = addPair.first;
	// map���� ����Ҫ�ж��Ƿ��¼��E
	table.insert({ p.first,{} });//ռһ��λ��(��ʼ��һ��) ����ܳ�û���뵽����İ취
	for (auto it = addPair.second.begin(); it != addPair.second.end(); it++) {
		if (*it == "#") {//�մ����⴦��
			// ��E->AB first(A)���ɿմ���ʱ�� table(E->AB) +=follow(E)
			//��ʱ addPair.first ������ ��ų������� E->AB
			//���û�ȡ�󲿵ĺ������Ի�ȡ��
			string startCh = aPartString(addPair.first).first;//��ȡ��E
			addToTable(table, follow, { addPair.first,follow[startCh] });
			continue;
		}
		
		//��table�ж�Ӧ��δ�ظ������
		if (find(table[p.first].begin(), table[p.first].end(), *it) == table[p.first].end()) {
			table[p.first].push_back(*it);
		}
	//}
	}
}

//�ֽ����ʽ�Ҳ� ���ڻ�ȡfollow
//�� c*A')BB'c  ->{"A'",")","B","B'","c"}  abd->{}
vector<string> splitStringRight(string str) {
	vector<string> res;
	int left = 0;//��¼��ǰ�������
	for (int i = 0; str[i]; i++) {
		if (str[i] >= 'A' && str[i] <= 'Z') {
			//�Ҳ�ǿ�
 			if (str[i + 1]) {
				if (str[i + 1] == '\'') {//' ת���ַ� \'
					res.push_back(str.substr(i,2));
					i++;
				}
				//�Ҳⲻ��'
				else {
					res.push_back(str.substr(i, 1));
				}
			}
			//�Ҳ�Ϊ��
			else {
				res.push_back(str.substr(i, 1));
			}
		}
		 
		else if (i >= 1) {
			if (str[i - 1] == '\'' || (str[i - 1] >= 'A' && str[i - 1] <= 'Z')) {//ǰ��Ϊ���ս��
				//����Сд��ĸ
				if (str[i] >= 'a' && str[i] <= 'z') {
					left = i;
					while (str[i] >= 'a' && str[i] <= 'z' && str[i])i++; //����������д��ĸ �������
					res.push_back(str.substr(left, i-left));
					i--;//i-1������ѭ����i++
				 }
				//��������Ϊ1���ս��
				else if (str[i] == '#' || str[i] == '(' || str[i] == ')' || str[i] == '[' || str[i] == ']' || str[i] == '*'
					|| str[i] == '+' || str[i] == '-' || str[i] =='$' || str[i] == ',') {
					res.push_back(str.substr(i, 1));
				}
				else {//����δ������ַ� ����
					cout << "splitStringRight����" << endl;
				}
			}
			else {//ǰ��Ϊ�ս�� ������
				continue;
			}
		}
		
	}
	return res;
}

//�ֽ����ʽ�Ҳ� ���ڻ�ȡfollow
//�� c*A')BB'c  ->{"A'",")","B","B'","c"}  abd->{abd} ��splitStringRight2ת���� ����ģ��ջ�����л�ȡ����ʽ�Ҳ����Ӽ�
vector<string> splitStringRight2(string str) {
	vector<string> res;
	int left = 0;//��¼��ǰ�������
	for (int i = 0; str[i]; i++) {
		if (str[i] >= 'A' && str[i] <= 'Z') {
			//�Ҳ�ǿ�
			if (str[i + 1]) {
				if (str[i + 1] == '\'') {//' ת���ַ� \'
					res.push_back(str.substr(i, 2));
					i++;
				}
				//�Ҳⲻ��'
				else {
					res.push_back(str.substr(i, 1));
				}
			}
			//�Ҳ�Ϊ��
			else {
				res.push_back(str.substr(i, 1));
			}
		}

		//����Сд��ĸ
		else if (str[i] >= 'a' && str[i] <= 'z') {
			left = i;
			while (str[i] >= 'a' && str[i] <= 'z' && str[i])i++; //����������д��ĸ �������
			res.push_back(str.substr(left, i - left));
			i--;//i-1������ѭ����i++
		}
		//��������Ϊ1���ս�� 
		else if (str[i] == '(' || str[i] == ')' || str[i] == '[' || str[i] == ']' || str[i] == '*'
			|| str[i] == '+' || str[i] == '-' || str[i] == '$' || str[i] == ',') {
			res.push_back(str.substr(i, 1));
		}
		//��ֵ���⴦��
		else if (str[i] == '#') {
			continue;
		}
		else {//����δ������ַ� ����
			cout << "splitStringRight2����" << endl;
		}
	}
	return res;
}

//����follow
void getFollow(map<string, vector<string>>text, map<string, vector<string>>first, map<string, vector<string>>& follow,string startCh) {
	//����
	/*
	��¼
	1.follow (S) += $
	2.�������������ս�� AB ��follow(A)+=first(B)  (ȥ��first(B)�е�#)	
	3.S->...ACB   ��follow(B)+=follow(S)
		ͬʱ����first(B)����# ����follow(C)+=follow(S)  
		�˻�����,��C����#������follow(A)+=follow(S)  
	4.��...E).. ��)����follow(E) 
	5.Ӧ�ÿ��Բ����ظ�Ԫ�أ�����vector����ȥ��
	����
	1.��ÿһ������ʽִ�м�¼1-4 ����
	2.����
	*/

	//��follow(startCh)���� $����
	addToFollow(follow, { startCh,{"$"} });
	

	//1. ��һ�鴦��
	for (auto cerPair = text.begin(); cerPair != text.end();cerPair++) {//��ȡһ������һ�����ʽ�������ʽ��ͷ��ΪE�� E A B..

		//ѭ����������pair��p��ֵ  
		pair<string, string> p;
		p.first = cerPair->first;

		//if (cerPair == text.begin()) {//����һ������ʽ���� $����
		//	addToFollow(follow, { p.first,{"$"} });
		//}
		
		//������������һ�����ʽ ��E->a  E->id
		for (auto it = cerPair->second.begin(); it != cerPair->second.end(); it++) {
			//������ʽ�Ҳ��ֽ�  �� c*A')BB'c  ->{"A'",")","B","B'","c"}
			vector<string>tmp = splitStringRight(*it);
			//�����ֽ������
			for (int i = 0; i<tmp.size(); i++) {
				if (tmp[i][0] <= 'A' || tmp[i][0] >= 'Z') {//�ս�� ���ս������ǰһ�����ս����follow
					//�˴����õ���Խ������,splitStringRight�õ���string�����һ��������Ϊ�ս��
					addToFollow(follow, { tmp[i - 1],{tmp[i]} });
				}
				else if (tmp[i][0] >= 'A' && tmp[i][0] <= 'Z') {//���ս�� 
					if (i >= 1) {
						//if (tmp[i - 1][0] <= 'A' && tmp[i - 1][0] >= 'Z')continue;//ǰһ�����ս��
						if (tmp[i - 1][0] <= 'A' || tmp[i - 1][0] >= 'Z')continue;//ǰһ�����ս��
						else if (tmp[i - 1][0] >= 'A' && tmp[i - 1][0] <= 'Z') {//ǰһ���Ƿ��ս��

							//AB follow(A)+=first(B)
							addToFollow(follow, { tmp[i - 1],first[tmp[i]] });

						}
					}
					if (i == tmp.size() - 1 && i >= 1) {
						//������� E->..A follow(A)+=follow(E)
						addToFollow(follow, { tmp[i],follow[p.first] });

						//E->AB ���ǰһ���ս��A first(B)�а�����ֵ������follow(A)+=follow(E)
						if (find(first[tmp[i]].begin(), first[tmp[i]].end(), "#") != first[tmp[i]].end()) {
							addToFollow(follow, { tmp[i - 1],follow[p.first] });
						}
					}
				}

			}
		}
	}

	//�ڶ������
	for (auto cerPair = text.begin(); cerPair != text.end(); cerPair++) {//��ȡһ������һ�����ʽ�������ʽ��ͷ��ΪE�� E->a E->b E...

		//ѭ����������pair��p��ֵ  
		pair<string, string> p;
		p.first = cerPair->first;

		//if (cerPair == text.begin()) {//����һ������ʽ���� $����
		//	addToFollow(follow, { p.first,{"$"} });
		//}

		//������������һ�����ʽ ��E->a  E->id
		for (auto it = cerPair->second.begin(); it != cerPair->second.end(); it++) {
			//������ʽ�Ҳ��ֽ�  �� c*A')BB'c  ->{"A'",")","B","B'","c"}
			vector<string>tmp = splitStringRight(*it);
			//�����ֽ������
			for (int i = 0; i < tmp.size(); i++) {
				if (tmp[i][0] <= 'A' || tmp[i][0] >= 'Z') {//�ս�� ���ս������ǰһ�����ս����follow
					//�˴����õ���Խ������,splitStringRight�õ���string�����һ��������Ϊ�ս��
					addToFollow(follow, { tmp[i - 1],{tmp[i]} });
				}
				else if (tmp[i][0] >= 'A' && tmp[i][0] <= 'Z') {//���ս�� 
					if (i >= 1) {
						if (tmp[i - 1][0] <= 'A' && tmp[i - 1][0] >= 'Z')continue;//ǰһ�����ս��
						else if (tmp[i - 1][0] >= 'A' && tmp[i - 1][0] <= 'Z') {//ǰһ���Ƿ��ս��

							//AB follow(A)+=first(B)
							addToFollow(follow, { tmp[i - 1],first[tmp[i]] });

						}
					}
					if (i == tmp.size() - 1 && i >= 1) {
						//������� E->..A follow(A)+=follow(E)
						addToFollow(follow, { tmp[i],follow[p.first] });

						//E->AB ���ǰһ���ս��A first(B)�а�����ֵ������follow(A)+=follow(E)
						if (find(first[tmp[i]].begin(), first[tmp[i]].end(), "#") != first[tmp[i]].end()) {
							addToFollow(follow, { tmp[i - 1],follow[p.first] });
						}
					}
				}

			}
		}
	}
}

//��ȡ��һ���ַ�Ƭ�� ��*AB ��ȡ *��T'A'c��ȡT', idAC��ȡ id��
string getFirstPart(string str) {
	int i = 0;
	string res = "";
	if (str[i] >= 'A' && str[i] <= 'Z') {
		if (i+1 < str.size()) {//���������'
			i++;
			if (str[i] != '\'') {//ת���ַ� \' = '
				i--;
			}
		}
		res = str.substr(0, i+1);
	}
	else if (str[i] >= 'a' && str[i] <= 'z') {
		while (str[i] >= 'a' && str[i] <= 'z' && str[i])i++; //����������д��ĸ �������
		res = str.substr(0, i);
	}
	//��������Ϊ1���ս��
	else if(str[i] == '#' || str[i] == '(' || str[i] == ')' || str[i] == '[' || str[i] == ']' || str[i] == '*'
		|| str[i] == '+'|| str[i] == '-' || str[i] == ',') {
		res = str.substr(0, 1);
	}
	else {//����Ϊ���ǵ������ ����
		cout << "getFirstPart()��������" << endl;
	}
	return res;
}

//���Ժ���
void test() {
	//��֤getFirstPart����
	/*cout << getFirstPart("A'B") << endl;;
	cout << getFirstPart("acbABD") << endl;
	cout << getFirstPart("acbb") << endl;
	cout << getFirstPart("ABB") << endl;
	cout << getFirstPart("A'''B") << endl;*/

	vector<string>res = splitStringRight("A')BCAabcB");
	vector<string>res2 = splitStringRight("abc");

	
}

//����ȡ��ĳһ�� �����Ϊ ��ͷ �� ���� �磺E->anc ��Ϊ {E,{anc}} E->A|a ��Ϊ{E,{A,a}}
pair<string, vector<string>> aPartString(string str) {
	int i = 0;
 	int left = 0, right = 0;//leftָ�������һ����ĸ,rightָ���Ҳ���һ����ĸ
	pair<string, vector<string>> inPair;
 	for (i = 0; str[i] && str[i] != '>'; i++);// >Ϊ�߽�
	left = i - 2;
	right = i + 1;
	inPair.first = str.substr(0, left + 1);//��ȡ��
	left = right;//�󲿺��� �����޽�������Ĳ���
	
	//��Ƭ���Ҳ� ���� |
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

//pair<string,string>���뵽map<string, vector<string>>���͵�������
//��Ҫ����&text ����text �����ܱ����text���޸�
void insertInMap(map<string, vector<string>>&text, pair<string, string> pair) {
	//����˵���map[key]=value �е�keyδ���壬����½�һ��map[key] �ʲ���Ҫ�ж�map�Ƿ����и�Ԫ��
	//��������ȷʵ���
	text[pair.first].push_back(pair.second);
}

//�����ȡ����text������
void coutTextMap(map<string, vector<string>> map) {
	for (pair<string, vector<string>> cerPair : map) {//��ȡһ�� E->a E->b E...
		//ѭ����������pair��p��ֵ  
		pair<string, string> p;
		p.first = cerPair.first;
		//ѭ������ĳһ�� E -> a b c ...
		for (auto it = cerPair.second.begin(); it != cerPair.second.end(); it++) {
			p.second = *it;//��ֵ
			cout << p.first + ": " << p.second << endl;
		}
	}
}

//�����ȡ����table������
void coutTableMap(map<string,string> map, vector<string> terminator,vector<string> startChs) {
	int width = 15;
	int toaWidth = (width+1) * (terminator.size()+1);

	//����
	cout << endl;
	for (int i = 0; i < toaWidth; i++) {
		cout << '-';
	}
	cout << endl;

	cout.width(width);
	cout << setiosflags(ios::left);
	cout << "|Ԥ�������";
	for (string s : terminator) {
		cout.width(width);
		cout << setiosflags(ios::left);
		cout << '|'+ s;
	}

	//����
	cout << endl;
	for (int i = 0; i < toaWidth; i++) {
		cout << '-';
	}
	cout << endl;

	for (string start : startChs) {//��ȡĳһ��  E E' T T'.
		//�������ͷ 
		cout.width(width);
		cout << setiosflags(ios::left);
		cout << '|' + start;
		

		//ѭ�����������ս�� id * ( ...
		for (string end : terminator) {
			string tmp;
			tmp = "" + start + " " + end;
			if (map.find(tmp) != map.end()) {//���������
				cout.width(width);
				cout << setiosflags(ios::left);
				cout << '|' + map[tmp];
			}
			else {//�������������ֵ Ϊ�˸�ʽ����
				cout.width(width);
				cout << setiosflags(ios::left);
				cout << '|';
			}

		}


		//����
		cout << endl;
		for (int i = 0; i < toaWidth; i++) {
			cout << '-';
		}
		cout << endl;
	
	}
}

//���first�е�����
void coutFirstMap(map<string, vector<string>> map) {
	for (pair<string, vector<string>> cerPair : map) {//��ȡһ�� E->a E->b E...
		//ѭ����������pair��p��ֵ  
		pair<string, string> p;
		p.first = cerPair.first;
		cout << p.first + ":";
		//ѭ������ĳһ�� E -> a b c ...
		for (auto it = cerPair.second.begin(); it != cerPair.second.end(); it++) {
			p.second = *it;//��ֵ
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
	// �����ʽ ÿһ���ķ����ĩβ��һ�����У���ֵ��#����
	string str;//��������
	cout << "������readFile������" << endl;
	cout << "�ļ��е�ʵ������:" << endl;

	int cnt = 0;//������¼����ʽ��ʼ����start
	while (input_file >> str) {
		//����ȡ��ĳһ�� �����Ϊ ��ͷ �� ���� �磺E->anc ��Ϊ {E,anc}
		cout << str << endl;
		pair<string, vector<string>> getPair = aPartString(str);

		//��start
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
	//��seletct����
	//A->*B  select(A)=*
	//A->BC  select(A) = first(B)
	//A->#  select(A) = follow(B)

	for (pair<string, vector<string>> cerPair : text) {//��ȡһ�� E->a E->b E...
		//ѭ����������pair��p��ֵ  
		pair<string, string> p;
		p.first = "" + cerPair.first + "->";

		//ѭ������ĳһ�� E -> a b c ...
		for (auto it = cerPair.second.begin(); it != cerPair.second.end(); it++) {
			p.second = *it;//��ֵ
			string head = getFirstPart(*it);//��ȡͷ��
			if (head[0] == '#') {
				addToTable(table, follow,{ "" + p.first + *it,follow[cerPair.first]});
				//��¼select�е��ս��
				for (auto item = follow[cerPair.first].begin(); item != follow[cerPair.first].end(); item++) {
					chs.push_back(*item);
				}
			}
			else if (head[0] >= 'A' && head[0] <= 'Z') {
				addToTable(table, follow, { "" + p.first + *it,first[head] });
				//��¼select�е��ս��
				for (auto item = first[head].begin(); item != first[head].end(); item++) {
					chs.push_back(*item);
				}
			}
			else if (head[0] <= 'A' || head[0] >= 'Z') {
				addToTable(table, follow, { "" + p.first + *it,{head} });
				//��¼select�е��ս��
				chs.push_back(head);
			}
		}
	}

	//chsȥ��
	set<string>s(chs.begin(), chs.end());
	chs.assign(s.begin(), s.end());
 }

void getSelect(map<string, vector<string>>table, vector<string> chs, map<string, string>&select) 
{
	for (pair<string, vector<string>> cerPair : table) {//��ȡһ�� E->a E->b E...
		//ѭ����������pair��p��ֵ  
		pair<string, string> p;
		p.first = getFirstPart(cerPair.first);
		//ѭ������ĳһ�� E -> a b c ...
		for (auto it = cerPair.second.begin(); it != cerPair.second.end(); it++) {
			pair<string, string> tmp;
			tmp.first = "" + p.first +" "+ * it;//��ֵ
			tmp.second = cerPair.first;
			select.insert(tmp);
		}
	}
}
void getStartChs(map<string, vector<string>>text, vector<string>&startChs) {
	for (pair<string, vector<string>> str : text) {
		startChs.push_back(str.first);
	}

	//ȥ��
	set<string>s(startChs.begin(), startChs.end());
	startChs.assign(s.begin(), s.end());
}


//���select�е�����
void coutSelect(map<string, string>select) {
	for (auto it = select.begin(); it != select.end(); it++) {
		cout.width(5);
		cout << setiosflags(ios::left);
		cout << it->first << " " << it->second << endl;
	}
}

//��vector<string> ���Ϊstring �������
string joinString(vector<string> strs) {
	string res = "";
	for (string ch : strs) {
		res += ch;
	}
	return res;
}

vector<string> splitTerminator(string str) {
	vector<string> res;
	int left = 0;//��¼��ǰ�������
	for (int i = 0; str[i]; i++) {
		if (str[i] >= 'A' && str[i] <= 'Z') {
			cout << "splitTeminator()������: �����˴�д��ĸ" << endl;
		}

		//����Сд��ĸ
		else if (str[i] >= 'a' && str[i] <= 'z') {
			left = i;
			while (str[i] >= 'a' && str[i] <= 'z' && str[i])i++; //����������д��ĸ �������
			res.push_back(str.substr(left, i - left));
			i--;//i-1������ѭ����i++
		}
		//��������Ϊ1���ս��
		else if (str[i] == '#' || str[i] == '(' || str[i] == ')' || str[i] == '[' || str[i] == ']' || str[i] == '*'
			|| str[i] == '+' || str[i] == '-' || str[i] == '$'|| str[i] == ',') {
			res.push_back(str.substr(i, 1));
		}
		else {//����δ������ַ� ����
			cout << "splitStringRight����" << endl;
		}
	}
	return res;
}

//��b�е�����������뵽A�� ����ģ��ջ�Ĺ�����
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
	cout << "getStringRight ������" << endl;
	return res;
}

//ģ������й��̲���� 
//�˴���û����stack ������vector<string>�������ݣ���Ϊ������cout
//startΪ ��ʼ����
void getAndCoutStack(string input,string start, map<string, string>select) {
	int width = 25;
	int tol_width = (width + 1) * 3;
	vector<string> startChs;
	vector<string> terminator;

	startChs.push_back("$");
	startChs.push_back(start);//��ʼ��������ս�����ַ���

	input += "$";
	terminator = splitTerminator(input);//��ʼ�������ַ���

	//����
	for (int i = 0; i < tol_width; i++) {
		cout << '-';
	}
	cout << endl;

	cout.width(width);
	cout << setiosflags(ios::left);
	cout << "|ջ";
	cout.width(width);
	cout << setiosflags(ios::left);
	cout << "|����";
	cout.width(width);
	cout << setiosflags(ios::left);
	cout << "|����"<<endl;

	//����
	for (int i = 0; i < tol_width; i++) {
		cout << '-';
	}
	cout << endl;

	//��һ�����⴦��(û�ж���)
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

	//����
	for (int i = 0; i < tol_width; i++) {
		cout << '-';
	}
	cout << endl;

	//ѭ��ģ��ջ(�����һ��$)
	while (startChs.size()) {
		string lastChs = startChs[startChs.size() - 1];
		string beginTerminator = terminator[0];
		string getProString;//����ʽ
		if (lastChs[0] <= 'Z' && lastChs[0] >= 'A') {//���ս��
			if (select.find(lastChs + " " + beginTerminator) != select.end()) {//����ж�Ӧ�Ĳ���ʽ
				//��ȡ�� ��Ӧ�Ĳ���ʽ 
				string getProString = select[lastChs + " " + beginTerminator];
				/*cout << "grtProString" << endl;
				cout << getProString << endl;*/

				//���ս������
				startChs.erase(startChs.end()-1);//ȥ��ĩβ
				string getProRight = getStringRight(getProString);//��ȡ����ʽ�Ҳ�
				//cout << "����ʽ�Ҳ��� " << getProRight << endl;
				joinVectorA(startChs, splitStringRight2(getProRight));// ����ʽ�Ҳ��ֽ��vector �ٷ������starChs

				//������º�ķ��ս��
				cout.width(width);
				cout << setiosflags(ios::left);
				cout << '|' + joinString(startChs);

				//������º��terminate �ս��
				cout.width(width);
				cout << setiosflags(ios::left);
				cout << '|' + joinString(terminator);

				//���˵��
				cout.width(width);
				cout << setiosflags(ios::left);
				cout << "|��� " + getProString;
				cout << endl;

				//����
				for (int i = 0; i < tol_width; i++) {
					cout << '-';
				}
				cout << endl;
			}
			else {//���������Ϣ
				cout << "��������Ԥ���������"+lastChs +" "+ beginTerminator+ "û�ж�Ӧ��ֵ" << endl;
				return;
			}
		}
		else {//���ս��
			if (lastChs == beginTerminator) { //�����ս����Ӧ
				//���� startChs �� terminator
				startChs.erase(startChs.end() - 1);//ȥ��ĩβ
				terminator.erase(terminator.begin());//ȥ��ͷ��
				
				//������º�ķ��ս��
				cout.width(width);
				cout << setiosflags(ios::left);
				cout << '|' + joinString(startChs);

				//������º��terminate �ս��
				cout.width(width);
				cout << setiosflags(ios::left);
				cout << '|' + joinString(terminator);

				//���˵��
				cout.width(width);
				cout << setiosflags(ios::left);
				cout << "|ƥ�� " + lastChs;
				cout << endl;

				//����
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
	map<string, vector<string>>text = {};//�����ȡ����������Ϣ
	string startCh;//��¼��һ������ʱ�Ŀ�ͷ������getFollowʱ���"$"
	map<string, vector<string>>first = {};//����first
	map<string, vector<string>>follow = {};//����follow

	map<string, vector<string>>table = {};//Ԥ�������
	vector<string> terminator;//��¼select���г��ֵ��ս��
	vector<string> startChs;//��¼select���г��ֵķ��ս��
	map<string, string>select;//��¼Ԥ��������ս������ս���Ķ�Ӧ��ϵ ��E->TE' ��select���к��� id �򱣴�Ϊ{"E id","E->TE'"}
	//1 ��ȡ�����ķ���������Ϣ
	readFile(text,startCh);

	//���Ժ��� 
	//test();

	//1.1 �����ȡ����text
	cout << "���ļ��е����ݴ���text,���Ҽ�¼��ʼ����" << endl;
	cout << "��ʼ����: " << startCh << endl;
	cout << endl;
	cout << "text�е���Ϣ:" << endl;
	coutTextMap(text);
	

	//2 ����text����first
	cout << "����text����first" << endl;
	getFirst(text, first);
	//2.1 ���������first
	
	cout << "���յõ���first����Ϣ:" << endl;
	coutFirstMap(first);
	cout << endl;

	//3 ����first����follow
	cout << "����first����follow" << endl;
	getFollow(text,first, follow,startCh);
	//3.1 ���������follow
	cout << "���յõ���follow����Ϣ:" << endl;
	coutFirstMap(follow);
	cout << endl;

	
	//����first follow ,text����table �� terminator(��¼�ս��)
	cout << "����first��follow��text��Ϣ ����table��terminator" << endl;
	getTableAndTerminator(text, first, follow, table,terminator);
	cout << "���յõ�table�е�����" << endl;
	coutTextMap(table);
	cout << endl;

	//��ȡ�������� startChs
	cout << "��ȡ�������� startChs" << endl;
	cout << endl;
	getStartChs(text, startChs);

	//��ȡselect
	cout << "��ȡselect" << endl;
	getSelect(table, terminator, select);
	cout << "select�б������Ϣ" << endl;
	coutSelect(select);
	cout << endl;

	cout << "��select�е����ݰ�Ԥ�������ĸ�ʽ���" << endl;
	coutTableMap(select, terminator,startChs);
	cout << endl;

	cout << "����Ԥ����������Ԥ����������������" << endl;
	 
	/*cout << "����1��" << endl;
	* cout << input1 <<endl;
	cin >> "����1��" >> endl;
	cout << "����1��" << endl;*/

	//����1 �α�����
	//string input1 = "id*id+id";
	//getAndCoutStack(input1, *startChs.begin(), select);//�˴�startChs���ڻ�ȡ ������ܳ��ֵķ��ս����Ϣ

	//����2 ϰ���
	//string input2 = "(a,(a,a))";
	//getAndCoutStack(input2,startCh,select);//�˴�startChs���ڻ�ȡ ������ܳ��ֵķ��ս����Ϣ

	//����3 ϰ���
	//string input2 = "(a,(b,a))";
	//getAndCoutStack(input2, startCh, select);//�˴�startChs���ڻ�ȡ ������ܳ��ֵķ��ս����Ϣ

	//����4 ϰ���
	//string input2 = "(a,(aa,a))";
	//getAndCoutStack(input2, startCh, select);//�˴�startChs���ڻ�ȡ ������ܳ��ֵķ��ս����Ϣ

	//����5 ϰ����
	string input2 = "id*id+id";
	getAndCoutStack(input2, startCh, select);//�˴�startChs���ڻ�ȡ ������ܳ��ֵķ��ս����Ϣ
	
	
	return 0;
}