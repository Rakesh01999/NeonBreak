#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cstdio>  // Added this include for sprintf
#define _USE_MATH_DEFINES
#include <cmath>
#include <windows.h>
#include <GL/freeglut.h>
using namespace std;

// Game variables
float posXP = 0, posYP = 20, posZP = 0;
float posXB = 0, posYB = 0, posZB = 0;
float dx = 0, dy = 1.0;
bool incx = false, incy = true, ballRel = false, pause = false;

// Modern color palette
GLfloat modern_colors[][3] = {
    {0.94f, 0.35f, 0.39f},  // Modern Red
    {0.20f, 0.68f, 0.89f},  // Bright Blue
    {0.35f, 0.89f, 0.50f},  // Modern Green
    {0.68f, 0.51f, 0.89f},  // Purple
    {0.98f, 0.77f, 0.27f},  // Golden Yellow
    {0.27f, 0.94f, 0.89f},  // Cyan
    {0.95f, 0.95f, 0.95f},  // Clean White
    {1.00f, 0.49f, 0.31f}   // Orange
};

GLfloat paddle_sizes[] = {60, 100, 150};

const int COLUMNS = 10, ROWS = 8;
int paddle_size = 1, brick_color = 0, paddle_color = 2, ball_color = 4;
int xmax = 1366, ymax = 768, menux = 0, menuy, score = 0, scrolly = 0;
int bricks[COLUMNS][ROWS], state = 1, windows[4];

void draw_ball() {
    glColor3fv(modern_colors[ball_color]);
    glPushMatrix();
    glTranslatef(0, 0, 0);
    glutSolidSphere(8.0, 20, 16);
    glPopMatrix();
}

void draw_paddle() {
    glColor3fv(modern_colors[paddle_color]);
    float width = paddle_sizes[paddle_size];
    glBegin(GL_QUADS);
    glVertex3f(0, 0, 0);
    glVertex3f(width, 0, 0);
    glVertex3f(width, 15, 0);
    glVertex3f(0, 15, 0);
    glEnd();
    
    // Rounded left edge
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(7.5f, 7.5f, 0);
    for(int i = 90; i <= 270; i += 15)
        glVertex3f(7.5f + 7.5f * cos(i * M_PI / 180), 7.5f + 7.5f * sin(i * M_PI / 180), 0);
    glEnd();
    
    // Rounded right edge
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(width - 7.5f, 7.5f, 0);
    for(int i = 270; i <= 450; i += 15)
        glVertex3f(width - 7.5f + 7.5f * cos(i * M_PI / 180), 7.5f + 7.5f * sin(i * M_PI / 180), 0);
    glEnd();
}

void draw_brick(float w, float h) {
    glColor3fv(modern_colors[brick_color]);
    glBegin(GL_QUADS);
    glVertex3f(2, 2, 0);
    glVertex3f(w-2, 2, 0);
    glVertex3f(w-2, h-2, 0);
    glVertex3f(2, h-2, 0);
    glEnd();
    
    // Border
    glColor3f(modern_colors[brick_color][0] * 0.7f, 
              modern_colors[brick_color][1] * 0.7f, 
              modern_colors[brick_color][2] * 0.7f);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
    glVertex3f(0, 0, 0);
    glVertex3f(w, 0, 0);
    glVertex3f(w, h, 0);
    glVertex3f(0, h, 0);
    glEnd();
}

void draw_button(const char* text, float x, float y) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    
    // Button background
    glColor4f(0.15f, 0.15f, 0.35f, 0.9f);
    glBegin(GL_QUADS);
    glVertex3f(0, 0, 0);
    glVertex3f(300, 0, 0);
    glVertex3f(300, 60, 0);
    glVertex3f(0, 60, 0);
    glEnd();
    
    // Button border
    glColor3f(0.6f, 0.8f, 1.0f);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
    glVertex3f(0, 0, 0);
    glVertex3f(300, 0, 0);
    glVertex3f(300, 60, 0);
    glVertex3f(0, 60, 0);
    glEnd();
    
    glPopMatrix();
    
    // Button text
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x + 100, y + 35);
    for(int i = 0; text[i]; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
}

