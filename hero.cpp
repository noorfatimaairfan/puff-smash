if (state == 2)
        {
            roundTime = roundTime - dt;
            levelTimer = levelTimer + dt;
            shootTimer = shootTimer - dt;
            hurtCooldown = hurtCooldown - dt;
            monsterFlash = monsterFlash - dt;

            //--------------------MOVING HERO--------------------
            float moveX = 0.f;
            float moveY = 0.f;

            if (keyLeft)
            {
                moveX = moveX - 1.f;
            }
            if (keyRight)
            {
                moveX = moveX + 1.f;
            }
            if (keyUp)
            {
                moveY = moveY - 1.f;
            }
            if (keyDown)
            {
                moveY = moveY + 1.f;
            }

            heroX = heroX + moveX * heroSpeed[selected] * dt;
            heroY = heroY + moveY * heroSpeed[selected] * dt;

            //KEEPING HERO INSIDE HIS AREA
            heroX = std::max(40.f, std::min(heroX, 640.f));
            heroY = std::max(130.f, std::min(heroY, GROUND_Y - 35.f));


            //--------------------HERO SHOOTING--------------------
            if (keyShoot && shootTimer <= 0.f)
            {
                //BULLETS GET STRONGER AND BIGGER AS POWER GROWS
                float damage = heroDamage[selected] * (1.f + 0.08f * power);
                float radius = heroBulletSize[selected] + std::min(power * 0.25f, 8.f);

                for (int s = 0; s < heroShots[selected]; s++)
                {
                    //ANGLE OF THIS BULLET (SPREAD FOR TRIPLE SHOT)
                    float angle = 0.f;
                    if (heroShots[selected] > 1)
                    {
                        angle = (s - (heroShots[selected] - 1) / 2.f) * 0.22f;
                    }

                    //FIND A FREE BULLET SLOT
                    for (int i = 0; i < MAX_BULLETS; i++)
                    {
                        if (bAlive[i] == false)
                        {
                            bX[i] = heroX + 36.f;
                            bY[i] = heroY;
                            bVX[i] = std::cos(angle) * heroBulletSpeed[selected];
                            bVY[i] = std::sin(angle) * heroBulletSpeed[selected];
                            bRadius[i] = radius;
                            bDamage[i] = damage;
                            bFromHero[i] = true;
                            bColor[i] = heroColor[selected];
                            bAlive[i] = true;
                            break;
                        }
                    }
                }

                shootTimer = heroDelay[selected] * std::max(0.6f, 1.f - 0.01f * power);
            }


            //--------------------MONSTER MOVEMENT--------------------

            //MONSTER WALKS TOWARDS THE HERO
            monsterX = monsterX - (45.f + 12.f * level) * dt;

            //MONSTER SLOWLY FOLLOWS THE HERO UP AND DOWN
            float follow = std::min(1.f, dt * (0.6f + 0.1f * level));
            monsterY = monsterY + (heroY - monsterY) * follow;
            monsterY = monsterY + std::sin(gameTime * 3.f) * 25.f * dt;


            //--------------------MONSTER SHOOTING--------------------
            monsterShootTimer = monsterShootTimer - dt;

            if (monsterShootTimer <= 0.f)
            {
                //DIRECTION FROM MONSTER TO HERO
                float dx = heroX - monsterX;
                float dy = heroY - monsterY;
                float length = std::sqrt(dx * dx + dy * dy);

                if (length > 1.f)
                {
                    dx = dx / length;
                    dy = dy / length;

                    for (int i = 0; i < MAX_BULLETS; i++)
                    {
                        if (bAlive[i] == false)
                        {
                            float speed = 230.f + 15.f * level;
                            bX[i] = monsterX + dx * (monsterRadius + 5.f);
                            bY[i] = monsterY + dy * (monsterRadius + 5.f);
                            bVX[i] = dx * speed;
                            bVY[i] = dy * speed;
                            bRadius[i] = 10.f;
                            bDamage[i] = 5.f + level;
                            bFromHero[i] = false;
                            bColor[i] = sf::Color(170, 60, 220);
                            bAlive[i] = true;
                            break;
                        }
                    }
                }

                //MONSTER SHOOTS FASTER AT HIGHER LEVELS
                monsterShootTimer = std::max(0.8f, 2.2f - 0.2f * level);
            }


            //--------------------MONSTER TOUCHES HERO--------------------
            if (circlesHit(monsterX, monsterY, monsterRadius * 0.9f, heroX, heroY, 26.f) && hurtCooldown <= 0.f)
            {
                heroHP = heroHP - (10.f + 2.f * level);
                hurtCooldown = 1.f;
                monsterX = WIN_W - 100.f;    //MONSTER IS PUSHED BACK

                popText = "OUCH!";
                popX = heroX;
                popY = heroY - 50.f;
                popLife = 0.9f;
                popSize = 34;
                popColor = sf::Color(255, 80, 80);
            }


            //--------------------MOVING BULLETS--------------------
            for (int i = 0; i < MAX_BULLETS; i++)
            {
                if (bAlive[i])
                {
                    bX[i] = bX[i] + bVX[i] * dt;
                    bY[i] = bY[i] + bVY[i] * dt;
                }
            }
a
            //HERO BULLET HITTING MONSTER BULLET: BOTH DISAPPEAR
            for (int i = 0; i < MAX_BULLETS; i++)
            {
                for (int j = 0; j < MAX_BULLETS; j++)
                {
                    if (bAlive[i] && bAlive[j] && bFromHero[i] && !bFromHero[j])
                    {
                        if (circlesHit(bX[i], bY[i], bRadius[i], bX[j], bY[j], bRadius[j]))
                        {
                            bAlive[i] = false;
                            bAlive[j] = false;
                        }
                    }
                }
            }

            //BULLETS HITTING MONSTER, HERO OR LEAVING THE SCREEN
            for (int i = 0; i < MAX_BULLETS; i++)
            {
                if (bAlive[i] == false)
                {
                    continue;
                }

                //HERO BULLET HITS MONSTER
                if (bFromHero[i] && circlesHit(bX[i], bY[i], bRadius[i], monsterX, monsterY, monsterRadius))
                {
                    bAlive[i] = false;
                    monsterHP = monsterHP - bDamage[i];
                    monsterFlash = 0.08f;
                    power = power + 1;           //HERO GROWS STRONGER
                    score = score + 10 + power;

                    //SHOW A PRAISE WORD
                    if (popLife <= 0.f)
                    {
                        popText = praise[rand() % 3];
                        popX = bX[i] - 40.f;
                        popY = bY[i] - 30.f;
                        popLife = 0.9f;
                        popSize = 34;
                        popColor = sf::Color(255, 230, 60);
                    }
                }
                //MONSTER BULLET HITS HERO
                else if (!bFromHero[i] && circlesHit(bX[i], bY[i], bRadius[i], heroX, heroY, 26.f))
                {
                    bAlive[i] = false;
                    heroHP = heroHP - bDamage[i];
                }
                //BULLET LEFT THE SCREEN
                else if (bX[i] < -60 || bX[i] > WIN_W + 60 || bY[i] < -60 || bY[i] > WIN_H + 60)
                {
                    bAlive[i] = false;
                }
            }


            //--------------------MONSTER LEVELS UP EVERY 30 SECONDS--------------------
            if (levelTimer >= 30.f)
            {
                levelTimer = levelTimer - 30.f;
                level = level + 1;
                monsterMaxHP = monsterMaxHP + 50.f;
                monsterHP = std::min(monsterMaxHP, monsterHP + 50.f);
                monsterRadius = std::min(90.f, 55.f + 5.f * (level - 1));

                popText = "MONSTER LEVEL UP!";
                popX = WIN_W / 2.f;
                popY = 200.f;
                popLife = 0.9f;
                popSize = 42;
                popColor = sf::Color(255, 120, 60);
            }


            //--------------------END OF THE ROUND--------------------
            if (monsterHP <= 0.f)
            {
                //HERO WON
                score = score + 100 * level;
                heroWins = heroWins + 1;
                heroWonLast = true;
                bestScore = std::max(bestScore, score);

                popText = "GREAT KILL";
                popX = monsterX;
                popY = monsterY - 80.f;
                popLife = 0.9f;
                popSize = 48;
                popColor = sf::Color(255, 230, 60);

                state = 4;
            }
            else if (heroHP <= 0.f || roundTime <= 0.f)
            {
                //MONSTER WON
                heroHP = std::max(heroHP, 0.f);
                roundTime = std::max(roundTime, 0.f);
                monsterWins = monsterWins + 1;
                heroWonLast = false;
                bestScore = std::max(bestScore, score);

                state = 4;
            }
        }


        //-----------------------------------------DRAWING----------------------------------------------------
        window.clear();

        //SKY
        drawRect(window, 0, 0, WIN_W, GROUND_Y, sf::Color(255, 200, 190));

        //TOWN BUILDINGS WITH WINDOWS
        for (int i = 0; i < 15; i++)
        {
            float h = (float)buildingH[i];
            float x = i * 70.f;
            drawRect(window, x, GROUND_Y - h, 62.f, h, sf::Color(90 + (i % 3) * 20, 60, 140));

            for (int wy = (int)(GROUND_Y - h + 12); wy < GROUND_Y - 20; wy += 28)
            {
                for (int wx = 8; wx < 54; wx += 20)
                {
                    drawRect(window, x + wx, (float)wy, 10.f, 14.f, sf::Color(255, 245, 150, 200));
                }
            }
        }

        //GROUND
        drawRect(window, 0, GROUND_Y, WIN_W, WIN_H - GROUND_Y, sf::Color(70, 190, 110));
        drawRect(window, 0, GROUND_Y, WIN_W, 6.f, sf::Color(40, 140, 80));

        //TOWNSVILLE BACKGROUND PICTURE (COVERS THE SHAPES ABOVE; IF THE PICTURE IS MISSING THE SHAPES STAY)
        drawBackgroundSprite(window);


        //--------------------INTRO SCREEN--------------------
        if (state == 0)
        {
            drawRect(window, 0, 0, WIN_W, WIN_H, sf::Color(0, 0, 0, 90));

            float bob = std::sin(gameTime * 3.f) * 8.f;
            drawText(window, font, "PUFF SMASH", 96, WIN_W / 2.f, 90 + bob, sf::Color(255, 120, 190), true);
            drawText(window, font, "Defend the townsville from the monsters!", 24, WIN_W / 2.f, 215, sf::Color::White, true);

            drawHero(window, 300, 350, 1.6f, heroColor[0], 0, false, gameTime);
            drawHero(window, 500, 350, 1.6f, heroColor[1], 0, false, gameTime);
            drawHero(window, 700, 350, 1.6f, heroColor[2], 0, false, gameTime);

            drawText(window, font, "ARROWS: move   SPACE: shoot   P: pause   ESC: exit", 20, WIN_W / 2.f, 470, sf::Color::White, true);

            //BLINKING TEXT
            if ((int)(gameTime * 2) % 2 == 0)
            {
                drawText(window, font, "Press ENTER to start", 32, WIN_W / 2.f, 520, sf::Color(255, 230, 60), true);
            }
        }

        //--------------------HERO SELECT SCREEN--------------------
        else if (state == 1)
        {
            drawRect(window, 0, 0, WIN_W, WIN_H, sf::Color(0, 0, 0, 90));
            drawText(window, font, "CHOOSE YOUR HERO", 50, WIN_W / 2.f, 30, sf::Color::White, true);

            for (int i = 0; i < 3; i++)
            {
                float cx = 190.f + i * 310.f;

                //CARD BEHIND THE HERO
                sf::RectangleShape card({ 250.f, 360.f });
                card.setOrigin({ 125.f, 0.f });
                card.setPosition({ cx, 130.f });

                if (i == selected)
                {
                    card.setFillColor(sf::Color(255, 255, 255, 230));
                    card.setOutlineThickness(6.f);
                    card.setOutlineColor(heroColor[i]);
                }
                else
                {
                    card.setFillColor(sf::Color(255, 255, 255, 120));
                    card.setOutlineThickness(2.f);
                    card.setOutlineColor(sf::Color(60, 60, 60));
                }
                window.draw(card);

                //SELECTED HERO BOUNCES UP AND DOWN
                float heroDrawY = 270.f;
                if (i == selected)
                {
                    heroDrawY = heroDrawY + std::sin(gameTime * 6.f) * 6.f;
                }
                drawHero(window, cx, heroDrawY, 1.8f, heroColor[i], 0, false, gameTime);

                drawText(window, font, heroName[i], 30, cx, 380, sf::Color(40, 20, 80), true);
                drawText(window, font, heroPower[i], 20, cx, 428, sf::Color(40, 20, 80), true);
                drawText(window, font, heroInfo[i], 16, cx, 458, sf::Color(40, 20, 80), true);
            }

            drawText(window, font, "LEFT / RIGHT to choose,  ENTER to fight", 24, WIN_W / 2.f, 535, sf::Color(255, 230, 60), true);
        }

        //--------------------GAME SCREEN (PLAYING, PAUSED, RESULT)--------------------
        else
        {
            //BULLETS (GLOW + BODY + WHITE CENTER)
            for (int i = 0; i < MAX_BULLETS; i++)
            {
                if (bAlive[i])
                {
                    drawCircle(window, bX[i], bY[i], bRadius[i] + 4, sf::Color(bColor[i].r, bColor[i].g, bColor[i].b, 80));
                    drawCircle(window, bX[i], bY[i], bRadius[i], bColor[i]);
                    drawCircle(window, bX[i], bY[i], bRadius[i] * 0.4f, sf::Color::White);
                }
            }

            //MONSTER AND HERO
            drawMonster(window, monsterX, monsterY, monsterRadius, monsterFlash > 0.f);

            bool flicker = (hurtCooldown > 0.f) && ((int)(gameTime * 20) % 2 == 0);
            drawHero(window, heroX, heroY, 1.f, heroColor[selected], power, flicker, gameTime);

            //FLOATING TEXT
            if (popLife > 0.f)
            {
                float fade = std::min(1.f, popLife * 2.f);
                sf::Color c(popColor.r, popColor.g, popColor.b, (std::uint8_t)(255 * fade));
                drawText(window, font, popText, popSize, popX, popY, c, true);
            }

            //HUD: HERO SIDE
            drawText(window, font, heroName[selected], 20, 20, 8, sf::Color::White, false);
            drawBar(window, 20, 38, 300, 22, heroHP / heroMaxHP, sf::Color(80, 220, 90));
            drawText(window, font, "POWER  " + std::to_string(power), 16, 20, 66, sf::Color(255, 230, 60), false);

            //HUD: MONSTER SIDE
            drawText(window, font, "MONSTER  LV " + std::to_string(level), 20, WIN_W - 320, 8, sf::Color::White, false);
            drawBar(window, WIN_W - 320, 38, 300, 22, monsterHP / monsterMaxHP, sf::Color(230, 60, 60));

            //HUD: CENTER
            int secondsLeft = (int)std::ceil(std::max(0.f, roundTime));
            drawText(window, font, "TIME  " + std::to_string(secondsLeft), 26, WIN_W / 2.f, 6, sf::Color::White, true);
            drawText(window, font, "SCORE  " + std::to_string(score), 20, WIN_W / 2.f, 44, sf::Color(255, 230, 60), true);
            drawText(window, font, "Heroes " + std::to_string(heroWins) + " - " + std::to_string(monsterWins) + " Monsters", 16, WIN_W / 2.f, 76, sf::Color::White, true);

            //PAUSED SCREEN
            if (state == 3)
            {
                drawRect(window, 0, 0, WIN_W, WIN_H, sf::Color(0, 0, 0, 150));
                drawText(window, font, "PAUSED", 80, WIN_W / 2.f, 240, sf::Color::White, true);
                drawText(window, font, "Press P to resume", 28, WIN_W / 2.f, 340, sf::Color(255, 230, 60), true);
            }

            //RESULT SCREEN
            if (state == 4)
            {
                drawRect(window, 0, 0, WIN_W, WIN_H, sf::Color(0, 0, 0, 160));

                if (heroWonLast)
                {
                    drawText(window, font, "HEROES WIN!", 80, WIN_W / 2.f, 120, sf::Color(90, 230, 120), true);
                }
                else
                {
                    drawText(window, font, "MONSTER WINS!", 80, WIN_W / 2.f, 120, sf::Color(230, 80, 80), true);
                }

                drawText(window, font, "Score: " + std::to_string(score) + "    Best: " + std::to_string(bestScore), 32, WIN_W / 2.f, 250, sf::Color(255, 230, 60), true);
                drawText(window, font, "Monster reached level " + std::to_string(level), 24, WIN_W / 2.f, 305, sf::Color::White, true);
                drawText(window, font, "Total wins   Heroes " + std::to_string(heroWins) + " - " + std::to_string(monsterWins) + " Monsters", 26, WIN_W / 2.f, 355, sf::Color::White, true);
                drawText(window, font, "ENTER: play again   C: change hero   ESC: exit", 20, WIN_W / 2.f, 460, sf::Color(255, 230, 60), true);
            }
        }

        window.display();
    }

    return 0;
}