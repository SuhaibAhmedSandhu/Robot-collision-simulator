#include <SFML/Graphics.hpp>
#include <cmath>
#include <sstream>
#include <string>
#include <algorithm>

// =====================================================================
// Slider: draggable horizontal control
// =====================================================================
class Slider {
public:
    Slider(sf::Vector2f pos, float width, float minVal, float maxVal,
           float initialVal, const std::string& labelText, const std::string& unit = "m/s")
        : position(pos), trackWidth(width),
          minValue(minVal), maxValue(maxVal), value(initialVal),
          label(labelText), unitLabel(unit), dragging(false)
    {
        track.setSize(sf::Vector2f(trackWidth, 6.f));
        track.setPosition(position);
        track.setFillColor(sf::Color(150, 150, 150));

        handle.setRadius(9.f);
        handle.setOrigin(9.f, 9.f);
        handle.setFillColor(sf::Color(60, 130, 230));
        handle.setOutlineThickness(2.f);
        handle.setOutlineColor(sf::Color::Black);
        updateHandlePosition();
    }

    void handleEvent(const sf::Event& event, sf::RenderWindow& window) {
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = window.mapPixelToCoords(
                sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
            if (distance(mousePos, handle.getPosition()) < 14.f) dragging = true;
        }
        if (event.type == sf::Event::MouseButtonReleased &&
            event.mouseButton.button == sf::Mouse::Left) {
            dragging = false;
        }
        if (event.type == sf::Event::MouseMoved && dragging) {
            sf::Vector2f mousePos = window.mapPixelToCoords(
                sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
            float minX = position.x;
            float maxX = position.x + trackWidth;
            float clampedX = mousePos.x;
            if (clampedX < minX) clampedX = minX;
            if (clampedX > maxX) clampedX = maxX;
            float t = (clampedX - position.x) / trackWidth;
            value = minValue + t * (maxValue - minValue);
            updateHandlePosition();
        }
    }

    float getValue() const { return value; }
    bool isDragging() const { return dragging; }

    void setValue(float v) {
        if (v < minValue) v = minValue;
        if (v > maxValue) v = maxValue;
        value = v;
        updateHandlePosition();
    }

    void draw(sf::RenderWindow& window, const sf::Font* font) const {
        window.draw(track);
        window.draw(handle);
        if (font) {
            sf::Text text;
            text.setFont(*font);
            text.setCharacterSize(14);
            text.setFillColor(sf::Color::White);
            std::ostringstream ss;
            ss.precision(2);
            ss << std::fixed << label << ": " << value << " " << unitLabel;
            text.setString(ss.str());
            text.setPosition(position.x, position.y - 22.f);
            window.draw(text);
        }
    }

private:
    sf::Vector2f position;
    float trackWidth;
    float minValue, maxValue, value;
    std::string label;
    std::string unitLabel;
    bool dragging;
    sf::RectangleShape track;
    sf::CircleShape handle;

    void updateHandlePosition() {
        float t = (value - minValue) / (maxValue - minValue);
        handle.setPosition(position.x + t * trackWidth, position.y + 3.f);
    }

    static float distance(sf::Vector2f a, sf::Vector2f b) {
        float dx = a.x - b.x, dy = a.y - b.y;
        return std::sqrt(dx * dx + dy * dy);
    }
};

// =====================================================================
// Button: simple clickable rectangle with a text label
// =====================================================================
class Button {
public:
    Button(sf::Vector2f pos, sf::Vector2f size, const std::string& labelText)
        : position(pos), sizeVal(size), label(labelText), active(false)
    {
        box.setPosition(position);
        box.setSize(sizeVal);
        box.setOutlineThickness(2.f);
        box.setOutlineColor(sf::Color::Black);
        box.setFillColor(sf::Color(70, 70, 80));
    }

