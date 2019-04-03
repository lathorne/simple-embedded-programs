#include "supportFiles/display.h"
#include "wamDisplay.h"
#include "wamControl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../switchAndButtonLab/switches.h"

#define NINE_COUNT 9 //a pound define for the number of moles in the game
#define SIX_COUNT 6 //a pound define for the number of moles in the game
#define FOUR_COUNT 4 //a pound define for the number of moles in the game
#define CLEAR 0 //to clear a variable

#define FOREVER 1 //for a forever loop

#define X display_width() //width of display
#define Y display_height() //height of display
#define SCORE_OFFSET 30 //offset for scoreboard

#define CIRCLE_RADIUS 25 // radius of mole circle
#define FIRST_COLUMN 35 // first column of mole matrix
#define COLUMN_BREAK 72 // difference between columns
#define FIRST_ROW 35 // first row of mole matrix
#define ROW_BREAK 125 // difference between rows

#define WORD_OFFSET 7 //offset the words on the scoreboard
#define SCORE_LENGTH_OFFSET 40 //offset for the variables on scoreboard
#define HIT_OFFSET 60 //offset for hits on scoreboard
#define MISS_WORD_OFFSET 95 //offset for words on scoreboard
#define MISS_OFFSET 165 //offset for miss on scoreboard
#define LEVEL_WORD_OFFSET 210 //offset for level on scoreboard
#define LEVEL_OFFSET 290 // offset for level variable on scoreboard

#define LARGE_TEXT_SIZE 4 //large test size
#define MEDIUM_TEXT_SIZE 3 //medium text size
#define TEXT_SIZE 2 //small text size

//definition of the holes within the array
#define HOLE_ZERO 0 //represent hole zero
#define HOLE_ONE 1 //represent hole one
#define HOLE_TWO 2//represent hole two
#define HOLE_THREE 3//represent hole three
#define HOLE_FOUR 4//represent hole four
#define HOLE_FIVE 5//represent hole five
#define HOLE_SIX 6 //represent hole six
#define HOLE_SEVEN 7 //represent hole seven
#define HOLE_EIGHT 8 //represent hole eight

//display defines
#define WHACK_X 50 //x coordinate for whack a mole title
#define WHACK_Y 90 //y coordinate for whack a mole title
#define START_X 70 //x coordinate for start
#define START_Y 140 //y coordinate for start
#define GO_X 50 //game over x coordinate
#define GO_Y 70 //game over y coordinate
#define HITS_X 70 //hits x
#define HITS_Y 110 //hits y
#define HITS_VAR_X 140 //hits variable x
#define HITS_VAR_Y 110 //hits variable y
#define MISSES_X 70 //misses x coordinate
#define MISSES_Y 140 //misses y coordinate
#define MISSES_VAR_X 165 //misses variable x
#define MISSES_VAR_Y 140 //misses variable y
#define FL_X 70 //final level x
#define FL_Y 170 //final level y
#define LEVEL_VAR_X 225 //level variable x
#define LEVEL_VAR_Y 170 //level variable y
#define TRY_X 35 //try again text x
#define TRY_Y 200 //try again text y

#define MOLE_OFFSET 1 //get the right number for a random index
#define INDEXER_OFFSET 2 //increment the array indexer of the origins arrays

#define NUM_POINT_VAR_18 18 //number of variables necessary to store x and y values of each origin (9 holes)
#define NUM_POINT_VAR_12 12 //number of variables necessary to store x and y values of each origin (6 holes)
#define NUM_POINT_VAR_8 8 //number of variables necessary to store x and y values of each origin (4 holes)

//split screen for mole whacking
#define LEFT display_width()/3 //right bound of left side
#define RIGHT LEFT*2 //left bound of right side
#define TOP display_height()/3 //bottom bound of top side
#define BOTTOM TOP*2 //top bound of bottom side

