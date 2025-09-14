#include<stdlib.h>
#include<string.h>
#define _USE_MATH_DEFINES
#include<math.h>
#include<random>
#include<windows.h>
#include<GL/freeglut.h>
using namespace std;

//Declaring some of the constants
float posXP = 0, posYP = 10, posZP = 0;
float posXB = 0, posYB = 0, posZB = 0;
float dx = 0, dy = 1.0;

bool incx = false, incy = true;
bool ballRel = false;
bool pause = false;

//Color added in array
GLfloat paddle_color_array[][3] = { { 1,0,0 },{ 0,0,1 },{ 0,1,0 },{ 1,0,1 },{ 1,1,0 },{ 0,1,1 },{ 1,1,1 },{ 1,0.5,0 } };
GLfloat brick_color_array[][3] = { { 1,0,0 },{ 0,0,1 },{ 0,1,0 },{ 1,0,1 },{ 1,1,0 },{ 0,1,1 },{ 1,1,1 },{ 1,0.5,0 } };
GLfloat ball_color_array[][3] = { { 1,0,0 },{ 0,0,1 },{ 0,1,0 },{ 1,0,1 },{ 1,1,0 },{ 0,1,1 },{ 1,1,1 },{ 1,0.5,0 } };
GLfloat paddle_size[] = { 50, 100, 200 };

const int column = 10;
const int row = 10;
int padsize = 2;
int brick_color = 6;
int paddle_color = 3;
int ball_color = 4;
int xmax, ymax;
int menux, menuy, scrolly;
int bricks[column][row];
int presentstate = 1;
int score = 0;
int windows[4];

//Creating a Ball
void ball() {
	// glColor3fv(ball_color_array[ball_color]);
	glColor3f(1.0f, 0.7f, 0.3f); // Light orange
	glScalef(5, 5, 0);
	glutSolidSphere(2.0, 100, 10);
}

// Creating a Paddle
void paddle() {
    glColor3f(0.0f, 0.3f, 0.8f); // Deep blue
    glBegin(GL_POLYGON);
    glVertex3f(5.0, 0, 0);
    glVertex3f(paddle_size[padsize] - 10.0, 0, 0);
    for (float i = -90; i < 91; i += 0.01f)
        glVertex3f((paddle_size[padsize] - 10.0) + 10 * cosf(i * M_PI), 10 + 10 * sinf(i * M_PI), 0.0);
    glVertex3f(paddle_size[padsize] - 10.0, 20, 0);
    glVertex3f(10.0, 20, 0);
    for (float i = 90; i < 270; i += 0.01f)
        glVertex3f(10.0 + 10 * cosf(i * M_PI), 10 + 10 * sinf(i * M_PI), 0.0);
    glEnd();
}

//Creating a Brick
void block(float xv, float yv)
{
	glColor3fv(brick_color[brick_color_array]);
	glBegin(GL_POLYGON);
	glVertex3f(0, 0, 0);
	glVertex3f(xv, 0, 0);
	glVertex3f(xv, yv, 0);
	glVertex3f(0, yv, 0);
	glEnd();
}

void button() {
    glColor3f(0.25f, 0.22f, 0.11f); // Button fill color

    glBegin(GL_POLYGON);

    // Left rounded edge
    for (float i = M_PI / 2.0f; i <= (3 * M_PI) / 2.0f; i += 0.01f)
        glVertex3f(50.0f + 50.0f * cosf(i), 50.0f + 50.0f * sinf(i), 0.0f);

    // Right rounded edge
    for (float i = (3 * M_PI) / 2.0f; i <= (5 * M_PI) / 2.0f; i += 0.01f)
        glVertex3f(350.0f + 50.0f * cosf(i), 50.0f + 50.0f * sinf(i), 0.0f);

    glEnd();

    // Outline
    glLineWidth(5.0f);
    glColor3f(0.99f, 0.95f, 0.0f); // Outline color

    glBegin(GL_LINE_LOOP);

    // Left rounded edge
    for (float i = M_PI / 2.0f; i <= (3 * M_PI) / 2.0f; i += 0.01f)
        glVertex3f(50.0f + 50.0f * cosf(i), 50.0f + 50.0f * sinf(i), 0.0f);

    // Right rounded edge
    for (float i = (3 * M_PI) / 2.0f; i <= (5 * M_PI) / 2.0f; i += 0.01f)
        glVertex3f(350.0f + 50.0f * cosf(i), 50.0f + 50.0f * sinf(i), 0.0f);

    glEnd();
}

