#include <iostream>
#include <Windows.h>
#include <cstdio>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <fstream>
#include <opencv2/core.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <opencv2/videoio.hpp>

struct Change
{
	short x, y;
	char ch;
};

typedef long long llong;

int main()
{
	const llong FRAME_DURATION = 1000000 / 30;	// in microseconds

	// so debug info won't gunk up console window for preferable console visualizer
	cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);

	cv::VideoCapture badAppleVid("badapple.mp4");

	if (!badAppleVid.isOpened())
	{
		std::cout << "Could not open file badapple.mp4";
		return -1;
	}

	const int VID_WIDTH = (int)badAppleVid.get(cv::CAP_PROP_FRAME_WIDTH),
		VID_HEIGHT = (int)badAppleVid.get(cv::CAP_PROP_FRAME_HEIGHT),
		TOTAL_FRAMES = (int)badAppleVid.get(cv::CAP_PROP_FRAME_COUNT);

	std::vector<std::vector<Change>> changes;

	cv::Mat vidFrame;

	std::string firstFrame;

	badAppleVid >> vidFrame;

	for (int row = 0; row < VID_HEIGHT / 12; row++)
	{
		for (int col = 0; col < VID_WIDTH / 6; col++)
			firstFrame += ((int)vidFrame.at<uchar>(row * 12, col * 6 * 3) == 0 ? " " : "#");
		firstFrame += "\n";
	}

	badAppleVid >> vidFrame;

	std::string lastFrame = firstFrame;
	while (!vidFrame.empty())
	{
		std::string thisFrame;

		changes.push_back(std::vector<Change>());

		for (int row = 0; row < VID_HEIGHT / 12; row++)
		{
			for (int col = 0; col < VID_WIDTH / 6; col++)
				thisFrame += ((int)vidFrame.at<uchar>(row * 12, col * 6 * 3) == 0 ? " " : "#");
			thisFrame += "\n";
		}

		auto &thisChanges = changes.back();
		for (int i = 0; i < thisFrame.length(); i++)
			if (thisFrame[i] != lastFrame[i])
				thisChanges.push_back({ (short)(i % (VID_WIDTH / 6 + 1)), (short)(i / (VID_WIDTH / 6 + 1)), thisFrame[i] });

		lastFrame = thisFrame;
		
		badAppleVid >> vidFrame;
	}


	std::cout << "Done processing!";
	std::this_thread::sleep_for(std::chrono::seconds(3));


	HANDLE hOut;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	std::ios_base::sync_with_stdio(false);

	SetConsoleCursorPosition(hOut, { 0, 0 });

	int frame = 0;
	const int width = 22, height = 10;

	namespace sch = std::chrono;
	using sc = sch::steady_clock;

	std::cout << firstFrame;

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

	std::cin.get();

	return 0;
}