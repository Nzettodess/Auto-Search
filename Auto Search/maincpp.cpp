#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <thread>
#include <vector>
#include <Windows.h>

std::string getChromeCommand() {
    // Replace "chrome" with the appropriate command for launching Chrome on your system
    return "start chrome";
}

std::string getRandomFilePath(const std::vector<std::string>& filePaths) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, static_cast<int>(filePaths.size()) - 1);
    return filePaths[dist(gen)];
}

std::vector<std::string> readUsedFiles(const std::string& usedFilePath) {
    std::vector<std::string> usedFiles;
    std::ifstream usedFile(usedFilePath);

    if (usedFile.is_open()) {
        std::string fileName;
        while (usedFile >> fileName) {
            usedFiles.push_back(fileName);
        }

        usedFile.close();
    }

    return usedFiles;
}

void writeUsedFiles(const std::string& usedFilePath, const std::vector<std::string>& usedFiles) {
    std::ofstream usedFile(usedFilePath);

    if (usedFile.is_open()) {
        for (const auto& fileName : usedFiles) {
            usedFile << fileName << "\n";
        }

        usedFile.close();
    }
}

// Function to simulate mouse click at a specific position
void simulateMouseClick(int x, int y) {
    SetCursorPos(x, y);
    mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));  // Add a small delay before mouse up
    mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);
}

int main() {
    std::vector<std::string> fileList = { "wordlist1.txt", "wordlist2.txt", /* add more file paths as needed */ };
    std::string usedFilePath = "used_wordlists.txt";
    std::string chromeCommand = getChromeCommand();

    std::vector<std::string> usedFiles = readUsedFiles(usedFilePath);

    std::vector<std::string> availableFiles;
    for (const auto& file : fileList) {
        if (std::find(usedFiles.begin(), usedFiles.end(), file) == usedFiles.end()) {
            availableFiles.push_back(file);
        }
    }

    if (availableFiles.empty()) {
        std::cout << "No available word lists. All lists have been used." << std::endl;
        return 0;
    }

    std::string filePath = getRandomFilePath(availableFiles);
    std::ifstream inputFile(filePath);

    if (!inputFile.is_open()) {
        std::cerr << "Error opening the file: " << filePath << std::endl;
        return 1;
    }

    usedFiles.push_back(filePath);
    writeUsedFiles(usedFilePath, usedFiles);

    std::vector<std::string> searchWords;
    std::string word;
    while (inputFile >> word) {
        searchWords.push_back(word);
    }

    inputFile.close();

    std::string commandTemplate = chromeCommand + " https://www.bing.com/search?q=";

    // Number of times to repeat the actions
    int repeatCount = 5;

    for (int repeat = 0; repeat < repeatCount; ++repeat) {
        for (const auto& word : searchWords) {
            std::string fullCommand = commandTemplate + word;
            system(fullCommand.c_str());

            std::cout << "Searching for: " << word << std::endl;

            // Pause to avoid immediately closing the browser
            std::this_thread::sleep_for(std::chrono::seconds(5));

            // Save the initial mouse position
            POINT initialPos;
            GetCursorPos(&initialPos);

            // Simulate mouse click on the search bar
            simulateMouseClick(558, 140);  // Coordinates of the search bar

            // Introduce a delay to allow time for the suggested pane to appear
            std::this_thread::sleep_for(std::chrono::seconds(5));

            // Vector of suggested fields
            std::vector<std::pair<int, int>> suggestedFields = {
                {644, 192}, {647, 288}, {775, 331},
                {588, 380}, {715, 420}, {710, 456}
            };

            // Shuffle the vector of suggested fields to add randomness
            std::random_device rd;
            std::mt19937 gen(rd());
            std::shuffle(suggestedFields.begin(), suggestedFields.end(), gen);

            // Choose one random field and simulate mouse click
            simulateMouseClick(suggestedFields[0].first, suggestedFields[0].second);

            // Introduce a delay between clicks
            std::this_thread::sleep_for(std::chrono::seconds(5));

            // Go back to the initial position
            simulateMouseClick(558, 140);

            // Wait for 4 seconds before the next iteration
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }

        // Wait for 4 seconds before the next repeat
        std::this_thread::sleep_for(std::chrono::seconds(4));
    }

    return 0;
}
