to build project: (flexible way) 
 - init submodules: git submodule update --init --recursive
 - build submodule assimp and DirectXTK (located in engine\general\dependencies)
 - add assimp dll file 
 - place assimp dll file, which is located in assimp\bin directory to the $(Target) directory of the solution  
 - go to the "project" property pages and in Configuration Properties\Debugging change working directory to $(SolutionDir)
 Run project   

 to build project: (fast build)
  - go to engine/general/dependencies/ and run init dependencies.bat
  - add assimp dll file (either build assimp or just move existing dll from static libs in engine/general/dependencies/ to the directory, where .exe is located)

  About hw 5 funtionality:
   - to turn on/off normal visualization press N key
   - to turn on/off wireframe mode press E key