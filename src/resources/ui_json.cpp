#include "ui_json.hpp"

#include <iostream>     // std::ios, std::istream, std::cout
#include <fstream>      // std::filebuf
#include <stdio.h>      // std::filebuf

#include "../ui/uielement.hpp"
#include "../ui/window.hpp"

UI_Resource::UI_Resource() {
    printf("this\n");
    this->element = new UiWindow;
    this->element->resource = this;
}

UI_Resource::UI_Resource(const char *type) {
    if(strcmp("window", type) == 0){
        this->element = new UiWindow;
    } else {
        this->element = new UiElement;
    }
    this->element->resource = this;
}

UI_Resource::~UI_Resource() {
    if(this->element != nullptr){
        delete this->element;
    }
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
    
    this->setByJson(jsonObj); 
    printf("resource::load mem: %p width:%d\n", &this->element, this->w);
    /*
    for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
        std::cout << it.key() << " : " << it.value() << "\n";
    }
    */

    return 1; 
}


void UI_Resource::setByJson(nlohmann::json &json){
    nlohmann::json jsonObj = json;
    try {
        this->x = jsonObj["x"].get<float>();
        this->y = jsonObj["y"].get<float>();
        this->w = jsonObj["w"].get<unsigned int>();
        this->h = jsonObj["h"].get<unsigned int>();
        
        if(jsonObj["color"].is_object()){
            this->color = glm::vec3(
                jsonObj["color"]["r"].get<float>(),
                jsonObj["color"]["g"].get<float>(),
                jsonObj["color"]["b"].get<float>()
            );
        }
        printf("setByJson\n");
        if(jsonObj["childs"].is_array()){
            lprintf(LOG_WARNING, "child exists");
            for (auto it = jsonObj["childs"].begin(); it != jsonObj["childs"].end(); ++it){
                const char *type = it.value()["type"].get<std::string>().c_str();

                UI_Resource *r = new UI_Resource(type);

                this->element->addElement(r->element);
                printf("%d\n", this->element->getElements().size());

                r->setByJson(it.value());
                //printf("WTFFFFFFF %f, ", it.value()["x"].get<float>());
            }
        }

    }catch(std::domain_error &e){
        lprintf(LOG_WARNING, e.what());
    }
}
