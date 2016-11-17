.PHONY: build run

all: build run

build:
	xcodebuild -configuration Debug -project xcode/pollen_project.xcodeproj/

run:
	./xcode/build/Debug/pollen_project.app/Contents/MacOS/pollen_project