#define HITS_PER_LEVEL 5 //number of hits per level
#define ERASE_RECT 30 //the length of the side of the erasing rectangle for the scoreboard

//taken from main.c
#define SWITCH_VALUE_9 9  // Binary 9 on the switches indicates 9 moles.
#define SWITCH_VALUE_6 6  // Binary 6 on the switches indicates 6 moles.
#define SWITCH_VALUE_4 4  // Binary 9 on the switches indicates 4 moles.
#define SWITCH_MASK 0xf   // Ignore potentially extraneous bits.

uint16_t wam_misses; //keep track of the misses
uint16_t wam_hits; //keep track of hits
uint16_t wam_level; //keep track of the level
uint8_t num_moles; //keep track of the number of moles
bool miss; //this boolean will keep track of whether or not a tap is a miss
uint16_t hits_per_level; //a variable to store the number of hits per level

//center of circles array for 9 holes
uint16_t origin_array_9[NUM_POINT_VAR_18] = {FIRST_ROW, FIRST_COLUMN, FIRST_ROW + ROW_BREAK, FIRST_COLUMN, FIRST_ROW + ROW_BREAK + ROW_BREAK, FIRST_COLUMN,
                           FIRST_ROW, FIRST_COLUMN + COLUMN_BREAK, FIRST_ROW + ROW_BREAK, FIRST_COLUMN + COLUMN_BREAK, FIRST_ROW + ROW_BREAK + ROW_BREAK, FIRST_COLUMN + COLUMN_BREAK,
                           FIRST_ROW, FIRST_COLUMN + COLUMN_BREAK + COLUMN_BREAK, FIRST_ROW + ROW_BREAK, FIRST_COLUMN + COLUMN_BREAK + COLUMN_BREAK, FIRST_ROW + ROW_BREAK + ROW_BREAK, FIRST_COLUMN + COLUMN_BREAK + COLUMN_BREAK};

//center of circles array for 6 holes
uint16_t origin_array_6[NUM_POINT_VAR_12] = {FIRST_ROW, FIRST_COLUMN, FIRST_ROW + ROW_BREAK, FIRST_COLUMN, FIRST_ROW + ROW_BREAK + ROW_BREAK, FIRST_COLUMN,
                           FIRST_ROW, FIRST_COLUMN + COLUMN_BREAK + COLUMN_BREAK, FIRST_ROW + ROW_BREAK, FIRST_COLUMN + COLUMN_BREAK + COLUMN_BREAK, FIRST_ROW + ROW_BREAK + ROW_BREAK, FIRST_COLUMN + COLUMN_BREAK + COLUMN_BREAK};

//center of circles array for 4 holes
uint16_t origin_array_4[NUM_POINT_VAR_8] = {FIRST_ROW, FIRST_COLUMN, FIRST_ROW + ROW_BREAK + ROW_BREAK, FIRST_COLUMN,
                           FIRST_ROW, FIRST_COLUMN + COLUMN_BREAK + COLUMN_BREAK, FIRST_ROW + ROW_BREAK + ROW_BREAK, FIRST_COLUMN + COLUMN_BREAK + COLUMN_BREAK};

/********************** typedefs **********************/
// This keeps track of all mole information.
typedef struct {
        wamDisplay_point_t origin;  // This is the origin of the hole for this mole.
        // A mole is active if either of the tick counts are non-zero. The mole is dormant otherwise.
        // During operation, non-zero tick counts are decremented at a regular rate by the control state machine.
        // The mole remains in his hole until ticksUntilAwake decrements to zero and then he pops out.
        // The mole remains popped out of his hole until ticksUntilDormant decrements to zero.
        // Once ticksUntilDomant goes to zero, the mole hides in his hole and remains dormant until activated again.
        wamDisplay_moleTickCount_t ticksUntilAwake;  // Mole will wake up (pop out of hole) when this goes from 1 -> 0.
        wamDisplay_moleTickCount_t ticksUntilDormant; // Mole will go dormant (back in hole) this goes 1 -> 0.
} wamDisplay_moleInfo_t;

