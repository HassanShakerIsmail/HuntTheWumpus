#include<iostream>
#include<vector>
#include<algorithm>

//This is my attempt (Hassan Ismail) to implement Gregory Yob's famous text based game 'Hunt the Wumpus' in C++.
//The rules are as follows: In a cave of 20 rooms, the player randomly spawns in one room (Not with the Wumpus) and with 5 arrows, must traverse the rooms hunting down the wumpus, while avoiding randomly placed 
//Bottomless pits, giant transporting bats, and the Wumpus himself.


//bad practice but will suffice in this case
using namespace std;

class Room {
	Room* nextLeft;
	Room* nextRight;
	Room* nextUp;
	Room* nextDown;
	int RoomNumber;
	bool Wumpus;
	bool pit;
	bool bat;
	bool Player;

public:
	Room(Room* nL = nullptr, Room* nR = nullptr, Room* nU = nullptr, Room* nD = nullptr, int n = 0)
		:nextLeft{ nL }, nextRight{ nR }, nextUp{ nU }, nextDown{ nD },RoomNumber{ n }, Wumpus{ false }, pit{ false }, bat{ false },Player(false){};

	friend vector<Room> initializeRooms();
	friend void spawnBats(vector<Room>&);
	friend void spawnPits(vector<Room>&);

	//the following helper functions are meant to avoid any problems with class member access, and overall try to improve readability. 
	void setWumpTrue() {
		Wumpus = true;
	}
	void setWumpFalse() {
		Wumpus = false;
	}

	void setPlayerTrue() {
		Player = true;
	}

	void setPlayerFalse() {
		Player = false;
	}

	int fetchRoomNumber() {
		return RoomNumber;
	}

	bool containsWumpus() {
		return Wumpus;
	}

	bool containsPlayer() {
		return Player;
	}

	bool containsBat() {
		return bat;
	}

	bool containsPit() {
		return pit; 
	}

	Room* rightRoom() {
		return nextRight;
	}

	Room* leftRoom() {
		return nextLeft;
	}

	Room* aboveRoom() {
		return nextUp;
	}

	Room* belowRoom() {
		return nextDown;
	}

	
};

class Wumpus {
	Room currentRoom;

public:
	Wumpus() {
		currentRoom = Room();
	}
	void spawnWumpus(vector<Room>&);
	int wumpusRoomNumber() {
		return currentRoom.fetchRoomNumber();
	}
};

void Wumpus::spawnWumpus(vector<Room>& rooms) {
	srand(time(NULL));
	unsigned seed = rand() % 19;
	rooms[seed].setWumpTrue();
	currentRoom = rooms[seed];
};


//gamestatus keeps track of whether or not player is dead (true is alive)
class Player {
	Room currentRoom;
	int arrows;
	bool Alivestatus;
	bool KilledWumpus;

public:
	explicit Player() {
		arrows = 5;
		Alivestatus = true;
		currentRoom = Room();
		KilledWumpus = false;
	}
	friend void StartGame();
	void CheckAdjacentForWumpus(vector<Room>&);
	void CheckAdjacentForPits(vector<Room>&);
	void CheckAdjacentForBats(vector<Room>&);

