Building & running

--- Microsoft Windows + Microsoft Visual Studio 2019

- Open TimeMapper.sln with MSVS2019 (it may ask to install the testing framework or Test project will not work if the testing framework is not included in VS installation)

- Choose the desired configuration: Debug or Release

- Launch build from the main menu: Build -> Build Solution

- Choose the project to run: App or Test ("Set as Startup project" in the context menu when right-button clicking on the specific project node in the Solution Explorer).
The input file can be set for App: Properties (from the same context menu) -> Configuration Properties -> Debugging -> Command Arguments ("<$(SolutionDir)input.txt" for example)

- Run the project from the main menu: Debug -> Start Debugging or Start Without Debugging. The built binaries also can be found in the build folder (x64) to run them from the command line

--- Ubuntu 20.4 + GCC

- To build the program itself go to src/App (inside TimeMapper) directory and then try the command line:
g++ App.cpp -std=c++17 -O3 -I../../include -o App

- Run:
./App

- To build the Test app Google Test framework is needed. On Ubuntu 20.4 it can be installed from package:
sudo apt install libgtest-dev

Other versions and distributions may require building binaries

- Then go to src/Test and try:
g++ Test.cpp -std=c++17 -O3 -I../../include -o Test -lgtest -lgtest_main -lpthread

- Run:
./Test