// This will contain pointers to all of the mole info records.
// This will ultimately be treated as an array of pointers.
static wamDisplay_moleInfo_t** wamDisplay_moleInfo;

// Allocates the memory for wamDisplay_moleInfo_t records.
// Computes the origin for each mole assuming a simple row-column layout:
// 9 moles: 3 rows, 3 columns, 6 moles: 2 rows, 3 columns, 4 moles: 2 rows, 2 columns
// Also inits the tick counts for awake and dormant.
void wamDisplay_computeMoleInfo() {
    // Setup all of the moles, creates and inits mole info records.
    // Create the container array. It contains pointers to each of the mole-hole info records.

    // allocate the memory to store the pointers
    uint8_t num_pointers = num_moles; //find out how many pointers there are
    wamDisplay_moleInfo = (wamDisplay_moleInfo_t**)malloc(num_pointers * sizeof(wamDisplay_moleInfo_t*));// Need to uncomment this line and initialize this variable via malloc().

    //allocate memory for each struct in the array wamDisplay_moleInfo
    for(uint8_t i = 0; i < num_pointers; i++){
        wamDisplay_moleInfo[i] = (wamDisplay_moleInfo_t*)malloc(sizeof(wamDisplay_moleInfo_t)); //grab the memory address for each struct
    }

    uint16_t *origins; //pointer to point to origins array that needs to be used

    //switch statement to choose the right origin array so we can know the origins of our holes
    switch(num_moles){ //depending on the number of holes
    case NINE_COUNT: //9 holes
        origins = origin_array_9; //origins will point to the array with nine mole origins
        break;
    case SIX_COUNT: //6 holes
        origins = origin_array_6; //origins will point to the array with six mole origins
        break;
    case FOUR_COUNT: //4 holes
        origins = origin_array_4; //origins will point to the array with four mole origins
        break;
    }

    //set the mole values here
    for(uint8_t i = 0; i < num_pointers; i++){ //go through the moles
        wamDisplay_moleInfo[i]->origin.x = origins[INDEXER_OFFSET * i]; //just need to set this to something
        wamDisplay_moleInfo[i]->origin.y = origins[INDEXER_OFFSET * i + MOLE_OFFSET]; //just need to set this to something
        wamDisplay_moleInfo[i]->ticksUntilAwake = CLEAR; //randomly assign this
        wamDisplay_moleInfo[i]->ticksUntilDormant = CLEAR; //these will be reassigned with random values when the randomMole function is called
    }

}

void wamDisplay_free(){
    // When you are done, you must return the memory to the system or you will create a memory leak.
    // First deallocate all of the structs.
    for (uint8_t l = 0; l < num_moles; l++) {
       free(wamDisplay_moleInfo[l]);   // This deallocates the memory.
       wamDisplay_moleInfo[l] = NULL;  // This step is not necessary but will keep you from reusing deallocated memory.
    }
    // Next, deallocate the array that contains the pointers to the structs.
    free(wamDisplay_moleInfo);   // Deallocates the container of arrays.
    wamDisplay_moleInfo = NULL;  // Also keeps you from reusing the deallocated memory.
}

//function to draw mole
void wamDisplay_drawMole(bool erase, uint16_t index){
    if(erase){ //erase the mole
        display_fillCircle(wamDisplay_moleInfo[index]->origin.x, wamDisplay_moleInfo[index]->origin.y, CIRCLE_RADIUS, DISPLAY_BLACK);
    }else{ //draw the mole
        display_fillCircle(wamDisplay_moleInfo[index]->origin.x, wamDisplay_moleInfo[index]->origin.y, CIRCLE_RADIUS, DISPLAY_RED);
    }
}