	//youll notice that this function is basically identical to the move player function. This makes sense, as the the only difference between the two functions is that rather than
	//alter what room our player is in, after checking the adjacent rooms - the shoot arrow function will check adjacent rooms but check for wumpus containment.
	void shootArrow(vector<Room>&rooms, Wumpus& wump) {
		char playerChoice;
		cout << "You currently have " << arrows << " arrows.\n";
		cout << "Would you like to shoot an arrow? y|n\n";
		while (true) {
			cin >> playerChoice;
			if (playerChoice != 'y' && playerChoice != 'n') {
				cout << "Sorry, please entry only y|n\n";
			}
			else { break; };
		}
		if (playerChoice == 'y') {
			int shootIntoRooms{ 0 };
			int CurRoomNo = currentRoom.fetchRoomNumber(); // whats the number of the room we're in?
			vector<int>options;//which rooms may we shoot into?
			if (rooms[CurRoomNo].leftRoom() != nullptr) {
				options.push_back(rooms[CurRoomNo].leftRoom()->fetchRoomNumber());
			}

			if (rooms[CurRoomNo].rightRoom() != nullptr) {
				options.push_back(rooms[CurRoomNo].rightRoom()->fetchRoomNumber());
			}

			if (rooms[CurRoomNo].aboveRoom() != nullptr) {
				options.push_back(rooms[CurRoomNo].aboveRoom()->fetchRoomNumber());
			}
			if (rooms[CurRoomNo].belowRoom() != nullptr) {
				options.push_back(rooms[CurRoomNo].belowRoom()->fetchRoomNumber());
			}
			cout << "You may shoot into rooms: ";
			for (int i = 0; i < options.size(); ++i) {
				cout << options[i] <<',';
			}
			cout << "\nWhich of your adjacent rooms would you like to shoot into?\n";
			bool flag = true;
			while (flag == true) {
				shootIntoRooms = 0;
				cin >> shootIntoRooms;
				if (cin.fail()) {
					cin.clear();
					cin.ignore();
				}
				for (int i = 0; i < options.size(); ++i) {
					if (shootIntoRooms == options[i]) {
						flag = false;
						goto exit;
					}
				}
				cout << "invalid entry - try again! \n";
			exit:
				continue;
			}
			if (rooms[shootIntoRooms].containsWumpus() == true) {
				cout << "You've killed the Wumpus!\n";
				rooms[shootIntoRooms].setWumpFalse();
				KilledWumpus = true;
				arrows -= 1;
			}
			else {
				arrows -= 1;
				cout << "You've startled the Wumpus! You hear scurrying around you..\n";
				int wumpusRoomNumber = wump.wumpusRoomNumber();
				rooms[wumpusRoomNumber].setWumpFalse();
				//respawn the wumpus in a different room, not containing bats or pits.
				while (true) {
					srand(time(NULL));
					int newSeed = rand() % 19;
					if (rooms[newSeed].containsPit() == false && rooms[newSeed].containsBat() == false) {
						rooms[newSeed].setWumpTrue();
						if (rooms[newSeed].containsPlayer() == true) {
							cout << "The Wumpus has eaten you! GAME OVER\n";
							Alivestatus = false;
						}
						break;
					}
				}
			};
		}
		if (arrows == 0) {
			cout << "You've run out of arrows! GAME OVER\n";
			Alivestatus = false;
		}

	}

