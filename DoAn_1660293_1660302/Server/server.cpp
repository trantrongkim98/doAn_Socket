#include <iostream>
#include <winsock2.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include<fstream>
#include<vector>
#include <process.h>
#include<exception>
#include<conio.h>
//#include<mutex>
#pragma comment (lib, "Ws2_32.lib")
using namespace std;
#define SERVER_ADDR "127.0.0.1"
#define PORT 23398
#define BUFF_SIZE 2048
#define MAX_CLIENT 1024
// struct g?m đ?a ch? client, s? l?n như?ng quy?n c?n l?i và đ? ưu tiên
struct ArrClient {
	SOCKET addsocket;
	int solannhuongquyen;
	int priv;
	string acc;
};
// c? báo hi?u game đang b?t đ?u
static bool gamestate;
//vector client đ? k?t n?i
static vector<ArrClient> clientgame;
// vector các client đang chơ đ? vào game khi game trư?c chưa xong
static vector<ArrClient> clientwait;
// struct câu h?i g?m câu h?i các tu? ch?n và đáp án
struct Question {
	string ques;
	string option;
	string answer;
};
// hàm tr? v? vector t?p các câu h?i tu? tr?n và đáp án
vector<Question> readFile(string filename)
{

	vector<Question> question;// vector ch?a t?p câu h?i 
	ifstream read(filename);// đ?c file
	string line;// save date one line in file
	string temp[3];//save question, option and answer
	while (getline(read, line))// take data one line from file to string line
	{
		int i = 0;// 
		istringstream iss(line);
		while (getline(iss, temp[i], ';'))i += 1;// classify question, option and answer
		Question a;// struct quetion contain question,option and answer
		a.ques = temp[0];
		a.option = temp[1];
		a.answer = temp[2];
		question.push_back(a);// push to vector
	}
	return question;
}
void swap(ArrClient &a, ArrClient &b)
{
	ArrClient tmp = a;
	a = b;
	b = a;
}
/*
??? có khi nào cleint sai k.. cái hàm này tao xuất đã xong àm nó không xuất là có vần đè gì đó ở đây
mà khoan để t xem
*/
static void sortClient()
{
	for (int i = 0; i < clientgame.size() - 1; i++)
	{
		for (int j = i + 1; j < clientgame.size(); j++)
		{
			if (clientgame[i].priv > clientgame[j].priv)
				swap(clientgame[i], clientgame[j]);
		}
	}
}
void changeUp()
{
	for (int i = 1; i < clientgame.size(); i++)
		clientgame[i].priv = clientgame[i - 1].priv;
	clientgame[0].priv = clientgame.size();
	clientgame[0].solannhuongquyen--;
	if (clientgame[0].solannhuongquyen < 0)
	{
		clientwait.push_back(clientgame[0]);//add waiting
		clientgame.erase(clientgame.begin());//delete 
	}
	else
		sortClient();

}
// hàm ch?y x? l? sever
struct Accout
{
	vector<string> accout;
};
static Accout ac;

bool subtr(Accout ac, string a)
{
	for (int i = 0; i < ac.accout.size(); i++)
	{
		if (a == ac.accout[i])return true;
	}
	return false;
}

//find priv min(hight)
int findMin(vector<ArrClient> clientgame)
{
	int pos = 0;
	int min = 10000;
	for (int i = 0; i < clientgame.size(); i++)
	{
		if (clientgame[i].priv < min)
		{
			min = clientgame[i].priv;
			pos = i;
		}
	}
	return pos;
}

