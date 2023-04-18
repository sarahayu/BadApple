# BadApple
Plays videos on a Windows console in real time

## Video Demos
 - [Touhou - Bad Apple!!](https://youtu.be/mRjDmoA1K0A)
 - [YUQI - Giant](https://youtu.be/-TTaTj-uSL8)
 
## Usage
Run the following command in the same directory as the `BadApple.exe` file, replacing `videoFile` with the name of your video file (I used an `mp4`, I don't know what other formats work) and `consoleWidthInChars` with the amount of characters you want the width of your video frames to be.
```bash
BadApple.exe videoFile consoleWidthInChars
```

Example (`badapple.mp4` not included in the source code):
```bash
BadApple.exe badapple.mp4 150
```

## Notes
Executable was compiled using Visual Studio on a 64-bit Windows 7 machine. Should you wish to compile the code yourself, I'm afraid I have no specific steps other than to use the Internet. This a relatively small and simple program, though, and OpenCV has good [documentation](https://docs.opencv.org/4.x/df/d65/tutorial_table_of_content_introduction.html) on how to compile its libraries. 
 
### Bugs
 Output console video does not match the speed of the original video if the source framerate is not 30 fps as the original Bad Apple video I tested the code with. I don't know if it's due to a math error or the fact that the code can't deal with fractional fps. I'll have to look into it once I have the free time to do so.