// Provide support to set games with varying numbers of moles. This function
// would be called prior to calling wamDisplay_init();
void wamDisplay_selectMoleCount(wamDisplay_moleCount_e moleCount){
    if(moleCount == wamDisplay_moleCount_4){ //if set to 4 mole
        num_moles = FOUR_COUNT; //set mole value
    }else if(moleCount == wamDisplay_moleCount_6){ //if set to 6 moles
        num_moles = SIX_COUNT; //set mole value
    }else{ //otherwise
        num_moles = NINE_COUNT; //set to 9
    }

}

// Call this before using any wamDisplay_ functions.
void wamDisplay_init(){
    wam_hits = CLEAR; //clear the hits
    wam_level = CLEAR; //set level to 0
    wam_misses = CLEAR; //set misses to 0
    hits_per_level = CLEAR; //init the hits per level
    if(wamDisplay_moleInfo != NULL){ //check if there are any current moles
        wamDisplay_free(); //free the previous mole info
    }
    wamDisplay_computeMoleInfo(); //compute the mole info
}

//helper function to draw a board with 4 holes
void wamDisplay_draw4Board(){
    display_fillCircle(FIRST_ROW, FIRST_COLUMN, CIRCLE_RADIUS, DISPLAY_BLACK); //top left
    display_fillCircle(FIRST_ROW + ROW_BREAK + ROW_BREAK, FIRST_COLUMN, CIRCLE_RADIUS, DISPLAY_BLACK); //top right
    display_fillCircle(FIRST_ROW + ROW_BREAK + ROW_BREAK, FIRST_COLUMN + COLUMN_BREAK + COLUMN_BREAK, CIRCLE_RADIUS, DISPLAY_BLACK); //bottom right
    display_fillCircle(FIRST_ROW, FIRST_COLUMN + COLUMN_BREAK + COLUMN_BREAK, CIRCLE_RADIUS, DISPLAY_BLACK); //bottom left
}

//helper function to draw a board with 6 holes
void wamDisplay_draw6Board(){
    wamDisplay_draw4Board(); //draw the first four circles, then add two
    display_fillCircle(FIRST_ROW + ROW_BREAK, FIRST_COLUMN, CIRCLE_RADIUS, DISPLAY_BLACK); //top middle
    display_fillCircle(FIRST_ROW + ROW_BREAK, FIRST_COLUMN + COLUMN_BREAK + COLUMN_BREAK, CIRCLE_RADIUS, DISPLAY_BLACK); //bottom middle
}

//helper function to draw a board with 9 holes
void wamDisplay_draw9Board(){
    wamDisplay_draw6Board(); //draw the first six circles, add three more
    display_fillCircle(FIRST_ROW, FIRST_COLUMN + COLUMN_BREAK, CIRCLE_RADIUS, DISPLAY_BLACK); //middle left
    display_fillCircle(FIRST_ROW + ROW_BREAK, FIRST_COLUMN + COLUMN_BREAK, CIRCLE_RADIUS, DISPLAY_BLACK); //middle middle
    display_fillCircle(FIRST_ROW + ROW_BREAK + ROW_BREAK, FIRST_COLUMN + COLUMN_BREAK, CIRCLE_RADIUS, DISPLAY_BLACK); //middle right
}

// Draw the game display with a background and mole holes.
void wamDisplay_drawMoleBoard(){
    display_fillRect(CLEAR, CLEAR, X, Y - SCORE_OFFSET, DISPLAY_GREEN); //fill the board as green
    switch(num_moles){ //determine the number of holes on the board

    case NINE_COUNT: //if there are nine holes
        wamDisplay_draw9Board(); //call helper function to draw nine holes
        break;


    case SIX_COUNT: //if there are six holes
        wamDisplay_draw6Board(); //call helper function to draw six holes
        break;


    case FOUR_COUNT: //if there are four holes
        wamDisplay_draw4Board(); //call helper function to draw four holes
        break;


    } //end switch
    wamDisplay_drawScoreScreen(); //draw the score screen
}

