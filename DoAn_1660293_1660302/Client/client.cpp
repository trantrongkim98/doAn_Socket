#include"Function.h"
/*
	 nhượng quyền dùng dấu *
	 câu hỏi và tuỳ chọn ngăn cách bởi ,
	 các 
*/

int main()
{
	WSADATA WSAData;
	SOCKET server;
	SOCKADDR_IN addr;
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	server = socket(AF_INET, SOCK_STREAM, 0);

	addr.sin_addr.s_addr = inet_addr("127.0.0.1");// ip server kết nối 2 console
	addr.sin_family = AF_INET;
	addr.sin_port = htons(23398);// port kết kết nôi
	if(connect(server, (SOCKADDR *)&addr, sizeof(addr))==false)
		cout << "ket noi toi server thanh cong!!!" << endl;
	string user;

	Socket(server, user);
	return 0;
}