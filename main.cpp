#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>

const int MAP_WIDTH = 2;
const int MAP_HEIGHT = 2;
const int ENEMIES_QUANTITY = 2;
const int PLAYER_START_X = 0;
const int PLAYER_START_Y = 0;
const int ENEMY_HEALTH_MIN = 50;
const int ENEMY_HEALTH_MAX = 150;
const int ENEMY_ARMOR_MIN = 0;
const int ENEMY_ARMOR_MAX = 50;
const int ENEMY_DAMAGE_MIN = 15;
const int ENEMY_DAMAGE_MAX = 30;

enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

struct Character
{
    int x;
    int y;
    bool isPlayer;
    bool isDead = false;
    std::string name = "Unknown";
    int health = 100;
    int armor = 0;
    int damage = 1;

    /**
     * @struct Character with ability to move and attack.
     * @param[in] x coordinate.
     * @param[in] y coordinate.
     * @param[in] isPlayer flag for player controlled character.
     */
    Character(int x, int y, bool isPlayer)
    {
        this->x = x;
        this->y = y;
        this->isPlayer = isPlayer;
    }

 /**
  * @method Move character or attack / stay on place if new place is busy.
  * @param moveRandomly[in] if true the movement direction is random (independent from dir argument).
  * @param characters List of all game characters in game.
  * @param dir Direction for movement. Default value is UP.
  */
    void move(bool moveRandomly, std::vector<Character>& characters, Direction dir = UP)
    {
        checkIfDead();
        if (isDead) return;

        int newX = x;
        int newY = y;

        if (moveRandomly)
            dir = (Direction)(rand() % 5);

        switch(dir)
        {
            case UP:
                --newY;
                break;
            case DOWN:
                ++newY;
                break;
            case RIGHT:
                ++newX;
                break;
            case LEFT:
                --newX;
                break;
        }
        if (newX < 0 || newX > MAP_WIDTH - 1 || newY < 0 || newY > MAP_HEIGHT - 1)
            return;
        if (cellIsFree(newX, newY, characters))
        {
            x = newX;
            y = newY;
            std::cout << "\t" << name << " is moved on " << x << " : " << y << "\n";
        }
    }

    /**
     * @method Attack the target character.
     * @param[in] target Character to be damaged.
     */
    void attack(Character& target)
    {
        std::cout << "\t" << name << " attacks " << target.name << " on " << damage << "\n";
        target.armor -= this->damage;
        if (target.armor <= 0)
        {
            target.health += target.armor;
            target.armor = 0;
        }
        std::cout << "\t" << target.name << ": armor is " << target.armor << "\n";
        std::cout << "\t" << target.name << ": health is " << target.health << "\n";
        target.checkIfDead();
        if (target.isDead) std::cout << "\t" << target.name << " is dead. Rest in piece.\n";
    }

    /**
     * If health <= 0 set isDead flag to true.
     */
    void checkIfDead()
    {
        if (health <= 0)
        {
            isDead = true;
        }
    }

    /**
     * @Method
     * Check if coords are free. If not - attack (enemy) or stay on place (friend).
     * @param[in] xCoord coordinate to be checked.
     * @param[in] yCoord coordinate to be checked.
     * @param[in][out] characters vector of all game characters.
     * @return true if cell if free or false if cell is busy.
     */
    bool cellIsFree(int xCoord, int yCoord, std::vector<Character>& characters)
    {
        for (Character& c : characters)
        {
            if (c.x == xCoord && c.y == yCoord && !c.isDead)
            {
                if (this->isPlayer != c.isPlayer)
                {
                    attack(c);
                }
                return false;
            }
        }
        return true;
    }
};

/**
 * @function Prints the map array in console.
 * @param[in] map array[int][int] whose values are printed.
 */
void printMap(char (&map)[MAP_HEIGHT][MAP_WIDTH])
{
    for (int i = 0; i < MAP_HEIGHT; ++i)
    {
        for (int j = 0; j < MAP_WIDTH; ++j)
        {
            std::cout << map[i][j] << " ";
        }
        std::cout << '\n';
    }
}

/**
 * @function Puts all characters on map.
 * @param[out] map Array to be placed on.
 * @param[in] characters Vector with all game characters.
 */
void putCharactersOnMap(char (&map)[MAP_HEIGHT][MAP_WIDTH], std::vector<Character>& characters)
{
    for (const Character& c : characters)
    {
        if (!c.isDead)
            map[c.y][c.x] = (c.isPlayer ? 'P' : 'E');
    }
}

/**
 * @function Set all map array value to ' . '.
 * @param[in] map array[int][int] whose values are reset.
 */
void resetMap(char (&map)[MAP_HEIGHT][MAP_WIDTH])
{
    for (int i = 0; i < MAP_HEIGHT; ++i)
    {
        for (int j = 0; j < MAP_WIDTH; ++j)
        {
            map[i][j] = '.';
        }
    }
}

/**
 * @function Generate random x and y.
 * @param[out] x Random coord
 * @param[out] y Random coord
 * @param[in] characters Vector of all game characters.
 */
