\
# CS3307 File Manager - Build Script (Makefile)
# Name: Shikha Patel
# Date: Jan 2026, Last Modified: Feb 2026

CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic $(shell wx-config --cxxflags)
LDFLAGS := $(shell wx-config --libs)

OBJS := MainApp.o FileManagerFrame.o FileSystemService.o VirtualClipboard.o DateUtil.o
TARGET := filemanager

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)

MainApp.o: MainApp.cpp MainApp.h FileManagerFrame.h
	$(CXX) $(CXXFLAGS) -c MainApp.cpp

FileManagerFrame.o: FileManagerFrame.cpp FileManagerFrame.h FileSystemService.h VirtualClipboard.h
	$(CXX) $(CXXFLAGS) -c FileManagerFrame.cpp

FileSystemService.o: FileSystemService.cpp FileSystemService.h DateUtil.h
	$(CXX) $(CXXFLAGS) -c FileSystemService.cpp

VirtualClipboard.o: VirtualClipboard.cpp VirtualClipboard.h
	$(CXX) $(CXXFLAGS) -c VirtualClipboard.cpp

DateUtil.o: DateUtil.cpp DateUtil.h
	$(CXX) $(CXXFLAGS) -c DateUtil.cpp

clean:
	rm -f $(OBJS) $(TARGET)