// Draw the initial splash (instruction) screen.
void wamDisplay_drawSplashScreen(){
    display_fillScreen(DISPLAY_BLACK); //clear the screen
    display_setTextColor(DISPLAY_WHITE); //set text color
    display_setCursor(WHACK_X , WHACK_Y ); //sets the cursor for the larger test
    display_setTextSize(MEDIUM_TEXT_SIZE); //sets  the text size for larger test
    display_print("Whack A Mole!"); //display text on LCD
    display_setCursor(START_X , START_Y ); //sets the cursor for the larger test
    display_setTextSize(TEXT_SIZE); //sets  the text size for larger test
    display_print("Touch to Start"); //display text on LCD
}

// Draw the game-over screen.
void wamDisplay_drawGameOverScreen(){
    display_fillScreen(DISPLAY_BLACK); //clear the screen
    display_setTextColor(DISPLAY_WHITE); //set text color
    display_setCursor(GO_X, GO_Y); //sets the cursor
    display_setTextSize(LARGE_TEXT_SIZE); //sets  the text size for larger test
    display_print("Game Over"); //display text on LCD
    display_setCursor(HITS_X, HITS_Y); //sets the cursor
    display_setTextSize(TEXT_SIZE); //sets  the text size for smaller text
    display_print("Hits:"); //display text on LCD
    display_setCursor(HITS_VAR_X, HITS_VAR_Y); //sets the cursor
    display_print(wam_hits); //display text on LCD
    display_setCursor(MISSES_X, MISSES_Y); //sets the cursor
    display_print("Misses:"); //print on display
    display_setCursor(MISSES_VAR_X, MISSES_VAR_Y); //sets the cursor
    display_print(wam_misses); //print on display
    display_setCursor(FL_X, FL_Y); //sets the cursor
    display_print("Final Level:"); //display text on LCD
    display_setCursor(LEVEL_VAR_X, LEVEL_VAR_Y); //sets the cursor
    display_print(wam_level); //display text on LCD
    display_setCursor(TRY_X, TRY_Y); //sets the cursor
    display_print("(Touch to Try Again)"); //display text on LCD

}

//function to check if a mole is active
bool wamDisplay_isMoleActive(wamDisplay_moleInfo_t* activeInfo){
    if((activeInfo->ticksUntilDormant != CLEAR) || (activeInfo->ticksUntilAwake != CLEAR)){ //if mole is active
        return true; //return true
    }
    return false; //otherwise, return false
}

// Selects a random mole and activates it.
// Activating a mole means that the ticksUntilAwake and ticksUntilDormant counts are initialized.
// See the comments for wamDisplay_moleInfo_t for details.
// Returns true if a mole was successfully activated. False otherwise. You can
// use the return value for error checking as this function should always be successful
// unless you have a bug somewhere.
bool wamDisplay_activateRandomMole(){
    //select a random mole
    uint16_t mole_index = rand() % num_moles; //random index, should things change up if the random number chose in already an active mole
    if(!wamDisplay_isMoleActive(wamDisplay_moleInfo[mole_index])){ //check if mole is active
        wamDisplay_moleInfo[mole_index]->ticksUntilAwake = wamControl_getRandomMoleAwakeInterval(); //create a random time between 0.5 and 1.5 seconds for 50ms tick
        wamDisplay_moleInfo[mole_index]->ticksUntilDormant = wamControl_getRandomMoleAsleepInterval() + wamDisplay_moleInfo[mole_index]->ticksUntilAwake - wam_level; //same as line above
        return true;
    }

    return false; //mole was active, so it was not initialized
}

//draws the level
void wamDisplay_drawLevel(){
    //first erase current level
    display_fillRect(LEVEL_OFFSET, Y - SCORE_OFFSET + WORD_OFFSET, ERASE_RECT, ERASE_RECT, DISPLAY_BLACK); //print on display
    //display new score
    display_setCursor(LEVEL_OFFSET, Y - SCORE_OFFSET + WORD_OFFSET); //set cursor for first text
    display_print(wam_level); //print on display
}