void brick()
{
	float xf = xmax / column;
	float yf = (0.50 * ymax) / row;
	for (int i = 0; i < column; i++) {
		for (int j = 0; j < row; j++) {
			if (bricks[i][j] == 1) {
				glPushMatrix();
				glLoadIdentity();
				glTranslated((i * xf) + (0.25 * xf), (0.40 * ymax) + (j * yf) + (0.25 * yf), 0);
				block(xf / 2, yf / 2);
				glPopMatrix();
			}
		}
	}
}

void writestring(void *font, char *s, float x, float y) {
	unsigned int i;
	glRasterPos2f(x, y);
	for (i = 0; i < strlen(s); i++)
		glutBitmapCharacter(font, s[i]);
}
void writestring(void *font, const char *s, float x, float y) {
	unsigned int i;
	glRasterPos2f(x, y);
	for (i = 0; i < strlen(s); i++)
		glutBitmapCharacter(font, s[i]);
}

void scoredisplay(); //To avoid error
void init();
void keyboardfunction(unsigned char key, int mousex, int mousey);

//Gameplay
void detectblock() { //To detect if ball hits the paddle
	float xf = xmax / column;
	float yf = (0.50 * ymax) / row;
	bool flag = false;
	for (int i = 0; i < column; i++) {
		for (int j = 0; j < row; j++) {
			if (bricks[i][j] == 1)
				flag = true;
		}
	}
	if (!flag) {
		presentstate = 3;
		ballRel = !ballRel;
		windows[2] = glutCreateWindow("Score");
		glutFullScreen();
		glutSetWindow(windows[2]);
		glutDestroyWindow(windows[1]);
		init();
		glutKeyboardFunc(keyboardfunction);
		glutDisplayFunc(scoredisplay);
		glutMainLoop();
	}
	for (int i = 0; i < column; i++) {
		for (int j = 0; j < row; j++) {
			float minx = (i * xf) + (0.25 * xf);
			float maxx = (i * xf) + (0.25 * xf) + (xf / 2);
			float miny = (0.40 * ymax) + (j * yf) + (0.25 * yf);
			float maxy = (0.40 * ymax) + (j * yf) + (0.25 * yf) + (yf / 2);
			if (bricks[i][j] == 1) {
				if ((posXB + 10.0) >= minx && (posXB - 10.0) <= maxx && (posYB + 10.0) >= miny && (posYB - 10.0) <= maxy) {
					bricks[i][j] = 0;
					score += 100;
					if (posYB < miny || posYB > maxy) {
						if (posYB < miny)
							incy = false;
						else if (posYB > maxy)
							incy = true;
						if (posXB <= ((maxx - minx) / 2)) {
							incx = false;
						}
						else {
							incx = true;
						}
					}
					else if (posXB < minx || posYB > maxx) {
						if (posXB < minx)
							incx = false;
						else if (posXB > maxx)
							incx = true;
						if ((posYB + 10.0) <= ((maxy - miny) / 2)) {
							incy = false;
						}
						else {
							incy = true;
						}
					}
				}
			}
		}
	}
}

//Functions for changing brick,ball and paddle color
void change_paddle_color(int action)
{
	paddle_color = action - 1;
}

void change_brick_color(int action)
{
	brick_color = action - 1;
}

void change_paddle_size(int action)
{
	padsize = action - 1;
}

void change_ball_color(int action)
{
	ball_color = action - 1;
}

