#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <optional>
#include <string>
#include <vector>

namespace
{
    constexpr unsigned int WINDOW_WIDTH = 1000;
    constexpr unsigned int WINDOW_HEIGHT = 700;

    constexpr float FIELD_LEFT = 40.f;
    constexpr float FIELD_TOP = 100.f;
    constexpr float FIELD_WIDTH = 920.f;
    constexpr float FIELD_HEIGHT = 560.f;
    constexpr float BALL_RADIUS = 14.f;

    constexpr float MIN_X = FIELD_LEFT + BALL_RADIUS;
    constexpr float MAX_X = FIELD_LEFT + FIELD_WIDTH - BALL_RADIUS;
    constexpr float MIN_Y = FIELD_TOP + BALL_RADIUS;
    constexpr float MAX_Y = FIELD_TOP + FIELD_HEIGHT - BALL_RADIUS;

    constexpr float PI = 3.14159265358979323846f;

    sf::Vector2f velocityFrom(float speed, float angleDegrees)
    {
        const float angle = angleDegrees * PI / 180.f;
        return { speed * std::cos(angle), -speed * std::sin(angle) };
    }

    sf::Text makeText(const sf::Font& font, const std::string& value,
                      unsigned int size, sf::Vector2f position)
    {
        sf::Text text(font, value, size);
        text.setPosition(position);
        text.setFillColor(sf::Color::White);
        return text;
    }

