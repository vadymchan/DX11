to build project:
 - init submodules: git submodule update --init --recursive
 - build submodule assimp and DirectXTK (located in engine\general\dependencies)
 - add assimp dll file 
 - place assimp dll file, which is located in assimp\bin directory to the $(Target) directory of the solution  
 - go to the "project" property pages and in Configuration Properties\Debugging change working directory to $(SolutionDir)
 Run project   