void handle_menu(int action)
{
	// No action needed here for now
}

//Drop Down  Menu for Changing Color
void menus()
{
	if (presentstate == 2) {
		int submenu1 = glutCreateMenu(change_ball_color);
		glutAddMenuEntry("Red", 1);
		glutAddMenuEntry("Blue", 2);
		glutAddMenuEntry("Green", 3);
		glutAddMenuEntry("Purple", 4);
		glutAddMenuEntry("Yellow", 5);
		glutAddMenuEntry("Cyan", 6);
		glutAddMenuEntry("White", 7);
		glutAddMenuEntry("Orange", 8);
		int submenu2 = glutCreateMenu(change_brick_color);
		glutAddMenuEntry("Red", 1);
		glutAddMenuEntry("Blue", 2);
		glutAddMenuEntry("Green", 3);
		glutAddMenuEntry("Purple", 4);
		glutAddMenuEntry("Yellow", 5);
		glutAddMenuEntry("Cyan", 6);
		glutAddMenuEntry("White", 7);
		glutAddMenuEntry("Orange", 8);
		int submenu3 = glutCreateMenu(change_paddle_color);
		glutAddMenuEntry("Red", 1);
		glutAddMenuEntry("Blue", 2);
		glutAddMenuEntry("Green", 3);
		glutAddMenuEntry("Purple", 4);
		glutAddMenuEntry("Yellow", 5);
		glutAddMenuEntry("Cyan", 6);
		glutAddMenuEntry("White", 7);
		glutAddMenuEntry("Orange", 8);
		int submenu4 = glutCreateMenu(change_paddle_size);
		glutAddMenuEntry("Small", 1);
		glutAddMenuEntry("Medium", 2);
		glutAddMenuEntry("Large", 3);
		glutCreateMenu(handle_menu);
		glutAddSubMenu("Ball Color", submenu1);
		glutAddSubMenu("Bricks Color", submenu2);
		glutAddSubMenu("Paddle Color", submenu3);
		glutAddSubMenu("Paddle Size", submenu4);
		glutAttachMenu(GLUT_RIGHT_BUTTON);
	}
}

//Display the starting screen
// void menudisplay() {
//     glMatrixMode(GL_MODELVIEW);
//     glPushMatrix();
//     glLoadIdentity();

//     // Modern gradient background: teal → deep blue
//     glBegin(GL_POLYGON);
//     glColor3f(0.0f, 0.7f, 0.7f); // Top-left: teal
//     glVertex2f(-xmax, ymax);

//     glColor3f(0.0f, 0.4f, 0.6f); // Bottom-left: muted cyan
//     glVertex2f(-xmax, -ymax);

//     glColor3f(0.0f, 0.2f, 0.5f); // Bottom-right: deep blue
//     glVertex2f(xmax, -ymax);

//     glColor3f(0.0f, 0.5f, 0.8f); // Top-right: vibrant blue
//     glVertex2f(xmax, ymax);
//     glEnd();

//     glPopMatrix();
//     glPushMatrix();
//     glLoadIdentity();

//     // Title text color: bright yellow for contrast
//     glColor3f(0.99f, 0.95f, 0.0f);
//     char displaystring[10] = "NeonBreak";
//     if (menux < (xmax - 350))
//         writestring(GLUT_BITMAP_TIMES_ROMAN_24, displaystring, menux + 50.0, menuy);
//     else
//         writestring(GLUT_BITMAP_TIMES_ROMAN_24, displaystring, (xmax - 300) - (menux - (xmax - 350)), menuy);
//     menux = (menux + 1) % (2 * (xmax - 350));
//     glutPostRedisplay();
//     Sleep(5);

//     glPushMatrix();
//     glTranslatef(0.35 * xmax, 0.50 * ymax, 0.0);
//     button();
//     glPopMatrix();
//     strcpy_s(displaystring, "New Game");
//     writestring(GLUT_BITMAP_TIMES_ROMAN_24, displaystring, (0.35 * xmax) + 150.0, (0.50 * ymax) + 50.0);

