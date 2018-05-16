#include<string>
#include<iostream>
#include<vector>
#include<conio.h>

#include<sstream>
#include<WinSock2.h>
#pragma comment (lib, "Ws2_32.lib")
using namespace std;


/*
	function return vector<string> contain question, option
	input is string you want slip and delam of string
*/
vector<string> slip(string str,char c);
/*
	function return bool when you send mail success
	input is SOCKET server and string you want send
*/
bool sendMail(SOCKET s, string str);
/*
	this function return a string contain information server send to you
	input of function is SOCKET server
*/
string receMail(SOCKET server);
/*
	this function return a type bool when you connect success to server
	input of function is SOCKET server and address of SOCKADDR_IN
*/
bool checkConnect(SOCKET server, SOCKADDR_IN adrr);
/*
	this function return a SOCKADDR_IN contain add was init add server, quantity add connect 
	here is two IP ==> a client to a server
	input of fuction is IP and port connect
*/
SOCKADDR_IN initAdd(const char* server, u_short port);
/*
	this function return a string type string contain imformation you want sent to server
*/
string input();
/*
	this function contain program SOCKET game Doi mat
*/
void Socket(SOCKET server,string user);
/*
	this fuction output string question
*/
void outQuestion(string ques);
/*
	this function output string option of question
*/
void outOption(string option);
/*
	
*/
string chondapan(int luachon);
void fullscreen();
