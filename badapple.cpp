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
	short index;
	char ch;
};

void readFrame(cv::Mat &src, std::string &dest, const cv::Size &resize)
{
	const static std::string CH_LIST = []() {
		std::ifstream charList("chars.txt");
		std::string chars;
		std::getline(charList, chars);
		return chars;
	}();
	const static int CHS = CH_LIST.size() - 1;
	cv::resize(src, src, resize);
	auto pix = src.begin<cv::Vec3b>();
	for (int row = 0, height = resize.height, width = resize.width; row < height; row++)
	{
		for (int col = 0; col < width; col++, ++pix)
			dest.push_back(CH_LIST[(int)cv::mean(*pix)[0] * CHS / 255]);
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

	const long long FRAME_DURATION = 1e6 / 15;	// in microseconds

	// so debug info won't gunk up console window for preferable console visualizer
	cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);

	cv::VideoCapture badAppleVid(argv[1]);

	if (!badAppleVid.isOpened())
	{
		std::cout << "Could not open file " << argv[1];
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

	// populate frame change information
	std::string lastFrame = firstFrame;
	int frameCount = 1, lastProgressMod = 0;	// for progress counting
	while (!vidFrame.empty())
	{
		changes.push_back(std::vector<Change>());

		std::string thisFrame;
		readFrame(vidFrame, thisFrame, CONSOLE_FRAME_SIZE);

		auto &thisChanges = changes.back();
		for (int i = 0; i < thisFrame.length(); i++)
			if (thisFrame[i] != lastFrame[i])
				thisChanges.push_back({ (short)i, thisFrame[i] });

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
	DWORD charsWritten;
	CONSOLE_SCREEN_BUFFER_INFO consoleSize;
	GetConsoleScreenBufferInfo(hOut, &consoleSize);
	SetConsoleCursorPosition(hOut, { 0, 0 });

	FillConsoleOutputCharacter(hOut, (TCHAR)' ', consoleSize.dwSize.X * consoleSize.dwSize.Y, { 0, 0 }, &charsWritten);
	WriteConsoleOutputCharacter(hOut, firstFrame.c_str(), firstFrame.size(), { 0, 0 }, &charsWritten);

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

			numFramesGoThrough++;
			frame++;

			if (frame >= TOTAL_FRAMES) break;
		}

		if (drawNewFrame)
		{
			before -= sch::microseconds(dt);
			int startFrame = frame - numFramesGoThrough;

			std::string buffer = lastFrame;

			for (int i = startFrame; i < frame; i++)
				for (const auto &change : changes[i])
					buffer[change.index] = change.ch;

			lastFrame = buffer;

			// clear screen completely if console size has changed to avoid fragments after frame
			auto newSize = consoleSize;
			GetConsoleScreenBufferInfo(hOut, &newSize);
			if (newSize.dwSize.X != consoleSize.dwSize.X || newSize.dwSize.Y != consoleSize.dwSize.Y)
			{
				consoleSize = newSize;
				FillConsoleOutputCharacter(hOut, (TCHAR)' ', consoleSize.dwSize.X * consoleSize.dwSize.Y, { 0, 0 }, &charsWritten);
			}

			const int whitespace = consoleSize.dwSize.X - VID_WIDTH_CONSOLE;
			if (whitespace > 0)
			{
				for (int i = 0; i < VID_HEIGHT_CONSOLE; i++)
					buffer.insert(VID_WIDTH_CONSOLE * (i + 1) + whitespace * i, whitespace, ' ');
			}

			WriteConsoleOutputCharacter(hOut, buffer.c_str(), buffer.size(), { 0,0 }, &charsWritten);
		}
	}

	FillConsoleOutputCharacter(hOut, (TCHAR)' ', consoleSize.dwSize.X * consoleSize.dwSize.Y, { 0, 0 }, &charsWritten);
	std::cout << "The End";
	std::cin.get();

	return 0;
}