void draw_all_bricks() {
    float bw = (float)xmax / COLUMNS;
    float bh = (0.3f * ymax) / ROWS;
    
    for(int i = 0; i < COLUMNS; i++) {
        for(int j = 0; j < ROWS; j++) {
            if(bricks[i][j]) {
                glPushMatrix();
                glTranslatef(i * bw + bw * 0.1f, 0.6f * ymax + j * bh + bh * 0.1f, 0);
                draw_brick(bw * 0.8f, bh * 0.8f);
                glPopMatrix();
            }
        }
    }
}

void write_text(const char* text, float x, float y, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    for(int i = 0; text[i]; i++)
        glutBitmapCharacter(font, text[i]);
}

bool check_brick_collision() {
    float bw = (float)xmax / COLUMNS;
    float bh = (0.3f * ymax) / ROWS;
    bool all_destroyed = true;
    
    for(int i = 0; i < COLUMNS; i++) {
        for(int j = 0; j < ROWS; j++) {
            if(bricks[i][j]) {
                all_destroyed = false;
                float minx = i * bw + bw * 0.1f;
                float maxx = minx + bw * 0.8f;
                float miny = 0.6f * ymax + j * bh + bh * 0.1f;
                float maxy = miny + bh * 0.8f;
                
                if(posXB + 8 >= minx && posXB - 8 <= maxx && 
                   posYB + 8 >= miny && posYB - 8 <= maxy) {
                    bricks[i][j] = 0;
                    score += 100;
                    
                    // Simple bounce logic
                    if(posYB < miny || posYB > maxy) incy = !incy;
                    else incx = !incx;
                    return all_destroyed;
                }
            }
        }
    }
    return all_destroyed;
}

// Menu callback functions
void change_ball_color(int c) { ball_color = c - 1; }
void change_brick_color(int c) { brick_color = c - 1; }
void change_paddle_color(int c) { paddle_color = c - 1; }
void change_paddle_size(int s) { paddle_size = s - 1; }
void handle_menu(int) {}

void create_menus() {
    if(state == 2) {
        int ball_menu = glutCreateMenu(change_ball_color);
        int brick_menu = glutCreateMenu(change_brick_color);
        int paddle_menu = glutCreateMenu(change_paddle_color);
        int size_menu = glutCreateMenu(change_paddle_size);
        
        const char* colors[] = {"Red", "Blue", "Green", "Purple", "Yellow", "Cyan", "White", "Orange"};
        const char* sizes[] = {"Small", "Medium", "Large"};
        
        glutSetMenu(ball_menu);
        for(int i = 0; i < 8; i++) glutAddMenuEntry(colors[i], i + 1);
        
        glutSetMenu(brick_menu);
        for(int i = 0; i < 8; i++) glutAddMenuEntry(colors[i], i + 1);
        
        glutSetMenu(paddle_menu);
        for(int i = 0; i < 8; i++) glutAddMenuEntry(colors[i], i + 1);
        
        glutSetMenu(size_menu);
        for(int i = 0; i < 3; i++) glutAddMenuEntry(sizes[i], i + 1);
        
        glutCreateMenu(handle_menu);
        glutAddSubMenu("Ball Color", ball_menu);
        glutAddSubMenu("Brick Color", brick_menu);
        glutAddSubMenu("Paddle Color", paddle_menu);
        glutAddSubMenu("Paddle Size", size_menu);
        glutAttachMenu(GLUT_RIGHT_BUTTON);
    }
}