	void MovePlayer(vector<Room>& rooms) {
		int CurRoomNo = currentRoom.fetchRoomNumber();
		vector<int> options; //what are rooms available to move to?
		cout << "You are currently in room " << CurRoomNo << '\n';
		cout << "You may move to rooms: ";
		if (rooms[CurRoomNo].leftRoom() != nullptr) {
			options.push_back(rooms[CurRoomNo].leftRoom()->fetchRoomNumber());
		}

		if (rooms[CurRoomNo].rightRoom() != nullptr) {
			options.push_back(rooms[CurRoomNo].rightRoom()->fetchRoomNumber());
		}

		if (rooms[CurRoomNo].aboveRoom() != nullptr) {
			options.push_back(rooms[CurRoomNo].aboveRoom()->fetchRoomNumber());
		}
		if (rooms[CurRoomNo].belowRoom() != nullptr) {
			options.push_back(rooms[CurRoomNo].belowRoom()->fetchRoomNumber());
		}

		for (int i = 0; i < options.size(); ++i) {
			cout << options[i] << ',';
		}
		cout << '\n' << "Enter one of the digits on screen to move to that room.\n";
		int choice;
		bool flag = true;
		while (flag == true) {
			choice = 0;
			cin >> choice;
			if (cin.fail()) {
				cin.clear();
				cin.ignore();
			}
			for (int i = 0; i < options.size(); ++i) {
				if (choice == options[i]) {
					flag = false;
					goto exit;
				}
			}
			cout << "invalid entry - try again! \n";
		exit:
			continue;
	}
		currentRoom.setPlayerFalse();
		rooms[choice].setPlayerTrue();
		currentRoom = rooms[choice];
		cout << "You've moved to room " << currentRoom.fetchRoomNumber() <<'\n';
		if (rooms[choice].containsWumpus() == true) {
			cout << "You've ran into the Wumpus, he feasts on you! GAME OVER\n";
			Alivestatus = false;
		}

		if (rooms[choice].containsPit() == true) {
			cout << "You fall into the abyss, never to be seen from again.. GAME OVER\n";
			Alivestatus = false;
		}

		//note this scenario is special, you CANNOT be dropped into a room with another bat. 
		if (rooms[choice].containsBat() == true) {
			cout << "You hear a loud screech, and are suddenly picked up! A giant bat has moved you to a new room!\n";
			while (true) {
				srand(time(NULL));
				int newRoom = rand() % 19;
				if (rooms[newRoom].containsBat() == false) {
					currentRoom.setPlayerFalse();
					rooms[newRoom].setPlayerTrue();
					currentRoom = rooms[newRoom];
					if (rooms[newRoom].containsPit() == true || rooms[newRoom].containsWumpus() == true) {
						cout << "You've died! GAME OVER\n";
						Alivestatus = false;
						break;
					}
					else {
						cout << "You are now in room " << currentRoom.fetchRoomNumber() << '\n';
						break;
					}
				}
			}
		}
}
	//spawns player using random seed
	void spawnPlayer(vector<Room>&rooms) {
		while (true) {
			srand(time(NULL));
			unsigned seed = rand() % 19;
			if (rooms[seed].containsWumpus() != true) {
				rooms[seed].setPlayerTrue();
				currentRoom = rooms[seed];
				cout << "You've spawned in room " << rooms[seed].fetchRoomNumber() <<'\n';
				break;
			}
		}
	}
};

//check adjacent rooms for the wumpus and displays a warning message if he is there.
void Player::CheckAdjacentForWumpus(vector<Room>& rooms) {
	int currentRoomNo = currentRoom.fetchRoomNumber();
	if (rooms[currentRoomNo].leftRoom() != nullptr) {
		if (rooms[currentRoomNo - 1].containsWumpus() == true) {
			cout << "A rancid stench fills the air..\n";
		}
	}

	if (rooms[currentRoomNo].rightRoom() != nullptr) {
		if (rooms[currentRoomNo + 1].containsWumpus() ==true) {
			cout << "A rancid stench fills the air..\n";
		}
	}

	if (rooms[currentRoomNo].aboveRoom() != nullptr) {
		if (rooms[currentRoomNo + 5].containsWumpus() == true) {
			cout << "A rancid stench fills the air..\n";
		}
	}

	if (rooms[currentRoomNo].belowRoom() != nullptr) {
		if (rooms[currentRoomNo - 5].containsWumpus() == true) {
			cout << "A rancid stench fills the air..\n";
		}
	}
}

//self-explanatory, will check adjacent rooms to the player for pits, and will display a warning
void Player::CheckAdjacentForPits(vector<Room>& rooms) {
	int currentRoomNo = currentRoom.fetchRoomNumber();
	if (rooms[currentRoomNo].leftRoom() != nullptr) {
		if (rooms[currentRoomNo - 1].containsPit() == true) {
			cout << "You feel a breeze nearby..\n";
		}
	}

	if (rooms[currentRoomNo].rightRoom() != nullptr) {
		if (rooms[currentRoomNo + 1].containsPit() == true) {
			cout << "You feel a breeze nearby..\n";
		}
	}

	if (rooms[currentRoomNo].aboveRoom() != nullptr) {
		if (rooms[currentRoomNo + 5].containsPit() == true) {
			cout << "You feel a breeze nearby..\n";
		}
	}

	if (rooms[currentRoomNo].belowRoom() != nullptr) {
		if (rooms[currentRoomNo - 5].containsWumpus() == true) {
			cout << "You feel a breeze nearby..\n";
		}
	}

}

