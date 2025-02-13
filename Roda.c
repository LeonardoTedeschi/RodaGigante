#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

#define RODA 1
#define CABINE 2
#define PI 3.14159265358979323846f

/* Configurações de iluminação */
GLfloat ambiente_global[] = {0.3f, 0.3f, 0.3f, 1.0f};
GLfloat luz_dir_ambiente[] = {0.6f, 0.6f, 0.6f, 1.0f};
GLfloat luz_dir_difusa[] = {0.8f, 0.8f, 0.8f, 1.0f};
GLfloat luz_dir_especular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat posicao_luz[] = {0.0f, 1.0f, 1.0f, 0.0f};
GLfloat material_especular[] = {0.6f, 0.6f, 0.6f, 1.0f};

/* Variáveis de controle */
GLfloat rotx = 0.0f;
GLfloat roty = 0.0f;
GLint num_cabines = 16;
GLfloat rot_roda = 0.0f;
GLfloat velocidade = 0.25f;
bool animacao = false;
bool inverso = false;

/* Protótipos */
float para_radianos(float graus);
void inicializar(void);
void desenhar_cena(void);
void display(void);
void atualizar(void);
void redimensionar(int largura, int altura);
void teclado(unsigned char tecla, int x, int y);

float para_radianos(float graus) {
    return (graus * PI) / 180.0f;
}

void inicializar(void) {
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambiente_global);
    glLightfv(GL_LIGHT0, GL_AMBIENT, luz_dir_ambiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luz_dir_difusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luz_dir_especular);
    glLightfv(GL_LIGHT0, GL_POSITION, posicao_luz);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_especular);
    glMateriali(GL_FRONT, GL_SHININESS, 30);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    /* Lista de exibição da RODA */
    glNewList(RODA, GL_COMPILE);
    glColor3f(0.8f, 0.0f, 0.0f);
    glutSolidTorus(2.0f, 30.0f, 50, 50);
    glEndList();

    /* Lista de exibição da CABINE com CUBO */
    glNewList(CABINE, GL_COMPILE);
    // Cilindro principal
    glColor3f(0.5f, 0.0f, 0.0f);
    GLUquadricObj *quadric = gluNewQuadric();
    gluCylinder(quadric, 1.0f, 1.0f, 8.0f, 20, 20);
    
    // Cubo conectado
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 8.0f);
    glColor3f(0.0f, 0.5f, 0.5f);
    glutSolidCube(7.0f);
    glPopMatrix();
    
    gluDeleteQuadric(quadric);
    glEndList();
}

void desenhar_cena(void) {
    /* Chão */
    glPushMatrix();
    glColor3f(0.1f, 0.3f, 0.1f);
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
    glRotatef(rot_roda, 0.0f, 0.0f, 1.0f);
    for (int i = 0; i < num_cabines; i++) {
        float angulo = (360.0f / num_cabines) * i;
        glPushMatrix();
        glRotatef(angulo, 0.0f, 0.0f, 1.0f);
        glTranslatef(28.0f, 0.0f, 0.0f);
        glRotatef(-rot_roda, 0.0f, 0.0f, 1.0f);
        glCallList(CABINE);
        glPopMatrix();
    }
    glPopMatrix();
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.6f, 0.8f, 1.0f, 1.0f);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 0.0f, 100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    
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
        case 'w': case 'W': rotx += 2.0f; break;
        case 's': case 'S': rotx -= 2.0f; break;
        case 'a': case 'A': roty += 2.0f; break;
        case 'd': case 'D': roty -= 2.0f; break;
        case '+': case '1': if(num_cabines < 24) num_cabines++; break;
        case '-': case '0': if(num_cabines > 8) num_cabines--; break;
        case 'x': case 'X': velocidade = fmaxf(0.1f, velocidade - 0.05f); break;
        case 'v': case 'V': velocidade = fminf(2.0f, velocidade + 0.05f); break;
        case 'c': case 'C': animacao = !animacao; break;
        case 'i': case 'I': inverso = !inverso; break;
        case 27: exit(0);
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    printf("\n****************************************************\n");
    printf(" CONTROLES:\n");
    printf(" W/S - Rotação vertical\n");
    printf(" A/D - Rotação horizontal\n");
    printf(" C - Ligar/Desligar animação\n");
    printf(" I - Inverter direção\n");
    printf(" V/X - Aumentar/Diminuir velocidade\n");
    printf(" +/- - Alterar número de cabines\n");
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
    
    glutMainLoop();
    return 0;
}