void display_menu() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Modern gradient background
    glBegin(GL_QUADS);
    glColor3f(0.05f, 0.05f, 0.15f);
    glVertex2f(0, 0);
    glVertex2f(xmax, 0);
    glColor3f(0.15f, 0.1f, 0.25f);
    glVertex2f(xmax, ymax);
    glVertex2f(0, ymax);
    glEnd();
    
    // Slower moving title - fixed speed
    glColor3f(0.4f, 0.8f, 1.0f);
    char title[] = "PIXELSHATTER";
    int title_x = 200 + (int)(200 * sin(menux * 0.01f)); // Smooth sine wave motion
    write_text(title, title_x, menuy, GLUT_BITMAP_TIMES_ROMAN_24);
    menux = (menux + 1) % 628; // Full sine wave cycle
    
    draw_button("NEW GAME", 0.35f * xmax, 0.45f * ymax);
    draw_button("CREDITS", 0.35f * xmax, 0.3f * ymax);
    
    // Instructions
    glColor3f(0.8f, 0.8f, 0.8f);
    write_text("Click buttons to navigate or use keyboard shortcuts:", xmax * 0.25f, ymax * 0.15f);
    write_text("N - New Game, C - Credits, Q - Quit", xmax * 0.3f, ymax * 0.1f);
    
    glutSwapBuffers();
}

void display_game() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Game background
    glBegin(GL_QUADS);
    glColor3f(0.02f, 0.02f, 0.08f);
    glVertex2f(0, 0);
    glVertex2f(xmax, 0);
    glColor3f(0.08f, 0.05f, 0.15f);
    glVertex2f(xmax, ymax);
    glVertex2f(0, ymax);
    glEnd();
    
    // Draw paddle
    glPushMatrix();
    glTranslatef(posXP, posYP, posZP);
    draw_paddle();
    glPopMatrix();
    
    // Draw ball
    glPushMatrix();
    if(ballRel) {
        glTranslatef(posXB, posYB, posZB);
    } else {
        glTranslatef(posXP + paddle_sizes[paddle_size]/2, posYP + 25, posZP);
    }
    draw_ball();
    glPopMatrix();
    
    draw_all_bricks();
    
    // UI elements
    glColor3f(1.0f, 1.0f, 1.0f);
    char score_str[30];
    sprintf(score_str, "Score: %d", score);
    write_text(score_str, xmax * 0.02f, ymax * 0.95f);
    
    if(pause) {
        glColor3f(1.0f, 1.0f, 0.0f);
        write_text("PAUSED - Press P to continue", xmax * 0.35f, ymax * 0.5f, GLUT_BITMAP_HELVETICA_18);
    } else {
        write_text("P-Pause | Q-Quit", xmax * 0.75f, ymax * 0.95f);
    }
    
    if(!ballRel) {
        glColor3f(0.8f, 0.8f, 0.8f);
        write_text("Click or press SPACE to launch ball", xmax * 0.35f, ymax * 0.05f);
    }
    
    glutSwapBuffers();
}

void display_score() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Score screen background
    glBegin(GL_QUADS);
    glColor3f(0.1f, 0.05f, 0.2f);
    glVertex2f(0, 0);
    glVertex2f(xmax, 0);
    glColor3f(0.2f, 0.1f, 0.3f);
    glVertex2f(xmax, ymax);
    glVertex2f(0, ymax);
    glEnd();
    
    glColor3f(1.0f, 1.0f, 1.0f);
    
    // Check win condition
    bool won = true;
    for(int i = 0; i < COLUMNS && won; i++)
        for(int j = 0; j < ROWS && won; j++)
            if(bricks[i][j]) won = false;
    
    if(won) {
        glColor3f(0.2f, 1.0f, 0.2f);
        write_text("CONGRATULATIONS! YOU WON!", xmax * 0.3f, ymax * 0.7f, GLUT_BITMAP_TIMES_ROMAN_24);
    } else {
        glColor3f(1.0f, 0.2f, 0.2f);
        write_text("GAME OVER - Ball missed!", xmax * 0.32f, ymax * 0.7f, GLUT_BITMAP_TIMES_ROMAN_24);
    }
    
    glColor3f(1.0f, 1.0f, 1.0f);
    char final_score[30];
    sprintf(final_score, "FINAL SCORE: %d", score);
    write_text(final_score, xmax * 0.4f, ymax * 0.5f, GLUT_BITMAP_HELVETICA_18);
    write_text("Press Enter to return to menu", xmax * 0.35f, ymax * 0.3f);
    
    glutSwapBuffers();
}

