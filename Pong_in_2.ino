#include "LedControl.h" //  need the library
#include "LiquidCrystal.h"
const int rs = 7, en = 6, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
LedControl lc=LedControl(12,11,10,1);
const int switchPin1 = 13;
const int yPin1 = 1; // PLAYER 1
// pin 12 is connected to the MAX7219 pin 1
// pin 11 is connected to the CLK pin 13
// pin 10 is connected to LOAD pin 12
// 1 as we are only using 1 MAX7219
const int scoreMax = 3;
const int buttonDown2 = 9; // PLAYER 2
const int buttonUp2 = 8;

enum eDir { STOP = 0, LEFT = 1, UPLEFT =2, DOWNLEFT =3, RIGHT = 4, UPRIGHT = 5, DOWNRIGHT = 6 } ;

class cBall
{
private:
    int x,y,originalX,originalY;
    eDir direction;
public:
    cBall(int posX,int posY)
    {
        originalX = posX;
        originalY = posY;
        x = posX ;
        y = posY;
        direction = STOP;
    }
    void Reset()
    {
        x = originalX ;
        y = originalY;
        direction = STOP;
    }
    void changeDirection(eDir d)
    {
        direction = d;
    }
    void randomDirection()
    {
        direction = (eDir)(rand()%6+1);
    }
    int getX()
    {
        return x;
    }
    int getY()
    {
        return y;
    }
    eDir getDirection()
    {
        return direction ;
    }
    void Move()
    {
        switch(direction)
        {
        case STOP:
            break;
        case LEFT:
            --x;
            break;
            break;
        case RIGHT:
            ++x;
            break;
        case UPLEFT:
            --x;
            --y;
            break;
        case DOWNLEFT:
            --x;
            ++y;
            break;
        case UPRIGHT:
            ++x;
            --y;
            break;
        case DOWNRIGHT:
            ++x;
            ++y;
            break;
        default:
            break;
        }
    }

};

class cPaddle
{
private:
    int x,y;
    int originalX,originalY;
public:
    cPaddle()
    {
        x = y = 0;
    }
    cPaddle(int posX,int posY):cPaddle()
    {
        originalX = posX;
        originalY = posY;
        x = posX;
        y = posY;
    }
    void Reset()
    {
        x = originalX;
        y = originalY;
    }
    int getX()
    {
        return x;
    }
    int getY()
    {
        return y;
    }
    void moveUp()
    {
        --y;
    }
    void moveDown()
    {
        ++y;
    }

};
class cGameManager
{
private:
    int width,  height;
    int score1, score2;
    int hitCount; // creste nivelul de dificultate ( direct proportional cu viteza mingii )
    bool up1, down1, up2, down2;  // input pentru paleta1 si paleta2
    bool quit;  // quit = false -> jocul merge , altfel stop
    cBall * ball;
    cPaddle *player1;
    cPaddle *player2;
public:
    cGameManager(int w, int h) //!!!
    {
        score1 = score2 = 0;
        hitCount = 0;
        up1 = down1 = up2 = down2 = false;
        width = w;
        height = h;
        ball = new cBall( width/2, height/2 );
        player1 = new cPaddle( 0, height/2-1 );
        player2 = new cPaddle( width-1, height/2-1 );
        quit = false;
    }
    ~cGameManager()
    {
        delete ball;
        delete player1;
        delete player2;
    }
    void endMenu(int playerNumber)
    {
        lcd.clear();
        lcd.setCursor(4,0);
        lcd.print("PLAYER ");
        lcd.print( playerNumber );
        lcd.setCursor(6,1);
        lcd.print(" WINS");
        delay(2000);

    }
    void ScoreUp(cPaddle * player)
    {
        Draw();
        // delay(500);
        if(player == player1)
        {
            score1++;
            lcd.setCursor(11,0);
            lcd.print(score1);
        }
        else if(player == player2)
        {
            score2++;
            lcd.setCursor(11,1);
            lcd.print(score2);
        }
        ball->Reset();
        player1->Reset();
        player2->Reset();
        hitCount = 0;
        Draw();
        if(score1 == scoreMax)
        {
            endMenu(1);
            quit = true;
        }
        if(score2 == scoreMax)
        {
            endMenu(2);
            quit = true;
        }
        delay(1500);
    }
    void Draw()   // Stinge/Aprinde led-urile in functie de pozitiile paletelor si mingii
    {
        int ballx = ball->getX();
        int bally = ball->getY();
        int player1x = player1->getX();
        int player1y = player1->getY();
        int player2x = player2->getX();
        int player2y = player2->getY();
        for(int i= 0; i<height; i++)
        {
            for(int j = 0; j<width; j++)
            {

                if(ballx == j && bally == i)  // Mingea
                    lc.setLed(0,i,j,true);

                else if(player1x == j && player1y == i)   // Paleta 1
                    lc.setLed(0,i,j,true);
                else if(player1x == j && player1y+1 == i)
                    lc.setLed(0,i,j,true);
                else if(player1x == j && player1y+2 == i)
                    lc.setLed(0,i,j,true);


                else if(player2x == j && player2y == i) // Paleta 2
                    lc.setLed(0,i,j,true);
                else if(player2x == j && player2y+1 == i)
                    lc.setLed(0,i,j,true);
                else if(player2x == j && player2y+2 == i)
                    lc.setLed(0,i,j,true);

                else   lc.setLed(0,i,j,false);

            }

        }

    }

