// SampleBot.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <cstring>
#include "map.cpp"
using namespace std;

void readStateFile(string filePath, string playerKey); 
void writeMoveFile(string filePath, int move);
int Moving(Position Player, Position path);

int _tmain(int argc, _TCHAR* argv[])
{
	string filePath = argv[2];
	string playerKey = argv[1];
	
	cout << "Args: " << argc << std::endl;
	cout << "Player Key: " << argv[1] << std::endl;
	cout << "File Path: " << argv[2] << std::endl;

	readStateFile(filePath, playerKey);
	return 0;
}

int Moving(Position Player, Position path){
	//Prosedur yang menentukan player harus bergerak ke arah mana sesuai jalur
	if ((Player.x - path.x) > 0){
		cout << "Player move to the left" << endl;
		return 2;
	}
	else
	if ((Player.x - path.x) < 0){
		cout << "Player move to the right" << endl;
		return 3;
	}
				
	else
	if ((Player.y - path.y) > 0){
		cout << "Player move to the north" << endl;
		return 1;
	}
				
	else
	if ((Player.y - path.y) < 0){
		cout << "Player move to the south" << endl;
		return 4;
	}	
}

void readStateFile(string filePath, string playerKey)
{
	string line;
	ifstream myfile(filePath + "/" + "map.txt");
	getline(myfile, line);
	
	//PROSES PARSING Lebar dan Panjang Map
	int width, length; 
	width = line[11] - '0';	
	width *= 10;
	width += line[12] - '0';
	length = line[27] - '0';
	length *= 10;
	length += line[28] - '0';
	
	//INISIALISASI OBJEK MAP
	Map M(width,length);
	M.readMapAndBomb(filePath, playerKey[0]);
	cout << "Reading done" << endl;
	//CARI POSISI PLAYER
	Position Player = M.findPlayer(playerKey[0]);
	cout << "Player is at " << Player.x << " " << Player.y << endl;
	
	int move  = 0; 
	bool enemyNearby = false;
	int code;
	if (M.checkSafety(Player.x,Player.y)){            //Jika posisi player saat ini aman dari bom
		
		//cek lawan
		for (char ch = 'A'; ch <= 'z'; ch == 'Z' ? ch = 'a': ch++)//loop through alphabet
			if (ch != toupper(playerKey[0])) // bukan player kita
				if (M.near(Player,ch)) //berada di sebelah musuh
					enemyNearby = true;
		
		cout << "Player is safe" << endl;
		if (M.near(Player,'!') || M.near(Player,'&') || (M.near(Player,'$'))){
			cout << "Player gets power up" << endl;
			move = M.getPowerUp(Player);
		}
		else
		if (enemyNearby){
			cout << "Player is close to enemies, bomb them !!";
			move = 5; // BOMB !!
		}
		else
		if (M.near(Player,'+')) {                        //Mengecek apakah posisi player bersebelahan dengan tembok
			cout << "Player is near wall and bomb it" << endl;
			move = 5;                                        // BOMB !!
		}
		else
		if (M.getbombPlanted() > 0){
			cout << "Player triggers a bomb " << endl;
			move = 6;
		}
		else {                   //Jika tidak bersebelahan, cari tembok / power up
			code = 3; //3 for power up
			Position path = M.findClosest(Player,code,playerKey[0]);
			if (path.x != 99) { //found power up
				cout << "Player found power up and are going there now" << endl;
				move = Moving(Player,path);
			}
			else{
				cout << "Player find shortest and safe wall" << endl;
				code = 1; //1 for wall
				Position path = M.findClosest(Player,code,playerKey[0]);     //Cari jalur terpendek menuju tembok
				if (path.x != 99) { // wall ditemukan
					move = Moving(Player,path);  
				}
				else { // wall tidak ditemukan, kejar player
						Position path = M.findClosest(Player,4,playerKey[0]);
						move = Moving(Player,path);
				}
			}	
		}
	}
	else
	{
		code = 2; //2 for safe spot
		cout << "Player isn't safe and find closest safe spot" << endl;
		Position path = M.findClosest(Player,code,playerKey[0]);  //Cari jalur terpendek menuju tempat aman
		move = Moving(Player,path);
	}
	
	writeMoveFile(filePath,move);
}

void writeMoveFile(string filePath, int move)
{
	cout << "Writing move file " << filePath + "/" + "move.txt" << std::endl;
	ofstream outfile(filePath + "/" + "move.txt");

	if (outfile.is_open())
	{
		outfile << move << std::endl;
		outfile.close();
	}
}
