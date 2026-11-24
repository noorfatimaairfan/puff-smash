// Graphics.hpp - Puff Smash graphics module (SFML 2.x)
// Everything is drawn with SFML shapes, so no image files are required.
#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>
#include <vector>

namespace gfx {

enum HeroType { BLOSSOM, BUBBLES, BUTTERCUP };

// ---------- small shape helpers ----------
inline sf::CircleShape ellipse(float rx, float ry, sf::Vector2f c, sf::Color col,
                               float outline = 2.f) {
    sf::CircleShape s(rx, 40);
    s.setOrigin(rx, rx);
    s.setScale(1.f, ry / rx);
    s.setPosition(c);
    s.setFillColor(col);
    s.setOutlineThickness(outline);
    s.setOutlineColor(sf::Color(40, 20, 40));
    return s;
}
inline sf::CircleShape circle(float r, sf::Vector2f c, sf::Color col, float outline = 2.f) {
    return ellipse(r, r, c, col, outline);
}

// ---------- background ----------
inline void drawBackground(sf::RenderWindow& w, float t) {
    float W = (float)w.getSize().x, H = (float)w.getSize().y;
    float groundY = H * 0.82f;

    // sky gradient
    sf::Color top(110, 190, 255), bottom(255, 225, 240);
    sf::VertexArray sky(sf::Quads, 4);
    sky[0] = sf::Vertex(sf::Vector2f(0, 0), top);
    sky[1] = sf::Vertex(sf::Vector2f(W, 0), top);
    sky[2] = sf::Vertex(sf::Vector2f(W, groundY), bottom);
    sky[3] = sf::Vertex(sf::Vector2f(0, groundY), bottom);
    w.draw(sky);

    // sun with halo
    sf::Vector2f sun(W * 0.82f, H * 0.18f);
    w.draw(circle(70, sun, sf::Color(255, 240, 150, 90), 0));
    w.draw(circle(48, sun, sf::Color(255, 225, 80), 0));

    // drifting clouds
    for (int i = 0; i < 5; ++i) {
        float x = std::fmod(i * 260.f + t * 15.f, W + 300.f) - 150.f;
        float y = 50.f + (i * 47 % 120);
        sf::Color c(255, 255, 255, 230);
        w.draw(circle(26, {x, y}, c, 0));
        w.draw(circle(34, {x + 32, y - 10}, c, 0));
        w.draw(circle(26, {x + 66, y}, c, 0));
        w.draw(ellipse(60, 14, {x + 33, y + 14}, c, 0));
    }

    // two skyline layers (far = lighter, near = darker)
    for (int layer = 0; layer < 2; ++layer) {
        sf::Color body = layer == 0 ? sf::Color(200, 165, 230) : sf::Color(150, 100, 190);
        float bw = layer == 0 ? 80.f : 64.f;
        float offset = layer == 0 ? 0.f : 30.f;
        for (int i = 0; i * bw - offset < W + bw; ++i) {
            float h = (layer == 0 ? 130.f : 90.f) + ((i * 53 + layer * 17) % 120);
            float x = i * bw - offset;
            sf::RectangleShape b({bw - 6.f, h});
            b.setPosition(x, groundY - h);
            b.setFillColor(body);
            b.setOutlineThickness(2);
            b.setOutlineColor(sf::Color(90, 50, 120));
            w.draw(b);
            if (layer == 1) {  // lit windows
                for (float wy = groundY - h + 12; wy < groundY - 14; wy += 22)
                    for (float wx = x + 10; wx < x + bw - 22; wx += 20) {
                        sf::RectangleShape win({9, 12});
                        win.setPosition(wx, wy);
                        bool lit = ((int)(wx + wy) % 3) != 0;
                        win.setFillColor(lit ? sf::Color(255, 240, 150) : sf::Color(110, 70, 150));
                        w.draw(win);
                    }
            }
        }
    }

    // ground
    sf::RectangleShape g({W, H - groundY});
    g.setPosition(0, groundY);
    g.setFillColor(sf::Color(255, 175, 205));
    w.draw(g);
    sf::RectangleShape stripe({W, 8});
    stripe.setPosition(0, groundY);
    stripe.setFillColor(sf::Color(230, 120, 170));
    w.draw(stripe);
}

// ---------- heroes ----------
// pos = center of the hero, s = scale, t = time (for floating motion)
inline void drawHero(sf::RenderWindow& w, HeroType type, sf::Vector2f pos, float s = 1.f,
                     bool faceRight = true, float t = 0.f) {
    float f = faceRight ? 1.f : -1.f;
    pos.y += std::sin(t * 3.f) * 4.f * s;
    auto P = [&](float dx, float dy) { return sf::Vector2f(pos.x + dx * f * s, pos.y + dy * s); };

    sf::Color skin(255, 225, 190), hair, dress, accent;
    switch (type) {
        case BLOSSOM:   hair = {255, 140, 20};  dress = {255, 110, 170}; accent = {220, 30, 60};  break;
        case BUBBLES:   hair = {255, 220, 60};  dress = {90, 170, 255};  accent = {60, 120, 220}; break;
        default:        hair = {30, 30, 40};    dress = {80, 200, 110};  accent = {40, 150, 70};  break;
    }

    // hair behind head
    if (type == BLOSSOM) w.draw(ellipse(38 * s, 46 * s, P(0, -46), hair));
    if (type == BUBBLES) {
        w.draw(circle(15 * s, P(-42, -46), hair));
        w.draw(circle(15 * s, P(42, -46), hair));
    }
    if (type == BUTTERCUP) w.draw(ellipse(36 * s, 38 * s, P(0, -52), hair));

    // legs and shoes
    for (int side = -1; side <= 1; side += 2) {
        w.draw(ellipse(6 * s, 14 * s, P(side * 9.f, 28), skin));
        w.draw(ellipse(10 * s, 5 * s, P(side * 9.f + 2, 43), sf::Color(30, 30, 40)));
    }

    // arms
    w.draw(ellipse(18 * s, 7 * s, P(32, -8), skin));   // reaching arm
    w.draw(ellipse(8 * s, 15 * s, P(-26, -4), skin));  // back arm

    // dress
    sf::ConvexShape d(4);
    d.setPoint(0, P(-18, -18)); d.setPoint(1, P(18, -18));
    d.setPoint(2, P(28, 20));   d.setPoint(3, P(-28, 20));
    d.setFillColor(dress); d.setOutlineThickness(2); d.setOutlineColor(sf::Color(40, 20, 40));
    w.draw(d);
    sf::ConvexShape belt(4);
    belt.setPoint(0, P(-20, -4)); belt.setPoint(1, P(20, -4));
    belt.setPoint(2, P(21, 3));   belt.setPoint(3, P(-21, 3));
    belt.setFillColor(sf::Color(30, 30, 40));
    w.draw(belt);

    // head
    w.draw(circle(34 * s, P(0, -50), skin));

    // hair in front (bangs)
    w.draw(ellipse(33 * s, 15 * s, P(0, -72), hair, 0));
    if (type == BUTTERCUP) {
        w.draw(circle(8 * s, P(-32, -62), hair));
        w.draw(circle(8 * s, P(32, -62), hair));
    }

    // eyes
    for (int side = -1; side <= 1; side += 2) {
        sf::Vector2f e = P(side * 13.f, -48);
        w.draw(ellipse(13 * s, 16 * s, e, sf::Color::White));
        w.draw(circle(8 * s, P(side * 13.f + 2, -47), sf::Color(20, 20, 20), 0));
        w.draw(circle(2.8f * s, P(side * 13.f + 5, -52), sf::Color::White, 0));
    }
    // cheeks and mouth
    w.draw(circle(5 * s, P(-24, -34), sf::Color(255, 150, 160, 200), 0));
    w.draw(circle(5 * s, P(24, -34), sf::Color(255, 150, 160, 200), 0));
    w.draw(ellipse(5 * s, 3 * s, P(0, -30), sf::Color(190, 40, 70), 1));

    // accessories
    if (type == BLOSSOM) {  // big red bow
        w.draw(circle(8 * s, P(-14, -88), accent));
        w.draw(circle(8 * s, P(14, -88), accent));
        w.draw(circle(5 * s, P(0, -88), sf::Color(180, 20, 50)));
    }
    if (type == BUBBLES) {  // pigtail ties
        w.draw(circle(4 * s, P(-42, -58), accent, 1));
        w.draw(circle(4 * s, P(42, -58), accent, 1));
    }
}

// ---------- monster ----------
inline void drawMonster(sf::RenderWindow& w, sf::Vector2f pos, float s = 1.f, float t = 0.f) {
    pos.y += std::sin(t * 4.f) * 5.f * s;
    auto P = [&](float dx, float dy) { return sf::Vector2f(pos.x + dx * s, pos.y + dy * s); };
    sf::Color body(140, 70, 190);

    // horns
    for (int side = -1; side <= 1; side += 2) {
        sf::ConvexShape horn(3);
        horn.setPoint(0, P(side * 20.f, -40)); horn.setPoint(1, P(side * 40.f, -40));
        horn.setPoint(2, P(side * 32.f, -75));
        horn.setFillColor(sf::Color(250, 220, 100));
        horn.setOutlineThickness(2); horn.setOutlineColor(sf::Color(40, 20, 40));
        w.draw(horn);
    }
    w.draw(ellipse(50 * s, 55 * s, P(0, 0), body));                      // body
    w.draw(ellipse(30 * s, 30 * s, P(0, 15), sf::Color(190, 130, 230)));  // belly
    // angry eyes
    for (int side = -1; side <= 1; side += 2) {
        w.draw(circle(11 * s, P(side * 20.f, -18), sf::Color::White));
        w.draw(circle(5 * s, P(side * 20.f - side * 2.f, -16), sf::Color::Black, 0));
        sf::ConvexShape brow(4);
        brow.setPoint(0, P(side * 8.f, -36));  brow.setPoint(1, P(side * 34.f, -26));
        brow.setPoint(2, P(side * 34.f, -32)); brow.setPoint(3, P(side * 8.f, -42));
        brow.setFillColor(sf::Color(40, 20, 40));
        w.draw(brow);
    }
    // mouth with teeth
    w.draw(ellipse(24 * s, 11 * s, P(0, 22), sf::Color(60, 10, 40)));
    for (int i = -1; i <= 1; ++i) {
        sf::ConvexShape tooth(3);
        tooth.setPoint(0, P(i * 12.f - 5, 14)); tooth.setPoint(1, P(i * 12.f + 5, 14));
        tooth.setPoint(2, P(i * 12.f, 24));
        tooth.setFillColor(sf::Color::White);
        w.draw(tooth);
    }
}

// ---------- health bar ----------
inline void drawHealthBar(sf::RenderWindow& w, sf::Vector2f pos, float width, float height,
                          float ratio) {
    if (ratio < 0) ratio = 0;
    if (ratio > 1) ratio = 1;
    sf::RectangleShape back({width, height});
    back.setPosition(pos);
    back.setFillColor(sf::Color(50, 30, 50));
    back.setOutlineThickness(3);
    back.setOutlineColor(sf::Color::White);
    w.draw(back);
    // green -> yellow -> red as health drops
    sf::Color c = ratio > 0.5f
        ? sf::Color((sf::Uint8)(255 * (1 - ratio) * 2), 220, 60)
        : sf::Color(235, (sf::Uint8)(220 * ratio * 2), 60);
    sf::RectangleShape fill({width * ratio, height});
    fill.setPosition(pos);
    fill.setFillColor(c);
    w.draw(fill);
}

// ---------- praise pop-ups ----------
struct PraiseText {
    std::string msg;
    sf::Vector2f pos;
    float life = 1.2f;  // seconds
};

inline void drawPraise(sf::RenderWindow& w, const sf::Font& font, std::vector<PraiseText>& list,
                       float dt) {
    for (size_t i = 0; i < list.size();) {
        PraiseText& p = list[i];
        p.life -= dt;
        p.pos.y -= 40.f * dt;  // float upward
        if (p.life <= 0) { list.erase(list.begin() + i); continue; }
        sf::Text t(p.msg, font, 38);
        t.setStyle(sf::Text::Bold);
        sf::Uint8 a = (sf::Uint8)(255 * std::min(1.f, p.life / 0.5f));  // fade out
        t.setFillColor(sf::Color(255, 240, 60, a));
        t.setOutlineColor(sf::Color(200, 30, 90, a));
        t.setOutlineThickness(4);
        t.setPosition(p.pos);
        w.draw(t);
        ++i;
    }
}

}  // namespace gfx