//draws the misses
void wamDisplay_drawMisses(){
    //first erase current misses
    display_fillRect(MISS_OFFSET, Y - SCORE_OFFSET + WORD_OFFSET, ERASE_RECT, ERASE_RECT, DISPLAY_BLACK); //print on display
    //display new score
    display_setCursor(MISS_OFFSET, Y - SCORE_OFFSET + WORD_OFFSET); //set cursor for first text
    display_print(wam_misses); //print on display
}

//draws the hit score
void wamDisplay_drawHitScore(){
    //first erase current score
    display_fillRect(HIT_OFFSET, Y - SCORE_OFFSET + WORD_OFFSET, ERASE_RECT, ERASE_RECT, DISPLAY_BLACK); //print on display
    //display new score
    display_setCursor(HIT_OFFSET, Y - SCORE_OFFSET + WORD_OFFSET); //set cursor for first text
    display_print(wam_hits); //print on display
}

//helper function to erase the mole and reset the tick values when it is hit
void wamDisplay_computeWhackage(int16_t index){
    //need to set miss in here
    if(wamDisplay_moleInfo[index]->ticksUntilAwake == CLEAR && wamDisplay_moleInfo[index]->ticksUntilDormant > CLEAR){ //this means the mole is active and was successfully hit!
        wamDisplay_drawMole(true, index); //erase the mole
        wam_hits++;; //add a hit!
        hits_per_level++; //increment the hits per level
        wamDisplay_drawHitScore();  //draw the new hit score
        wamDisplay_moleInfo[index]->ticksUntilAwake = CLEAR; //force dormancy
        wamDisplay_moleInfo[index]->ticksUntilDormant = CLEAR; //force dormancy

        //check if level should be incremented here
        if(hits_per_level == HITS_PER_LEVEL){ //this means we need to increase the level
            wam_level++; //increment the level
            wamDisplay_drawLevel(); //redraw the level
            hits_per_level = CLEAR; //clear the number of hits per level
        }
    }
}

// This takes the provided coordinates and attempts to whack a mole. If a
// mole is successfully whacked, all internal data structures are updated and
// the display and score is updated. You can only whack a mole if the mole is awake (visible).
// The return value can be used during testing (you could just print which mole is
// whacked without having to implement the entire game).
wamDisplay_moleIndex_t wamDisplay_whackMole(wamDisplay_point_t* whackOrigin){ //returns int16_t
    int16_t whack_x = whackOrigin->x; //store the x coordinate of the whack
    int16_t whack_y = whackOrigin->y; //store the y coordinate of the whack
    if(whack_x <= LEFT){ //on left side
        if(whack_y <= TOP){ //on top
            wamDisplay_computeWhackage(HOLE_ZERO);
        }else if(whack_y >= BOTTOM){ //on bottom
            if(num_moles == NINE_COUNT){ //for the nine count board
                wamDisplay_computeWhackage(HOLE_SIX); //this is hole six
            }else if(num_moles == SIX_COUNT){ //for the six count board
                wamDisplay_computeWhackage(HOLE_THREE); //this is hole three
            }else{ //for the four count board
                wamDisplay_computeWhackage(HOLE_TWO); //this is hole two
            }
        }else if(num_moles > SIX_COUNT){ //in middle, this row only matters for boards with 9 holes
            wamDisplay_computeWhackage(HOLE_THREE);
        }
    }else if(whack_x >= RIGHT){ //on right side
        if(whack_y <= TOP){ //on top
            if(num_moles >= SIX_COUNT){ //if there are six or nine holes
                wamDisplay_computeWhackage(HOLE_TWO); //this is hole two
            }else{
                wamDisplay_computeWhackage(HOLE_ONE); //otherwise, this is hole one
            }
        }else if(whack_y >= BOTTOM){ //on bottom
            if(num_moles == NINE_COUNT){
                wamDisplay_computeWhackage(HOLE_EIGHT); // this is hole eight
            }else if(num_moles == SIX_COUNT){
                wamDisplay_computeWhackage(HOLE_FIVE); //this is hole five
            }else{
                wamDisplay_computeWhackage(HOLE_THREE); //this is hole three
            }
        }else if(num_moles > SIX_COUNT){ //in middle, this row only matters for boards with 9 holes
            wamDisplay_computeWhackage(HOLE_FIVE);
        }
    }else if(num_moles > FOUR_COUNT){ //on middle, this row only matters if there are more than 4 holes
        if(whack_y <= TOP){ //on top, only matters if there are six or nine holes
            wamDisplay_computeWhackage(HOLE_ONE); //this is hole one on a nine or six board
        }else if(whack_y >= BOTTOM){ //on bottom, only matters if there are six or nine holes
            if(num_moles == NINE_COUNT){
                wamDisplay_computeWhackage(HOLE_SEVEN); //this is hole seven
            }else{
                wamDisplay_computeWhackage(HOLE_FOUR); //on a six board this is hole four
            }
        }else if(num_moles > SIX_COUNT){ //in middle, this row only matters for boards with 9 holes
            wamDisplay_computeWhackage(HOLE_FOUR); //compute whackage on hole four
        }
    }
}

