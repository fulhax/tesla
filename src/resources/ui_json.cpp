#include "ui_json.hpp"

#include <iostream>     // std::ios, std::istream, std::cout
#include <fstream>      // std::filebuf
#include <stdio.h>      // std::filebuf

#include "../../externals/json.hpp"
UI_Resource::UI_Resource() {
    
}

UI_Resource::~UI_Resource() {
    
}

int UI_Resource::load(const char *filename) {
    std::filebuf fb;
    nlohmann::json jsonObj;
    if (fb.open (filename, std::ios::in)){
        std::istream is(&fb);
        try {
            jsonObj = nlohmann::json::parse(is);
        } catch(std::invalid_argument &e) {
            lprintf(LOG_WARNING, e.what());
        }
        fb.close();
    } else {
        lprintf(LOG_WARNING, "Unable to open ^g\"%s\"^0", filename);
        return 0;   
    }
    
    try {
        this->x = jsonObj["x"].get<float>();
        this->y = jsonObj["y"].get<float>();
        this->w = jsonObj["w"].get<unsigned int>();
        this->h = jsonObj["h"].get<unsigned int>();
        lprintf(LOG_WARNING, "%d %d", this->w, this->h);
    }catch(std::domain_error &e){
        lprintf(LOG_WARNING, e.what());
    }

    for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
        std::cout << it.key() << " : " << it.value() << "\n";
    }

    return 1; 
}
