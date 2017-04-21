#include <iostream>
#include <string>
#include <cmath>
#include "datadictionary.h"
using namespace std;

//HEADER
class Map {
	public :
		Map(int w, int l);  //Constructor
		~Map();              //Destructor
		int getWidth();
		int getLength();
		int getbombPlanted();
		int getPowerUp(Position P);
		Position findClosest(Position Player, int code, char playerKey);   //Mencari objek terdekat dengan player, objek dispesifikasikan dengan variable code
		void readMapAndBomb(string, char);        //Membaca map dan bom dari file eksternal
		Position findPlayer(char playerKey);  //Mencari posisi player dalam map
		bool checkSafety(int PositionX, int PositionY);  //Mengecek keamanan suatu koordinat dari bom
		Position dijkstra(Position src, Position des, int& Distance); //Mencari jalur terdekat antara dua titik koordinat
		bool near(Position P, char C);  //Mengecek apakah suatu titik P berada di sebelah objek C
	private :
		char** M;   //Map dalam matrix 2d
		const int width;  
		const int length;
		BombData B[100]; 
		int nBomb;
		int bombPlanted;
};

//IMPLEMENTATION

Map::Map(int w, int l) : width(w) , length(l) {
	nBomb = 0;
	bombPlanted = 0;
	M = new char* [length];
	for(int i = 0; i < length; i++)
		M[i] = new char [width];
}

Map::~Map(){
	for(int i = 0; i < length; i++)
		delete [] M[i];
	delete [] M;
}

int Map::getWidth(){
	return width;
}

int Map::getLength(){
	return length;
}

int Map::getbombPlanted(){
	return bombPlanted;
}

void Map::readMapAndBomb(string filePath, char playerKey){
	cout << "Reading map from " << filePath + "/" + "map.txt" << std::endl;
	ifstream myfile(filePath + "/" + "map.txt");
	string line;
	getline(myfile,line);
	
	//Proses membaca map
	for(int i = 0; i < length; i++){
		getline(myfile,line);                  //baca sebaris map
		for (int j = 0; j < width; j++)
			M[i][j] = line[j];         //assign map ke matrix
	} 
	
	//Proses membaca bomb
	bool myData = false;
	while(getline(myfile,line)){
		if (line[0] == 'K') { //Parsing Key Player
			if (line[5] == playerKey)
				myData = true;
			else
				myData = false;
		}	
		else
		if (line[0] == 'B' && line[1] == 'o' && line[2] == 'm' && line[3] == 'b' && line[4] == 's'){   //Parsing kalimat
			cout << "Reading bomb " << endl;
			unsigned int k = 6;
			while(k < line.length()){
				if (line[k] == '{'){
					k += 3; 
					B[nBomb].x = line[k] - '0';
					k++;
					if(line[k] != ','){
						B[nBomb].x *= 10;
						B[nBomb].x += line[k] - '0';
						k++;
					}
					B[nBomb].x -= 1;
					k += 3;
					B[nBomb].y = line[k] - '0';
					k++;
					if (line[k] != ','){
						B[nBomb].y *= 10;
						B[nBomb].y += line[k] - '0';
						k++;
					}
					B[nBomb].y -= 1;
					k += 6;
					B[nBomb].fuse = line[k] - '0';
					k++;
					if (line[k] != ','){
						B[nBomb].fuse *= 10;
						B[nBomb].fuse += line[k] - '0';
						k++;
					}
					k += 8;
					B[nBomb].radius = line[k] - '0';
					k++;
					if (line[k] != '}'){
						B[nBomb].radius *= 10;
						B[nBomb].radius += line[k] - '0';
						k++;
					}
					//Positionisi sekarang menunjuk ke tanda tutup bracket  '}'
					if (myData){ //Jika ini merupakan data dari player bot , save nilai banyaknya bom yang sudah kita pasang untuk nantinya ditrigger
						bombPlanted++;
					}
					nBomb++;
				}
				k++;
			}
		}
	}
}

bool Map::checkSafety(int PositionX, int PositionY){
	bool safe = true;
	
	int i =0;
	while (i < nBomb && safe) {
		int kiri = B[i].x - B[i].radius;
		int kanan = B[i].x + B[i].radius;
		int atas =  B[i].y - B[i].radius;
		int bawah =  B[i].y + B[i].radius;
		if ((PositionX <= kanan && PositionX >= kiri && B[i].y == PositionY) || (PositionX == B[i].x && PositionY >= atas && PositionY <= bawah))  //Positionisi ada diantara bom
			safe = false;
		else
			i++;
	}
	return safe;
}