void Player::CheckAdjacentForBats(vector<Room>& rooms) {
	int currentRoomNo = currentRoom.fetchRoomNumber();
	if (rooms[currentRoomNo].leftRoom() != nullptr) {
		if (rooms[currentRoomNo - 1].containsBat() == true) {
			cout << "You hear massive wings beating..\n";
		}
	}

	if (rooms[currentRoomNo].rightRoom() != nullptr) {
		if (rooms[currentRoomNo + 1].containsBat() == true) {
			cout << "You hear massive wings beating..\n";
		}
	}

	if (rooms[currentRoomNo].aboveRoom() != nullptr) {
		if (rooms[currentRoomNo + 5].containsBat() == true) {
			cout << "You hear massive wings beating..\n";
		}
	}

	if (rooms[currentRoomNo].belowRoom() != nullptr) {
		if (rooms[currentRoomNo - 5].containsBat() == true) {
			cout << "You hear massive wings beating..\n";
		}
	}
}


//spawns bats in 2 random rooms - cannot be the same as the Wumpus, or player. NOTICE: There is a heirachy here that can't be violated. Wumpus must be spawned, then player, then the bats.
void spawnBats(vector<Room>& rooms) {
	int bat1, bat2;
	while (true) {
		srand(time(NULL));
		bat1 = rand() % 19;
		bat2 = (rand() % 17) +2;
		if (rooms[bat1].containsWumpus() == false && rooms[bat1].containsPlayer() == false) {
			if (rooms[bat2].containsWumpus() == false && rooms[bat2].containsPlayer() == false) {
						rooms[bat1].bat = true;
						rooms[bat2].bat = true;
							break;
			}
		}
	}

}

//again this function follows a clear hiearchy, it must be called after spawnBats(), that is - Wumpus spawns, player spawns, bats spawn, and then the pits spawn.
void spawnPits(vector<Room>& rooms) {
	int pit1, pit2;
	while (true) {
		srand(time(NULL));
		pit1 = rand() % 19;
		pit2 = (rand() % 17) + 2;
		if (rooms[pit1].containsPlayer() == false && rooms[pit1].containsWumpus() == false && rooms[pit1].containsBat() == false) {
			if (rooms[pit2].containsPlayer() == false && rooms[pit2].containsWumpus() == false && rooms[pit2].containsBat() == false) {
				rooms[pit1].pit = true;
				rooms[pit2].pit = true;
				break;
			}
		};

	}
}


//helper function that creates the rooms in the form of a vector - numbers them, and connects them appropriately - this is the main gameinstance.
	//notice this idea can be extended for n rooms by using modular arithmetic coupled with if statements to handle the linking of rooms in a corner.
	//eg. if i'm at the topleft corner, i will only have a room below me, and to the right. However, since I know my grid is of size 20, i will not use abstraction.