// This updates the ticksUntilAwake/ticksUntilDormant clocks for all of the moles.
void wamDisplay_updateAllMoleTickCounts(){
    //till awake and till dormant get subtracted
    for(uint8_t i = 0; i < num_moles; i++){ //go through all the moles
        if((wamDisplay_moleInfo[i]->ticksUntilAwake) > CLEAR){ //if this isn't zero, we decrement, if this is zero, we know the mole is on the board
            (wamDisplay_moleInfo[i]->ticksUntilAwake)--; //decrement the time until awake
            if(wamDisplay_moleInfo[i]->ticksUntilAwake == CLEAR){ //if the amount of time until awaking has passed
                wamDisplay_drawMole(false, i); //draw the mole
            }
        }

        if((wamDisplay_moleInfo[i]->ticksUntilDormant) > CLEAR){
            (wamDisplay_moleInfo[i]->ticksUntilDormant)--; //decrement the time until asleep
            if(wamDisplay_moleInfo[i]->ticksUntilDormant == CLEAR){ //if the amount of time until going dormant has passed
                wamDisplay_drawMole(true, i); //erase the mole
                //need to increment a miss here
                wamDisplay_incrementMissScore(); //they didn't hit, so its a miss
                wamDisplay_drawMisses(); //redraw the misses
            }
        }
    }

}

// Returns the count of currently active moles.
// A mole is active if it is not dormant, if:
// ticksUntilAwake or ticksUntilDormant are non-zero (in the moleInfo_t struct).
uint16_t wamDisplay_getActiveMoleCount(){
    uint16_t count_active_moles = CLEAR; //set this to zero
    for(uint8_t i = 0; i < num_moles; i++){ //go through all moles
        if(wamDisplay_isMoleActive(wamDisplay_moleInfo[i])){ //if mole is active
            count_active_moles++; //increment the number of active moles
        }
    }
    return count_active_moles; //return the number of active moles
}

// Sets the hit value in the score window.
void wamDisplay_setHitScore(uint16_t hits){
    wam_hits = hits; //set the hit score
}

// Gets the current hit value.
uint16_t wamDisplay_getHitScore(){
    return wam_hits; //return the hits
}

// Sets the miss value in the score window.
void wamDisplay_setMissScore(uint16_t misses){
    wam_misses = misses; //set misses
}

// Gets the miss value.
// Can be used for testing and other functions.
uint16_t wamDisplay_getMissScore(){
    return wam_misses; //return misses
}

