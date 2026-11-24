#include "TextBuffer.h"

bool TextBuffer::insert(std::string_view characters) {
    bool changed = false;
    for (unsigned char character : characters) {
        if (character >= 32 && character <= 126 && content.size() < 64) {
            content.push_back(character);
        }
    }
    return changed;
}

bool TextBuffer::backspace() {
    if (content.empty()) {  return false;  }
    content.pop_back();
    return true;
}

const std::string& TextBuffer::text() const {
    return content;
}