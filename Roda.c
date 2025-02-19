#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

#define RODA 1 // comentar
#define CABINE 2 // comentar
#define PI 3.14159265358979323846f

/* Configurações de iluminação */
    GLfloat ambiente_global[] = {0.5f, 0.5f, 0.5f, 1.0f};

    /* Luz direcional */
        GLfloat luz_dir_ambiente[] = {0.6f, 0.6f, 0.6f, 1.0f};
        GLfloat luz_dir_difusa[] = {0.8f, 0.8f, 0.8f, 1.0f};
        GLfloat luz_dir_especular[] = {1.0f, 1.0f, 1.0f, 1.0f};
        GLfloat posicao_luz[] = {0.0f, 1.0f, 1.0f, 0.0f};

    /* Luz pontual */
        GLfloat luz_pontual_ambiente[] = {0.5f, 0.5f, 0.5f, 1.0f};
        GLfloat luz_pontual_difusa[] = {0.5f, 0.5f, 0.5f, 1.0f};
        GLfloat luz_pontual_especular[] = {1.0f, 1.0f, 1.0f, 1.0f};
        GLfloat posicao_luz_pontual[] = {0.0f, -35.0f, 0.0f, 1.0f};
        

/* COnfiguração do material */
GLfloat material_especular[] = {0.6f, 0.6f, 0.6f, 1.0f};

/* Variáveis de controle */
GLfloat rotx = 0.0f; // comentar
GLfloat roty = 0.0f; // comentar
GLint num_cabines = 10; // numero de cabines inicialmente
GLfloat rot_roda = 0.0f; // comentar
GLfloat velocidade = 0.25f; // velocidade inicial da roda
bool animacao = false;
bool inverso = false;
bool luz_direcional = false;
bool luz_pontual = false;

/* Protótipos */
float para_radianos(float graus); // Transforma graus em radianos
void inicializar(void);
void desenhar_luz_pontual(float x, float y, float z); // Desenha um retângulo amarelo para indicar a posição da luz pontual
void atualizar_luzes(); // Mantêm o funcionamento das luzes na cena
void desenhar_cena(void);
void display(void);
void atualizar(void); // Aumenta/diminui velocidade da roda
void redimensionar(int largura, int altura);
void teclado(unsigned char tecla, int x, int y);
void teclado_especial(int tecla, int x, int y);

float para_radianos(float graus) {
    return (graus * PI) / 180.0f;
}

void inicializar(void) {
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambiente_global);

    glLightfv(GL_LIGHT0, GL_AMBIENT, luz_dir_ambiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luz_dir_difusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luz_dir_especular);
    glLightfv(GL_LIGHT0, GL_POSITION, posicao_luz);

    glLightfv(GL_LIGHT1, GL_AMBIENT, luz_pontual_ambiente);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, luz_pontual_difusa);
    glLightfv(GL_LIGHT1, GL_SPECULAR, luz_pontual_especular);
    glLightfv(GL_LIGHT1, GL_POSITION, posicao_luz_pontual);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_especular);
    glMateriali(GL_FRONT, GL_SHININESS, 30);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    /* Lista de exibição da RODA */
    glNewList(RODA, GL_COMPILE);
    glColor3f(0.3f, 0.3f, 0.3f);
    glutSolidTorus(2.0f, 30.0f, 50, 50);
    glEndList();

    /* Lista de exibição da CABINE com CUBO */
    glNewList(CABINE, GL_COMPILE);
    // Cilindro principal
    glColor3f(0.1f, 0.0f, 0.1f);
    GLUquadricObj *quadric = gluNewQuadric();
    gluCylinder(quadric, 1.0f, 1.0f, 8.0f, 20, 20);
    
    // Cubo conectado
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 8.0f);
    glColor3f(0.3f, 0.0f, 0.3f);
    glutSolidCube(7.0f);
    glPopMatrix();
    
    gluDeleteQuadric(quadric);
    glEndList();
}

void desenhar_luz_pontual(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z); 
    glColor3f(1.0f, 1.0f, 0.0f); 
    glutSolidCube(1.0f); 
    glPopMatrix();
}

void atualizar_luzes() {
    if (luz_direcional) glEnable(GL_LIGHT0);
    else glDisable(GL_LIGHT0);

    if (luz_pontual) glEnable(GL_LIGHT1);
    else glDisable(GL_LIGHT1);
}

