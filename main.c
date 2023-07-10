#include <windows.h>
#include <gl/glut.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>



#define VK_W 0x57
#define VK_S 0x53

int width = 1000;
int height = 700;
int interval = 1000 / 144;

int score_left = 0;
int score_right = 0;

// rackets in general
int racket_width = 12;
int racket_height =100;
int racket_speed = 5;

// left racket position
float racket_left_x = 10.0f;
float racket_left_y = 340.0f;

// right racket position
float racket_right_x = 978.0f;
float racket_right_y = 340.0f;

// ball
float ball_pos_x = 350.0f;
float ball_pos_y = 250.0f;
float ball_dir_x = -1.0f;
float ball_dir_y = 0.0f;
int ball_size = 11;
int ball_speed = 8;

int wall_hits_left = 0-1;
int wall_hits_right = 0;

bool jogo_iniciado = false;

int time_left = 60; // tempo restante em segundos


void DesenhaCronometro(void) {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// desenha o tempo restante na posição (10, 10)
	char time_text[50];
	sprintf(time_text, "Tempo: %d ", time_left);
	glRasterPos2i(10, 650);
	
	int i;
	for (i = 0; time_text[i] != '\0'; i++) {
	    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, time_text[i]);
	}
}

void timer(int value) {
    // atualiza o tempo restante
    time_left--;

    // verifica se o tempo acabou
    if (time_left == 0) {
        jogo_iniciado = false;
    }

    // registra a próxima chamada da função timer
    glutTimerFunc(1000, timer, 0);

    // redesenha a tela
    glutPostRedisplay();
}

void vec2_norm(float* x, float* y) {
    // sets a vectors length to 1 (which means that x + y == 1)
    float length = sqrt((*x * *x) + (*y * *y));
    if (length != 0.0f) {
        length = 1.0f / length;
        *x *= length;
        *y *= length;
    }
}

void raquetes(float x, float y, float width, float height) {
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
    glEnd();

}
void DesenhaPlacar(void) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // desenha o placar na posição (10, 10)
    char score_text[50];
    sprintf(score_text, "%d    %d", wall_hits_left, wall_hits_right);
    glRasterPos2i(480, 650);

    int i;
    for (i = 0; score_text[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, score_text[i]);
    }
}
void DesenhaTelaInicio(void) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // desenha a mensagem de boas-vindas na posição central da tela
    char welcome_text[50];
    sprintf(welcome_text, "Bem-vindo ao Pong!");
    glRasterPos2i(width/2 - 90, height/2 + 120);

    int i;
    for (i = 0; welcome_text[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, welcome_text[i]);
    }

    // desenha o botão "iniciar" no centro da tela
    glColor3f(0.5f, 0.8f, 0.5f);
    glBegin(GL_QUADS);
        glVertex2f(width/2 - 50, height/2 - 20);
        glVertex2f(width/2 + 50, height/2 - 20);
        glVertex2f(width/2 + 50, height/2 + 20);
        glVertex2f(width/2 - 50, height/2 + 20);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    char start_text[50];
    sprintf(start_text, "Iniciar");
    glRasterPos2i(width/2 - 25, height/2 - 5);

    for (i = 0; start_text[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, start_text[i]);
    }
    
    char considera_text[50];
    sprintf(considera_text, "By: Vinicius Dias Vieira");
    glRasterPos2i(width/2 - 495, height/2 - 340);

    for (i = 0; considera_text[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, considera_text[i]);
    }
}
void DesenhaTelaFimDeJogo(void) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // desenha o texto informando o lado vencedor e o placar
    char winner_text[50];
    if (score_left > score_right) {
        sprintf(winner_text, "Lado Esquerdo Venceu! %d - %d", score_left, score_right);
    } else {
        sprintf(winner_text, "Lado Direito Venceu! %d - %d", score_right, score_left);
    }
    glRasterPos2i(width/2 - 170, height/2 + 50);

    int i;
    for (i = 0; winner_text[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, winner_text[i]);
    }

    // desenha o botão "voltar"
    glColor3f(0.5f, 0.5f, 0.5f);
    raquetes(width/2 - 50, height/2 - 100, 100, 50);
    glColor3f(0.0f, 0.0f, 0.0f);
    char back_text[] = "Voltar";
    glRasterPos2i(width/2 - 25, height/2 - 75);
    for (i = 0; back_text[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, back_text[i]);
    }
}