void display_credits() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Credits background
    glBegin(GL_QUADS);
    glColor3f(0.05f, 0.1f, 0.2f);
    glVertex2f(0, 0);
    glVertex2f(xmax, 0);
    glColor3f(0.1f, 0.2f, 0.3f);
    glVertex2f(xmax, ymax);
    glVertex2f(0, ymax);
    glEnd();
    
    glColor3f(0.8f, 0.9f, 1.0f);
    write_text("DEVELOPED BY", xmax * 0.42f, (ymax * 0.7f) + scrolly, GLUT_BITMAP_TIMES_ROMAN_24);
    write_text("Rakesh Biswas", xmax * 0.4f, (ymax * 0.6f) + scrolly, GLUT_BITMAP_HELVETICA_18);
    write_text("Nafis Ahamed", xmax * 0.4f, (ymax * 0.55f) + scrolly, GLUT_BITMAP_HELVETICA_18);
    write_text("Press any key to return to menu", xmax * 0.35f, (ymax * 0.3f) + scrolly);
    
    // Scroll effect
    scrolly++;
    if(scrolly > 100) scrolly = -100;
    
    glutSwapBuffers();
}

void update_game() {
    if(state == 2 && !pause && ballRel) {
        // Wall collisions
        if(posXB <= 8) { incx = true; posXB = 8; }
        if(posXB >= xmax - 8) { incx = false; posXB = xmax - 8; }
        if(posYB >= ymax - 8) { incy = false; posYB = ymax - 8; }
        
        // Paddle collision
        if(posYB <= posYP + 15 && posXB >= posXP - 8 && posXB <= posXP + paddle_sizes[paddle_size] + 8) {
            incy = true;
            posYB = posYP + 15;
            
            // Calculate bounce angle based on hit position
            float hit_pos = (posXB - (posXP + paddle_sizes[paddle_size]/2)) / (paddle_sizes[paddle_size]/2);
            dx = hit_pos * 0.6f;
            dy = sqrt(1.0f - dx * dx);
            incx = (hit_pos > 0);
        } else if(posYB < 0) {
            // Game over
            state = 3;
            ballRel = false;
            glutDisplayFunc(display_score);
            return;
        }
        
        // Brick collisions
        if(check_brick_collision()) {
            // All bricks destroyed - win condition
            state = 3;
            ballRel = false;
            glutDisplayFunc(display_score);
            return;
        }
        
        // Update ball position
        float speed = 3.0f;
        if(incx) posXB += speed * (dx == 0 ? 1 : abs(dx) + 0.5f);
        else posXB -= speed * (dx == 0 ? 1 : abs(dx) + 0.5f);
        
        if(incy) posYB += speed * dy;
        else posYB -= speed * dy;
    }
    
    glutPostRedisplay();
}

void handle_mouse_movement(int x, int y) {
    if(state == 2 && !pause) {
        float new_pos = x - paddle_sizes[paddle_size]/2;
        if(new_pos < 0) new_pos = 0;
        if(new_pos > xmax - paddle_sizes[paddle_size]) new_pos = xmax - paddle_sizes[paddle_size];
        posXP = new_pos;
        glutPostRedisplay();
    }
}

