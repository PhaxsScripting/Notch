#pragma once 

#include <string>
#include <string_view>

class TextBuffer {
public:
    bool insert(std::string_view characters);
    bool backspace();
    const std::string& text() const;

private:
    std::string content;
};