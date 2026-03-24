# Makefile for Filamenta

CC = gcc
CFLAGS = -Wall -std=c99 -O2
SRC = main.c grid/grid.c helper/helper.c
TARGET = filamenta

# Detect OS
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux)
	LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

ifeq ($(UNAME_S), Darwin)
	# macOS paths for Homebrew
	BREW_PATH := $(shell brew --prefix)
	CFLAGS += -I$(BREW_PATH)/include
	LDFLAGS = -L$(BREW_PATH)/lib
	LIBS = -lraylib -lm -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
	APP_NAME = Filamenta.app
endif

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)
	rm -rf $(APP_NAME)

run: $(TARGET)
	./$(TARGET)

# macOS .app bundle
ifeq ($(UNAME_S), Darwin)
app: $(TARGET)
	@echo "Creating macOS app bundle..."
	@rm -rf $(APP_NAME)
	@mkdir -p $(APP_NAME)/Contents/MacOS
	@mkdir -p $(APP_NAME)/Contents/Resources
	@cp $(TARGET) $(APP_NAME)/Contents/MacOS/filamenta
	@# Copy raylib dylib into the bundle and fix the link
	@RAYLIB_DYLIB=$$(otool -L $(TARGET) | grep libraylib | awk '{print $$1}'); \
	 if [ -n "$$RAYLIB_DYLIB" ]; then \
	   cp "$$RAYLIB_DYLIB" $(APP_NAME)/Contents/MacOS/; \
	   DYLIB_NAME=$$(basename "$$RAYLIB_DYLIB"); \
	   install_name_tool -change "$$RAYLIB_DYLIB" "@executable_path/$$DYLIB_NAME" $(APP_NAME)/Contents/MacOS/filamenta; \
	 fi
	@echo '<?xml version="1.0" encoding="UTF-8"?>' > $(APP_NAME)/Contents/Info.plist
	@echo '<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">' >> $(APP_NAME)/Contents/Info.plist
	@echo '<plist version="1.0">' >> $(APP_NAME)/Contents/Info.plist
	@echo '<dict>' >> $(APP_NAME)/Contents/Info.plist
	@echo '  <key>CFBundleName</key>' >> $(APP_NAME)/Contents/Info.plist
	@echo '  <string>Filamenta</string>' >> $(APP_NAME)/Contents/Info.plist
	@echo '  <key>CFBundleDisplayName</key>' >> $(APP_NAME)/Contents/Info.plist
	@echo '  <string>Filamenta</string>' >> $(APP_NAME)/Contents/Info.plist
	@echo '  <key>CFBundleIdentifier</key>' >> $(APP_NAME)/Contents/Info.plist
	@echo '  <string>com.agigames.filamenta</string>' >> $(APP_NAME)/Contents/Info.plist
	@echo '  <key>CFBundleVersion</key>' >> $(APP_NAME)/Contents/Info.plist
	@echo '  <string>1.0</string>' >> $(APP_NAME)/Contents/Info.plist
	@echo '  <key>CFBundleShortVersionString</key>' >> $(APP_NAME)/Contents/Info.plist
	@echo '  <string>1.0</string>' >> $(APP_NAME)/Contents/Info.plist
	@echo '  <key>CFBundleExecutable</key>' >> $(APP_NAME)/Contents/Info.plist
	@echo '  <string>filamenta</string>' >> $(APP_NAME)/Contents/Info.plist
	@echo '  <key>CFBundlePackageType</key>' >> $(APP_NAME)/Contents/Info.plist
	@echo '  <string>APPL</string>' >> $(APP_NAME)/Contents/Info.plist
	@echo '  <key>NSHighResolutionCapable</key>' >> $(APP_NAME)/Contents/Info.plist
	@echo '  <true/>' >> $(APP_NAME)/Contents/Info.plist
	@echo '  <key>NSSupportsAutomaticGraphicsSwitching</key>' >> $(APP_NAME)/Contents/Info.plist
	@echo '  <true/>' >> $(APP_NAME)/Contents/Info.plist
	@echo '</dict>' >> $(APP_NAME)/Contents/Info.plist
	@echo '</plist>' >> $(APP_NAME)/Contents/Info.plist
	@echo "✅ Created $(APP_NAME) — double-click to launch!"
endif

.PHONY: clean run app
