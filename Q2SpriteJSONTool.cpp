#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"
#include <vector>
#include <cstdio>
#include <filesystem>

using namespace std;
using json = nlohmann::ordered_json;
namespace fs = std::filesystem;

#define IDSPRITEHEADER   (('2'<< 24 )+('S'<< 16 )+('D'<< 8 )+'I')
#define SPRITE_VERSION   2
#define	MAX_SKINNAME	64

typedef struct
{
    int      width, height;
    int      origin_x, origin_y;
    char   name[MAX_SKINNAME];
} dsprframe_t;

typedef struct {
    int         ident;
    int         version;
    int         numframes;
    dsprframe_t   frames[1];
} dsprite_t;

void JSON2Sprite(fs::path inpath, fs::path outpath) {
    ifstream fjson(inpath);
    if (!fjson.is_open()) {
        cout << "Error: Could not open file." << endl;
        return;
    }
    try {
        json jsonsprite = json::parse(fjson);
        fjson.close();

        if (jsonsprite.at("ident").get<std::string>() == "IDS2" && jsonsprite.at("version").get<int>() == 2)
            cout << "Magic Numbers are correct." << endl;
        else {
            cout << "Magic Numbers are wrong." << endl;
            return;
        }

        vector<dsprframe_t> frames;

        for (const auto& item : jsonsprite.at("frames")) {
            dsprframe_t nuframe;
            nuframe.width = item.at("width").get<int>();
            nuframe.height = item.at("height").get<int>();
            nuframe.origin_x = item.at("origin_x").get<int>();
            nuframe.origin_y = item.at("origin_y").get<int>();
            strncpy(nuframe.name, item.at("name").get<std::string>().c_str(), MAX_SKINNAME - 1);
            nuframe.name[MAX_SKINNAME - 1] = '\0';
            frames.push_back(nuframe);
        }
        
        ofstream outSprite(outpath, ios::binary);
        outSprite.write("IDS2", 4);
        int version = SPRITE_VERSION;
        outSprite.write(reinterpret_cast<const char*>(&version), sizeof(version));
        int frameCount = static_cast<int>(frames.size());
        outSprite.write(reinterpret_cast<const char*>(&frameCount), sizeof(frameCount));
        for (const auto& frame : frames) {
            outSprite.write(reinterpret_cast<const char*>(&frame.width), sizeof(frame.width));
            outSprite.write(reinterpret_cast<const char*>(&frame.height), sizeof(frame.height));
            outSprite.write(reinterpret_cast<const char*>(&frame.origin_x), sizeof(frame.origin_x));
            outSprite.write(reinterpret_cast<const char*>(&frame.origin_y), sizeof(frame.origin_y));
            outSprite.write(frame.name, sizeof(frame.name));
        }
        outSprite.close();

        cout << "Sprite created successfully.";

    }
    catch (nlohmann::json::parse_error& e) {
        cout << "JSON Syntax Error: " << e.what() << endl;
    }
}

void Sprite2JSON(fs::path inpath, fs::path outpath) {
    ifstream inFile(inpath, ios::binary);
    if (!inFile.is_open()) {
        cout << "Error: Could not open input file." << endl;
        return;
    }

    char header[4];
    inFile.read(header, 4);
    if (memcmp(header, "IDS2", 4) != 0) {
        cout << "Error: Invalid file header." << endl;
        return;
    }

    int versionnum;
    inFile.read(reinterpret_cast<char*>(&versionnum), sizeof(versionnum));
    int frameCount;
    inFile.read(reinterpret_cast<char*>(&frameCount), sizeof(frameCount));

    json jsonsprite;
    jsonsprite["ident"] = "IDS2";
    jsonsprite["version"] = versionnum;
    jsonsprite["numframes"] = frameCount;
    jsonsprite["frames"] = json::array();

    for (int i = 0; i < frameCount; ++i) {
        dsprframe_t frame;
        inFile.read(reinterpret_cast<char*>(&frame.width), sizeof(frame.width));
        inFile.read(reinterpret_cast<char*>(&frame.height), sizeof(frame.height));
        inFile.read(reinterpret_cast<char*>(&frame.origin_x), sizeof(frame.origin_x));
        inFile.read(reinterpret_cast<char*>(&frame.origin_y), sizeof(frame.origin_y));
        inFile.read(frame.name, MAX_SKINNAME);

        json item;
        item["width"] = frame.width;
        item["height"] = frame.height;
        item["origin_x"] = frame.origin_x;
        item["origin_y"] = frame.origin_y;
        item["name"] = std::string(frame.name);
        jsonsprite["frames"].push_back(item);
    }
    inFile.close();

    ofstream outFile(outpath);
    if (outFile.is_open()) {
        outFile << jsonsprite.dump(4);
        outFile.close();
        cout << "JSON created successfully: " << outpath << endl;
    }
    else {
        cout << "Error: Could not write output file." << endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "No file provided. Please provide either a SP2 sprite, or a valid JSON file." << std::endl;
        return 1;
    }

    fs::path filePath(argv[1]);
    
    if (filePath.extension() == ".json") {
        std::cout << "Processing JSON file..." << std::endl;
        fs::path outPath = filePath;
        outPath.replace_extension(".sp2");
        JSON2Sprite(filePath, outPath);
    }
    else if (filePath.extension() == ".sp2") {
        std::cout << "Processing SP2 file..." << std::endl;
        fs::path outPath = filePath;
        outPath.replace_extension(".json");
        Sprite2JSON(filePath, outPath);
    }
    else {
        std::cout << "Unsupported file extension: " << filePath.extension() << std::endl;
    }

    return 0;
}