    bool isClicked(const sf::Event& event, sf::RenderWindow& window) const {
        if (event.type != sf::Event::MouseButtonPressed ||
            event.mouseButton.button != sf::Mouse::Left) return false;
        sf::Vector2f mousePos = window.mapPixelToCoords(
            sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        return box.getGlobalBounds().contains(mousePos);
    }

    void setActive(bool a) { active = a; }
    void setLabel(const std::string& l) { label = l; }

    void draw(sf::RenderWindow& window, const sf::Font* font) const {
        sf::RectangleShape drawBox = box;
        drawBox.setFillColor(active ? sf::Color(60, 150, 90) : sf::Color(70, 70, 80));
        window.draw(drawBox);
        if (font) {
            sf::Text text;
            text.setFont(*font);
            text.setCharacterSize(15);
            text.setFillColor(sf::Color::White);
            text.setString(label);
            sf::FloatRect b = text.getLocalBounds();
            text.setPosition(position.x + (sizeVal.x - b.width) / 2.f - b.left,
                              position.y + (sizeVal.y - b.height) / 2.f - b.top);
            window.draw(text);
        }
    }

private:
    sf::Vector2f position, sizeVal;
    std::string label;
    bool active;
    sf::RectangleShape box;
};

// =====================================================================
// IconButton: small square button that draws a play triangle or a
// pause icon (two bars) depending on state - no font required
// =====================================================================
class IconButton {
public:
    IconButton(sf::Vector2f pos, float size) : position(pos), sizeVal(size) {
        box.setPosition(position);
        box.setSize(sf::Vector2f(sizeVal, sizeVal));
        box.setFillColor(sf::Color(60, 60, 70));
        box.setOutlineThickness(2.f);
        box.setOutlineColor(sf::Color::Black);
    }

