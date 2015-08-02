#include <stdlib.h>
#include <stdbool.h>
#include "Time.h"

//These are the associated libraries with running my LED board.

#include "SPI.h"      
#include "DMD.h" 
#include "TimerOne.h"
#include "Arial_black_16.h"<arial_black_16.h> 
// you can remove the fonts if unused
#include "SystemFont5x7.h"
#define DISPLAYS_ACROSS 1   
#define DISPLAYS_DOWN 1       
/* change these values if you have more than one DMD connected */
DMD dmd(DISPLAYS_ACROSS,DISPLAYS_DOWN);

//

int const COLUMNS = 38;
int const ROWS = 16;
int const SIZE = COLUMNS * ROWS;
int const exit_button = 2;
int const randomised = 3;
int const pattern = 4;
int const factory = 1;


void setup_grid(bool grid[], bool *wrap)
{
	int i;
        *wrap = true;
        if (digitalRead(randomised) == HIGH)
        {
            int time = year() + month() + weekday() + day() + hour() + minute() + second();
            srand(time);
    	    for (i = 0; i < SIZE; i++)
	    {
		if (rand() % 2 == 0)
			grid[i] = true;
		else
			grid[i] = false;
	    }
        }
        else
        {
            for (i = 0; i < SIZE; i++)
	    {
		grid[i] = false;
	    }
            i = 0;
            if (digitalRead(pattern) == HIGH)
            {
                i = 0;
                while (i <= 20)
                {
                    grid[(5 + i) + (6 * COLUMNS)] = true;
                    grid[(6 + i) + (4 * COLUMNS)] = true;
                    grid[(6 + i) + (6 * COLUMNS)] = true;
                    grid[(7 + i) + (5 * COLUMNS)] = true;
                    grid[(7 + i) + (6 * COLUMNS)] = true;
                    i += 5;
                }
            }
            else
            {
                *wrap = false;
                grid[(1) + (5 * COLUMNS)] = true;
                grid[(1) + (6 * COLUMNS)] = true;
                grid[(2) + (5 * COLUMNS)] = true;
                grid[(2) + (6 * COLUMNS)] = true;
                grid[(11) + (5 * COLUMNS)] = true;
                grid[(11) + (6 * COLUMNS)] = true;
                grid[(11) + (7 * COLUMNS)] = true;
                grid[(12) + (4 * COLUMNS)] = true;
                grid[(12) + (8 * COLUMNS)] = true;
                grid[(13) + (3 * COLUMNS)] = true;
                grid[(13) + (9 * COLUMNS)] = true;
                grid[(14) + (3 * COLUMNS)] = true;
                grid[(14) + (9 * COLUMNS)] = true;
                grid[(15) + (6 * COLUMNS)] = true;
                grid[(16) + (4 * COLUMNS)] = true;
                grid[(16) + (8 * COLUMNS)] = true;
                grid[(17) + (5 * COLUMNS)] = true;
                grid[(17) + (6 * COLUMNS)] = true;
                grid[(17) + (7 * COLUMNS)] = true;
                grid[(18) + (6 * COLUMNS)] = true;
                grid[(21) + (3 * COLUMNS)] = true;
                grid[(21) + (4 * COLUMNS)] = true;
                grid[(21) + (5 * COLUMNS)] = true;
                grid[(22) + (3 * COLUMNS)] = true;
                grid[(22) + (4 * COLUMNS)] = true;
                grid[(22) + (5 * COLUMNS)] = true;
                grid[(23) + (2 * COLUMNS)] = true;
                grid[(23) + (6 * COLUMNS)] = true;
                grid[(25) + (1 * COLUMNS)] = true;
                grid[(25) + (2 * COLUMNS)] = true;
                grid[(25) + (6 * COLUMNS)] = true;
                grid[(25) + (7 * COLUMNS)] = true;
                grid[(35) + (3 * COLUMNS)] = true;
                grid[(35) + (4 * COLUMNS)] = true;
                grid[(36) + (3 * COLUMNS)] = true;
                grid[(36) + (4 * COLUMNS)] = true;
            }
        }
}

