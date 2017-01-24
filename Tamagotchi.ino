#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;
//all base shield port numbers
int foodPin = 2;
int gamePin = 3;
int cleanPin = 4;
int hungryPin = 5;
int tiredPin = 6;
int dirtyPin = 7;
int warningPin = 8;
//all positions a tamagotchi can take on the screen
String pos [13] = {"<..>            ",
                   " <..>           ",
                   "  <..>          ",
                   "   <..>         ",
                   "    <..>        ",
                   "     <..>       ",
                   "      <..>      ",
                   "       <..>     ",
                   "        <..>    ",
                   "         <..>   ",
                   "          <..>  ",
                   "           <..> ",
                   "            <..>",};
//when your tamagotchi is dirty take these positions
String dirtyPos [11] = {"`<..>`         ",
                        " `<..>`         ",
                        "  `<..>`        ",
                        "   `<..>`       ",
                        "    `<..>`      ",
                        "     `<..>`     ",
                        "      `<..>`    ",
                        "       `<..>`   ",
                        "        `<..>`  ",
                        "         `<..>` ",
                        "          `<..>`"};
int x = 0; //index for posision lists
int tempo = 500; //the rate of movement for the tamagotchi. This is a planned feature and with the current code the animation speen does not change
//the levels of health for the tamagotchi, should probably be higher so you dont have to constantly care for him/her
int hunger = 300;
int alertness = 300;
int clean = 300;
//if you dont care for him and his levels get to 0, it hurts him, when health hits 0 he dies
int health = 25;
//checks to see if the dirty animation should be used
boolean dust = false;

void setup() {
    Serial.begin(9600);
    pinMode(foodPin, INPUT); //button to feed
    pinMode(gamePin, INPUT); //button to play a mini game, this has not been added yet :(
    pinMode(cleanPin, INPUT); //button to clean him
    pinMode(hungryPin, OUTPUT); //LED to tell you if its hungry
    pinMode(tiredPin, OUTPUT); //LED to tell you if its tired
    pinMode(dirtyPin, OUTPUT); //LED to tell you if its dirty
    pinMode(warningPin, OUTPUT); //buzzer that goes off is hes getting hurt
    lcd.begin(16, 2); //starts screen
    lcd.setRGB(50, 50, 0); //sets screen colour, an option for the user to change the colour is planned but has not been added yet either
}

void loop() {
    //checks all levels of health and preforms actions based on those levels
    checkHunger();
    checkTired();
    checkDirty();
    //picks which animation needs to be used
    if(dust){
        lcd.print(dirtyPos[x]);
        moveDirtyGuy();
    }
    else{
        lcd.print(pos[x]);
        moveGuy();
    }
    //if one of the buttons is hit preform required action
    if(digitalRead(foodPin)){
        feed(20);
    }
    if(digitalRead(cleanPin)){
        cleanOff(20);
    }
    //delay affects animation speed
    delay(tempo);
    //clears screen so you dont get multiple tamagotchis in one game
    lcd.clear();
}

boolean moveRight(){ //checks direction of movement, has a random chance of moving to the right or left for a random animation
    int num = random(1, 10);
    if(num > 5){
        return true;
    }
    else{
        return false;
    }
}

void moveGuy(){ //moves the tamagotchi
    //first two if statements make sure the tamagotchi dosent move off the screen
    if(x == 0){
        x += 1;
    }
    else if(x == 12){
        x -= 1;
    }
    else if(moveRight() == true){ //move to the right
        x += 1;
    }
    else if(moveRight() == false){ //move to the left
        x -= 1;
    }
}

void moveDirtyGuy(){ //same as moveGuy, but for the dirty animation
    if(x == 0){ 
        x += 1;
    }
    else if(x == 10){ //the dirty animation is bigger so stop the tamagotchi earlier
        x -= 1;
    }
    else if(moveRight() == true){
        x += 1;
    }
    else if(moveRight() == false){
        x -= 1;
    }
}

void checkHunger(){ //checks to see if hunger is low
    if(hunger <= 20 && hunger >= 1){ //starts warning you of low hunger
        hunger -= 1; //continue to make hungrier
        digitalWrite(hungryPin, HIGH); //light alert
    }
    else if(hunger == 0){ //no hunger left, hurt the tamagotchi because its starving
        digitalWrite(hungryPin, HIGH);
        digitalWrite(warningPin, HIGH); //buzzer goes off
        delay(500);
        digitalWrite(hungryPin, LOW);
        digitalWrite(warningPin, LOW);
        hurtGuy(); //take away health
    }
    else{
        hunger -= 1; //make hungrier with every movement
        digitalWrite(hungryPin, LOW); //this turns the light and buzzer off if you have just fed your tamagotchi
        digitalWrite(warningPin, LOW);
    }
}

void checkTired(){ //checks to see if the tamagotchi is sleepy or not
    if(alertness <= 20 && alertness >= 1){
        alertness -= 1;
        digitalWrite(tiredPin, HIGH); //start alerting when its about to sleep
    }
    else if(alertness == 0){
        sleepGuy(); //tamagotchi goes to sleep
        digitalWrite(tiredPin, HIGH); 
    }
    else{
        alertness -= 1; //start making sleepier with every movement
        digitalWrite(tiredPin, LOW);
    }
}

void checkDirty(){ //checks to see if the tamagotchi is dirty or clean
    if(clean <= 20 && clean >= 1){ //tamagotchi is getting dirtier
        clean -= 1;
        digitalWrite(dirtyPin, HIGH);
        dust = true;
    }
    else if(clean == 0){ //so far besides the flashing light this dosent change much, other planned features to set 0 cleanliness apart have yet to be added
        digitalWrite(dirtyPin, HIGH);
        delay(500);
        digitalWrite(dirtyPin, LOW);
        dust = true;
    }
    else{ //resets dirty light and dust animation if you just cleaned the tamagotchi
        clean -= 1;
        dust = false;
        digitalWrite(hungryPin, LOW);
    }
}

void hurtGuy(){ //takes away tamagotchi health
    health -= 1;
    if(health == 0){
        gameOver(); //ends the game if it dies
    }
}

void gameOver(){ //this method uses an infinite loop to stop the program from running on the arduino
    lcd.print("Your buddy died!");
    while(true){}
}

void sleepGuy(){ //sleeps the tamagotchi, simply wait until hes awake to feed and clean. a mini game was planned to be playable during sleep but that also has yet to be completed
    lcd.print("      <_ _>      ");
    delay(20000);
    alertness = 300; //resets alertness when he stops sleeping
    digitalWrite(tiredPin, LOW); //turns off light
}

void feed(int h){ //feeds the tamagotchi
    hunger += h;
    if(hunger > 300){ //cant go over fullness cap
        hunger = 300;
    }
}

void cleanOff(int c){ //cleans the tamagotchi
    clean += c;
    if(clean > 300){ //cant go over cleanliness cap (in hindsight a variable for the max value of eat health level would have been ideal)
        clean = 300;
    }
}