    bool isClicked(const sf::Event& event, sf::RenderWindow& window) const {
        if (event.type != sf::Event::MouseButtonPressed ||
            event.mouseButton.button != sf::Mouse::Left) return false;
        sf::Vector2f mousePos = window.mapPixelToCoords(
            sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        return box.getGlobalBounds().contains(mousePos);
    }

    void draw(sf::RenderWindow& window, bool playing) const {
        window.draw(box);
        float cx = position.x + sizeVal / 2.f;
        float cy = position.y + sizeVal / 2.f;

        if (playing) {
            // Pause icon: two vertical bars
            sf::RectangleShape bar1(sf::Vector2f(5.f, sizeVal * 0.5f));
            bar1.setFillColor(sf::Color::White);
            bar1.setPosition(cx - 8.f, cy - sizeVal * 0.25f);
            window.draw(bar1);

            sf::RectangleShape bar2(sf::Vector2f(5.f, sizeVal * 0.5f));
            bar2.setFillColor(sf::Color::White);
            bar2.setPosition(cx + 3.f, cy - sizeVal * 0.25f);
            window.draw(bar2);
        } else {
            // Play icon: right-pointing triangle
            sf::ConvexShape triangle;
            triangle.setPointCount(3);
            triangle.setPoint(0, sf::Vector2f(cx - 8.f, cy - 10.f));
            triangle.setPoint(1, sf::Vector2f(cx - 8.f, cy + 10.f));
            triangle.setPoint(2, sf::Vector2f(cx + 10.f, cy));
            triangle.setFillColor(sf::Color::White);
            window.draw(triangle);
        }
    }

private:
    sf::Vector2f position;
    float sizeVal;
    sf::RectangleShape box;
};

// Draws a horizontal arrow starting at (x, y), pointing right if vel > 0,
// left if vel < 0. Length scales with |vel|. Does nothing if vel == 0.
void drawVelocityArrow(sf::RenderWindow& window, float x, float y, float vel, sf::Color color) {
    if (std::fabs(vel) < 0.05f) return;
    float length = std::fabs(vel) * 18.f;
    if (length > 70.f) length = 70.f;
    float dir = vel > 0.f ? 1.f : -1.f;

    sf::RectangleShape shaft(sf::Vector2f(length, 3.f));
    shaft.setFillColor(color);
    shaft.setPosition(dir > 0.f ? x : x - length, y);
    window.draw(shaft);

    sf::ConvexShape head;
    head.setPointCount(3);
    float tipX = dir > 0.f ? x + length + 8.f : x - length - 8.f;
    head.setPoint(0, sf::Vector2f(tipX, y + 1.5f));
    head.setPoint(1, sf::Vector2f(tipX - dir * 10.f, y - 6.f));
    head.setPoint(2, sf::Vector2f(tipX - dir * 10.f, y + 9.f));
    head.setFillColor(color);
    window.draw(head);
}

// Draws a fading spark burst at (x, y). progress goes from 0 (moment of
// impact) to 1 (fully faded out).
void drawImpactEffect(sf::RenderWindow& window, float x, float y, float progress) {
    if (progress >= 1.f) return;
    sf::Uint8 alpha = static_cast<sf::Uint8>(255.f * (1.f - progress));
    float radius = 12.f + progress * 30.f;

    sf::CircleShape ring(radius);
    ring.setOrigin(radius, radius);
    ring.setPosition(x, y);
    ring.setFillColor(sf::Color::Transparent);
    ring.setOutlineThickness(3.f);
    ring.setOutlineColor(sf::Color(255, 190, 60, alpha));
    window.draw(ring);

    const int spikes = 8;
    float innerR = 6.f + progress * 8.f;
    float outerR = 16.f + progress * 22.f;
    for (int i = 0; i < spikes; ++i) {
        float angle = i * (360.f / spikes) * 3.14159265f / 180.f;
        sf::Vertex line[2];
        line[0] = sf::Vertex(sf::Vector2f(x + std::cos(angle) * innerR, y + std::sin(angle) * innerR),
                              sf::Color(255, 220, 100, alpha));
        line[1] = sf::Vertex(sf::Vector2f(x + std::cos(angle) * outerR, y + std::sin(angle) * outerR),
                              sf::Color(255, 220, 100, alpha));
        window.draw(line, 2, sf::Lines);
    }
}

// Draws a small robot: body, head, antenna, eyes, arms, and hands. (x, y) is the
// top-left of the overall bounding box, matching the old block's footprint,
// so wheels/labels placed relative to it don't need to move.
void drawRobot(sf::RenderWindow& window, float x, float y, float size,
                sf::Color color, char label, const sf::Font* font, bool fontLoaded) {
    float bodyH = size * 0.7f;
    float bodyY = y + size * 0.3f;

    // Arms
    sf::RectangleShape armL(sf::Vector2f(6.f, bodyH * 0.5f));
    armL.setPosition(x - 6.f, bodyY + bodyH * 0.15f);
    armL.setFillColor(sf::Color(90, 90, 95));
    armL.setOutlineThickness(1.f);
    armL.setOutlineColor(sf::Color::Black);
    window.draw(armL);

    sf::RectangleShape armR(sf::Vector2f(6.f, bodyH * 0.5f));
    armR.setPosition(x + size, bodyY + bodyH * 0.15f);
    armR.setFillColor(sf::Color(90, 90, 95));
    armR.setOutlineThickness(1.f);
    armR.setOutlineColor(sf::Color::Black);
    window.draw(armR);

    // Hands (at the bottom tip of each arm)
    float handY = bodyY + bodyH * 0.15f + bodyH * 0.5f;
    sf::CircleShape handL(5.f);
    handL.setFillColor(sf::Color(60, 60, 65));
    handL.setOutlineThickness(1.f);
    handL.setOutlineColor(sf::Color::Black);
    handL.setPosition(x - 3.f - 5.f, handY - 5.f);
    window.draw(handL);

    sf::CircleShape handR(5.f);
    handR.setFillColor(sf::Color(60, 60, 65));
    handR.setOutlineThickness(1.f);
    handR.setOutlineColor(sf::Color::Black);
    handR.setPosition(x + size + 3.f - 5.f, handY - 5.f);
    window.draw(handR);

    // Body
    sf::RectangleShape body(sf::Vector2f(size, bodyH));
    body.setPosition(x, bodyY);
    body.setFillColor(color);
    body.setOutlineThickness(2.f);
    body.setOutlineColor(sf::Color::Black);
    window.draw(body);

    // Head
    float headW = size * 0.6f;
    float headH = size * 0.35f;
    float headX = x + (size - headW) / 2.f;
    float headY = y;
    sf::RectangleShape head(sf::Vector2f(headW, headH));
    head.setPosition(headX, headY);
    head.setFillColor(color);
    head.setOutlineThickness(2.f);
    head.setOutlineColor(sf::Color::Black);
    window.draw(head);

    // Antenna
    sf::RectangleShape antenna(sf::Vector2f(2.f, 10.f));
    antenna.setPosition(x + size / 2.f - 1.f, headY - 10.f);
    antenna.setFillColor(sf::Color(90, 90, 95));
    window.draw(antenna);

    sf::CircleShape antennaTip(3.f);
    antennaTip.setFillColor(sf::Color(220, 70, 70));
    antennaTip.setPosition(x + size / 2.f - 3.f, headY - 13.f);
    window.draw(antennaTip);

    // Eyes
    float eyeY = headY + headH * 0.3f;
    float eyeOffset = headW * 0.22f;

    sf::CircleShape eyeL(3.5f), eyeR(3.5f);
    eyeL.setFillColor(sf::Color::White);
    eyeR.setFillColor(sf::Color::White);
    eyeL.setPosition(x + size / 2.f - eyeOffset - 3.5f, eyeY);
    eyeR.setPosition(x + size / 2.f + eyeOffset - 3.5f, eyeY);
    window.draw(eyeL);
    window.draw(eyeR);

    sf::CircleShape pupilL(1.5f), pupilR(1.5f);
    pupilL.setFillColor(sf::Color::Black);
    pupilR.setFillColor(sf::Color::Black);
    pupilL.setPosition(eyeL.getPosition().x + 1.f, eyeL.getPosition().y + 1.f);
    pupilR.setPosition(eyeR.getPosition().x + 1.f, eyeR.getPosition().y + 1.f);
    window.draw(pupilL);
    window.draw(pupilR);

    // Chest label (A / B)
    if (fontLoaded) {
        sf::Text text(std::string(1, label), *font, 16);
        text.setFillColor(sf::Color::White);
        sf::FloatRect b = text.getLocalBounds();
        text.setPosition(x + size / 2.f - b.width / 2.f - b.left,
                          bodyY + bodyH / 2.f - b.height / 2.f - b.top - 4.f);
        window.draw(text);
    }
}

int main() {
    const unsigned int WIDTH = 800, HEIGHT = 540;
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Robot Collision Detection and Response Simulator");
    window.setFramerateLimit(60);

    sf::Font font;
    bool fontLoaded = font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    if (!fontLoaded) fontLoaded = font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");

    // --- Track geometry ---
    const float trackY = 260.f;
    const float trackXStart = 100.f;
    const float trackXEnd = 700.f;
    const float blockSize = 55.f;
    const float massA = 1.f, massB = 1.f;      // kg
    const float pxPerMps = 60.f;               // pixels per (m/s) of velocity

    sf::RectangleShape track(sf::Vector2f(trackXEnd - trackXStart, 4.f));
    track.setPosition(trackXStart, trackY + blockSize);
    track.setFillColor(sf::Color(120, 120, 110));

    // --- UI controls ---
    Slider velASlider(sf::Vector2f(60.f, 430.f), 220.f, -3.f, 3.f, -1.5f, "A");
    Slider velBSlider(sf::Vector2f(320.f, 430.f), 220.f, -3.f, 3.f, 1.5f, "B");
    Slider speedSlider(sf::Vector2f(580.f, 430.f), 160.f, 0.2f, 4.f, 1.5f, "Speed", "x");

    Button elasticBtn(sf::Vector2f(60.f, 470.f), sf::Vector2f(150.f, 34.f), "Elastic");
    Button inelasticBtn(sf::Vector2f(220.f, 470.f), sf::Vector2f(180.f, 34.f), "Perfectly inelastic");
    Button resetBtn(sf::Vector2f(650.f, 470.f), sf::Vector2f(90.f, 34.f), "Reset");

    IconButton playPauseBtn(sf::Vector2f(700.f, 60.f), 40.f);

    bool elasticMode = true;
    elasticBtn.setActive(true);
    inelasticBtn.setActive(false);

    // --- Simulation state ---
    float posA, posB, velA, velB;
    bool playing = true;   // runs continuously by default
    std::string statusText = "No collision on this track";

    // Impact animation state
    const float impactDuration = 0.4f;
    float impactTimer = 0.f;   // counts down from impactDuration to 0
    bool showImpact = false;
    float impactX = 0.f, impactY = 0.f;

    auto resetSim = [&]() {
        posA = trackXStart + 60.f;
        posB = trackXEnd - 60.f - blockSize;
        velA = velASlider.getValue();
        velB = velBSlider.getValue();
        statusText = "No collision on this track";
    };
    resetSim();

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            velASlider.handleEvent(event, window);
            velBSlider.handleEvent(event, window);
            speedSlider.handleEvent(event, window);

            if (elasticBtn.isClicked(event, window)) {
                elasticMode = true;
                elasticBtn.setActive(true);
                inelasticBtn.setActive(false);
            }
            if (inelasticBtn.isClicked(event, window)) {
                elasticMode = false;
                elasticBtn.setActive(false);
                inelasticBtn.setActive(true);
            }
            if (playPauseBtn.isClicked(event, window)) {
                playing = !playing;
            }
            if (resetBtn.isClicked(event, window)) {
                resetSim();
            }
        }

