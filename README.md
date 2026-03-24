# Filamenta
A program that simulates formation of filaments from densities.

![](https://github.com/AgiGames/Filamenta/blob/main/images/nth_iteration.png)

### Prerequisites
- **macOS** with [Homebrew](https://brew.sh) installed (Apple Silicon & Intel supported)

- Install raylib:

    ```
    brew install raylib
    ```

### Build & Run

- Clone the repository.

    ```
    git clone git@github.com:AgiGames/Filamenta.git
    cd Filamenta
    git checkout macos-support
    ```

- Build and run:

    ```
    make run
    ```

- Build as a macOS app (double-clickable `.app` bundle):

    ```
    make app
    open Filamenta.app
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
