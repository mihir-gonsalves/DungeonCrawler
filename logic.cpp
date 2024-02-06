#include <iostream>
#include <fstream>
#include <string>
#include "logic.h"

using std::cout, std::endl, std::ifstream, std::string;

/**
 * TODO: Student implement this function
 * Load representation of the dungeon level from file into the 2D map.
 * Calls createMap to allocate the 2D array.
 * @param   fileName    File name of dungeon level.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference to set starting position.
 * @return  pointer to 2D dynamic array representation of dungeon map with player's location., or nullptr if loading fails for any reason
 * @updates  maxRow, maxCol, player
 */
char** loadLevel(const string& fileName, int& maxRow, int& maxCol, Player& player) {
    // open file
    ifstream dungeonFile;
    dungeonFile.open(fileName);

    // a) does the file get opened properly
    if (!dungeonFile.is_open()){
        cout << "Error: Unable to open file" << fileName << endl;
        return nullptr; // if file is invalid in any way, return nullptr
    }

    // b) are the row, col, player row, player col values integers
    // row
    dungeonFile >> maxRow;
    string maxRowString = std::to_string(maxRow);
    for(unsigned int i = 0; i < maxRowString.length(); i++){
        if (!isdigit(maxRowString[i])){
            cout << "Error: Non-integer value in file" << endl;
            return nullptr; // if values are not integers, return nullptr
        }
    }
    
    // col
    dungeonFile >> maxCol;
    string maxColString = std::to_string(maxCol);
    for(unsigned int i = 0; i < maxColString.length(); i++){
        if (!isdigit(maxColString[i])){
                cout << "Error: Non-integer value in file" << endl;
                return nullptr; // if values are not integers, return nullptr
        }
    }

    // player row
    dungeonFile >> player.row;
    string playerRowString = std::to_string(player.row);
    for(unsigned int i = 0; i < playerRowString.length(); i++){
        if (!isdigit(playerRowString[i])){
                cout << "Error: Non-integer value in file" << endl;
                return nullptr; // if values are not integers, return nullptr
        }
    }

    // player col
    dungeonFile >> player.col;
    string playerColString = std::to_string(player.col);
    for(unsigned int i = 0; i < playerColString.length(); i++){
        if (!isdigit(playerColString[i])){
                cout << "Error: Non-integer value in file" << endl;
                return nullptr; // if values are not integers, return nullptr
        }
    }

    // c) does maxrow * maxcol > INT32_MAX
    if ((maxRow > INT32_MAX * 1.0 / maxCol) || (maxCol > INT32_MAX * 1.0 / maxRow)){
        cout << "Error: Overflow" << endl;
        return nullptr; // if there are too many elements, return nullptr
    }

    // d) are maxCol or maxRow <= 0
    if ((maxCol <= 0 || maxRow <= 0)){
        cout << "Error: Invalid map dimensions" << endl;
        return nullptr; // if dimensions are invalid, return nullptr
    }

    // e) are player row, player col within bounds
    if (player.row < 0 || player.row >= maxRow || player.col < 0 || player.col >= maxCol){
        cout << "Error: Player position out of bounds" << endl;
        return nullptr; // if player position is out of bounds, return nullptr
    }

    // create 2D array to store dungeon level
    char** level = createMap(maxRow, maxCol);
    // set the player's starting position on the map
    level[player.row][player.col] = TILE_PLAYER;

    // f) are there the same number of rows and columns in the map as defined by the 1st line
    
    int rowCount = sizeof(*level) / sizeof(level[0]);
    int colCount = sizeof(*level[0]) / sizeof(level[0][0]);

    if (rowCount != maxRow || colCount != maxCol){
        cout << "Error: Invalid map dimensions" << endl;
        return nullptr; // if dimensions are invalid, return nullptr
    }

    // g) are all the characters in the map valid (do they have a corresponding TILE definition in logic.h)
    for (int i = 0; i < maxRow; i++) {
        for (int j = 0; j < maxCol; j++) {
            if (level[i][j] != TILE_OPEN || level[i][j] != TILE_PLAYER || level[i][j] != TILE_TREASURE || level[i][j] != TILE_AMULET || level[i][j] != TILE_MONSTER || level[i][j] != TILE_PILLAR || level[i][j] != TILE_DOOR || level[i][j] != TILE_EXIT){
                cout << "Error: Invalid map character" << endl;
                return nullptr; // if any characters are invalid, return nullptr
            }
        }
    }

    // h) is there a door/exit in the map
    bool escape = false;
    for (int i = 0; i < maxRow; i++){
        for (int j = 0; j < maxCol; j++){
            if (level[i][j] == TILE_DOOR || level[i][j] == TILE_EXIT){
                escape = true;
            }
        }
    }
    if (!escape){
        cout << "Error: No door or exit in the map" << endl;
        return nullptr; // if there is no escape, return nullptr
    }

    // close the file
    dungeonFile.close();
    // return a pointer to the 2D array
    return level;
}