        float rawDt = clock.restart().asSeconds();
        if (rawDt > 0.033f) rawDt = 0.033f;
        float dt = rawDt * speedSlider.getValue();

        // While the user is actively dragging a velocity slider, let it
        // control the live velocity in real time.
        if (velASlider.isDragging()) velA = velASlider.getValue();
        if (velBSlider.isDragging()) velB = velBSlider.getValue();

        if (playing) {
            posA += velA * pxPerMps * dt;
            posB += velB * pxPerMps * dt;

            // Bounce off the ends of the track so motion never stops
            if (posA < trackXStart) {
                posA = trackXStart;
                velA = -velA;
            }
            if (posB + blockSize > trackXEnd) {
                posB = trackXEnd - blockSize;
                velB = -velB;
            }
            if (posB < trackXStart) {
                posB = trackXStart;
                velB = -velB;
            }
            if (posA + blockSize > trackXEnd) {
                posA = trackXEnd - blockSize;
                velA = -velA;
            }

            // Keep A left of B for collision math to make sense; if they
            // pass through each other's positions treat it the same way.
            float leftPos = std::min(posA, posB);
            float rightPos = std::max(posA, posB);
            bool aIsLeft = (posA <= posB);

            float gap = rightPos - (leftPos + blockSize);
            if (gap <= 0.f) {
                float& leftVel  = aIsLeft ? velA : velB;
                float& rightVel = aIsLeft ? velB : velA;
                float leftMass  = aIsLeft ? massA : massB;
                float rightMass = aIsLeft ? massB : massA;

                float relVel = rightVel - leftVel; // along +x
                if (relVel < 0.f) {
                    float e = elasticMode ? 1.0f : 0.0f;
                    float j = -(1.0f + e) * relVel / (1.0f / leftMass + 1.0f / rightMass);
                    leftVel  -= j / leftMass;
                    rightVel += j / rightMass;

                    statusText = elasticMode
                        ? "Elastic collision - velocities exchanged"
                        : "Perfectly inelastic collision - blocks move together";

                    // Trigger the impact spark animation at the contact point
                    showImpact = true;
                    impactTimer = impactDuration;
                    impactX = leftPos + blockSize;
                    impactY = trackY + blockSize / 2.f;
                }

                // Separate them so they don't stay overlapped
                float overlap = -gap;
                if (aIsLeft) { posA -= overlap / 2.f; posB += overlap / 2.f; }
                else         { posB -= overlap / 2.f; posA += overlap / 2.f; }
            } else {
                statusText = "No collision on this track";
            }
        }

