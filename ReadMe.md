To build the project:

- Open a command prompt (or any other console).
- Navigate to the root directory of the solution.
- To specify the Visual Studio generator, enter the following command: `cmake -G "Visual Studio 17 2022" .\CMakeLists.txt`. This ensures the Visual Studio generator is used, even if other generators like Ninja are installed.
- Open the 'project.sln' file generated by CMake.
- In Visual Studio, right-click on the project in the Solution Explorer and select 'Set as StartUp Project'.
- Build and run the project.

Notes:
- If you encounter issues with building the project, deleting the 'CMakeCache.txt' and 'CMakeFiles' may help. These are temporary files generated by CMake and can sometimes cause conflicts.
- The generator you specify in the CMake command needs to match the version of Visual Studio you are using. If you are using a different version of Visual Studio, replace '17 2022' with the appropriate version."
