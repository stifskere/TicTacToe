#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

enum types {
    T_EMPTY, T_X, T_O
};

struct piece {
    types type;
    bool operator==(const piece &p) const{
        return p.type == type;
    }
};

piece board[3][3] = {};
const std::string messages[5] = {"TicTacToe - It's X turn", "TicTacToe - It's O turn", "TicTacToe - X won!", "TicTacToe - O won!", "TicTacToe - It's a tie!"};
types winner;
bool fullTable = false;

bool load_sound(const std::string& path, sf::Sound& sound, sf::SoundBuffer& buffer){
    if (!buffer.loadFromFile(path)) return false;
    sound.setBuffer(buffer);
    return true;
}

types check_winner() {
    for (int i = 0; i < 3; i++) {
        if ((board[i][0] == board[i][1] && board[i][1] == board[i][2]) && board[i][2].type != T_EMPTY) return board[i][2].type;
        if ((board[0][i] == board[1][i] && board[1][i] == board[2][i]) && board[2][i].type != T_EMPTY) return board[2][i].type;
    }

    if ((board[0][0] == board[1][1] && board[1][1] == board[2][2]) && board[2][2].type != T_EMPTY) return board[2][2].type;
    if ((board[0][2] == board[1][1] && board[1][1] == board[2][0]) && board[2][0].type != T_EMPTY) return board[2][0].type;
    return T_EMPTY;
}

int main() {
    unsigned int window_size = 400;
    sf::RenderWindow window{{window_size, window_size}, messages[0], sf::Style::Close};
    sf::Event event{};
    sf::Texture pieces_texture;
    sf::Sprite X_sprite, O_sprite;
    pieces_texture.loadFromFile("pieces.png");
    X_sprite.setTexture(pieces_texture);
    X_sprite.setTextureRect({0, 0, 512, 512});
    O_sprite.setTexture(pieces_texture);
    O_sprite.setTextureRect({512, 0, 512, 512});
    window.setVerticalSyncEnabled(true);

    sf::Image icon{};
    if(icon.loadFromFile("icon.png")) window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    float margin = (float)window_size / 50.0f;
    float size = ((float)window_size - margin) / 3;

    types current_team = T_X;

    sf::Sound startSound{};
    sf::SoundBuffer startBuffer{};
    if(!load_sound("sounds/Start.wav", startSound, startBuffer)) return -1;

    sf::Sound pieceUpdateSound{};
    sf::SoundBuffer pieceUpdateBuffer{};
    if(!load_sound("sounds/PieceUpdate.wav", pieceUpdateSound, pieceUpdateBuffer)) return -1;

    sf::Sound exitSound{};
    sf::SoundBuffer exitBuffer{};
    if(!load_sound("sounds/Exit.wav", exitSound, exitBuffer)) return -1;

    sf::Font retroFont{};
    retroFont.loadFromFile("retro.ttf");

    startSound.play();

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                exitSound.play();
                while (exitSound.getStatus() == sf::SoundSource::Playing) sf::sleep(sf::milliseconds(10));
                window.close();
            }
            if (event.type == sf::Event::LostFocus) window.requestFocus();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                for (piece *p : board) {
                    p[0] = {T_EMPTY};
                    p[1] = {T_EMPTY};
                    p[2] = {T_EMPTY};
                }
                current_team = T_X;
                winner = T_EMPTY;
                fullTable = false;
                window.setTitle(messages[0]);
                startSound.play();
            }
            if (event.type == sf::Event::MouseButtonPressed){
                if (winner != T_EMPTY || fullTable) continue;
                sf::Vector2i where{-1, -1};
                for (int y = 0; y < 3; y++){
                    for (int x = 0; x < 3; x++){
                        if (sf::FloatRect{
                            {size * (float)x + margin * (float)x, size * (float)y + margin * (float)y},
                            {size, size}
                        }.contains({(float)event.mouseButton.x, (float)event.mouseButton.y}))
                            where = {x, y};
                    }
                }
                if (where == sf::Vector2i{-1, -1} || board[where.x][where.y].type != T_EMPTY) continue;
                pieceUpdateSound.play();
                board[where.x][where.y] = {current_team};
                current_team = current_team == T_X ? T_O : T_X;
                window.setTitle(current_team == T_X ? messages[0] : messages[1]);

                winner = check_winner();
                if(winner != T_EMPTY) window.setTitle(winner == T_X ? messages[2] : messages[3]);

                if(winner == T_EMPTY){
                    bool hasEmpty = false;
                    for (piece* pa : board) {
                        for (int i = 0; i < 3; i++){
                            if(pa[i].type == T_EMPTY) hasEmpty = true;
                        }
                    }
                    fullTable = !hasEmpty;
                    if (fullTable) window.setTitle(messages[4]);
                }
            }
        }
        window.clear(sf::Color(0xE7EAEFff));

        sf::RectangleShape shape;
        shape.setFillColor(sf::Color(0x7A8490ff));
        shape.setSize(sf::Vector2f{size, size});

        for (int y = 0; y < 3; y++){
            for (int x = 0; x < 3; x++){
                shape.setPosition(size * (float)x + margin * (float)x, size * (float)y + margin * (float)y);
                window.draw(shape);
                if (board[x][y].type != T_EMPTY) {
                    sf::Sprite &currentSprite = board[x][y].type == T_X ? X_sprite : O_sprite;
                    currentSprite.setPosition(size * (float) x + margin * (float) x + 10, size * (float) y + margin * (float) y + 10);
                    currentSprite.setScale(sf::Vector2f{1, 1} * (size - 20) / 512.0f);
                    window.draw(currentSprite);
                }
            }
        }

        if (winner != T_EMPTY || fullTable){
            sf::RectangleShape winnersScreen{{(float) window_size, (float)window_size}};
            winnersScreen.setFillColor(sf::Color{0x000000af});
            window.draw(winnersScreen);

            sf::Text MessageText{};
            MessageText.setFont(retroFont);
            MessageText.setCharacterSize(50);

            if (winner != T_EMPTY) {
                MessageText.setString((std::string)(winner == T_X ? "X" : "O") + " won!");
                MessageText.setPosition({(float)window_size / 2.f - MessageText.getGlobalBounds().width / 2, (float)window_size / 2.f - MessageText.getGlobalBounds().height / 2 - 50});
                window.draw(MessageText);

                MessageText.setCharacterSize(25);
                MessageText.setString("Press R to restart");
                MessageText.setPosition({(float)window_size / 2.f - MessageText.getGlobalBounds().width / 2, (float)window_size / 2.f - MessageText.getGlobalBounds().height / 2});
                window.draw(MessageText);
            }

            if (fullTable) {
                MessageText.setString("It's a tie!");
                MessageText.setPosition({(float)window_size / 2.f - MessageText.getGlobalBounds().width / 2, (float)window_size / 2.f - MessageText.getGlobalBounds().height / 2 - 50});
                window.draw(MessageText);

                MessageText.setCharacterSize(25);
                MessageText.setString("Press R to restart");
                MessageText.setPosition({(float)window_size / 2.f - MessageText.getGlobalBounds().width / 2, (float)window_size / 2.f - MessageText.getGlobalBounds().height / 2});
                window.draw(MessageText);
            }
        }
        window.display();
    }
}