        if (showImpact) {
            impactTimer -= rawDt;
            if (impactTimer <= 0.f) {
                impactTimer = 0.f;
                showImpact = false;
            }
        }

        window.clear(sf::Color(35, 35, 45));

        window.draw(track);

        // Wheels (small dots under each robot)
        sf::CircleShape wheel(4.f);
        wheel.setFillColor(sf::Color(40, 40, 40));

        drawRobot(window, posA, trackY, blockSize, sf::Color(50, 120, 220), 'A', &font, fontLoaded);
        wheel.setPosition(posA + blockSize * 0.25f, trackY + blockSize + 2.f);
        window.draw(wheel);
        wheel.setPosition(posA + blockSize * 0.65f, trackY + blockSize + 2.f);
        window.draw(wheel);

        drawRobot(window, posB, trackY, blockSize, sf::Color(60, 170, 90), 'B', &font, fontLoaded);
        wheel.setPosition(posB + blockSize * 0.25f, trackY + blockSize + 2.f);
        window.draw(wheel);
        wheel.setPosition(posB + blockSize * 0.65f, trackY + blockSize + 2.f);
        window.draw(wheel);

        // Velocity arrows above each block
        drawVelocityArrow(window, posA + blockSize / 2.f, trackY - 45.f, velA, sf::Color(90, 150, 240));
        drawVelocityArrow(window, posB + blockSize / 2.f, trackY - 45.f, velB, sf::Color(100, 200, 130));

