#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <vector>

struct CustomTitleBar {
    sf::RectangleShape background;
    sf::RectangleShape closeButton;
    sf::RectangleShape minimizeButton;

    CustomTitleBar(float width, float height) {
        background.setSize(sf::Vector2f(width, height));
        background.setFillColor(sf::Color(255, 255, 191, 200)); // Màu nền trong suốt
        background.setOutlineColor(sf::Color::Black);
        background.setOutlineThickness(1.f);

        float buttonSize = height * 0.6f;
        closeButton.setSize(sf::Vector2f(buttonSize, buttonSize));
        closeButton.setFillColor(sf::Color::Red);
        closeButton.setOutlineColor(sf::Color::Black);
        closeButton.setOutlineThickness(1.f);
        closeButton.setPosition(width - buttonSize * 2.f, (height - buttonSize) / 2.f);

        minimizeButton.setSize(sf::Vector2f(buttonSize, buttonSize));
        minimizeButton.setFillColor(sf::Color::Green);
        minimizeButton.setOutlineColor(sf::Color::Black);
        minimizeButton.setOutlineThickness(1.f);
        minimizeButton.setPosition(width - buttonSize * 3.5f, (height - buttonSize) / 2.f);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(background);
        window.draw(closeButton);
        window.draw(minimizeButton);
    }
};

const std::string FILE_PREFIX = "sticky_note";
const std::string FILE_EXTENSION = ".txt";
const std::string DEFAULT_CONTENT = "Type your note here...";

struct Note {
    sf::Text text;
    std::string content;
    bool isDragging;
    sf::Vector2f offset;

    Note(const sf::Font& font) {
        text.setFont(font);
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::Black);
        text.setString(DEFAULT_CONTENT);
        content = DEFAULT_CONTENT;
        isDragging = false;
    }
};

void saveNotes(const std::vector<Note>& notes) {
    for (size_t i = 0; i < notes.size(); ++i) {
        std::ofstream outputFile(FILE_PREFIX + std::to_string(i) + FILE_EXTENSION);
        if (outputFile.is_open()) {
            outputFile << notes[i].content;
            outputFile.close();
        } else {
            std::cerr << "Unable to save to file\n";
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(400, 400), "Sticky Notes");
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font\n";
        return 1;
    }

    std::vector<Note> notes;

    sf::RectangleShape addButton(sf::Vector2f(100, 30));
    addButton.setFillColor(sf::Color::Green);
    addButton.setPosition(10, 10);
    sf::Text addButtonText("Add Note", font, 16);
    addButtonText.setPosition(20, 15);

    bool addingNote = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        if (addButton.getGlobalBounds().contains(sf::Vector2f(event.mouseButton.x, event.mouseButton.y))) {
                            notes.emplace_back(font);
                            addingNote = true;
                        }
                        for (Note& note : notes) {
                            if (note.text.getGlobalBounds().contains(sf::Vector2f(event.mouseButton.x, event.mouseButton.y))) {
                                note.isDragging = true;
                                note.offset = note.text.getPosition() - sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
                            }
                        }
                    }
                    break;
                case sf::Event::MouseButtonReleased:
                    for (Note& note : notes) {
                        note.isDragging = false;
                    }
                    break;
                case sf::Event::MouseMoved:
                    for (Note& note : notes) {
                        if (note.isDragging) {
                            note.text.setPosition(sf::Vector2f(event.mouseMove.x, event.mouseMove.y) + note.offset);
                        }
                    }
                    break;
                case sf::Event::TextEntered:
                    if (addingNote && notes.size() > 0) {
                        if (event.text.unicode < 128 && event.text.unicode != 8) {
                            notes.back().content += static_cast<char>(event.text.unicode);
                            notes.back().text.setString(notes.back().content);
                        } else if (event.text.unicode == 8 && notes.back().content.size() > 0) {
                            notes.back().content.pop_back();
                            notes.back().text.setString(notes.back().content);
                        }
                    }
                    break;
            }
        }

        window.clear(sf::Color(255, 255, 191)); // Light yellow background
        window.draw(addButton);
        window.draw(addButtonText);
        for (const Note& note : notes) {
            window.draw(note.text);
        }
        window.display();
    }

    // Saving notes to files
    saveNotes(notes);

    return 0;
}
