#include<iostream>
#include<cmath>
#include <sstream>
#include<map>
#include<Windows.h>
using namespace std;

#pragma region Constants
const int WIDTH = 60;
const int HEIGHT = 50;
const float CUBE_SIZE = 10;
const float CUBE_SCALE = 20;
const float INTERVAL = 0.3f;
const char BACKGROUND_CHARACTER = '0';

const float ROTATE_X_DELTA = 0.05f;
const float ROTATE_Y_DELTA = 0.05f;
const float ROTATE_Z_DELTA = 0.05f;

const float CAMERA_DISTANCE = CUBE_SIZE * 3;
const int UPDATE_PREQUENCY = 12;

const map<char, string> match_face_color = {
    {'0', "\033[30;40m"},
    {'1', "\033[31;41m"},
    {'2' , "\033[32;42m"},
	{'3', "\033[33;43m"},
	{'4', "\033[34;44m"},
    {'5', "\033[35;45m"},
    {'6', "\033[36;46m"},
};
#pragma endregion

float currentX = 0;
float currentY = 0;
float currentZ = 0;

float z_buffer[WIDTH * HEIGHT];
char buffer[WIDTH * HEIGHT];

struct Pos
{
    float x;
    float y;
    float z;
};

#pragma region RotateFunction
Pos RotateZ(Pos pos, float theta)
{
    float x = pos.x * cos(theta) - pos.y * sin(theta);
    float y = pos.x * sin(theta) + pos.y * cos(theta);

    return {x, y, pos.z};
}

Pos RotateX(Pos pos, float theta)
{
    float y = pos.y * cos(theta) - pos.z * sin(theta);
    float z = pos.y * sin(theta) + pos.z * cos(theta);

    return {pos.x, y, z};
}

Pos RotateY(Pos pos, float theta)
{
    float x = pos.x * cos(theta) - pos.z * sin(theta);
    float z = pos.x * sin(theta) + pos.z * cos(theta);

    return {x, pos.y, z};
}
#pragma endregion



void Calculate(Pos pos, char faceNumber)
{
    //Rotate
    pos = RotateX(pos, currentX);
    pos = RotateY(pos, currentY);
    pos = RotateZ(pos, currentZ);
    pos.z += CAMERA_DISTANCE;

    //Projection
    float ooz = 1 / pos.z;
    int x = static_cast<int>(ooz * CUBE_SCALE * pos.x * 2 + WIDTH / 2);
    int y = static_cast<int>(ooz * CUBE_SCALE * pos.y + HEIGHT / 2);
    int index = y * WIDTH + x;
    if (index < 0 || index >= WIDTH * HEIGHT)
        return;

    if (ooz > z_buffer[index]) {

        z_buffer[index] = ooz;
        buffer[index] = faceNumber;
    }
}

int main()
{

    while (true)
    {
        //init state
        memset(z_buffer, 0, WIDTH * HEIGHT * sizeof(int));
        memset(buffer, BACKGROUND_CHARACTER, WIDTH * HEIGHT);

        //add theta
        currentX += ROTATE_X_DELTA;
        currentY += ROTATE_Y_DELTA;
        currentZ += ROTATE_Z_DELTA;

        //calculate at all position
        for (float i = -CUBE_SIZE; i <= CUBE_SIZE; i += INTERVAL)
        {
            for (float j = -CUBE_SIZE; j <= CUBE_SIZE; j += INTERVAL)
            {
                Calculate({i, j, CUBE_SIZE}, '1');
                Calculate({i, j, -CUBE_SIZE}, '2');
                Calculate({i, CUBE_SIZE, j}, '3');
                Calculate({i, -CUBE_SIZE, j}, '4');
                Calculate({CUBE_SIZE, i, j}, '5');
                Calculate({-CUBE_SIZE, i, j}, '6');
            }
        }

        //output
        ostringstream output;
        char before = 127;
        for (int i = 0; i < WIDTH * HEIGHT; i++)
        {
            if (before != buffer[i]) {
                
                output << match_face_color.at(buffer[i]);
                before = buffer[i];
            }
            output << buffer[i];
            if ((i + 1) % WIDTH == 0) {

                output << "\033[0m\n";
                before = 127;
            }
        }

        cout << output.str();
        Sleep(UPDATE_PREQUENCY);
        system("cls");
    }
}