vector<Room> initializeRooms() {
		constexpr int NumbofRooms = 20;
		int rowCount{ 1 }; //row counter to help keep track of whether we're in the first and last row, so we avoid having a room below or above respectively.

		vector<Room>rooms(NumbofRooms);
		for (int i = 0; i < NumbofRooms; ++i) {
			if (rowCount == 1) //if we're on the first row, there will be no room below, and for the first and last entries - no left or right rooms.
			{
				//check for the case we are on the last entry of the first row - no room below nor right.
				if (i % 5 == 4) {
					rooms[i].RoomNumber = i;
					rooms[i].nextLeft = &rooms[i - 1];
					rooms[i].nextDown = nullptr;
					rooms[i].nextRight = nullptr;
					rooms[i].nextUp = &rooms[i + 5];
				}
				else {
					if (i % 5 == 0)//check we are not on the first entry of row 1 (no room to the left or below)
					{
						rooms[i].RoomNumber = i;
						rooms[i].nextLeft = nullptr;
						rooms[i].nextRight = &rooms[i + 1];
						rooms[i].nextUp = &rooms[i + 5];
						rooms[i].nextDown = nullptr;
					}
					//if we arent on the first or last entry of row 1, the other entries just wont have a room above them.
					else {
						rooms[i].RoomNumber = i;
						rooms[i].nextLeft = &rooms[i - 1];
						rooms[i].nextRight = &rooms[i + 1];
						rooms[i].nextUp = &rooms[i + 5];
						rooms[i].nextDown = nullptr;
					}
				}

			}

			if (rowCount == 4) //the last row will have no rooms above it, and depending on where we are, wont have rooms to the left or right.
			{
				if (i % 5 == 0)//case we're on the first entry of the last row
				{
					rooms[i].RoomNumber = i;
					rooms[i].nextLeft = nullptr;
					rooms[i].nextDown = &rooms[i - 5];
					rooms[i].nextUp = nullptr;
					rooms[i].nextRight = &rooms[i + 1];
				}
				else {
					if (i % 5 == 4)//last entry of the last row, no room to the right or above.
					{
						rooms[i].RoomNumber = i;
						rooms[i].nextLeft = &rooms[i - 1];
						rooms[i].nextRight = nullptr;
						rooms[i].nextUp = nullptr;
						rooms[i].nextDown = &rooms[i - 5];
					}
					else //if this is not the first nor last entry of the last rows, we'll have rooms everywhere except above us.
					{
						rooms[i].RoomNumber = i;
						rooms[i].nextUp = nullptr;
						rooms[i].nextDown = &rooms[i - 5];
						rooms[i].nextLeft = &rooms[i - 1];
						rooms[i].nextRight = &rooms[i + 1];
					}
				}

			}

			if (rowCount != 4 && rowCount != 1)//we handle the case of all the inbetween rows here. 
			{
				if (i % 5 == 0) //if i is the first entry on the row, no room to the left only.
				{
					rooms[i].RoomNumber = i;
					rooms[i].nextLeft = nullptr;
					rooms[i].nextUp = &rooms[i + 5];
					rooms[i].nextRight = &rooms[i + 1];
					rooms[i].nextDown = &rooms[i - 5];
				}
				else {
					if (i % 5 == 4)//case we're on the last entry of the row, no right room.
					{
						rooms[i].RoomNumber = i;
						rooms[i].nextRight = nullptr;
						rooms[i].nextLeft = &rooms[i - 1];
						rooms[i].nextUp = &rooms[i + 5];
						rooms[i].nextDown = &rooms[i - 5];
					}
					else //if we arent in  the last or first entry of the rows, we should have 4 adjacent rooms.
					{
						rooms[i].RoomNumber = i;
						rooms[i].nextDown = &rooms[i - 5];
						rooms[i].nextUp = &rooms[i + 5];
						rooms[i].nextLeft = &rooms[i - 1];
						rooms[i].nextRight = &rooms[i + 1];
					}
				}
			}
			if (i % 5 == 4) {
				++rowCount;
			}
		}
		return rooms;


}

void StartGame() {
	vector<Room>GameInstance;
	GameInstance = initializeRooms();
	Wumpus Wump;
	Wump.spawnWumpus(GameInstance);
	Player player;
	player.spawnPlayer(GameInstance);
	spawnBats(GameInstance);
	spawnPits(GameInstance);

	player.CheckAdjacentForWumpus(GameInstance);
	player.CheckAdjacentForBats(GameInstance);
	player.CheckAdjacentForPits(GameInstance);
	while (player.Alivestatus == true && player.KilledWumpus == false) {
		player.MovePlayer(GameInstance);
		player.CheckAdjacentForWumpus(GameInstance);
		player.CheckAdjacentForBats(GameInstance);
		player.CheckAdjacentForPits(GameInstance);
		if (player.Alivestatus == false || player.KilledWumpus == true) {
			break;
		}
		player.shootArrow(GameInstance, Wump);
	}
}

int main() {
	StartGame();
}