    void drawButton(sf::RenderWindow& window, const sf::Font& font,
                    const sf::RectangleShape& button, const std::string& label)
    {
        window.draw(button);
        sf::Text text(font, label, 15);
        const auto bounds = text.getLocalBounds();
        text.setPosition({
            button.getPosition().x + (button.getSize().x - bounds.size.x) / 2.f,
            button.getPosition().y + (button.getSize().y - bounds.size.y) / 2.f - 3.f
        });
        text.setFillColor(sf::Color::White);
        window.draw(text);
    }
}

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }),
        "SFML - Variant 5: Reflection from walls"
    );
    window.setFramerateLimit(120);

    sf::Font font;
    if (!font.openFromFile("resources/fonts/arial.ttf"))
    {
        if (!font.openFromFile("../resources/fonts/arial.ttf"))
            return 1;
    }

    sf::RectangleShape field({ FIELD_WIDTH, FIELD_HEIGHT });
    field.setPosition({ FIELD_LEFT, FIELD_TOP });
    field.setFillColor(sf::Color(18, 27, 42));
    field.setOutlineThickness(3.f);
    field.setOutlineColor(sf::Color(100, 190, 230));

    sf::CircleShape ball(BALL_RADIUS);
    ball.setOrigin({ BALL_RADIUS, BALL_RADIUS });
    ball.setFillColor(sf::Color(255, 190, 70));

    sf::RectangleShape speedMinus({ 42.f, 30.f });
    sf::RectangleShape speedPlus({ 42.f, 30.f });
    sf::RectangleShape angleMinus({ 42.f, 30.f });
    sf::RectangleShape anglePlus({ 42.f, 30.f });
    sf::RectangleShape pauseButton({ 105.f, 30.f });
    sf::RectangleShape resetButton({ 105.f, 30.f });

    speedMinus.setPosition({ 330.f, 38.f });
    speedPlus.setPosition({ 378.f, 38.f });
    angleMinus.setPosition({ 570.f, 38.f });
    anglePlus.setPosition({ 618.f, 38.f });
    pauseButton.setPosition({ 760.f, 38.f });
    resetButton.setPosition({ 875.f, 38.f });

    for (auto* button : { &speedMinus, &speedPlus, &angleMinus, &anglePlus,
                          &pauseButton, &resetButton })
    {
        button->setFillColor(sf::Color(45, 75, 105));
        button->setOutlineThickness(1.f);
        button->setOutlineColor(sf::Color(130, 180, 210));
    }

    float initialSpeed = 100.f;
    float angleDegrees = 30.f;
    sf::Vector2f position{ (MIN_X + MAX_X) / 2.f, (MIN_Y + MAX_Y) / 2.f };
    sf::Vector2f velocity = velocityFrom(initialSpeed, angleDegrees);

    bool running = true;
    unsigned long long collisions = 0;
    float elapsed = 0.f;
    float trajectoryTimer = 0.f;
    std::vector<sf::Vector2f> trajectory;
    trajectory.push_back(position);

    auto restart = [&]()
    {
        position = { (MIN_X + MAX_X) / 2.f, (MIN_Y + MAX_Y) / 2.f };
        velocity = velocityFrom(initialSpeed, angleDegrees);
        collisions = 0;
        elapsed = 0.f;
        trajectoryTimer = 0.f;
        trajectory.clear();
        trajectory.push_back(position);
    };

    auto updateVelocity = [&]()
    {
        velocity = velocityFrom(initialSpeed, angleDegrees);
    };

    sf::Clock clock;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (event->is<sf::Event::KeyPressed>())
            {
                const auto* key = event->getIf<sf::Event::KeyPressed>();
                if (key->code == sf::Keyboard::Key::Escape)
                    window.close();
                else if (key->code == sf::Keyboard::Key::Space)
                    running = !running;
                else if (key->code == sf::Keyboard::Key::R)
                    restart();
                else if (key->code == sf::Keyboard::Key::Up)
                {
                    initialSpeed = std::min(initialSpeed + 20.f, 600.f);
                    updateVelocity();
                }
                else if (key->code == sf::Keyboard::Key::Down)
                {
                    initialSpeed = std::max(initialSpeed - 20.f, 20.f);
                    updateVelocity();
                }
                else if (key->code == sf::Keyboard::Key::Right)
                {
                    angleDegrees = std::min(angleDegrees + 5.f, 85.f);
                    updateVelocity();
                }
                else if (key->code == sf::Keyboard::Key::Left)
                {
                    angleDegrees = std::max(angleDegrees - 5.f, 5.f);
                    updateVelocity();
                }
            }

            if (event->is<sf::Event::MouseButtonPressed>())
            {
                const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>();
                const sf::Vector2f mousePosition(mouse->position);

                if (speedMinus.getGlobalBounds().contains(mousePosition))
                {
                    initialSpeed = std::max(initialSpeed - 20.f, 20.f);
                    updateVelocity();
                }
                else if (speedPlus.getGlobalBounds().contains(mousePosition))
                {
                    initialSpeed = std::min(initialSpeed + 20.f, 600.f);
                    updateVelocity();
                }
                else if (angleMinus.getGlobalBounds().contains(mousePosition))
                {
                    angleDegrees = std::max(angleDegrees - 5.f, 5.f);
                    updateVelocity();
                }
                else if (anglePlus.getGlobalBounds().contains(mousePosition))
                {
                    angleDegrees = std::min(angleDegrees + 5.f, 85.f);
                    updateVelocity();
                }
                else if (pauseButton.getGlobalBounds().contains(mousePosition))
                    running = !running;
                else if (resetButton.getGlobalBounds().contains(mousePosition))
                    restart();
            }
        }

        float dt = std::min(clock.restart().asSeconds(), 0.05f);

        if (running)
        {
            elapsed += dt;
            position += velocity * dt;

            // Reflection from the vertical walls.
            if (position.x <= MIN_X)
            {
                position.x = MIN_X;
                velocity.x = std::abs(velocity.x);
                ++collisions;
            }
            else if (position.x >= MAX_X)
            {
                position.x = MAX_X;
                velocity.x = -std::abs(velocity.x);
                ++collisions;
            }

            // Reflection from the horizontal walls.
            if (position.y <= MIN_Y)
            {
                position.y = MIN_Y;
                velocity.y = std::abs(velocity.y);
                ++collisions;
            }
            else if (position.y >= MAX_Y)
            {
                position.y = MAX_Y;
                velocity.y = -std::abs(velocity.y);
                ++collisions;
            }

            trajectoryTimer += dt;
            if (trajectoryTimer >= 0.025f)
            {
                trajectory.push_back(position);
                trajectoryTimer = 0.f;
                if (trajectory.size() > 1800)
                    trajectory.erase(trajectory.begin());
            }
        }

        ball.setPosition(position);

        window.clear(sf::Color(10, 16, 27));
        window.draw(field);

        if (trajectory.size() > 1)
        {
            sf::VertexArray line(sf::PrimitiveType::LineStrip);

            for (const auto& point : trajectory)
            {
                sf::Vertex vertex;
                vertex.position = point;
                vertex.color = sf::Color(90, 170, 220);

                line.append(vertex);
            }

            window.draw(line);
        }

        window.draw(ball);

        sf::Text speedLabel = makeText(font,
            "Speed (Up/Down): " + std::to_string(static_cast<int>(initialSpeed)),
            17, { 30.f, 12.f });
        sf::Text angleLabel = makeText(font,
            "Angle (Left/Right): " + std::to_string(static_cast<int>(angleDegrees)) + " deg",
            17, { 410.f, 12.f });
        window.draw(speedLabel);
        window.draw(angleLabel);

        drawButton(window, font, speedMinus, "-20");
        drawButton(window, font, speedPlus, "+20");
        drawButton(window, font, angleMinus, "-5");
        drawButton(window, font, anglePlus, "+5");
        drawButton(window, font, pauseButton, running ? "Pause" : "Resume");
        drawButton(window, font, resetButton, "Restart");

        const float currentSpeed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
        sf::Text info = makeText(font,
            "Time: " + std::to_string(elapsed).substr(0, 5) + " s"
            + "     Speed: " + std::to_string(static_cast<int>(currentSpeed)) + " px/s"
            + "     Collisions: " + std::to_string(collisions)
            + (running ? "     [RUNNING]" : "     [PAUSED]"),
            18, { 55.f, 665.f });
        window.draw(info);

        window.display();
    }

    return 0;
}