/**
 * TODO: Student implement this function
 * Translate the character direction input by the user into row or column change.
 * That is, updates the nextRow or nextCol according to the player's movement direction.
 * @param   input       Character input by the user which translates to a direction.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @updates  nextRow, nextCol
 */
void getDirection(char input, int& nextRow, int& nextCol) {
    // use switch statement to determine which direction the player is moving
    switch (input){
        case MOVE_UP: // case 'w'
            nextRow--;
            break;
        case MOVE_DOWN: // case 's'
            nextRow++;
            break;
        case MOVE_LEFT: // case 'a'
            nextCol--;
            break;
        case MOVE_RIGHT: // case 'd'
            nextCol++;
            break;
        default: // default case 'e' to not move the player
            break;
    }
}

/**
 * TODO: [suggested] Student implement this function
 * Allocate the 2D map array.
 * Initialize each cell to TILE_OPEN.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @return  2D map array for the dungeon level, holds char type.
 */
char** createMap(int maxRow, int maxCol) {

    // if the input values are invalid, return nullptr
    if ((maxRow <= 0 || maxCol <= 0) || (maxRow * maxCol > INT32_MAX))
        return nullptr;

    /* initialize each elemental to TILE_OPEN */
    // allocate memory for the 2D array
    char** level = new char*[maxRow];
    // allocate memory for each row and set each element to TILE_OPEN
    for (int i = 0; i < maxRow; i++){
        level[i] = new char[maxCol];
        for (int j = 0; j < maxCol; j++){
            level[i][j] = TILE_OPEN;
        }
    }

    // return a pointer to the 2D array
    return level;
}

/**
 * TODO: Student implement this function
 * Deallocates the 2D map array.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @return None
 * @update map, maxRow
 */
void deleteMap(char**& map, int& maxRow) {
    // if the map is already nullptr
    if (map == nullptr){
        maxRow = 0;
        return;
    }

    /* if the map is not nullptr complete the rest of the function */
    // deallocate the 2D array elements pointed to by the map
    for (int i = 0; i < maxRow; i++) 
        delete[] map[i];

    // deallocate the map pointer itself
    delete[] map;

    // update maxRow to be 0
    maxRow = 0;
    // update map to be nullptr
    map = nullptr;
}

/**
 * TODO: Student implement this function
 * Resize the 2D map by doubling both dimensions.
 * Copy the current map contents to the right, diagonal down, and below.
 * Do not duplicate the player, and remember to avoid memory leaks!
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height), to be doubled.
 * @param   maxCol      Number of columns in the dungeon table (aka width), to be doubled.
 * @return  pointer to a dynamically-allocated 2D array (map) that has twice as many columns and rows in size.
 * @update maxRow, maxCol
 */