    void input()  // actualizeaza pozitia paletelor in functie de input
    {
        ball->Move();
        int ballx =  ball->getX();
        int bally = ball->getY();
        int player1x = player1->getX();
        int player1y = player1->getY();
        int player2x = player2->getX();
        int player2y = player2->getY();
        up1 = up2 = down1 = down2 = false;
        
        up2 = digitalRead(buttonUp2);     // input player 2
        down2 = digitalRead(buttonDown2);
       
        if(analogRead(yPin1)>800) // input player 1
        down1 = true;
        if(analogRead(yPin1)<250)
        up1 = true;
     
        // ne asiguram ca paletele un ies in afara jocului 
        if( up1 == true )
          if( player1y > 0)
            player1->moveUp();

        if( up2 == true )
          if( player2y > 0 )
            player2->moveUp();

        if( down1 == true )
          if(player1y+3 < height )
            player1->moveDown();

        if( down2 == true )
          if( player2y+3 < height)
            player2->moveDown();

        if(ball->getDirection() == STOP )  // inceput de joc
          ball->changeDirection(LEFT);
         
        delay( max(200-hitCount*6, 90) );
    }
    
    void Logic()
    {   delay(max( 200-hitCount*6, 90) );
        int ballx =  ball->getX();
        int bally = ball->getY();
        int player1x = player1->getX();
        int player1y = player1->getY();
        int player2x = player2->getX();
        int player2y = player2->getY();


         //marginea de sus
        if(bally == height -1)
            ball->changeDirection(ball->getDirection() ==DOWNRIGHT ? UPRIGHT : UPLEFT);
        //marginea de jos
        if(bally == 0)
            ball->changeDirection(ball->getDirection() ==UPRIGHT ? DOWNRIGHT : DOWNLEFT);
 
        // paleta 1 
        for(int i = 0; i<3 ; i++)
            if( ballx == player1x+1)
                if(bally == player1y + i)
                {
                    ball->changeDirection( ( eDir)( rand()%3+4 ) );
                    ++hitCount;
                }
           
         if(ballx == player1x+1)    // mingea pe coltul paletei 1
        {
          if(bally == player1y -1)
            if( ball->getDirection() == DOWNLEFT)
              ball->changeDirection(UPRIGHT);
          if(bally == player1y +3)
            if(ball->getDirection() == UPLEFT )
              ball->changeDirection(DOWNRIGHT);
          ++hitCount;
        }

        //paleta dreapta
        for(int i = 0 ; i<3 ; i++)
            if(ballx == player2x-1)
                if(bally == player2y + i)
                    {
                      ball->changeDirection( ( eDir)(rand()%3+1 ) );
                      ++hitCount;
                    }
        if(ballx == player2x-1) // mingea pe coltul paletei 2
        {
          if(bally == player2y -1 )
            if(ball->getDirection() == DOWNRIGHT)
              ball->changeDirection(UPLEFT);
          if(bally == player2y +3 )
            if(ball->getDirection() == UPRIGHT )
              ball->changeDirection(DOWNLEFT);
       
          ++hitCount;
        }
                    
        //margine dreapta
        if(ballx == width -1)
            ScoreUp(player1);

        //marginea stanga
        if(ballx == 0)
            ScoreUp(player2);
            
    }
    void Run()  // cicleaza jocul cat timp quit = false;
    {
        while(!quit)
        {
            Draw(); 
            input();
          //  Draw();
            Logic();
        }
    }

};
void setup() {
  lc.shutdown(0,false);// turn off power saving, enables display
  lc.setIntensity(0,8);// sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen
  pinMode(buttonUp2, INPUT);   // PLAYER 2  (2 butoane)
  pinMode(buttonDown2 , INPUT);
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  pinMode(switchPin1,INPUT); // PLAYER 1 ( joystick )
  digitalWrite(switchPin1,HIGH);
}
void startMenu()  // meniu inainte de inceperea jocului
{
  //lcd.noBlink();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Press the Switch");
  lcd.setCursor(4,1);
  lcd.print("to START");
  delay(600);
}
void initGameMenu() // Initializeaza meniul de pe lcd
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("PLAYER 1 : 0");
  lcd.setCursor(0,1);
  lcd.print("PLAYER 2 : 0");
}
cGameManager *game;
void loop() {
  startMenu(); 
  if(digitalRead(switchPin1)==0)
  {
    game = new cGameManager(8,8);
    initGameMenu();
    game->Run();
    delete game;
  }
//  lcd.blink();
  
  
}

