#include <iostream>
#include <windows.h>
#include <cstdio>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <fstream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <opencv2/videoio.hpp>

struct Change
{
	short x, y;
	char ch;
};

typedef long long llong;

void readFrame(cv::Mat &src, std::string &dest, const cv::Size &resize)
{
	const static char *CH_LIST = " .=I@";
	cv::resize(src, src, resize);
	auto pix = src.begin<cv::Vec3b>();
	for (int row = 0, height = resize.height, width = resize.width; row < height; row++)
	{
		for (int col = 0; col < width; col++, ++pix)
			dest.push_back(CH_LIST[(int)cv::mean(*pix)[0] * 4 / 255]);
		dest += "\n";
	}
}

int main(int argc, char **argv)
{
	if (argc <= 1)
	{
		std::cout << "Usage: BadApple.exe videoFile consoleWidthInChars";
		return 0;
	}

	namespace sch = std::chrono;
	using sc = sch::steady_clock;

	const llong FRAME_DURATION = 1000000 / 15;	// in microseconds

	// so debug info won't gunk up console window for preferable console visualizer
	cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);

	cv::VideoCapture badAppleVid(argv[1]);

	if (!badAppleVid.isOpened())
	{
		std::cout << "Could not open file badapple.mp4";
		return -1;
	}

	const int CONSOLE_WIDTH_CHAR = atoi(argv[2]);
	const int VID_WIDTH = (int)badAppleVid.get(cv::CAP_PROP_FRAME_WIDTH),
		VID_HEIGHT = (int)badAppleVid.get(cv::CAP_PROP_FRAME_HEIGHT),
		VID_WIDTH_CONSOLE = CONSOLE_WIDTH_CHAR,
		VID_HEIGHT_CONSOLE = VID_HEIGHT * CONSOLE_WIDTH_CHAR / (VID_WIDTH * 2),
		FRAME_SKIP = (int)badAppleVid.get(cv::CAP_PROP_FPS) / 15,
		TOTAL_FRAMES = (int)badAppleVid.get(cv::CAP_PROP_FRAME_COUNT) / FRAME_SKIP;
	const auto CONSOLE_FRAME_SIZE = cv::Size(VID_WIDTH_CONSOLE, VID_HEIGHT_CONSOLE);

	//badAppleVid.set(cv::CAP_PROP_POS_FRAMES, 60 * 30);

	std::vector<std::vector<Change>> changes;

	cv::Mat vidFrame;
	std::string firstFrame;

	badAppleVid >> vidFrame;
	readFrame(vidFrame, firstFrame, CONSOLE_FRAME_SIZE);

	for (int i = 0; i < FRAME_SKIP; i++)
		badAppleVid >> vidFrame;

	std::string lastFrame = firstFrame;
	// for progress counting
	int frameCount = 1, lastProgressMod = 0;
	while (!vidFrame.empty())
	{
		changes.push_back(std::vector<Change>());

		std::string thisFrame;
		readFrame(vidFrame, thisFrame, CONSOLE_FRAME_SIZE);

		auto &thisChanges = changes.back();
		for (int i = 0; i < thisFrame.length(); i++)
			if (thisFrame[i] != lastFrame[i])
				thisChanges.push_back({ (short)(i % (VID_WIDTH_CONSOLE + 1)), (short)(i / (VID_WIDTH_CONSOLE + 1)), thisFrame[i] });

		lastFrame = thisFrame;

		for (int i = 0; i < FRAME_SKIP; i++)
			badAppleVid >> vidFrame;

		// progress output
		const int progress = ++frameCount * 100 / TOTAL_FRAMES;
		if (progress % 10 == 0 && progress / 10 != lastProgressMod)
		{
			lastProgressMod = progress / 10;
			std::cout << progress << "%\n";
		}
	}


	std::cout << "Done processing!";
	std::this_thread::sleep_for(std::chrono::milliseconds(1500));

	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	// clear screen
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hOut, &csbi);
	FillConsoleOutputCharacter(hOut, (TCHAR)' ', csbi.dwSize.X * csbi.dwSize.Y, { 0, 0 }, &cCharsWritten);

	std::ios_base::sync_with_stdio(false);

	SetConsoleCursorPosition(hOut, { 0, 0 });
	std::cout << firstFrame;

	int frame = 0;
	auto before = sc::now();
	lastFrame = firstFrame;
	while (frame < TOTAL_FRAMES - 1)
	{
		auto after = sc::now();
		auto dt = sch::duration_cast<sch::microseconds>(after - before).count();

		bool drawNewFrame = false;
		if (drawNewFrame = (dt >= FRAME_DURATION))
			before = after;

		int numFramesGoThrough = 0;
		while (dt >= FRAME_DURATION)
		{
			dt -= FRAME_DURATION;

			SetConsoleCursorPosition(hOut, { 0, 0 });

			numFramesGoThrough++;
			frame++;

			if (frame >= TOTAL_FRAMES) break;
		}

		if (drawNewFrame)
		{
			int startFrame = frame - numFramesGoThrough;

			for (int i = startFrame; i < frame; i++)
				for (const auto &change : changes[i])
				{
					SetConsoleCursorPosition(hOut, { change.x, change.y });
					std::cout << change.ch;
				}
		}
	}

	SetConsoleCursorPosition(hOut, { 0, 0 });
	std::cout << "The End";
	std::cin.get();

	return 0;
}