        if (showImpact) {
            float progress = 1.f - (impactTimer / impactDuration);
            drawImpactEffect(window, impactX, impactY, progress);
        }

        if (fontLoaded) {
            std::ostringstream massALabel, massBLabel;
            massALabel.precision(0);
            massALabel << std::fixed << "A - " << massA << " kg";
            massBLabel << std::fixed << "B - " << massB << " kg";

            sf::Text massAText(massALabel.str(), font, 14);
            massAText.setFillColor(sf::Color(200, 200, 200));
            massAText.setPosition(posA, trackY + blockSize + 20.f);
            window.draw(massAText);

            sf::Text massBText(massBLabel.str(), font, 14);
            massBText.setFillColor(sf::Color(200, 200, 200));
            massBText.setPosition(posB, trackY + blockSize + 20.f);
            window.draw(massBText);

            std::ostringstream velALabel, velBLabel;
            velALabel.precision(1);
            velBLabel.precision(1);
            velALabel << std::fixed << "A " << velA << " m/s";
            velBLabel << std::fixed << "B " << velB << " m/s";

            sf::Text velAText(velALabel.str(), font, 14);
            velAText.setFillColor(sf::Color(120, 170, 255));
            velAText.setPosition(posA - 10.f, trackY - 70.f);
            window.draw(velAText);

            sf::Text velBText(velBLabel.str(), font, 14);
            velBText.setFillColor(sf::Color(140, 220, 150));
            velBText.setPosition(posB - 10.f, trackY - 70.f);
            window.draw(velBText);

            sf::Text status(statusText, font, 16);
            status.setFillColor(sf::Color::White);
            sf::FloatRect sb = status.getLocalBounds();
            status.setPosition((WIDTH - sb.width) / 2.f, 90.f);
            window.draw(status);

            sf::Text title("Robot collision detection and response simulator", font, 18);
            title.setFillColor(sf::Color::White);
            sf::FloatRect tb = title.getLocalBounds();
            title.setPosition((WIDTH - tb.width) / 2.f, 20.f);
            window.draw(title);
        }

        velASlider.draw(window, fontLoaded ? &font : nullptr);
        velBSlider.draw(window, fontLoaded ? &font : nullptr);
        speedSlider.draw(window, fontLoaded ? &font : nullptr);
        elasticBtn.draw(window, fontLoaded ? &font : nullptr);
        inelasticBtn.draw(window, fontLoaded ? &font : nullptr);
        resetBtn.draw(window, fontLoaded ? &font : nullptr);
        playPauseBtn.draw(window, playing);

        window.display();
    }

    return 0;
}