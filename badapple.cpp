#include <iostream>
#include <Windows.h>
#include <cstdio>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <fstream>
#include "test.h"

struct Change
{
	short x, y;
	char ch;
};

int main()
{
	const int FRAME_DURATION = 1000 / 30;
/*
	std::vector<std::vector<Change>> changes;
	std::ifstream changeFile("changes.txt");

	int amtChanges;
	while (changeFile >> amtChanges)
	{
		changes.push_back(std::vector<Change>(amtChanges));
		auto &curFrame = changes.back();
		for (int i = 0; i < amtChanges; i++)
		{
			Change change;
			changeFile >> change.x >> change.y >> change.ch;
			if (change.ch == 'd') change.ch = ' ';
			curFrame[i] = change;
		}
	}
*/

	HANDLE hOut;
	COORD Position;

	hOut = GetStdHandle(STD_OUTPUT_HANDLE);


	std::ios_base::sync_with_stdio(false);


	//int frame = 0;
	//const int len = changes.size();
	//while (frame < len)
	//{
	//	for (const auto &frame : changes[frame])
	//	{
	//		Position.X = frame.x;
	//		Position.Y = frame.y;
	//		SetConsoleCursorPosition(hOut, Position);
	//		std::cout << frame.ch;
	//	}

	//	frame++;
	//	//std::cout << frames::list[0];
	//	std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_DURATION));
	//}


	int frame = 0;
	const int len = a.size();

	using ch = std::chrono::steady_clock;

	auto before = ch::now();
	while (frame < len)
	{
		Position.X = 0;
		Position.Y = 0;
		SetConsoleCursorPosition(hOut, Position);
		std::cout << a[frame];
		frame++;
		std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_DURATION));
	}
	std::cin.get();

	return 0;
}