char** resizeMap(char** map, int& maxRow, int& maxCol) { 
    INFO(map);
    INFO(maxRow);
    INFO(maxCol);

    // return nullptr if the dimensions are invalid
    if (maxRow <= 0 || maxCol <= 0)
        return nullptr;
    // return nullptr if the map is nullptr
    if (map == nullptr)
        return nullptr;

    /* if the dimensions are valid and the map is not nullptr, complete the rest of the function */
    // create a new and enlarged map of the dungeon of twice the size dimensions
    char** bigLevel = createMap(maxRow * 2, maxCol * 2);

    /* Create four instances of the map as a 2x2 grid */
    // Map A (top left): is the original map and includes the player
    for (int i = 0; i < maxRow; ++i){
        for (int j = 0; j < maxCol; ++j){
            bigLevel[i][j] = map[i][j];
        }
    }
    // Map B (top right): is a copy of Map A, but shifted to the right and excludes the player tile
    for (int i = maxRow; i < maxRow * 2; i++){
        for (int j = 0; j < maxCol; j++){
            bigLevel[i][j] = map[i - maxRow][j];
            if (map[i - maxRow][j] == TILE_PLAYER)
                bigLevel[i][j] = TILE_OPEN;
        }
    }
    // Map C (bottom left): is a copy of Map A, but shifted down and excludes the player tile
    for (int i = 0; i < maxRow; i++){
        for (int j = maxCol; j < maxCol * 2; j++){
            bigLevel[i][j] = map[i][j - maxCol];
            if (map[i][j - maxCol] == TILE_PLAYER)
                bigLevel[i][j] = TILE_OPEN;
        }
    }
    // Map D (bottom right): is a copy of Map A, but shifted down and to the right and excludes the player tile
    for (int i = maxRow; i < maxRow * 2; i++){
        for (int j = maxCol; j < maxCol * 2; j++){
            bigLevel[i][j] = map[i - maxRow][j - maxCol];
            if (map[i - maxRow][j - maxCol] == TILE_PLAYER)
                bigLevel[i][j] = TILE_OPEN;
        }
    }

    // create a newMaxRow and newMaxCol to be twice the size of the original map before you delete them
    int newMaxRow = maxRow * 2;
    int newMaxCol = maxCol * 2;

    // deallocate the original array
    deleteMap(map, maxRow);
    // update maxRow and maxCol to be twice their original values
    maxRow = newMaxRow;
    maxCol = newMaxCol;
    // return the newly created map
    return bigLevel;
}

/**
 * TODO: Student implement this function
 * Checks if the player can move in the specified direction and performs the move if so.
 * Cannot move out of bounds or onto TILE_PILLAR or TILE_MONSTER.
 * Cannot move onto TILE_EXIT without at least one treasure. 
 * If TILE_TREASURE, increment treasure by 1.
 * Remember to update the map tile that the player moves onto and return the appropriate status.
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object to by reference to see current location.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @return  Player's movement status after updating player's position.
 * @update map contents, player
 */
int doPlayerMove(char** map, int maxRow, int maxCol, Player& player, int nextRow, int nextCol) {
    
    INFO(map);
    INFO(maxRow);
    INFO(maxCol);
    INFO(player.row);
    INFO(player.col);
    INFO(nextRow);
    INFO(nextCol);

    /* all if statements to set the appropriate STATUS */
    // if the next position places the player out of bounds, set the status to STATUS_STAY
    if (nextRow < 0 || nextRow >= maxRow || nextCol < 0 || nextCol >= maxCol)
        return STATUS_STAY;
    // if the next position places the player on a TILE_PILLAR or TILE_MONSTER, set the status to STATUS_STAY
    if (map[nextRow][nextCol] == TILE_PILLAR || map[nextRow][nextCol] == TILE_MONSTER)
        return STATUS_STAY;
    // if the next position places the player on a TILE_TREASURE, set the status to STATUS_TREASURE and increment the player's treasure count
    if (map[nextRow][nextCol] == TILE_TREASURE){
        player.treasure++;
        return STATUS_TREASURE;
    }
    // if the next position places the player on a TILE_AMULET, set the status to STATUS_AMULET and resize the map
    if (map[nextRow][nextCol] == TILE_AMULET){
        resizeMap(map, maxRow, maxCol);
        return STATUS_AMULET;
    }
    // if the next position places the player on a TILE_DOOR, set the status to STATUS_LEAVE
    if (map[nextRow][nextCol] == TILE_DOOR)
        return STATUS_LEAVE;
    // if the next position places the player on a TILE_EXIT and the player has no treasure, set the status to STATUS_STAY
    if (map[nextRow][nextCol] == TILE_EXIT && player.treasure == 0)
        return STATUS_STAY;
    // if the next position places the player on a TILE_EXIT and the player has treasure, set the status to STATUS_ESCAPE
    if (map[nextRow][nextCol] == TILE_EXIT && player.treasure > 0)
        return STATUS_ESCAPE;

    /* if none of the if else statments are passed over then set the status to STATUS_MOVE */
    // set the player's current position to TILE_OPEN
    map[player.row][player.col] = TILE_OPEN;
    // set the player's next position to TILE_PLAYER
    player.row = nextRow;
    player.col = nextCol;
    map[player.row][player.col] = TILE_PLAYER;
    // return the movement
    return STATUS_MOVE;
}

