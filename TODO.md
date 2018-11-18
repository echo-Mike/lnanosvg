# Todo list
1. [ ] Add build system based on CMake
  * Use FindLua CMake module
2. [ ] Add Travis CI
  * Use multiple C compilers:
    - [ ] GCC
      * Multiple versions based on Ubuntu LTS defaults
      * Additional versions for every major GCC release
    - [ ] CLANG
      * Find out what versions to use
  * Add support for multiple unix distro
    - [ ] Ubuntu (all LTS)
    - [ ] Debian (all LTS)
    - [ ] Find out what are the other possibilities
3. [ ] Add Appveyor CI
  * Use multiple versions of MSVC
    - [ ] Add here plan for MSVC versions
  * Use multiple other compilers
    - [ ] mingw32
    - [ ] mingw64
    - [ ] msys2
    - [ ] cygwin
4. [ ] Use multiple versions of Lua with CI
  - [ ] Lua 5.1
  - [ ] Lua 5.2
  - [ ] Lua 5.3
  - [ ] LuaJit
    * Check possible versions
  - [ ] LOVE Lua
    * And combinations
5. [ ] Add support for NSVGrasterizer
  - [ ] Make Lua object for simple pixel image manipulations
    - [ ] Add piximage class
    - [ ] Add rasterizer class
  - [ ] Add support for LOVE2D love.image.ImageData objects
6. [ ] Add tests to be used in CI
  - [ ] Use nanosvg/example SVG files for tests
  - [ ] Add test in Lua for every created class
    * Test setters/getters
    * Test member functions
    * Test meta functions
      - [ ] image
      - [ ] shape
      - [ ] path
      - [ ] paint
      - [ ] gradient
      - [ ] gradientStop
      - [ ] piximage
      - [ ] rasterizer
7. [ ] Reorganize code into src and include dirs