//     glPushMatrix();
//     glTranslatef(0.35 * xmax, 0.25 * ymax, 0.0);
//     button();
//     glPopMatrix();
//     strcpy_s(displaystring, "Credits");
//     writestring(GLUT_BITMAP_TIMES_ROMAN_24, displaystring, (0.35 * xmax) + 175.0, (0.25 * ymax) + 50.0);

//     glFlush();
// }


// Display the starting screen
// Utility: measure string width in pixels for GLUT fonts
int getBitmapStringWidth(void* font, const char* str) {
    int width = 0;
    while (*str) {
        width += glutBitmapWidth(font, *str);
        str++;
    }
    return width;
}

// Display the starting screen
void menudisplay() {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    float centerX = 0.5f * xmax;
    float titleY  = 0.85f * ymax;

    // ---- Gradient Background ----
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.7f, 0.7f); glVertex2f(-xmax, ymax);
    glColor3f(0.0f, 0.4f, 0.6f); glVertex2f(-xmax, -ymax);
    glColor3f(0.0f, 0.2f, 0.5f); glVertex2f(xmax, -ymax);
    glColor3f(0.0f, 0.5f, 0.8f); glVertex2f(xmax, ymax);
    glEnd();

    glPopMatrix();
    glPushMatrix();
    glLoadIdentity();

    // ---- Title Box ----
    float boxTop    = titleY + 40.0f;
    float boxBottom = titleY - 40.0f;

    // Filled background
    glColor3f(0.1f, 0.05f, 0.2f);
    glBegin(GL_POLYGON);
    glVertex3f(-xmax, boxTop, 0.0f);
    glVertex3f(xmax,  boxTop, 0.0f);
    glVertex3f(xmax,  boxBottom, 0.0f);
    glVertex3f(-xmax, boxBottom, 0.0f);
    glEnd();

    // Border
    glLineWidth(4.0f);
    glColor3f(0.99f, 0.95f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(-xmax, boxTop, 0.0f);
    glVertex3f(xmax,  boxTop, 0.0f);
    glVertex3f(xmax,  boxBottom, 0.0f);
    glVertex3f(-xmax, boxBottom, 0.0f);
    glEnd();

    // ---- Title Text ----
    const char* title = "NeonBreak";
    int titleWidth = getBitmapStringWidth(GLUT_BITMAP_TIMES_ROMAN_24, title);
    glColor3f(0.99f, 0.95f, 0.0f);
    glRasterPos2f(centerX - titleWidth / 2.0f, titleY - 10.0f);
    for (const char* c = title; *c; c++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }

    // ---- Buttons ----
    glPushMatrix();
    glTranslatef(0.35f * xmax, 0.50f * ymax, 0.0f);
    button();
    glPopMatrix();
    writestring(GLUT_BITMAP_TIMES_ROMAN_24, "New Game",
                (0.35f * xmax) + 150.0f, (0.50f * ymax) + 50.0f);

    glPushMatrix();
    glTranslatef(0.35f * xmax, 0.25f * ymax, 0.0f);
    button();
    glPopMatrix();
    writestring(GLUT_BITMAP_TIMES_ROMAN_24, "Info",
                (0.35f * xmax) + 175.0f, (0.25f * ymax) + 50.0f);

    // ---- Game Controls / Shortcuts ----
    glColor3f(1.0f, 1.0f, 1.0f); // white text
    float controlY = 0.15f * ymax;

    writestring(GLUT_BITMAP_HELVETICA_18, "Controls:", 0.10f * xmax, controlY + 100.0f);
    writestring(GLUT_BITMAP_HELVETICA_18, "Mouse Move  - Move Paddle", 0.10f * xmax, controlY + 70.0f);
    writestring(GLUT_BITMAP_HELVETICA_18, "Left Mouse  - Launch Ball / Select Button", 0.10f * xmax, controlY + 40.0f);
    writestring(GLUT_BITMAP_HELVETICA_18, "P           - Pause / Resume Game", 0.10f * xmax, controlY + 10.0f);
    writestring(GLUT_BITMAP_HELVETICA_18, "Enter       - Continue from Score Screen", 0.10f * xmax, controlY - 20.0f);
    writestring(GLUT_BITMAP_HELVETICA_18, "Q           - Quit Game", 0.10f * xmax, controlY - 50.0f);

    glFlush();
}


