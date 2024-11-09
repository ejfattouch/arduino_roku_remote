//
// Created by Edward-Joseph Fattouch on 2024-11-08.
//
#include "rokuIPDiscovery.cpp"
#include "rokuSendCommand.cpp"
#include <ncurses.h>

#define DOUBLE_PRESS_MILLISECOND_DELAY 500

using namespace std::chrono;

Mode currentMode = DIRECTIONAL;

void handleKeyUp(RokuCommandHandler* commandHandler){
    if (currentMode == DIRECTIONAL) {
        time_point<high_resolution_clock> start, end;

        start = high_resolution_clock::now();
        end = high_resolution_clock::now();

        int c = 1; // Number of times up has been pressed;
        while (end - start < milliseconds(DOUBLE_PRESS_MILLISECOND_DELAY)){
            end = high_resolution_clock::now();

            int ch = getch();

            if (ch == KEY_UP ){
                c = 2;
                break;
            }
        }

        if (c == 1) {
            commandHandler->sendRightCommand();
        }
        else {
            commandHandler->sendUpCommand();
        }
    }
    else if (currentMode == OK_BACK){
        commandHandler->sendOkCommand();
    }
    else if (currentMode == VOL){
        commandHandler->sendVolumeUp();
    }
    else if (currentMode == NETFLIX_YOUTUBE){
        commandHandler->sendNetflixCommand();
    }
}

void handleKeyDown(RokuCommandHandler* commandHandler){
    if (currentMode == DIRECTIONAL) {
        time_point<high_resolution_clock> start, end;

        start = high_resolution_clock::now();
        end = high_resolution_clock::now();

        int c = 1; // Number of times up has been pressed;
        while (end - start < milliseconds(DOUBLE_PRESS_MILLISECOND_DELAY)){
            end = high_resolution_clock::now();

            int ch = getch();

            if (ch == KEY_DOWN ){
                c = 2;
                break;
            }
        }

        if (c == 1) {
            commandHandler->sendLeftCommand();
        }
        else {
            commandHandler->sendDownCommand();
        }
    }
    else if (currentMode == OK_BACK){
        time_point<high_resolution_clock> start, end;

        start = high_resolution_clock::now();
        end = high_resolution_clock::now();

        int c = 1; // Number of times down has been pressed;
        while (end - start < milliseconds(DOUBLE_PRESS_MILLISECOND_DELAY)){
            end = high_resolution_clock::now();

            int ch = getch();

            if (ch == KEY_DOWN ){
                c = 2;
                break;
            }
        }

        if (c == 1) {
            commandHandler->sendBackCommand();
        }
        else {
            commandHandler->sendHomeCommand();
        }
    }
    else if (currentMode == VOL){
        commandHandler->sendVolumeDown();
    }
    else if (currentMode == NETFLIX_YOUTUBE){
        commandHandler->sendYouTubeCommand();
    }
}

void switchModes() {
    currentMode = static_cast<Mode>((currentMode + 1) % 4);  // Example cycle through modes
    std::cout << "Mode switched to: " << currentMode << std::endl;
}

int main() {
    std::string* rokuIP = findRokuIP();
    if (rokuIP == nullptr) {
        std::cerr << "Roku device not found." << std::endl;
        return 1;
    }

    auto* commandHandler = new RokuCommandHandler(rokuIP);

    initscr();
    keypad(stdscr, TRUE);  // Enable keypad mode for capturing special keys like arrow keys
    timeout(100);
    noecho();  // Disable echoing of input

    time_point<high_resolution_clock> start, end;
    bool isPressed = false;
    bool hold = false;

    while (true) {
        int ch = getch();  // Get the input key (non-blocking)

        // Check if UP arrow key (key code 259 in ncurses)
        if (ch == KEY_UP) {
            if (!isPressed && !hold) {
                // Start the timer when the key is first pressed
                start = high_resolution_clock::now();
                isPressed = true;
            }
            else if (isPressed) {
                // Stop the timer when the key is released
                end = high_resolution_clock::now();

                // Calculate and print duration
                auto duration = duration_cast<milliseconds>(end - start).count();
                if (duration > 1000){
                    switchModes();
                    isPressed = false;
                    hold = true;
                }
            }
        }
        else if (ch == KEY_DOWN){
            handleKeyDown(commandHandler);
            std::this_thread::sleep_for(milliseconds(100));
        }
        else {
            if (isPressed){
                handleKeyUp(commandHandler);
                std::this_thread::sleep_for(milliseconds(100));
            }

            isPressed = false;
            hold = false;
        }

        // Check for exit condition (press 'q' to quit)
        if (ch == 'q') {
            break;
        }
    }

    // Clean up ncurses
    endwin();
    return 0;
}