/**
 * TODO: Student implement this function
 * Update monster locations:
 * We check up, down, left, right from the current player position.
 * If we see an obstacle, there is no line of sight in that direction, and the monster does not move.
 * If we see a monster before an obstacle, the monster moves one tile toward the player.
 * We should update the map as the monster moves.
 * At the end, we check if a monster has moved onto the player's tile.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference for current location.
 * @return  Boolean value indicating player status: true if monster reaches the player, false if not.
 * @update map contents
 */
bool doMonsterAttack(char** map, int maxRow, int maxCol, const Player& player) {
    
    INFO(map);
    INFO(maxRow);
    INFO(maxCol);
    INFO(player.row);
    INFO(player.col);
    
    // starting from the tile above the player's location and working upward, check each individual tile to see if there is a monster on the tile
    for (int i = player.row - 1; i >= 0; i--){
        if(map[i][player.col] == TILE_PILLAR)
            break;

        if (map[i][player.col] == TILE_MONSTER){
            // if there is a monster on the tile, move the monster one tile down and set its previous tile to TILE_OPEN
            map[i][player.col] = TILE_OPEN;
            map[i + 1][player.col] = TILE_MONSTER;
            // if the player is on the tile below the monster, return true
            if (map[i + 1][player.col] == TILE_PLAYER)
                return true;
        }
    }
    // starting from the tile below the player's location and working downward, check each individual tile to see if there is a monster on the tile
    for (int i = player.row + 1; i < maxRow; i++){
        if(map[i][player.col] == TILE_PILLAR)
            break;
        
        if (map[i][player.col] == TILE_MONSTER){
            // if there is a monster on the tile, move the monster one tile up and set its previous tile to TILE_OPEN
            map[i][player.col] = TILE_OPEN;
            map[i - 1][player.col] = TILE_MONSTER;
            // if the player is on the tile above the monster, return true
            if (map[i - 1][player.col] == TILE_PLAYER)
                return true;
        }
    }
    // starting from the tile to the left of the player's location and working leftward, check each individual tile to see if there is a monster on the tile
    for (int i = player.col - 1; i >= 0; i--){
        if(map[player.row][i] == TILE_PILLAR)
            break;

        if (map[player.row][i] == TILE_MONSTER){
            // if there is a monster on the tile, move the monster one tile to the right and set its previous tile to TILE_OPEN
            map[player.row][i] = TILE_OPEN;
            map[player.row][i + 1] = TILE_MONSTER;
            // if the player is on the tile to the right of the monster, return true
            if (map[player.row][i + 1] == TILE_PLAYER)
                return true;
        }
    }
    // starting from the tile to the right of the player's location and working rightward, check each individual tile to see if there is a monster on the tile
    for (int i = player.col + 1; i < maxCol; i++){
        if(map[player.row][i] == TILE_PILLAR)
            break;
        
        if (map[player.row][i] == TILE_MONSTER){
            // if there is a monster on the tile, move the monster one tile to the left and set its previous tile to TILE_OPEN
            map[player.row][i] = TILE_OPEN;
            map[player.row][i - 1] = TILE_MONSTER;
            // if the player is on the tile
            if (map[player.row][i - 1] == TILE_PLAYER)
                return true;
        }
    }
    // if the player is not on the tile of a monster, return false
    return false;
}