//Display the game page
void gamedisplay() {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glBegin(GL_QUADS);
	glColor3f(0.202, 0.45, 0.802);
	glVertex2f(-xmax, ymax);
	glVertex2f(-xmax, -ymax);
	glColor3f(0.37, 0.650, 0.920);
	glVertex2f(xmax, -ymax);
	glVertex2f(xmax, ymax);
	glEnd();
	glPopMatrix();
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(posXP, posYP, posZP);
	paddle();
	glPopMatrix();
	glPushMatrix();
	if (ballRel){
		glTranslatef(posXB, posYB, posZB);
	}else{
		glTranslatef(posXP + (paddle_size[padsize] / 2), posYP + 30.0, posZP);
	}
	ball();
	glPopMatrix();
	brick();
	char scorestring[10] = "Score : ";
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, scorestring, xmax * 0.75, ymax * 0.95);
	_itoa_s(score, scorestring, 10);//To convert score to string
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, scorestring, xmax * 0.85, ymax * 0.95);
	if (pause) {
		writestring(GLUT_BITMAP_TIMES_ROMAN_24, " Press P to continue ... ", xmax * 0.25, ymax * 0.95);
		writestring(GLUT_BITMAP_TIMES_ROMAN_24, " GAME PAUSED ", xmax * 0.45, ymax * 0.15);
	}
	else
		writestring(GLUT_BITMAP_TIMES_ROMAN_24, " Press P to pause ... ", xmax * 0.25, ymax * 0.95);
	glutPostRedisplay();
	glFlush();
}
//Display the score page after playing
// Display the score page after playing
void scoredisplay() {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Enhanced green-blue gradient background
    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.8f, 0.3f); // Top-left: vibrant green
    glVertex2f(-xmax, ymax);
    glVertex2f(-xmax, -ymax);
    glColor3f(0.0f, 0.5f, 0.4f); // Bottom-right: teal-green
    glVertex2f(xmax, -ymax);
    glVertex2f(xmax, ymax);
    glEnd();

    glPopMatrix();
    glPushMatrix();
    glLoadIdentity();
    glPushMatrix();

    glColor3f(0.0f, 0.0f, 0.0f); // Text color: black
    writestring(GLUT_BITMAP_TIMES_ROMAN_24, "X--------------------------------XXXXXXXXXXXXXXX-------------------------------------X", 0.05 * xmax, 0.80 * ymax);

    bool flag = false;
    for (int i = 0; i < column; i++) {
        for (int j = 0; j < row; j++) {
            if (bricks[i][j] == 1)
                flag = true;
        }
    }

    if (flag)
        writestring(GLUT_BITMAP_TIMES_ROMAN_24, "The Ball missed  ... ", 0.40 * xmax, 0.75 * ymax);
    else
        writestring(GLUT_BITMAP_TIMES_ROMAN_24, "Congratulations! You have won. ", 0.35 * xmax, 0.75 * ymax);

    writestring(GLUT_BITMAP_TIMES_ROMAN_24, "X--------------------------------XXXXXXXXXXXXXXX-------------------------------------X", 0.05 * xmax, 0.70 * ymax);
    writestring(GLUT_BITMAP_TIMES_ROMAN_24, "YOU SCORE IS :", 0.35 * xmax, 0.50 * ymax);

    char scorestr[10];
    _itoa_s(score, scorestr, 10); // Convert score to string
    writestring(GLUT_BITMAP_TIMES_ROMAN_24, scorestr, 0.55 * xmax, 0.50 * ymax);
    writestring(GLUT_BITMAP_TIMES_ROMAN_24, " Press Enter to continue ... ", xmax * 0.35, ymax * 0.30);

    glPopMatrix();
    glFlush();
}