void desenhar_cena(void) {
    /* Chão */
    glPushMatrix();
    glColor3f(0.3f, 0.3f, 0.3f);
    glTranslatef(0.0f, -40.0f, 0.0f);
    glScalef(50.0f, 2.0f, 50.0f);
    glutSolidCube(2.0f);
    glPopMatrix();

    /* Roda principal */
    glPushMatrix();
    glCallList(RODA);
    glPopMatrix();

    /* Cabines com cubos */
    glPushMatrix();
    glRotatef(rot_roda, 0.0f, 0.0f, 1.0f); // Rotaciona a roda inteira
    for (int i = 0; i < num_cabines; i++) {
        float angulo = (360.0f / num_cabines) * i; // Ângulo entre as cabines
        glPushMatrix();

        glRotatef(angulo, 0.0f, 0.0f, 1.0f);
        glTranslatef(28.0f, 0.0f, 0.0f);
        glRotatef(-rot_roda - angulo, 0.0f, 0.0f, 1.0f);

        glCallList(CABINE);

        glPopMatrix();

    }
    glPopMatrix();

    /* Pilares da roda */
    glPushMatrix();
    glColor3f(0.1f, 0.1f, 0.1f); 

        // Primeiro pilar
        glPushMatrix();
        glTranslatef(-20.0f, -22.0f, 0.0f); // Posição do primeiro pilar (esquerda)
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f); 
        GLUquadricObj *quadric = gluNewQuadric();
        gluCylinder(quadric, 2.0f, 2.0f, 20.0f, 20, 20); 
        gluDeleteQuadric(quadric);
        glPopMatrix();

        // Segundo pilar
        glPushMatrix();
        glTranslatef(20.0f, -22.0f, 0.0f); // Posição do segundo pilar (direita)
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f); 
        quadric = gluNewQuadric();
        gluCylinder(quadric, 2.0f, 2.0f, 20.0f, 20, 20); 
        gluDeleteQuadric(quadric);
        glPopMatrix();

    glPopMatrix();

    /* Base da roda */
    glPushMatrix();
    glColor3f(0.1f, 0.1f, 0.1f); 
    glTranslatef(0.0f, -38.0f, 0.0f);
    glScalef(25.0f, 2.0f, 10.0f);
    glutSolidCube(2.0f);
    glPopMatrix();

    /* Luz pontual abaixo da roda */
    glPushMatrix();
    glLightfv(GL_LIGHT1, GL_POSITION, posicao_luz_pontual); // Atualiza a posição da luz
    desenhar_luz_pontual(0.0f, -36.0f, 0.0f); // Desenha o indicador da luz
    glPopMatrix();
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.2f, 0.5f, 1.0f);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 0.0f, 100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    atualizar_luzes();
    
    glPushMatrix();
    glRotatef(rotx, 1.0f, 0.0f, 0.0f);
    glRotatef(roty, 0.0f, 1.0f, 0.0f);
    desenhar_cena();
    glPopMatrix();
    
    glutSwapBuffers();
}

void atualizar(void) {
    if (animacao) {
        if (inverso) {
            rot_roda -= velocidade;
        } else {
            rot_roda += velocidade;
        }
        glutPostRedisplay();
    }
}

void redimensionar(int largura, int altura) {
    glViewport(0, 0, largura, altura);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65.0, (double)largura/(double)altura, 1.0, 200.0);
}

void teclado(unsigned char tecla, int x, int y) {
    switch (tecla) {
        case '+': case '1': if(num_cabines < 15) num_cabines++; break;
        case '-': case '0': if(num_cabines > 10) num_cabines--; break;
        case 'x': case 'X': velocidade = fmaxf(0.1f, velocidade - 0.05f); break;
        case 'v': case 'V': velocidade = fminf(2.0f, velocidade + 0.05f); break;
        case 'a': case 'A': animacao = !animacao; break;
        case 'i': case 'I': inverso = !inverso; break;
        case 's': case 'S': luz_direcional = !luz_direcional; break;
        case 'l': case 'L': luz_pontual = !luz_pontual; break;
        case 27: exit(0);
    }
    glutPostRedisplay();
}

void teclado_especial(int tecla, int x, int y) {
    switch (tecla) {
        case GLUT_KEY_UP:    rotx += 2.0f; break; // Rotaciona para cima
        case GLUT_KEY_DOWN:  rotx -= 2.0f; break; // Rotaciona para baixo
        case GLUT_KEY_LEFT:  roty += 2.0f; break; // Rotaciona para a esquerda
        case GLUT_KEY_RIGHT: roty -= 2.0f; break; // Rotaciona para a direita
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    printf("\n****************************************************\n");
    printf(" CONTROLES:\n");
    printf(" Setas (cima e baixo) - Rotação vertical\n");
    printf(" Setas (direita e esquerda) - Rotação horizontal\n");
    printf(" A - Ligar/Desligar animação\n");
    printf(" I - Inverter direção\n");
    printf(" V/X - Aumentar/Diminuir velocidade\n");
    printf(" +/- - Alterar número de cabines\n");
    printf(" S - Ligar/Desligar luz direcional\n");
    printf(" L - Ligar/Desligar luz pontual\n");
    printf(" ESC - Sair\n");
    printf("****************************************************\n");

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Roda Gigante 3D com Cubos");
    
    inicializar();
    glutReshapeFunc(redimensionar);
    glutIdleFunc(atualizar);
    glutDisplayFunc(display);
    glutKeyboardFunc(teclado);
    glutSpecialFunc(teclado_especial);
    
    glutMainLoop();
    return 0;
}
