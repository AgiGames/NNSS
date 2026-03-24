# Filamenta
A program that simulates formation of filaments from densities.

![](https://github.com/AgiGames/Filamenta/blob/main/images/nth_iteration.png)

### Prerequisites
- Development Environment

    - **Linux** — Ubuntu or any distro with `apt` (other package managers work too).
    - **Windows** — Use WSL:
      
        ```
        wsl --install
        ```
        
        or
      
        ```
        wsl --install ubuntu
        ```

    - **macOS** — Requires [Homebrew](https://brew.sh). Tested on Apple Silicon & Intel.

- Libraries

    **Linux:**

    Install git, cmake & build tools.
  
    ```
    sudo apt update
    sudo apt install build-essential git cmake
    ```

    Install X11 developer libraries.
  
    ```
    sudo apt install libx11-dev libxrandr-dev libxi-dev libxcursor-dev libxinerama-dev libgl1-mesa-dev
    ```

    Clone & build raylib (graphics library).
  
    ```
    git clone https://github.com/raysan5/raylib.git
    cd raylib
    mkdir build && cd build
    cmake ..
    make -j$(nproc)
    sudo make install
    sudo ldconfig
    ```

    **macOS:**

    Install raylib via Homebrew.

    ```
    brew install raylib
    ```

### Build & Run Application
- Clone the repository.

    ```
    git clone git@github.com:AgiGames/Filamenta.git
    cd Filamenta
    ```

- Build and run using the Makefile (works on both Linux and macOS).

    ```
    make run
    ```

- Or build and run manually:

    **Linux:**
    ```
    gcc main.c grid/grid.c helper/helper.c -o filamenta -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 && ./filamenta
    ```

    **macOS:**
    ```
    gcc main.c grid/grid.c helper/helper.c -o filamenta $(pkg-config --cflags --libs raylib) -lm -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo && ./filamenta
    ```

- **macOS .app bundle** (double-clickable app):

    ```
    make app
    ```

### Controls
| Key | Action |
|-----|--------|
| `R` | Reset grid |
| `G` | Toggle grid display |
| `A` | Toggle accumulator display |
| `C` | Toggle connections display |
| `I` | Run one iteration |
| `Shift + I` | Run all iterations to convergence |
| `E` | Run Gaussian blur only |