void infodisplay() {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Gradient background: top → bottom
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.2f, 0.6f); // Deep blue (top)
    glVertex2f(-xmax, ymax);
    glVertex2f(xmax, ymax);

    glColor3f(0.3f, 0.6f, 0.9f); // Light blue (bottom)
    glVertex2f(xmax, -ymax);
    glVertex2f(-xmax, -ymax);
    glEnd();

    glPopMatrix();
    glPushMatrix();
    glLoadIdentity();
    glPushMatrix();

	// Text color: bright yellow for contrast
	glColor3f(1.0f, 1.0f, 0.0f);

	// Centered horizontal position
	float centerX = 0.5f * xmax;

	writestring(GLUT_BITMAP_TIMES_ROMAN_24, "XX--------------------------------XXXXXXXXXXXXXX------------------------------------XX", centerX - 0.45f * xmax, (0.85f * ymax) + scrolly);
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, "Developed by", centerX - 0.08f * xmax, (0.80f * ymax) + scrolly);
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, "XX--------------------------------XXXXXXXXXXXXXX------------------------------------XX", centerX - 0.45f * xmax, (0.75f * ymax) + scrolly);
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, "		Rakesh Biswas    :200112", centerX - 0.15f * xmax, (0.70f * ymax) + scrolly);
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, "		Nafis Ahamed     :200129", centerX - 0.15f * xmax, (0.65f * ymax) + scrolly);

    glPopMatrix();
    glutPostRedisplay();
    Sleep(1);
    glFlush();
}

void idlefunction()
{
	if (presentstate == 2 && !pause) {
		if (ballRel)
		{
			if (posXB > 10.0 && posXB < (xmax - 10.0) && posYB >(posYP + 20.0) && posYB < (ymax - 10.0))
			{
				detectblock();
				if (incx)
					posXB += 10.0 * dx;
				else
					posXB -= 10.0 * dx;
				if (incy)
					posYB += 10.0 * dy;
				else
					posYB -= 10.0 * dy;
				Sleep(1);
			}
			if (posXB < 10) {
				incx = true;
				posXB += 10.0 * dx;
			}
			if (posXB >(xmax - 10.0)) {
				incx = false;
				posXB -= 10.0 * dx;
			}
			if (posYB > ((ymax * 0.95) - 10.0)) {
				incy = false;
				posYB -= 10.0 * dy;
			}
			if (posYB <= (posYP + 20.0)) {
				if ((posXB + 10.0) > posXP && (posXB - 10.0) < (posXP + paddle_size[padsize])) {
					incy = true;
					if (posXB == (posXP + (paddle_size[padsize] / 2))) {
						dx = 0;
					}
					else if (posXB < (posXP + (paddle_size[padsize] / 2))) {
						dx = (((posXP + (paddle_size[padsize] / 2)) - posXB) / (paddle_size[padsize] / 2)) + 0.25;
						incx = false;
						posXB -= 10.0 * dx;
					}
					else if (posXB >(posXP + (paddle_size[padsize] / 2))) {
						dx = ((posXB - (posXP + (paddle_size[padsize] / 2))) / (paddle_size[padsize] / 2)) + 0.25;
						incx = true;
						posXB += 10.0 * dx;
					}
					dy = abs(1 - dx);
					posYB += 10.0 * dy;
				}
				else {
					presentstate = 3;
					ballRel = !ballRel;
					windows[2] = glutCreateWindow("Score");
					glutFullScreen();
					glutSetWindow(windows[2]);
					glutDestroyWindow(windows[1]);
					init();
					glutKeyboardFunc(keyboardfunction);
					glutDisplayFunc(scoredisplay);
					glutMainLoop();
				}
			}
		}
		glutPostRedisplay();
	}
	if (presentstate == 4) {
		scrolly++;
		if (scrolly > (0.35 * ymax)) {
			presentstate = 1;
			glutSetWindow(windows[0]);
			glutDestroyWindow(windows[3]);
		}
	}
}