Position Map::findPlayer(char playerKey){
	Position P;
	char temp;
	for(int i = 0; i < length; i++){
		for (int j = 0; j < width; j++){
			temp = toupper(M[i][j]);
			if (temp == playerKey) {
				P.x = j;
				P.y = i;
			}
		}
	}
	return P;
}


Position Map::findClosest(Position Player, int code, char playerKey) {
	int TempDistance;
	int MinDistance = 99;
	Position tempPath;
	Position Path;
	Path.x = 99;
	Path.y = 99; //default value
	
	for(int i = Player.y - 42; i < Player.y + 42; i++){    //Mencari objek tertentu yang posisinya terdekat ke posisi player
		for(int j = Player.x - 42; j < Player.x + 42; j++){ // alam area 14 x 14 dari posisi player
			if (i > 0 && i < length && j > 0 && j < width){   //Titik tidak boleh di luar batas
				Position Ptemp;
				Ptemp.x = j;
				Ptemp.y = i;
				
				if ((M[i][j] == ' ') && checkSafety(j,i)) {        
					if (code == 1) { // wall 
						if (near(Ptemp,'+')){ //Target harus berada di dekat tembok
							tempPath = dijkstra(Player, Ptemp, TempDistance);  //Cari jalur terdekat dan jaraknya dari Player ke tembok
							if (TempDistance < MinDistance) {  //Cari objek dengan jalur minimum ke posisi player
								cout << " Found candidate at " << tempPath.x << " " << tempPath.y << " " << "Distance : " << TempDistance << endl;
								MinDistance = TempDistance;
								Path = tempPath;
							}
						}
					}
					else
					if (code == 3) { //power up
						if (near(Ptemp,'!') || near(Ptemp,'&') || near(Ptemp,'$')){ //Target harus berada di dekat power up
							tempPath = dijkstra(Player, Ptemp, TempDistance);  //Cari jalur terdekat dan jaraknya dari Player ke power up
							if (TempDistance < MinDistance) {  //Cari objek dengan jalur minimum ke posisi player
								cout << " Found candidate at " << tempPath.x << " " << tempPath.y << " " << "Distance : " << TempDistance << endl;
								MinDistance = TempDistance;
								Path = tempPath;
							}
						}
					}
					else
					if (code == 4 ) { // enemy
						for (char ch = 'A'; ch <= 'z'; ch == 'Z' ? ch = 'a': ch++){ //loop through alphabet
							if (ch != toupper(playerKey)) {// bukan player kita{
								if (near(Ptemp,ch)){
									tempPath = dijkstra(Player, Ptemp, TempDistance);  //Cari jalur terdekat dan jaraknya dari Player ke power up
									if (TempDistance < MinDistance) {  //Cari objek dengan jalur minimum ke posisi player
										cout << " Found candidate at " << tempPath.x << " " << tempPath.y << " " << "Distance : " << TempDistance << endl;
										MinDistance = TempDistance;
										Path = tempPath;
									}
								}	
							}
						}
					}	
					else
					{
						tempPath = dijkstra(Player, Ptemp, TempDistance);  //Cari jalur terdekat dan jaraknya dari Player ke tempat aman
						if (TempDistance < MinDistance) {  //Cari objek dengan jalur minimum ke posisi player
							cout << " Found candidate at " << tempPath.x << " " << tempPath.y << " " << "Distance : " << TempDistance << endl;
							MinDistance = TempDistance;
							Path = tempPath;
						}
					}
				}
			}
		}
	}
	return Path;
}