// Sets the level value on the score board.
void wamDisplay_incrementLevel(){
    wam_level++; //increment the current level
}

// Retrieves the current level value.
// Can be used for testing and other functions.
uint16_t wamDisplay_getLevel(){
    return wam_level; //returns the wam level
}

// Completely draws the score screen.
// This function renders all fields, including the text fields for "Hits" and "Misses".
// Usually only called once when you are initializing the game.
void wamDisplay_drawScoreScreen(){
    display_fillRect(CLEAR, Y - SCORE_OFFSET, X, Y, DISPLAY_BLACK); //fill score box as black
    display_setCursor(CLEAR, Y - SCORE_OFFSET + WORD_OFFSET); //set cursor for first text
    display_setTextSize(TEXT_SIZE); //set the text size
    display_print("Hit:"); //print on display
    wamDisplay_drawHitScore(); //draw hit score
    display_setCursor(MISS_WORD_OFFSET, Y - SCORE_OFFSET + WORD_OFFSET); //set cursor for first text
    display_print("Miss:"); //print on display
    wamDisplay_drawMisses(); //draw the misses
    display_setCursor(LEVEL_WORD_OFFSET, Y - SCORE_OFFSET + WORD_OFFSET); //set cursor for first text
    display_print("Level:"); //print on display
    wamDisplay_drawLevel(); //draws the level
}

// Make this function available for testing purposes.
void wamDisplay_incrementMissScore(){
    wam_misses++; //increment misses
}

// Reset the scores and level to restart the game.
void wamDisplay_resetAllScoresAndLevel(){
    wam_hits = CLEAR; //reset score
    wam_level = CLEAR; //reset the level
    wam_misses = CLEAR; //reset the misses
}

// Mole count is selected by setting the slide switches. The binary value for the switches
// determines the mole count (1001 - nine moles, 0110 - 6 moles, 0100 - 4 moles).
// All other switch values should default to 9 moles).
void wamDisplay_selectMoleCountFromSwitches(uint16_t switchValue) {
    switch(switchValue & SWITCH_MASK) {
    case SWITCH_VALUE_9:    // this switch pattern = 9 moles.
        wamDisplay_selectMoleCount(wamDisplay_moleCount_9);
        break;
    case SWITCH_VALUE_6:    // this switch pattern = 6 moles.
        wamDisplay_selectMoleCount(wamDisplay_moleCount_6);
        break;
    case SWITCH_VALUE_4:   // this switch pattern = 4 moles.
        wamDisplay_selectMoleCount(wamDisplay_moleCount_4);
        break;
    default:    // Any other pattern of switches = 9 moles.
        wamDisplay_selectMoleCount(wamDisplay_moleCount_9);
        break;
    }
}

// Test function that can be called from main() to demonstrate milestone 1.
// Invoking this function should provide the same behavior as shown in the Milestone 1 video.
void wamDisplay_runMilestone1_test(){
    wamDisplay_init(); //init the display
    while(FOREVER){ //forever loop
        display_fillScreen(DISPLAY_BLACK); //blank the screen
        wamDisplay_drawSplashScreen(); //display start screen
        while(FOREVER){ //wait for touch
            if(display_isTouched()){ //if touched, break out of forever loop
                break;
            }
        }
        display_fillScreen(DISPLAY_BLACK); //blank the screen
        wamDisplay_selectMoleCountFromSwitches(switches_read()); //get the mole count from the switches
        wamDisplay_drawMoleBoard(); //display mole board and scoreboard
        while(FOREVER){ //wait for next touch
            if(display_isTouched()){ //if touched, break out of forever loop
                break;
            }
        }
        display_fillScreen(DISPLAY_BLACK); //blank the screen
        wamDisplay_drawGameOverScreen(); //display game over screen
        while(FOREVER){ //wait for next touch
            if(display_isTouched()){ //if touched, break out of forever loop
                break;
            }
        }
    } //forever loop to keep the test running

}