void passivefunction(int x, int y)
{
	if (presentstate == 2 && !pause) {
		if (x < (xmax - (paddle_size[padsize] / 2)) && x >(paddle_size[padsize] / 2))//Paddle Size and Curved Surface
			posXP = x - (paddle_size[padsize] / 2);
		else if (x >= (xmax - (paddle_size[padsize] / 2)))
			posXP = xmax - paddle_size[padsize];
		else
			posXP = 0.0;
		glutPostRedisplay();
	}
}
//To control the paddle using mouse
void mousefunction(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (presentstate == 1)
			if ((x > (0.35 * xmax) && x < ((0.35 * xmax) + 400.0)) && ((ymax - y) >(0.50 * ymax) && (ymax - y) < ((0.50 * ymax) + 100.0))) {
				for (int i = 0; i < column; i++)
					for (int j = 0; j < row; j++)
						bricks[i][j] = (rand() % 2);
				posXP = 0;
				posYP = 10;
				posZP = 0;
				posXB = 0;
				posYB = 0;
				posZB = 0;
				dx = 0;
				dy = 1.0;
				incx = false;
				incy = true;
				ballRel = false;
				score = 0;
				windows[1] = glutCreateWindow("Game");
				glutSetWindow(windows[1]);
				glutFullScreen();
				presentstate = 2;
				init();
				menus();
				glutDisplayFunc(gamedisplay);
				glutIdleFunc(idlefunction);
				glutPassiveMotionFunc(passivefunction);
				glutKeyboardFunc(keyboardfunction);
				glutMouseFunc(mousefunction);
				glutMainLoop();
			}
			else if ((x > (0.35 * xmax) && x < ((0.35 * xmax) + 400.0)) && ((ymax - y) >(0.25 * ymax) && (ymax - y) < ((0.25 * ymax) + 100.0))) {
				scrolly = -1 * (ymax * 0.85);
				windows[3] = glutCreateWindow("Info");
				glutSetWindow(windows[3]);
				glutFullScreen();
				presentstate = 4;
				init();
				glutIdleFunc(idlefunction);
				glutDisplayFunc(infodisplay);
				glutKeyboardFunc(keyboardfunction);
				glutMainLoop();
			}
			if (presentstate == 2 && !pause) {
				if (!ballRel)
				{
					ballRel = true;
					posXB = posXP + (paddle_size[padsize] / 2);
					posYB = posYP + 35.0;
					posZB = posZP;
				}
			}
	}
	
}

void init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glColor3f(1.0, 1.0, 1.0);
	glOrtho(0.0, xmax, 0.0, ymax, 1.0, -1.0);
}
//Keyboard function to pause or resume the game
void keyboardfunction(unsigned char key, int mousex, int mousey) {
	switch (key) {
	case '\r':if (presentstate == 3) {
		presentstate = 1;
		score = 0;
		glutDestroyWindow(windows[2]);
		menux = 0;
		menuy = 0.9*ymax;
	}
			  break;
	case 'p':
	case 'P':pause = !pause;
		break;
	case 'q':
	case 'Q':
		exit(0);
	default:;
	}
}
//Main function to call other function 
int main(int argc, char** argv)
{
	xmax = 1366;
	ymax = 768;  // Resolution
	menux = 0;
	menuy = 0.9*ymax;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(xmax, ymax);
	glutInitWindowPosition(0, 0);
	windows[1] = glutCreateWindow("Menu");
	glutFullScreen();
	init();
	glutMouseFunc(mousefunction);
	glutKeyboardFunc(keyboardfunction);
	glutDisplayFunc(menudisplay);
	glutMainLoop();
}