void bola() {
    // fly a bit
    ball_pos_x += ball_dir_x * ball_speed;
    ball_pos_y += ball_dir_y * ball_speed;
    
    if (jogo_iniciado) {
    // hit by left racket?
    if (ball_pos_x < racket_left_x + racket_width &&
        ball_pos_x > racket_left_x &&
        ball_pos_y < racket_left_y + racket_height &&
        ball_pos_y > racket_left_y) {
        // set fly direction depending on where it hit the racket
        // (t is 0.5 if hit at top, 0 at center, -0.5 at bottom)
        float t = ((ball_pos_y - racket_left_y) / racket_height) - 0.5f;
        ball_dir_x = fabs(ball_dir_x); // force it to be positive
        ball_dir_y = t;
    }
   
    // hit by right racket?
    if (ball_pos_x > racket_right_x &&
        ball_pos_x < racket_right_x + racket_width &&
        ball_pos_y < racket_right_y + racket_height &&
        ball_pos_y > racket_right_y) {
        // set fly direction depending on where it hit the racket
        // (t is 0.5 if hit at top, 0 at center, -0.5 at bottom)
        float t = ((ball_pos_y - racket_right_y) / racket_height) - 0.5f;
        ball_dir_x = -fabs(ball_dir_x); // force it to be negative
        ball_dir_y = t;
    }

    // hit left wall?
    if (ball_pos_x < 0) {
        
        ball_pos_x = width / 2;
        ball_pos_y = height / 2;
        ball_dir_x = fabs(ball_dir_x); // force it to be positive
        ball_dir_y = 0;
        wall_hits_left++;
    }

    // hit right wall?
    if (ball_pos_x > width) {
        
        ball_pos_x = width / 2;
        ball_pos_y = height / 2;
        ball_dir_x = -fabs(ball_dir_x); // force it to be negative
        ball_dir_y = 0;
        wall_hits_right++;
    }

    // hit top wall?
    if (ball_pos_y > height) {
        ball_dir_y = -fabs(ball_dir_y); // force it to be negative
    }

    // hit bottom wall?
    if (ball_pos_y < 0) {
        ball_dir_y = fabs(ball_dir_y); // force it to be positive
    }
    }

    // make sure that length of dir stays at 1
    vec2_norm(&ball_dir_x, &ball_dir_y);
}

void Desenha(void)
{
	 if (jogo_iniciado) {
	     glMatrixMode(GL_MODELVIEW);
	     glLoadIdentity();  
		                  
	     glClear(GL_COLOR_BUFFER_BIT);
	  	 glLineStipple(1, 0xAAAA);
    	glEnable(GL_LINE_STIPPLE);
    	glColor3f(1.0, 1.0, 1.0);
    	glBegin(GL_LINES);
    	glVertex2f(width/2, 0);
    	glVertex2f(width/2, height);
    	glEnd();
    	glDisable(GL_LINE_STIPPLE);
    	
	
	     raquetes(ball_pos_x - ball_size / 2, ball_pos_y - ball_size / 2, ball_size, ball_size);
	     raquetes(racket_left_x, racket_left_y, racket_width, racket_height);
	     raquetes(racket_right_x, racket_right_y, racket_width, racket_height);
	     
	     DesenhaPlacar();
		 DesenhaCronometro();
	
	     glutSwapBuffers();
     } else {
        // desenha a tela de início
        glClear(GL_COLOR_BUFFER_BIT);
        DesenhaTelaInicio();
        glutSwapBuffers();
    }
}
void IniciaJogo(void) {
    // inicia o jogo aqui
    jogo_iniciado = true;
}
void update(int value) {
   // input handling
   teclado();
   bola();
   // Call update() again in 'interval' milliseconds
   glutTimerFunc(interval, update, 0);

   // Redisplay frame
   glutPostRedisplay();
}
void mouse(int button, int state, int x, int y) {
    // verifica se o usuário clicou no botão "iniciar"
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (x > width/2 - 50 && x < width/2 + 50 && y > height/2 - 20 && y < height/2 + 20) {
            IniciaJogo();
        }
    }
    
}
void teclado(unsigned char key, int x, int y) {
    // left racket
    if (GetAsyncKeyState(VK_W)) racket_left_y += racket_speed;
    if (GetAsyncKeyState(VK_S)) racket_left_y -= racket_speed;
   
    // right racket
    if (GetAsyncKeyState(VK_UP)) racket_right_y += racket_speed;
    if (GetAsyncKeyState(VK_DOWN)) racket_right_y -= racket_speed;
    
}

void Inicializa (void)
{   
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}
 

void enable2D(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, width, 0.0f, height, 0.0f, 1.0f);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
}

int main(void)
{
	 glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
 	 glutInitWindowSize(width, height);
     glutCreateWindow("pong");
     glutDisplayFunc(Desenha);
     glutMouseFunc(mouse);
     glutTimerFunc(interval, update, 0);
     glutTimerFunc(1000, timer, 0);
     enable2D(width, height);
     glColor3f(1.0f, 1.0f, 1.0f);
     Inicializa();                                
 	 glutMainLoop();
 	 return 0;
}