#include "Function.h"
#include<wchar.h>

vector<string> slip(string chuoi,char elam)
{
	vector<string> str;
	istringstream line(chuoi);
	string a;
	while (getline(line, a, elam))
	{
		str.push_back(a);
	}
	return str;
}

bool sendMail(SOCKET server, string mailbuff)
{
	char mail[1024];
	strcpy(mail, mailbuff.c_str());
	if (send(server, mail, sizeof(mailbuff.c_str()), 0))
		return true;
	return false;
}

string receMail(SOCKET server)
{
	
	char mail[1024];
	recv(server, mail, sizeof(mail), 0);
	string str(mail);
	return str;
}

bool checkConnect(SOCKET server, SOCKADDR_IN addr)
{
	if (connect(server, (SOCKADDR *)&addr, sizeof(addr)) != 0)
	{
		return false;
	}
	return true;
}

SOCKADDR_IN initAdd(const char * server, u_short port)
{
	SOCKADDR_IN addr;
	addr.sin_addr.s_addr = inet_addr(server);// ip server kết nối 2 console
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);// port kết kết nôi
	return addr;
}

string input()
{
	string flag;
	cin >> flag;
	return flag;
}
void fullscreen()
{
	keybd_event(VK_MENU, 0x38, 0, 0);
	keybd_event(VK_RETURN, 0x1c, 0, 0);
	keybd_event(VK_RETURN, 0x1c, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0);
}
string chondapan(int soluachon)
{
	string da = "";
	if (soluachon > 4)
	{
		while (da != "A" && da != "B" && da != "C" && da != "D" && da != "next")
		{
			cout << endl << "dap an: ";
			da = input();
		}
	}
	else {
		while (da != "A" && da != "B" && da != "C" && da != "D")
		{
			cout << endl << "dap an: ";
			da = input();
		}
	}
	return da;
}
void Socket(SOCKET server, string user)
{
	//#1: send receive Username between Client and Server
	while (1)
	{
		cout << "input your user: ";
		user = input();
		user.resize(1024);
		sendMail(server, user);
		string flag = receMail(server);
		cout << flag<<endl;
		if (flag == "success")break;
		cout <<endl<< "server: " << flag << endl;
	}
	// flag again game
game:
	// xuất thông tin của game
	MessageBoxW(NULL,L"Chào mừng bạn đến với game đối mặt, ở game này bạn phải chọn đạp án đúng nhất trong 4 đáp án, và bạn có 1 lần nhường quyền trả lời cho người kế tiếp, nêu bạn đã nhường quyền trước đó bạn bắt buộc phải trả lời câu hỏi đó.",L"Luật",MB_ICONINFORMATION);
	//#2: wait start game
	while (1)
	{
		int kt = MessageBoxW(NULL, L"Bạn có muốn tiếp tục chơi game không", L"Thông báo", MB_YESNO);
		if (kt == 7)
		{
			//#4. close game
			sendMail(server, "not");
			closesocket(server);
			exit(0);
		}
		else if (kt == 6) {
			sendMail(server, "accept");
			break;
		}
	}
	string nghe = receMail(server);
	while (nghe != "start") 
	{
		MessageBoxW(NULL, L"Waiting...", L"Thông báo", MB_OK | MB_ICONERROR);
		nghe = receMail(server);
	}
	MessageBoxW(NULL, L"Start", L"Thông báo", MB_OK);
	string rule = receMail(server);
	MessageBoxA(NULL, rule.c_str(), "Thông báo", MB_OK);
wait:
	//#3: star game
	while (1)
	{
		
		//#3.1 rece question and option from server
		string mail = receMail(server);
		vector<string> QuesOpt = slip(mail, ';');// between question and option are ;
		cout << "Server: "<<endl;
		outQuestion(QuesOpt[0]);
		outOption(QuesOpt[1]);
		//#3.2 send answer
		mail = chondapan(10);
		cout << "Client: "<<mail;
		sendMail(server, mail);
		mail = receMail(server);
		if (mail == "lose") break;
		else if (mail == "erro")
		{
			mail = chondapan(1);
			sendMail(server, mail);
		}
		else if (mail == "agree")
		{
			while (receMail(server) != "start");
			goto wait;
		}
		
	}
	if (receMail(server) == "prepare")
	{
		goto game;
	}
}

void outQuestion(string ques)
{
	cout << "Question: " << ques << endl;
}

void outOption(string option)
{
	cout << "Option: " << option << endl;
}

