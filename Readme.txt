CS5249 Guitar Sound Synthesis 
Hua Binh Son, HT081432W
huabinhs@comp.nus.edu.sg

===== Compilation instruction =====
This project uses Boost.Thread library, AntTweakBar library, GLUT/GLEW library, and STK library. You will need these libraries in order to compile the code successfully. The project comes with AntTweakBar, GLUT, and necessary STK files. Boost.Thread is not included.

Boost.Thread (header and precompiled binary) is available at:
http://www.boostpro.com/download/boost_1_42_setup.exe
Only Boost.Thread and Boost.DateTime are required. Other Boost library may take time to download if you choose to install all.

Refer to settings in Property Sheet Gosling.vsprops in VC++ for environment variables to include/link to the above libraries. My library location is set in environment variable $(SH_LIBRARY).

I have included an excutable file, Gosling.exe, for you to run.

FFTW: Go to Visual Studio 2008 command prompt. Change directory to fftw folder that contains libfftw3-3.dll and libfftw3-3.def. Type lib /def:libfftw3-3.def to create .lib file. Use this .lib file to link to fftw3 in your Visual Studio project.


===== Folder structure =====
  - output		: the synthesized sound is saved here.
  - tablature	: tablatures are saved in "tablature" folder.
  - report		: a 2-page report in PDF format.
  - matlab		: MATLAB code for mode extraction.
  
===== Usage =====
Parameters can be tweak to control the playback speed of the synthesis. AntTweakBar allows using mouse to increase/decrease parameter values when clicking and dragging circularly at the dot button.

===== Tablatures ======
Sample tablatures are from the following website:

FF9 Theme:
http://www.guitaretab.com/m/misc-computer-games/29699.html

