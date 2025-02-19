#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

#define RODA 1 // Lista de exibição para a roda
#define CABINE 2 // Lista de exibição para a cabine
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

/* Configuração do material */
GLfloat material_especular[] = {0.6f, 0.6f, 0.6f, 1.0f};

/* Variáveis de controle */
GLfloat rotx = 0.0f;        // Rotação em torno do eixo X da cena
GLfloat roty = 0.0f;        // Rotação em torno do eixo Y da cena
GLint num_cabines = 10;     // Número inicial de cabines
GLfloat rot_roda = 0.0f;    // Ângulo de rotação da roda
GLfloat velocidade = 0.25f; // Velocidade inicial da rotação
bool animacao = false;
bool inverso = false;
bool luz_direcional = false;
bool luz_pontual = false;

/**
 * @brief Converte ângulos de graus para radianos.
 *
 * @param graus Ângulo em graus.
 * @return float Ângulo em radianos.
 */
float para_radianos(float graus) {
    return (graus * PI) / 180.0f;
}

/**
 * @brief Inicializa as configurações de iluminação, materiais e objetos da cena.
 *
 * Esta função configura as luzes (direcional e pontual), os materiais e os parâmetros
 * de renderização do OpenGL. Além disso, cria as listas de exibição para a roda e para a cabine,
 * facilitando a renderização dos objetos repetitivos.
 */
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
    // Desenha o cilindro principal da cabine
    glColor3f(0.1f, 0.0f, 0.1f);
    GLUquadricObj *quadric = gluNewQuadric();
    gluCylinder(quadric, 1.0f, 1.0f, 8.0f, 20, 20);
    
    // Desenha o cubo acoplado à extremidade do cilindro
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 8.0f);
    glColor3f(0.3f, 0.0f, 0.3f);
    glutSolidCube(7.0f);
    glPopMatrix();
    
    gluDeleteQuadric(quadric);
    glEndList();
}

/**
 * @brief Desenha um indicador para a posição da luz pontual.
 *
 * Esta função desenha um pequeno cubo amarelo na posição especificada,
 * servindo como referência visual para a luz pontual.
 *
 * @param x Coordenada X.
 * @param y Coordenada Y.
 * @param z Coordenada Z.
 */
void desenhar_luz_pontual(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z); 
    glColor3f(1.0f, 1.0f, 0.0f); 
    glutSolidCube(1.0f); 
    glPopMatrix();
}

/**
 * @brief Atualiza o estado das luzes da cena.
 *
 * Habilita ou desabilita as luzes direcional e pontual conforme os
 * respectivos flags de controle.
 */
void atualizar_luzes() {
    if (luz_direcional) glEnable(GL_LIGHT0);
    else glDisable(GL_LIGHT0);

    if (luz_pontual) glEnable(GL_LIGHT1);
    else glDisable(GL_LIGHT1);
}

/**
 * @brief Renderiza toda a cena 3D.
 *
 * A função desenha os diversos elementos da cena:
 * - O chão
 * - A roda principal e suas cabines (rotacionadas de acordo com o ângulo atual)
 * - Pilares e base da roda
 * - Indicador da posição da luz pontual
 */
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
    // Rotaciona a roda inteira de acordo com o ângulo acumulado
    glRotatef(rot_roda, 0.0f, 0.0f, 1.0f);
    for (int i = 0; i < num_cabines; i++) {
        float angulo = (360.0f / num_cabines) * i; // Ângulo entre as cabines
        glPushMatrix();

        glRotatef(angulo, 0.0f, 0.0f, 1.0f);
        glTranslatef(28.0f, 0.0f, 0.0f);
        // Garante que a cabine fique na orientação correta
        glRotatef(-rot_roda - angulo, 0.0f, 0.0f, 1.0f);

        glCallList(CABINE);

        glPopMatrix();
    }
    glPopMatrix();

    /* Pilares da roda */
    glPushMatrix();
    glColor3f(0.1f, 0.1f, 0.1f); 

        // Primeiro pilar (lado esquerdo)
        glPushMatrix();
        glTranslatef(-20.0f, -22.0f, 0.0f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f); 
        GLUquadricObj *quadric = gluNewQuadric();
        gluCylinder(quadric, 2.0f, 2.0f, 20.0f, 20, 20); 
        gluDeleteQuadric(quadric);
        glPopMatrix();

        // Segundo pilar (lado direito)
        glPushMatrix();
        glTranslatef(20.0f, -22.0f, 0.0f);
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

/**
 * @brief Função de callback para renderização.
 *
 * Esta função é chamada sempre que a janela precisa ser redesenhada.
 * Ela limpa os buffers, configura a câmera e chama as funções que desenham a cena.
 */
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.2f, 0.5f, 1.0f);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // Define a posição da câmera
    gluLookAt(0.0f, 0.0f, 100.0f,  // Posição da câmera
              0.0f, 0.0f, 0.0f,    // Para onde a câmera aponta
              0.0f, 1.0f, 0.0f);   // Vetor "up"

    atualizar_luzes();
    
    glPushMatrix();
    glRotatef(rotx, 1.0f, 0.0f, 0.0f);
    glRotatef(roty, 0.0f, 1.0f, 0.0f);
    desenhar_cena();
    glPopMatrix();
    
    glutSwapBuffers();
}

/**
 * @brief Função de atualização da animação.
 *
 * Caso a animação esteja ativada, essa função incrementa ou decrementa
 * o ângulo de rotação da roda com base na velocidade e na direção definida.
 */
void atualizar(void) {
    if (animacao) {
        if (inverso) {
            rot_roda -= velocidade;
        } else {
            rot_roda += velocidade;
        }
        glutPostRedisplay(); // Solicita redesenho da cena
    }
}

/**
 * @brief Ajusta as dimensões da viewport e da projeção quando a janela é redimensionada.
 *
 * @param largura Nova largura da janela.
 * @param altura Nova altura da janela.
 */
void redimensionar(int largura, int altura) {
    glViewport(0, 0, largura, altura);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65.0, (double)largura/(double)altura, 1.0, 200.0);
}

/**
 * @brief Processa as entradas do teclado para comandos gerais.
 *
 * Permite o controle do número de cabines, velocidade da animação,
 * ativação da animação, inversão da direção e controle das luzes.
 *
 * @param tecla Código da tecla pressionada.
 * @param x Posição X do mouse (não utilizado).
 * @param y Posição Y do mouse (não utilizado).
 */
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

/**
 * @brief Processa as entradas das teclas especiais (setas).
 *
 * Permite a rotação da cena nos eixos X e Y, possibilitando a visualização
 * da roda de diferentes ângulos.
 *
 * @param tecla Código da tecla especial pressionada.
 * @param x Posição X do mouse (não utilizado).
 * @param y Posição Y do mouse (não utilizado).
 */
void teclado_especial(int tecla, int x, int y) {
    switch (tecla) {
        case GLUT_KEY_UP:    rotx += 2.0f; break;
        case GLUT_KEY_DOWN:  rotx -= 2.0f; break;
        case GLUT_KEY_LEFT:  roty += 2.0f; break;
        case GLUT_KEY_RIGHT: roty -= 2.0f; break;
    }
    glutPostRedisplay();
}

/**
 * @brief Função principal.
 *
 * Inicializa a biblioteca GLUT, configura os modos de exibição e registra
 * as funções de callback. Por fim, entra no loop principal de eventos.
 *
 * @param argc Número de argumentos passados na linha de comando.
 * @param argv Vetor com os argumentos da linha de comando.
 * @return int Código de saída do programa.
 */
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