void generateRandomCoords(int& x, int& y, std::vector<Character> characters)
{
    if ((MAP_WIDTH * MAP_HEIGHT - 1) < ENEMIES_QUANTITY)
    {
        std::cerr << "Number of enemies is bigger than free cells on map.\n";
        return;
    }

    bool isFree;
    int preliminaryX;
    int preliminaryY;
    do
    {
        isFree = true;
        preliminaryX = rand() % MAP_WIDTH;
        preliminaryY = rand() % MAP_HEIGHT;
        for (Character& c : characters)
        {
            if (c.x == preliminaryX && c.y == preliminaryY)
            {
                isFree = false;
                break;
            }
        }
    } while (!isFree);
    x = preliminaryX;
    y = preliminaryY;
}

/**
 * @function Get integer from std::cin stream.
 * @param[in] label std::string to be printed before std::cin stream opening.
 * @returns value after std::stoi() conversion of std::cin stream.
 * @throws exception std::invalid_argument Thrown if no std::cin conversion could be performed.
 */
int getIntFromCin(std::string label)
{
    do
    {
        std::cout << label << ":\n";
        std::string buffer;
        std::getline(std::cin, buffer);
        try
        {
            int value = std::stoi(buffer);
            return value;
        }
        catch (const std::invalid_argument& exception)
        {
            std::cerr << "GetIntFromString: invalid argument exception.\n";
        }
    } while (true);
}

/**
 * @function Get std::string from std::cin stream.
 * @param[in] label std::string to be printed before std::cin stream opening.
 * @returns value after reading of std::cin stream.
 */
std::string getStringFromCin(std::string label)
{
    std::cout << label << ":\n";
    std::string buffer;
    std::getline(std::cin, buffer);

    std::string value = buffer;
    return value;
}

void load(std::vector<Character>& characters)
{

}

int main()
{
    std::srand(time(nullptr));
    //1 Initialize map and game characters vector:
    std::cout << "--- Initializing ---\n";
    char map[MAP_HEIGHT][MAP_WIDTH];
    resetMap(map);
    std::vector<Character> gameCharacters;

    //2. Create player
    std::cout << "--- Create your character ---\n";
    Character* player = new Character(PLAYER_START_X, PLAYER_START_Y, true);
    player->name = getStringFromCin("Please enter your name");
    player->health = getIntFromCin("Please enter your health");
    player->armor = getIntFromCin("Please enter your armor");
    player->damage = getIntFromCin("Please enter your damage");
    gameCharacters.push_back(*player);
    delete player;

    //3. Create gameCharacters
    std::cout << "--- Creating other game characters ---\n";

    for (int i = 1; i <= ENEMIES_QUANTITY; ++i)
    {

        int x = 0;
        int y = 0;
        generateRandomCoords(x, y, gameCharacters);
        Character* enemy = new Character(x, y, false);
        enemy->name = "Enemy#" + std::to_string(i);
        enemy->health = rand() % (ENEMY_HEALTH_MAX - ENEMY_HEALTH_MIN + 1) + ENEMY_HEALTH_MIN;
        enemy->armor = rand() % (ENEMY_ARMOR_MAX - ENEMY_ARMOR_MIN + 1) + ENEMY_ARMOR_MIN;
        enemy->damage = rand() % (ENEMY_DAMAGE_MAX - ENEMY_DAMAGE_MIN + 1) + ENEMY_DAMAGE_MIN;
        gameCharacters.push_back(*enemy);
        delete enemy;
        enemy = 0;
    }

    //5. Start loop
    std::cout << "--- Starting game ---\n";
    while (true)
    {
        resetMap(map);
        putCharactersOnMap(map, gameCharacters);
        printMap(map);

        //5.1 Ask for action
        int next_x, next_y;
        std::string command = getStringFromCin("Please enter the command\n(up / down / left / right / exit");
        Direction dir;
        if (command == "exit")  break;
        else if (command == "up")
        {
            dir = UP;
        }
        else if (command == "down")
        {
            dir = DOWN;
        }
        else if (command == "left")
        {
            dir = LEFT;
        }
        else if (command == "right")
        {
            dir = RIGHT;
        }
        else if (command == "save")
        {
            //save(gameCharacters);
        }
        else if (command == "load")
        {
            load(gameCharacters);
        }
        else
        {
            std::cerr << "Bad command. Try again.\n";
            continue;
        }

        //Update characters:
        for (Character& c : gameCharacters)
        {
            c.move (!c.isPlayer, gameCharacters, dir);
        }

        //Check if victory:
        int deadEnemiesCount = 0;
        bool playerIsDead = false;
        for (Character& c : gameCharacters)
        {
            if (!c.isPlayer) {
                if (c.isDead) ++deadEnemiesCount;
            }
            else
            {
                if (c.isDead) playerIsDead = true;
                break;
            }
        }
        if (playerIsDead || deadEnemiesCount == ENEMIES_QUANTITY)
        {
            if (playerIsDead) std::cout << "DEFEAT...\n";
            else std::cout << "VICTORY!\n";
            break;
        }
    }

    return 0;
}