Position Map::dijkstra(Position src, Position des, int& distance){
	int** dist;
	bool** visited;
	Position** parent;
	
	dist = new int* [length];
	visited = new bool* [length];
	parent = new Position* [length];
	for(int i = 0; i < length; i++){
		dist[i] = new int [width];
		parent[i] = new Position [width];
		visited[i] = new bool [width];
	}
		
	
	//INISIALISASI
	parent[src.y][src.x].x = -1;
	parent[src.y][src.x].y = -1;
	for(int i = 0; i < length; i++){
		for(int j = 0; j < width; j++){
			dist[i][j] = 99;
			visited[i][j] = false;
		}
	}
	dist[src.y][src.x] = 0;
	
	//PROSES PENCARIAN SHORTEST PATH
	for(int count = 1; count < (length*width); count++){      //Telusuri semua vertex kecuali target
		
		//Cari vertex minimum yang belum diproses
		int min = 99;
		Position choosenV;
		for(int i = 0; i < length; i++){
			for(int j = 0; j < width; j++){
				if (!visited[i][j] && (dist[i][j] < min)){
					choosenV.x = j;
					choosenV.y = i;
					min = dist[i][j];
				}
			}
		}
			
		if (min != 99){
			visited[choosenV.y][choosenV.x] = true;  //vertex dipilih
			if ((choosenV.y == des.y) && (choosenV.x == des.x)) //cek apakah target merupakan destinasi
			{
				int m = choosenV.y;
				int n = choosenV.x;
				Position path;
				
				while ((n != src.x) || (m != src.y)){  //Proses Backtracking jalur
					path.x = n;
					path.y = m;
					m = parent[path.y][path.x].y;
					n = parent[path.y][path.x].x;
				}
				distance = dist[choosenV.y][choosenV.x];
				return path;
			}
			
			
			//update nilai dari distance tetangga vertex terpilih
			int kiri = choosenV.x - 1;
			int kanan = choosenV.x + 1;
			int atas = choosenV.y - 1;
			int bawah = choosenV.y + 1;
			if ((kiri >= 0) && (M[choosenV.y][kiri] != '#') && ((M[choosenV.y][kiri] != '+') && (!visited[choosenV.y][kiri]))){   
			//jika tidak di luar batas dan bukan wall dan belum dikunjungi
				if (dist[choosenV.y][kiri] > (dist[choosenV.y][choosenV.x] + 1)){
					dist[choosenV.y][kiri] = dist[choosenV.y][choosenV.x] + 1;
					parent[choosenV.y][kiri].x = choosenV.x;
					parent[choosenV.y][kiri].y = choosenV.y;
				}
					
			}
			if ((kanan< width) && (M[choosenV.y][kanan] != '#') && ((M[choosenV.y][kanan] != '+') && (!visited[choosenV.y][kanan]))){    
			//jika tidak di luar batas dan bukan wall dan belum dikunjungi
				if (dist[choosenV.y][kanan] > (dist[choosenV.y][choosenV.x] + 1)){
					dist[choosenV.y][kanan] = dist[choosenV.y][choosenV.x] + 1;
					parent[choosenV.y][kanan].x = choosenV.x;
					parent[choosenV.y][kanan].y = choosenV.y;
				}
					
			}
			if ((atas >= 0) && (M[atas][choosenV.x] != '#') && (M[atas][choosenV.x] != '+') && (!visited[atas][choosenV.x])){    
			//jika tidak di luar batas dan bukan wall dan belum dikunjungi
				if (dist[atas][choosenV.x] > (dist[choosenV.y][choosenV.x] + 1)) {
					dist[atas][choosenV.x] = dist[choosenV.y][choosenV.x] + 1;
					parent[atas][choosenV.x].x = choosenV.x;
					parent[atas][choosenV.x].y = choosenV.y;
				}
			}
			if ((bawah < length) && (M[bawah][choosenV.x] != '#') && (M[bawah][choosenV.x] != '+') && (!visited[bawah][choosenV.x])){    
			//jika tidak di luar batas dan bukan wall dan belum dikunjungi
				if (dist[bawah][choosenV.x] > (dist[choosenV.y][choosenV.x] + 1)){
					dist[bawah][choosenV.x] = dist[choosenV.y][choosenV.x] + 1;
					parent[bawah][choosenV.x].x = choosenV.x;
					parent[bawah][choosenV.x].y = choosenV.y;
				}
			}
		}
		else //sudah tidak ada kandidat
		{
			break;
		}
	}
	
	//path tidak ditemukan
	Position path;
	path.x = 99;
	path.y = 99;
	distance = 99;
	return path;
}

bool Map::near(Position P, char C){
	return ((M[P.y-1][P.x] == C) || (M[P.y+1][P.x] == C) || (M[P.y][P.x-1] == C) || (M[P.y][P.x+1] == C));
}

int Map::getPowerUp(Position P){
	if (((M[P.y-1][P.x] == '!') || (M[P.y-1][P.x] == '&') || (M[P.y-1][P.x] == '$')) && checkSafety(P.x,P.y-1))
		return 1;
	else
	if (((M[P.y+1][P.x] == '!') || (M[P.y+1][P.x] == '&') || (M[P.y+1][P.x] == '$')) &&checkSafety(P.x,P.y+1))
		return 4;
	else
	if (((M[P.y][P.x-1] == '!') ||  (M[P.y][P.x-1] == '&') || (M[P.y][P.x-1] == '$')) && checkSafety(P.x-1,P.y))
		return 2;
	else
	if (((M[P.y][P.x+1] == '!') ||  (M[P.y][P.x+1] == '&') || (M[P.y][P.x+1] == '$')) && checkSafety(P.x+1,P.y))
		return 3;
	else
		return 0;
}