/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xgpio.h"
#include "xparameters.h"
#include "xil_printf.h"
#include <unistd.h>  // For usleep

// Define GPIO device IDs
#define GPIO_PADDLE_RIGHT_DEVICE_ID XPAR_GPIO_0_DEVICE_ID   // Right paddle position (input)
#define GPIO_PADDLE_LEFT_DEVICE_ID  XPAR_GPIO_1_DEVICE_ID   // Left paddle position (input)
#define GPIO_SCORE_DEVICE_ID        XPAR_GPIO_2_DEVICE_ID   // Scores (output: score1 in 1st 16 bits, score2 in last 16 bits)
#define GPIO_BALL_DEVICE_ID         XPAR_GPIO_3_DEVICE_ID   // Ball position (output: ball_x in 1st 16 bits, ball_y in last 16 bits)
#define GPIO_OUTPUT_PADDLE_DEVICE_ID XPAR_GPIO_4_DEVICE_ID  // Paddle positions (output: left paddle in 1st 16 bits, right paddle in last 16 bits)

// Declare GPIO instances
XGpio Gpio_Paddle_Right, Gpio_Paddle_Left;
XGpio Gpio_Score, Gpio_Ball;
XGpio Gpio_Output_Paddle;

// Game constants
#define PADDLE_HEIGHT 60
#define PADDLE_WIDTH  10
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define BALL_SIZE     10
#define BALL_SPEED    2

// Ball state
int ball_x = SCREEN_WIDTH / 2;
int ball_y = SCREEN_HEIGHT / 2;
int ball_dx = BALL_SPEED;
int ball_dy = BALL_SPEED;

// Score state
int score_left = 0;
int score_right = 0;

// Update ball position
void update_ball_position() {
    // Move the ball
    ball_x += ball_dx;
    ball_y += ball_dy;

    // Ball collision with top and bottom walls
    if (ball_y <= 0 || ball_y >= (SCREEN_HEIGHT - BALL_SIZE)) {
        ball_dy = -ball_dy;  // Invert Y direction
    }

    // Read paddle positions
    int left_paddle_pos = XGpio_DiscreteRead(&Gpio_Paddle_Left, 1);
    int right_paddle_pos = XGpio_DiscreteRead(&Gpio_Paddle_Right, 1);

    // Ball collision with left paddle
    if (ball_x <= (PADDLE_WIDTH + 20) && ball_y >= left_paddle_pos && ball_y <= (left_paddle_pos + PADDLE_HEIGHT)) {
        ball_dx = BALL_SPEED;  // Bounce back to the right
    }
    // Ball collision with right paddle
    else if (ball_x >= (SCREEN_WIDTH - PADDLE_WIDTH - BALL_SIZE - 20) &&
             ball_y >= right_paddle_pos && ball_y <= (right_paddle_pos + PADDLE_HEIGHT)) {
        ball_dx = -BALL_SPEED;  // Bounce back to the left
    }
    // Score for player 2 (right side)
    else if (ball_x <= 0) {
        score_right++;
        ball_x = SCREEN_WIDTH / 2;
        ball_y = SCREEN_HEIGHT / 2;
    }
    // Score for player 1 (left side)
    else if (ball_x >= SCREEN_WIDTH) {
        score_left++;
        ball_x = SCREEN_WIDTH / 2;
        ball_y = SCREEN_HEIGHT / 2;
    }
}

// Update paddle positions to GPIO
void update_paddle_positions_to_gpio() {
    int left_paddle_pos = XGpio_DiscreteRead(&Gpio_Paddle_Left, 1);
    int right_paddle_pos = XGpio_DiscreteRead(&Gpio_Paddle_Right, 1);

    // Combine paddle positions into a single 32-bit value (left paddle in 1st 16 bits, right paddle in last 16 bits)
    uint32_t paddle_value = ((left_paddle_pos & 0xFFFF) << 16) | (right_paddle_pos & 0xFFFF);

    // Write the combined paddle positions to GPIO
    XGpio_DiscreteWrite(&Gpio_Output_Paddle, 1, paddle_value);
}

// Update ball position to GPIO
void update_ball_to_gpio() {
    // Combine ball_x and ball_y into a single 32-bit value (ball_x in 1st 16 bits, ball_y in last 16 bits)
    uint32_t ball_value = ((ball_x & 0xFFFF) << 16) | (ball_y & 0xFFFF);

    // Write the combined ball position to GPIO
    XGpio_DiscreteWrite(&Gpio_Ball, 1, ball_value);
}

// Update scores to GPIO
void update_scores_to_gpio() {
    // Combine scores into a single 32-bit value (score_left in 1st 16 bits, score_right in last 16 bits)
    uint32_t score_value = ((score_left & 0xFFFF) << 16) | (score_right & 0xFFFF);

    // Write the combined scores to GPIO
    XGpio_DiscreteWrite(&Gpio_Score, 1, score_value);
}

int main() {
    init_platform();

    // Initialize all GPIO devices
    XGpio_Initialize(&Gpio_Paddle_Right, GPIO_PADDLE_RIGHT_DEVICE_ID);
    XGpio_Initialize(&Gpio_Paddle_Left, GPIO_PADDLE_LEFT_DEVICE_ID);
    XGpio_Initialize(&Gpio_Score, GPIO_SCORE_DEVICE_ID);
    XGpio_Initialize(&Gpio_Ball, GPIO_BALL_DEVICE_ID);
    XGpio_Initialize(&Gpio_Output_Paddle, GPIO_OUTPUT_PADDLE_DEVICE_ID);

    // Set ball, paddle output, and score GPIO directions (output)
    XGpio_SetDataDirection(&Gpio_Output_Paddle, 1, 0x00);  // Output for paddles
    XGpio_SetDataDirection(&Gpio_Ball, 1, 0x00);           // Output for ball
    XGpio_SetDataDirection(&Gpio_Score, 1, 0x00);          // Output for scores

    // Set paddles GPIO direction (input)
    XGpio_SetDataDirection(&Gpio_Paddle_Left, 1, 0xFF);    // Input for left paddle
    XGpio_SetDataDirection(&Gpio_Paddle_Right, 1, 0xFF);   // Input for right paddle

    while (1) {
        update_paddle_positions_to_gpio();   // Read and update paddle positions
        update_ball_position();              // Update ball logic (movement, collisions, scores)
        update_ball_to_gpio();               // Write ball position to GPIO
        update_scores_to_gpio();             // Write scores to GPIO

        usleep(10000);  // Delay for ball speed (~10ms)
    }

    cleanup_platform();
    return 0;
}
