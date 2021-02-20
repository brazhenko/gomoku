brew install numcpp
brew install glew
brew install glfw
brew install cmake
brew install doxygen


cd /goinfre/$USER
git clone https://github.com/google/googletest
cd googletest
mkdir build
cd build
cmake ..
make
make install