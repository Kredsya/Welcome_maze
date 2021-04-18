#include <bangtal.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include <string.h>
using namespace std;

#pragma warning(disable:4996)
#pragma warning(disable:26495)
#pragma warning(disable:6031)

struct game_data {
	string name;
	int map_size, pixel_size, zeroX, zeroY;
	int startX, startY;
	int goalX, goalY;
	ObjectID button;
};
game_data chobo = { "chobo", 47, 14, 311, 689, 23, 1, 1, 46 };
game_data jungsu = { "jungsu", 59, 12, 286, 714, 1, 5, 55, 58 };
game_data gosu = { "gosu", 161, 4, 318, 38, 1, 1, 1, 1 };
game_data type;

int playerX, playerY;
bool map[161][161]; //wall = true
SceneID main_screen, ingame_screen;
ObjectID thumbs_up[4], wall[25921], player, main_button;
clock_t start, finish;

int x_converter(int i) { return type.zeroX + i * type.pixel_size; }
int y_converter(int i) { return type.zeroY - i * type.pixel_size; }

void keyCallback(KeyCode key, KeyState state) {
	if (state == KeyState::KEY_RELEASED) return;

	if (key == KeyCode::KEY_LEFT_ARROW) {
		if (map[playerY][playerX - 1] || playerX == 0) return;
		playerX--;
		locateObject(player, ingame_screen, x_converter(playerX), y_converter(playerY));
	}
	else if (key == KeyCode::KEY_RIGHT_ARROW) {
		if (map[playerY][playerX + 1] || playerX == type.map_size - 1) return;
		playerX++;
		locateObject(player, ingame_screen, x_converter(playerX), y_converter(playerY));
	}
	else if (key == KeyCode::KEY_UP_ARROW) {
		if (map[playerY - 1][playerX] || playerY == 0) return;
		playerY--;
		locateObject(player, ingame_screen, x_converter(playerX), y_converter(playerY));
	}
	else if (key == KeyCode::KEY_DOWN_ARROW) {
		if (map[playerY + 1][playerX] || playerY == type.map_size - 1) return;
		playerY++;
		locateObject(player, ingame_screen, x_converter(playerX), y_converter(playerY));
	}

	if (playerX == type.goalX && playerY == type.goalY) {
		finish = clock();
		string buf = "축하합니다! " + to_string((int)((finish - start) / CLOCKS_PER_SEC)) + "초 걸렸습니다.";
		showMessage(buf.c_str());
		for (int i = 0; i < 4; i++) showObject(thumbs_up[i]);
		showObject(main_button);
	}
}

ObjectID createObject(string path, SceneID scene, int x, int y, bool shown) {
	ObjectID object = createObject(path.c_str());
	locateObject(object, scene, x, y);
	if (shown) showObject(object);
	return object;
}

SceneID ingame_init(game_data temp) {
	type = temp;
	string buf = "Images/" + type.name + "_player.png";
	playerX = type.startX;
	playerY = type.startY;
	player = createObject(buf.c_str(), ingame_screen, x_converter(playerX), y_converter(playerY), true);
	start = clock();
	
	buf = "data/" + type.name + "_map_data.txt";
	FILE* map_data = fopen(buf.c_str(), "r");

	buf = "Images/" + type.name + "_wall.png";
	int x = 0;
	for (int i = 0; i < type.map_size; i++) {
		char map_tmp[170];
		fscanf(map_data, "%s", map_tmp);
		for (int j = 0; j < type.map_size; j++) {
			if (map_tmp[j] == '1') {
				map[i][j] = true;
				wall[x++] = createObject(buf.c_str(), ingame_screen, x_converter(j), y_converter(i), true);
			}
		}
	}

	return ingame_screen;
}

void mouseCallback(ObjectID object, int x, int y, MouseAction action) {
	if (object == chobo.button)		  enterScene(ingame_init(chobo));
	else if (object == jungsu.button) enterScene(ingame_init(jungsu));
	else if (object == gosu.button)	  enterScene(ingame_init(gosu));
	else if (object == main_button) {
		enterScene(main_screen);
		hideObject(main_button); 
		hideObject(player);
		for (int i = 0; i < 25921; i++) {
			hideObject(wall[i]);
			wall[i] = 0;
		}
		for (int i = 0; i < type.map_size; i++) {
			for (int j = 0; j < type.map_size; j++) {
				map[i][j] = 0;
			}
		}
		for (int i = 0; i < 4; i++) hideObject(thumbs_up[i]);
	}
}

SceneID init() {
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setKeyboardCallback(keyCallback);
	setMouseCallback(mouseCallback);
	
	main_screen = createScene("main", "Images/main_screen.png");
	ingame_screen = createScene("ingame", "Images/ingame_screen.png");
	
	chobo.button =  createObject("Images/chobo_button.png",  main_screen, 520, 310, true);
	jungsu.button = createObject("Images/jungsu_button.png", main_screen, 520, 180, true);
	gosu.button =   createObject("Images/gosu_button.png",   main_screen, 520, 50,  true);

	main_button = createObject("Images/main_button.png", ingame_screen, 80, 150, false);

	thumbs_up[0] = createObject("Images/thumbs_up0.png", ingame_screen, 0,   0,   false);
	thumbs_up[1] = createObject("Images/thumbs_up1.png", ingame_screen, 0,   570, false);
	thumbs_up[2] = createObject("Images/thumbs_up2.png", ingame_screen, 980, 0,   false);
	thumbs_up[3] = createObject("Images/thumbs_up3.png", ingame_screen, 980, 570, false);
	
	return main_screen;
}

int main() {
	startGame(init());
}