static int counter = 0;
bool addUserSocket(string user, SOCKET sk)
{
	for (int i = 0; i < clientgame.size(); i++)
	{
		if (clientgame[i].addsocket == sk)
		{
			clientgame[i].acc = user;
			return true;
		}
	}
	return false;
}
unsigned _stdcall echoThread(void *param)
{
	char senbuffer[1024];  // thông tin g?i
	char mailbuffer[1024];// thông tin nh?n
	string mail;
	while (1)
	{
		string thongtingui;
		char senbuffer[1024];  // thông tin g?i
		char mailbuffer[1024];// thông tin nh?n
		string mail;
		//moving wating to game
		if (clientwait.size() > 0)
		{
			for (int p = 0; p < clientwait.size(); p++)
				clientgame.push_back(clientwait[p]);
			while (clientwait.size() != 0)
				clientwait.erase(clientwait.begin());
		}
		SOCKET client = (SOCKET)param;
		recv(client, mailbuffer, sizeof(mailbuffer), 0);
		mail.assign(mailbuffer);

		// ph?n ki?m tra accout
		//check correct character
		if (subtr(ac, mail) == true)
		{
			do {
				thongtingui = "Tai khoan da ton tai, nhap lai!";
				memset(senbuffer, 0, sizeof(senbuffer));
				strcpy(senbuffer, thongtingui.c_str());
				send(client, senbuffer, sizeof(senbuffer), 0);// accout đ? t?n t?i

				memset(mailbuffer, 0, sizeof(mailbuffer));
				recv(client, mailbuffer, sizeof(mailbuffer), 0);
				mail.assign(mailbuffer);// chuy?n qua chu?i string
			} while (subtr(ac, mail) == true);
			ac.accout.push_back(mail);
			string succ = "success";
			memset(senbuffer, 0, sizeof(senbuffer));
			strcpy(senbuffer, succ.c_str());
			send(client, senbuffer, sizeof(senbuffer), 0);
			//set account
			//counter = clientgame.size();//update counter
			addUserSocket(mail, client);
			counter++;

		}
		else
		{
			ac.accout.push_back(mail);
			string succ = "success";//sửa cái thông báo này
			memset(senbuffer, 0, sizeof(senbuffer));
			strcpy(senbuffer, succ.c_str());
			send(client, senbuffer, sizeof(senbuffer), 0);

			addUserSocket(mail, client);
			counter++;
		}
		//sent info
		while (1)
		{

			if (kbhit())//b?t đ?u chơi
			{
				char k = getch();
				if (k == 13)//enter
				{
					memset(mailbuffer, 0, sizeof(mailbuffer));
					// lắng nghe cái gì ở đây vậy? rồi ok
					recv(client, mailbuffer, sizeof(mailbuffer), 0);
					mail.assign(mailbuffer);
					if (mail.compare("not") == 0)
					{
						for (int f = 0; f < clientgame.size(); f++)
						{
							if (clientgame[f].addsocket == client)
							{
								//  xoá rồi mà m xoá bằng niềm tin à?? :D :D
								closesocket(clientgame[f].addsocket);
								clientgame.erase(clientgame.begin() + f);
								break;
							}
						}
					}
					if (clientgame.size() < 1)exit(0);
					//để cho người ta ngủ
					sortClient();
					for (int i = 0; i < clientgame.size(); i++)
					{
						string s = "start";
						memset(senbuffer, 0, sizeof(senbuffer));
						strcpy(senbuffer, s.c_str());
						send(clientgame[i].addsocket, senbuffer, sizeof(senbuffer), 0);
						//memset(mailbuffer, 0, sizeof(mailbuffer));
						//recv(client, mailbuffer, sizeof(mailbuffer), 0);
						//gửi thông tin, m sẽ nhận số
						string c = to_string(clientgame.size());
						c = c + ";" + to_string(clientgame[i].priv) + ";50";
						memset(senbuffer, 0, sizeof(senbuffer));
						strcpy(senbuffer, c.c_str());
						send(clientgame[i].addsocket, senbuffer, sizeof(senbuffer), 0);
					}
					break;
				}
			}
		}

		vector<Question> ques = readFile("Question_English.txt");
		cout << ques.size() << endl;
		//recv ok
		//memset(mailbuffer, 0, sizeof(mailbuffer));
		//recv(client, mailbuffer, sizeof(mailbuffer), 0);
		for (int i = 0; i < ques.size(); i++)
		{
			//sent question and option
			memset(senbuffer, 0, sizeof(senbuffer));
			string c = ques[i].ques;
			c = c + ";" + ques[i].option;
			strcpy(senbuffer, c.c_str());
			send(clientgame[0].addsocket, senbuffer, sizeof(senbuffer), 0);
		nghe:
			//receive aswer
			recv(client, mailbuffer, sizeof(mailbuffer), 0);
			string asw;
			asw.assign(mailbuffer);

			if (asw.compare(ques[i].answer) == 0)
			{
				c = "correct";
				memset(senbuffer, 0, sizeof(senbuffer));
				strcpy(senbuffer, c.c_str());
				send(clientgame[0].addsocket, senbuffer, sizeof(senbuffer), 0);
			}
			else if (asw.compare("next") == 0)
			{
				if (clientgame[0].priv > 0)
				{
					c = "agree";
					memset(senbuffer, 0, sizeof(senbuffer));
					strcpy(senbuffer, c.c_str());
					send(clientgame[0].addsocket, senbuffer, sizeof(senbuffer), 0);
					changeUp();
				}
				else
				{
					c = "erro";
					memset(senbuffer, 0, sizeof(senbuffer));
					strcpy(senbuffer, c.c_str());
					send(clientgame[0].addsocket, senbuffer, sizeof(senbuffer), 0);
					memset(mailbuffer, 0, sizeof(mailbuffer));
					goto nghe;
				}

			}
			else
			{
				c = "lose";
				memset(senbuffer, 0, sizeof(senbuffer));
				strcpy(senbuffer, c.c_str());
				send(clientgame[0].addsocket, senbuffer, sizeof(senbuffer), 0);
				//clientgame[0].priv -=1;
				//changeUp();
			}

			if (i == 49)
			{
				mail = "You win, ";
				mail += clientgame[0].acc;
				memset(senbuffer, 0, sizeof(senbuffer));
				strcpy(senbuffer, mail.c_str());
				send(clientgame[0].addsocket, senbuffer, sizeof(senbuffer), 0);
				for (int i = 1; i < clientgame.size(); i++)
				{
					mail = "Winner is: ";
					mail += clientgame[0].acc;
					memset(senbuffer, 0, sizeof(senbuffer));
					strcpy(senbuffer, mail.c_str());
					send(clientgame[i].addsocket, senbuffer, sizeof(senbuffer), 0);
				}
			}
		}
		cout << "\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Het<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
		cout << "Server muon bat dau tro choi moi(1) khong(2)?" << endl;
		int flag;
		cin >> flag;
		if (flag == 0)
			break;
		for (int i = 0; i < clientgame.size(); i++)
		{
			mail = "prepare";
			memset(senbuffer, 0, sizeof(senbuffer));
			strcpy(senbuffer, mail.c_str());
			send(clientgame[i].addsocket, senbuffer, sizeof(senbuffer), 0);
		}
	}
	//closesocket(client);
	for (int i = 0; i < clientgame.size(); i++)
	{
		mail = "not";
		memset(senbuffer, 0, sizeof(senbuffer));
		strcpy(senbuffer, mail.c_str());
		send(clientgame[i].addsocket, senbuffer, sizeof(senbuffer), 0);
		closesocket(clientgame[i].addsocket);
	}
	for (int i = 0; i < clientwait.size(); i++)
	{
		mail = "sorry";
		memset(senbuffer, 0, sizeof(senbuffer));
		strcpy(senbuffer, mail.c_str());
		send(clientgame[i].addsocket, senbuffer, sizeof(senbuffer), 0);
		closesocket(clientwait[i].addsocket);
	}
	cout << "Client disconnected." << endl;
	return 0;
}
int main()
{
	//Step 1: kh?i t?o Winsocket
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData))
		printf("Version is not supported\n");
	//Step 2: Construct socket	
	SOCKET listenSock;
	listenSock = socket(AF_INET, SOCK_STREAM, 0/*IPPROTO_TCP*/);

	//Step 3: Bind address to socket
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	bind(listenSock, (sockaddr*)&serverAddr, sizeof(serverAddr));

	if (bind(listenSock, (sockaddr*)&serverAddr, sizeof(serverAddr))==0)
	{
		cout << "fail!!";
		return 0;
	}
	//Step 4: Listen request from client
	if (listen(listenSock, 10)) {
		printf("Error! Cannot listen.");
		system("pause");
		return 0;
	}

	printf("Server started!\n");
	SOCKET connSocket;
	sockaddr_in clientAddr;
	int clientAddrLen = sizeof(clientAddr);
	int i = 0;
	gamestate = false;
	while (1)
	{
		if (gamestate == false)
		{
			connSocket = accept(listenSock, (sockaddr *)&clientAddr, &clientAddrLen);//l?ng nghe k?t n?i m?i v?i client
			printf("connSocket: %d", connSocket);
			ArrClient a;
			a.addsocket = connSocket;// đ?a ch? client
			a.priv = ++i;
			a.solannhuongquyen = 1;
			clientgame.push_back(a);
			_beginthreadex(0, 0, echoThread, (void*)connSocket, 0, 0);// b?t đ?u k?t n?i
		}
		else if (gamestate == true)
		{
			connSocket = accept(listenSock, (sockaddr *)&clientAddr, &clientAddrLen);//l?ng nghe k?t n?i m?i v?i client
			ArrClient a;
			a.addsocket = connSocket;// đ?a ch? client
			a.priv = ++i;
			a.solannhuongquyen = 1;
			clientwait.push_back(a);
		}
	}

	closesocket(listenSock);
	WSACleanup();
	system("pause");
}
