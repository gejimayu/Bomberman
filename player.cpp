#include <iostream>
#include <string>
#include "map.cpp"
using namespace std;

class Player {
	public :
		friend class Map;
		Player(Map, string);
	private :
		int posX;
		int posY;
};

Player::Player(Map M, string playerKey){
		
}