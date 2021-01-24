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

typedef long long llong;

int main()
{
	const llong FRAME_DURATION = 1000000 / 30;	// in microseconds

	HANDLE hOut;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	std::ios_base::sync_with_stdio(false);


	int frame = 0;
	const int len = a.size();
	const int width = 22, height = 10;

	namespace sch = std::chrono;
	using sc = sch::steady_clock;

	auto before = sc::now();
	std::string lastFrame = a[0];
	while (frame < len)
	{
		auto after = sc::now();
		auto dt = sch::duration_cast<sch::microseconds>(after - before).count();

		bool drawNewFrame = false;
		if (drawNewFrame = (dt >= FRAME_DURATION))
			before = after;

		std::string curFrame = lastFrame;
		while (dt >= FRAME_DURATION)
		{
			dt -= FRAME_DURATION;

			SetConsoleCursorPosition(hOut, { 0, 0 });

			curFrame = a[frame];
			frame++;

			if (frame >= len) break;
		}

		if (drawNewFrame)
		{
			std::vector<Change> changes;
			for (int i = 0; i < curFrame.length(); i++)
				if (curFrame[i] != lastFrame[i])
					changes.push_back({ (short)(i % (width + 1)), (short)(i / (width + 1)), curFrame[i] });

			for (const auto &change : changes)
			{
				SetConsoleCursorPosition(hOut, { change.x, change.y });
				std::cout << change.ch;
			}

			lastFrame = curFrame;
		}
	}

	std::cin.get();

	return 0;
}