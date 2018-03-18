# gameEngine

Well, not a game engine yet. Right now it's just a sandbox for my OpenGL experiments. Since this project is very small right now, there are no libraries I've written and the ugly code simply resides in one ugly file.  
However, you can run it to see at least something on screen.



#### Build and run:

    mkdir build
    cd build
    cmake ..
    make
    ./gameEngine
    
    
#### Dependencies
- GLM
- GLEW
- SDL2
- Boost (will be removed once `std::filesystem` is supported by most compilers)
