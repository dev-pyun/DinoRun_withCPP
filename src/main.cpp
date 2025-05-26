#include <SFML/Graphics.hpp>

int main(){
    // SFML2 방식: VideoMode(width, height)
    sf::RenderWindow window(
        sf::VideoMode(800, 1000),     // ← 벡터 대신 두 개의 unsigned int
        "DinoRun",                   // 창 제목
        sf::Style::Default
    );

    // SFML2 이벤트 처리: pollEvent(ev)
    sf::Event ev;
    while (window.isOpen()){
        while (window.pollEvent(ev)){   // ← 매개변수로 ev 전달
            if (ev.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);
        // ... 그리기 ...
        window.display();
    }
    return 0;
}
