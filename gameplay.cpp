int main()
{
    // CREATING GAME WINDOW
    sf::RenderWindow window(sf::VideoMode({ WIN_W, WIN_H }), "Puff Smash");

    //SETTING GAME FRAME RATE TO 60 FPS
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);
    window.requestFocus();

    //TIME PASSING IN ONE FRAME (SINCE THE GAME RUNS AT 60 FPS)
    const float dt = 1.f / 60.f;

    //INITIALIZING RANDOM NUMBER GENERATOR
    srand(static_cast<unsigned int>(time(0)));

    //LOADING FONT
    sf::Font font;
    if (!font.openFromFile("fonts/Orbitron-VariableFont_wght.ttf"))
    {
        return -1;  //EXIT IF FONT CANNOT BE LOADED
    }

    //LOADING THE POWERPUFF GIRLS, MONSTER AND BACKGROUND PICTURES
    loadPuffGraphics();

    //-------------------------------------HERO INFORMATION (3 HEROES)--------------------------------------
    std::string heroName[3] = { "BLOSSOM", "BUBBLES", "BUTTERCUP" };
    std::string heroPower[3] = { "Rapid Bolt", "Triple Spread", "Mega Blast" };
    std::string heroInfo[3] = { "Fast shots", "3 bolts at once", "Slow but heavy" };
    sf::Color heroColor[3] = { sf::Color(255, 105, 180), sf::Color(80, 170, 255), sf::Color(90, 220, 120) };

    float heroSpeed[3] = { 330.f, 300.f, 270.f };       //MOVE SPEED (PIXELS PER SECOND)
    float heroDamage[3] = { 4.f, 3.f, 13.f };           //DAMAGE OF ONE BULLET
    float heroDelay[3] = { 0.18f, 0.40f, 0.75f };       //SECONDS BETWEEN SHOTS
    float heroBulletSpeed[3] = { 700.f, 600.f, 480.f }; //BULLET SPEED
    float heroBulletSize[3] = { 7.f, 6.f, 15.f };       //BULLET RADIUS
    int heroShots[3] = { 1, 3, 1 };                     //BULLETS PER SHOT

    //-----------------------------------------BACKGROUND BUILDINGS-----------------------------------------
    int buildingH[15];
    for (int i = 0; i < 15; i++)
    {
        buildingH[i] = 120 + rand() % 200;
    }

    //---------------------------------------------GAME VARIABLES-------------------------------------------

    //GAME STATE: 0 = INTRO, 1 = HERO SELECT, 2 = PLAYING, 3 = PAUSED, 4 = RESULT
    int state = 0;
    int selected = 0;
    int heroWins = 0;
    int monsterWins = 0;
    int bestScore = 0;
    float gameTime = 0.f;
    bool needReset = true;    //WHEN TRUE THE ROUND IS RESET AT THE START OF THE NEXT FRAME
    bool heroWonLast = false;

    //KEY STATE TRACKED FROM WINDOW EVENTS (WORKS WITHOUT MACOS INPUT MONITORING)
    bool keyLeft = false;
    bool keyRight = false;
    bool keyUp = false;
    bool keyDown = false;
    bool keyShoot = false;

    //HERO
    float heroX = 120.f;
    float heroY = 400.f;
    float heroHP = 100.f;
    float heroMaxHP = 100.f;
    float shootTimer = 0.f;
    float hurtCooldown = 0.f;
    int power = 0;

    //MONSTER
    float monsterX = WIN_W - 100.f;
    float monsterY = 400.f;
    float monsterHP = 150.f;
    float monsterMaxHP = 150.f;
    float monsterRadius = 55.f;
    float monsterShootTimer = 2.f;
    float monsterFlash = 0.f;
    int level = 1;

    //SCORE AND TIMERS
    int score = 0;
    float roundTime = 90.f;     //EACH FIGHT LASTS 90 SECONDS
    float levelTimer = 0.f;     //MONSTER LEVELS UP EVERY 30 SECONDS

    //BULLETS (ONE SLOT PER BULLET, bAlive TELLS IF THE SLOT IS IN USE)
    float bX[MAX_BULLETS];
    float bY[MAX_BULLETS];
    float bVX[MAX_BULLETS];
    float bVY[MAX_BULLETS];
    float bRadius[MAX_BULLETS];
    float bDamage[MAX_BULLETS];
    bool bFromHero[MAX_BULLETS];
    bool bAlive[MAX_BULLETS];
    sf::Color bColor[MAX_BULLETS];
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        bAlive[i] = false;
    }

    //FLOATING TEXT ("BRAVO", "OUCH!" ETC.)
    std::string popText = "";
    float popX = 0.f;
    float popY = 0.f;
    float popLife = 0.f;
    int popSize = 34;
    sf::Color popColor = sf::Color::White;

    std::string praise[3] = { "BRAVO", "SUPERB", "GREAT KILL" };


    //---------------------MAIN GAME LOOP---------------------------------------------------------------
    while (window.isOpen())
    {
        gameTime = gameTime + dt;

        //HANDLING WINDOW EVENTS
        while (const auto event = window.pollEvent())
        {
            if (const auto* pressed = event->getIf<sf::Event::KeyPressed>())
            {
                switch (pressed->code)
                {
                    case sf::Keyboard::Key::Left:  keyLeft = true; break;
                    case sf::Keyboard::Key::Right: keyRight = true; break;
                    case sf::Keyboard::Key::Up:    keyUp = true; break;
                    case sf::Keyboard::Key::Down:  keyDown = true; break;
                    case sf::Keyboard::Key::Space: keyShoot = true; break;
                    default: break;
                }
            }
            if (const auto* released = event->getIf<sf::Event::KeyReleased>())
            {
                switch (released->code)
                {
                    case sf::Keyboard::Key::Left:  keyLeft = false; break;
                    case sf::Keyboard::Key::Right: keyRight = false; break;
                    case sf::Keyboard::Key::Up:    keyUp = false; break;
                    case sf::Keyboard::Key::Down:  keyDown = false; break;
                    case sf::Keyboard::Key::Space: keyShoot = false; break;
                    default: break;
                }
            }
            if (event->is<sf::Event::FocusLost>())
            {
                keyLeft = false;
                keyRight = false;
                keyUp = false;
                keyDown = false;
                keyShoot = false;
            }

            //CLOSING GAME WHEN CLOSE BUTTON PRESSED
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                sf::Keyboard::Key key = keyPressed->code;

                if (key == sf::Keyboard::Key::Escape)
                {
                    window.close();
                }

                //INTRO SCREEN
                if (state == 0)
                {
                    if (key == sf::Keyboard::Key::Enter)
                    {
                        state = 1;
                    }
                }
                //HERO SELECT SCREEN
                else if (state == 1)
                {
                    if (key == sf::Keyboard::Key::Left)
                    {
                        selected = (selected + 2) % 3;
                    }
                    if (key == sf::Keyboard::Key::Right)
                    {
                        selected = (selected + 1) % 3;
                    }
                    if (key == sf::Keyboard::Key::Enter)
                    {
                        needReset = true;
                        state = 2;
                    }
                }
                //PLAYING
                else if (state == 2)
                {
                    if (key == sf::Keyboard::Key::P)
                    {
                        state = 3;
                    }
                }
                //PAUSED
                else if (state == 3)
                {
                    if (key == sf::Keyboard::Key::P)
                    {
                        state = 2;
                    }
                }
                //RESULT SCREEN
                else if (state == 4)
                {
                    if (key == sf::Keyboard::Key::Enter)
                    {
                        needReset = true;
                        state = 2;
                    }
                    if (key == sf::Keyboard::Key::C)
                    {
                        state = 1;
                    }
                }
            }
        }

        //-------------------------------------RESET THE ROUND------------------------------------------------
        if (needReset)
        {
            heroX = 120.f;
            heroY = 400.f;
            heroMaxHP = 100.f;
            heroHP = heroMaxHP;
            power = 0;
            shootTimer = 0.f;
            hurtCooldown = 0.f;

            level = 1;
            monsterMaxHP = 150.f;
            monsterHP = monsterMaxHP;
            monsterRadius = 55.f;
            monsterX = WIN_W - 100.f;
            monsterY = 400.f;
            monsterShootTimer = 2.f;
            monsterFlash = 0.f;

            score = 0;
            roundTime = 90.f;
            levelTimer = 0.f;
            popLife = 0.f;

            for (int i = 0; i < MAX_BULLETS; i++)
            {
                bAlive[i] = false;
            }

            needReset = false;
        }

        //MOVE THE FLOATING TEXT UPWARDS AND FADE IT OUT
        if (popLife > 0.f)
        {
            popLife = popLife - dt;
            popY = popY - 60.f * dt;
        }