int neighbour_count(bool grid[], int cell, bool wrap)
{
	int result = 0;
	int nbr_col;
	int nbr_row;
        int temp_cell;
        int temp_col;
	for (nbr_col = (cell - 1); nbr_col <= (cell + 1); nbr_col++)
	{
                if ((nbr_col == cell - 1) && (cell % COLUMNS == 0))
                        {if (wrap) temp_col = nbr_col + COLUMNS; else continue;}
		else if ((nbr_col == cell + 1) && ((cell + 1) % COLUMNS == 0))
                        {if (wrap) temp_col = nbr_col - COLUMNS; else continue;}
                else
                        temp_col = nbr_col;
		for (nbr_row = (temp_col - COLUMNS); nbr_row <= (temp_col + COLUMNS); nbr_row += COLUMNS)
		{
			if (cell == nbr_row)
				continue;
                        if (nbr_row > (SIZE - 1))
                                {if (wrap) temp_cell = nbr_row - SIZE; else continue;}
                        else if (nbr_row < 0)
                                {if (wrap) temp_cell = nbr_row + SIZE; else continue;}
                        else
                                temp_cell = nbr_row;
			if (grid[temp_cell] == true)
				result++;
		}
	}
	return result;
}

void update_grid(bool grid[], bool wrap)
{
	bool new_grid[SIZE];
	int cell;
	int cell_count;
	int i;
	for (i = 0; i < SIZE; i++)
	{
		new_grid[i] = grid[i];
	}
	for (cell = 0; cell < SIZE; cell++)
	{
		cell_count = neighbour_count(grid, cell, wrap);
		if (grid[cell])
		{
			if ((cell_count == 0) || (cell_count == 1))
				new_grid[cell] = false;
			if ((cell_count == 2) || (cell_count == 3))
				continue;
			if (cell_count > 3)
				new_grid[cell] = false;
		}
		else if (cell_count == 3)
			new_grid[cell] = true;
	}
	for (i = 0; i < SIZE; i++)
	{
		grid[i] = new_grid[i];
	}
}

void draw_grid(bool grid[])
{
	int col, row, x, y, i;
	for (col = 0; col < COLUMNS; col++)
	{
		i = 0;
		for (row = col; row < SIZE; row += COLUMNS)
		{
			x = col;
			y = i;
			if (grid[row])
				dmd.writePixel( x, y, GRAPHICS_NORMAL, 1 );
			else
				dmd.writePixel( x, y, GRAPHICS_NORMAL, 0 );
			i++;
		}
	}
}

void drawText(String dispString) 
{
  dmd.clearScreen( true );
  dmd.selectFont( Arial_Black_16 );
  char newString[256];
  int sLength = dispString.length();
  dispString.toCharArray( newString, sLength+1 );
  dmd.drawMarquee(newString,sLength,( 32*DISPLAYS_ACROSS )-1 , 0 );
  long start=millis();
  long timer=start;
  long timer2=start;
  boolean ret=false;
  while(!ret){
    if ((digitalRead(randomised) == HIGH) || (digitalRead(pattern) == HIGH) || (digitalRead(factory) == HIGH))
        break;
    if ( ( timer+40 ) < millis() ) {
      ret=dmd.stepMarquee( -1 , 0 );
      timer=millis();
    }
  }
}

void ScanDMD()
{ 
  dmd.scanDisplayBySPI();
}

void setup()
{
   Timer1.initialize( 5000 );           
/*period in microseconds to call ScanDMD. Anything longer than 5000 (5ms) and you can see flicker.*/

   Timer1.attachInterrupt( ScanDMD );  
/*attach the Timer1 interrupt to ScanDMD which goes to dmd.scanDisplayBySPI()*/

   dmd.clearScreen( true );            
/* true is normal (all pixels off), false is negative (all pixels on) */
   pinMode(exit_button, INPUT);
   pinMode(randomised, INPUT);
   pinMode(pattern, INPUT);
   pinMode(factory, INPUT);
}

void loop()
{
    bool wrap;
    drawText("Please Select a Mode");  
    if ((digitalRead(randomised) == HIGH) || (digitalRead(pattern) == HIGH) || (digitalRead(factory) == HIGH))
    {
  	bool grid[SIZE];
	dmd.clearScreen( true );
	setup_grid(grid, &wrap);
	draw_grid(grid);
	do
	{
		update_grid(grid, wrap);
		draw_grid(grid);
                delay(100);
	} while ((digitalRead(exit_button) == LOW) && (digitalRead(randomised) == LOW) && (digitalRead(pattern) == LOW) && (digitalRead(factory) == LOW));
        dmd.clearScreen( true );
        drawText("Ending All Life...");
    }
}