void handle_mouse_click(int button, int action, int x, int y) {
    if(button == GLUT_LEFT_BUTTON && action == GLUT_DOWN) {
        if(state == 1) {
            // Menu clicks
            if(x >= 0.35f * xmax && x <= 0.35f * xmax + 300) {
                if(ymax - y >= 0.45f * ymax && ymax - y <= 0.45f * ymax + 60) {
                    // New Game clicked
                    for(int i = 0; i < COLUMNS; i++)
                        for(int j = 0; j < ROWS; j++)
                            bricks[i][j] = (rand() % 4 != 0); // 75% chance of brick
                    
                    posXP = xmax/2 - 50; posYP = 20; 
                    posXB = posYB = 0; dx = 0; dy = 1.0; 
                    incx = false; incy = true; ballRel = false; 
                    pause = false; score = 0; state = 2;
                    
                    glutDisplayFunc(display_game);
                    glutIdleFunc(update_game);
                    glutPassiveMotionFunc(handle_mouse_movement);
                    create_menus();
                } else if(ymax - y >= 0.3f * ymax && ymax - y <= 0.3f * ymax + 60) {
                    // Credits clicked
                    state = 4;
                    scrolly = 0;
                    glutDisplayFunc(display_credits);
                    glutIdleFunc(update_game);
                }
            }
        } else if(state == 2 && !pause && !ballRel) {
            // Launch ball
            ballRel = true;
            posXB = posXP + paddle_sizes[paddle_size]/2;
            posYB = posYP + 25;
            dx = 0; dy = 1.0; incx = false; incy = true;
        }
    }
}

void handle_keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case '\r': case '\n':
            if(state == 3) {
                // Return to menu from score screen
                state = 1; score = 0; menux = 0;
                glutDisplayFunc(display_menu);
                glutIdleFunc(update_game);
                glutPassiveMotionFunc(nullptr);
            }
            break;
            
        case ' ':
            if(state == 2 && !pause && !ballRel) {
                // Launch ball with spacebar
                ballRel = true;
                posXB = posXP + paddle_sizes[paddle_size]/2;
                posYB = posYP + 25;
                dx = 0; dy = 1.0; incx = false; incy = true;
            }
            break;
            
        case 'p': case 'P':
            if(state == 2) pause = !pause;
            break;
            
        case 'n': case 'N':
            if(state == 1) {
                // Start new game with N key
                for(int i = 0; i < COLUMNS; i++)
                    for(int j = 0; j < ROWS; j++)
                        bricks[i][j] = (rand() % 4 != 0);
                
                posXP = xmax/2 - 50; posYP = 20; 
                posXB = posYB = 0; dx = 0; dy = 1.0; 
                incx = false; incy = true; ballRel = false; 
                pause = false; score = 0; state = 2;
                
                glutDisplayFunc(display_game);
                glutIdleFunc(update_game);
                glutPassiveMotionFunc(handle_mouse_movement);
                create_menus();
            }
            break;
            
        case 'c': case 'C':
            if(state == 1) {
                state = 4;
                scrolly = 0;
                glutDisplayFunc(display_credits);
                glutIdleFunc(update_game);
            }
            break;
            
        case 'q': case 'Q':
            exit(0);
            break;
            
        default:
            if(state == 4) {
                // Return to menu from credits
                state = 1;
                glutDisplayFunc(display_menu);
                glutIdleFunc(update_game);
            }
            break;
    }
}

void init_opengl() {
    glClearColor(0, 0, 0, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, xmax, 0, ymax, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}

int main(int argc, char** argv) {
    menuy = ymax * 0.75f;
    srand(time(nullptr));
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(xmax, ymax);
    glutCreateWindow("PixelShatter - Modern Arkanoid");
    glutFullScreen();
    
    init_opengl();
    
    glutDisplayFunc(display_menu);
    glutMouseFunc(handle_mouse_click);
    glutKeyboardFunc(handle_keyboard);
    glutIdleFunc(update_game);
    
    glutMainLoop();
    return 0;
}