#include <math.h>
#include <time.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>

struct spaceBody {
    int id;

    float m; // масса тела // - 24 нуля

    float x; // координаты тела
    float y;

    float vx; // проекции скорости тела
    float vy;
};


float W = 1200, H = 1200; // текущие размеры окна
double K = 2422; // переменная для вычисления гравитационных сил
int delay = 20, pause = 0; // delay - задержка между кадрами; pause (1 или 0) - находимся ли в состоянии остановки
int time0 = 0;
int addedComet = 0; // addedComet (1 или 0) - добавлена ли комета;
float x = 0, y = 0, z = 0; // переменные для передвижения камеры

float scale = 1.0f; // текущий масштаб
float pathX[3000], pathY[3000]; // путь кометы в точках
int pathIter = 0; // текущая точка в пути кометы
struct spaceBody sun, mercury, venus, earth, mars, jupiter, saturn, uranus, neptune, comet;


void initBodies(void); // задает начальные физические параметры всем телам
void reshapePlanet(struct spaceBody *, int, int); // изменение положения планет после изменения размера окна
void moveBody(struct spaceBody *); // расчёт сил, которые действуют на тело
void reshape(int, int); // изменение размеров окна


void drawCircle(float, float, float, int); // рисование круга
void drawPath(float *, float *, int); // рисование пути кометы
void drawBodies(void); // отрисовка небесных тел
void display(void); // функция отрисовки

void processSpecialKeys(int); // обработчик спец. клавиш
void processKeys(unsigned char); // обработчик обычных клавиш
void idle(void); // функция расчётов


int main(int argc, char *argv[]) {
    initBodies();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // включаем двойную буферизацию и четырехкомпонентный цвет

    glutInitWindowSize((int)W, (int)H);
    glutCreateWindow("Space");

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);

    glutIdleFunc(idle);
    glutSpecialFunc((void (*)(int, int, int)) processSpecialKeys);
    glutKeyboardFunc((void (*)(unsigned char, int, int)) processKeys);
    glutMainLoop();

    return 0;
}


void initBodies() {
    sun.id = 0;
    sun.m = 1980000; // масса, умноженная на 10^-24
    sun.x = W / 2.0f;
    sun.y = H / 2.0f;

    mercury.id = 1;
    mercury.m = 0.3285f;
    mercury.x = W / 2;
    mercury.y = H / 2 - 58;
    mercury.vx = 4.7f;
    mercury.vy = 0;

    venus.id = 2;
    venus.m = 4.867f;
    venus.x = W / 2;
    venus.y = H / 2 - 108;
    venus.vx = 3.5f;
    venus.vy = 0;

    earth.id = 3;
    earth.m = 5.9724f;
    earth.x = W / 2;
    earth.y = H / 2 - 150;
    earth.vx = 2.9f;
    earth.vy = 0;

    mars.id = 4;
    mars.m = 0.639f;
    mars.x = W / 2;
    mars.y = H / 2 - 230;
    mars.vx = 2.4f;
    mars.vy = 0;

    jupiter.id = 5;
    jupiter.m = 1898.7f;
    jupiter.x = W / 2;
    jupiter.y = H / 2 - 780;
    jupiter.vx = 1.3f;
    jupiter.vy = 0;

    saturn.id = 6;
    saturn.m = 568.51f;
    saturn.x = W / 2;
    saturn.y = H / 2 - 1400;
    saturn.vx = 0.9f;
    saturn.vy = 0;

    uranus.id = 7;
    uranus.m = 86.81f;
    uranus.x = W / 2;
    uranus.y = H / 2 - 2800;
    uranus.vx = 0.6f;
    uranus.vy = 0;

    neptune.id = 8;
    neptune.m = 102.4f;
    neptune.x = W / 2;
    neptune.y = H / 2 - 4500;
    neptune.vx = 0.5f;
    neptune.vy = 0;

    comet.id = 11;
    comet.m = 0.00000000022f;
    comet.x = W / 2 - 800;
    comet.y = H / 2 - 800;
    comet.vx = 0.5f; // 0.5
    comet.vy = 0.8f; // 0.8
    for (int i = 0; i < 3000; i++) {
        pathX[i] = pathY[i] = 0;
    }
}


void moveBody(struct spaceBody *p) {
    struct spaceBody arr[9] = {sun, mercury, venus, earth, mars, jupiter, saturn, uranus, neptune};
    for (int i = 0; i < 9; i++) {
        if (arr[i].id == p->id) continue; // не рассчитываем как планета притягивает сама себя
        double r = sqrt(pow(arr[i].x - p->x, 2) + pow(arr[i].y - p->y, 2)); // расстояние между центрами тел по Пифагору
        double aa = arr[i].m / K / pow(r, 2); // ускорение по формуле гравитационной силы

        // где находится тело относительно тела, которое прикладывает силу
        int sign_x = p->x < arr[i].x ? 1 : -1;
        int sign_y = p->y < arr[i].y ? 1 : -1;

        // угол между телами
        double fi = asin(fabsf(arr[i].y - p->y) / r);

        // изменение скорости с помощью проекций полученного ускорения
        p->vx += (float)(sign_x * aa * cos(fi));
        p->vy += (float)(sign_y * aa * sin(fi));
    }
}

void reshapePlanet(struct spaceBody *p, int w, int h) {
    p->x = p->x * (float) w / (float) W; // меняем обе координаты пропорционально
    p->y = p->y * (float) h / (float) H;
}

void reshape(int w, int h) {
    initBodies();
    // при изменении размеров окна, меняем планеты
    reshapePlanet(&mercury, w, h);
    reshapePlanet(&venus, w, h);
    reshapePlanet(&earth, w, h);
    reshapePlanet(&mars, w, h);
    reshapePlanet(&jupiter, w, h);
    reshapePlanet(&saturn, w, h);
    reshapePlanet(&uranus, w, h);
    reshapePlanet(&neptune, w, h);
    reshapePlanet(&sun, w, h);

    W = (float)w;
    H = (float)h;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, (int)W, (int)H);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glScalef(scale, scale, scale); // задаем текущий масштаб
    gluOrtho2D(0, W, 0, H); // рисуем всё на плоскости

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(x, y, z); // передвигаем камеру в нужное место

    drawBodies();

    glutSwapBuffers();
}

void processSpecialKeys(int key) {
    const float step = 100; // шаг движения камеры
    if (GLUT_KEY_LEFT == key) // стрелочка влево
        x += step;
    if (GLUT_KEY_RIGHT == key) // стрелочка вправо
        x -= step;
    if (GLUT_KEY_UP == key) // стрелочка вверх
        y -= step;
    if (GLUT_KEY_DOWN == key) // стрелочка вниз
        y += step;
    if (GLUT_KEY_F1 == key) // F1 - ускорение
        delay = 20 - delay;

    glutPostRedisplay();
}

void processKeys(unsigned char key) {
    if (key == 99 || key == 241) // c | с - увеличение масштаба
        scale += 0.25f;
    if (key == 118 || key == 236) // v | м - уменьшение масштаба
        scale -= 0.25f;
    if (key == 107 || key == 235)  // k | л - добавление или удаление кометы
        addedComet = 1 - addedComet;
    
    if (key == 27) // Esc - пауза
        pause = 1 - pause;

    if (scale < 0.05f) // не даем уменьшить слишком сильно и уйти в минус :)
        scale = 0.05f;

    glutPostRedisplay();
}

void idle() {
    if (clock() - time0 < delay || pause)
        return;
    time0 = clock();

    struct spaceBody *arr[9] = {&sun, &mercury, &venus, &earth, &mars, &jupiter, &saturn, &uranus, &neptune};

    // считаем новую скорость всех тел в моменте
    for (int i = 0; i < 9; i++)
        moveBody(arr[i]);

    if (addedComet == 1) { // считаем комету только если она существует
        moveBody(&comet);
        comet.x += comet.vx;
        comet.y += comet.vy;
        pathX[pathIter] = comet.x;
        pathY[pathIter] = comet.y;

        pathIter++;
        if (pathIter >= 3000) { // если превысили допустимое количество точек, стираем старые
            pathIter = 0;
        }
    }

    for (int i = 0; i < 9; i++) { // перемещаем тела одновременно
        arr[i]->x += arr[i]->vx;
        arr[i]->y += arr[i]->vy;
    }

    glutPostRedisplay(); // отправляем все рисовать заново
}

void drawBodies() {
    glColor3f(0.8f, 0.39f, 0.13f); // задаем нужный цвет
    drawCircle(sun.x, sun.y, 30, 100); // рисуем круг этого цвета

    glColor3f(0.69f, 0.69f, 0.7f);
    drawCircle(mercury.x, mercury.y, 5, 100);

    glColor3f(0.86f, 0.72f, 0.4f);
    drawCircle(venus.x, venus.y, 12, 100);

    glColor3f(0.39f, 0.78f, 0.85f);
    drawCircle(earth.x, earth.y, 13, 100);

    glColor3f(0.89f, 0.3f, 0.09f);
    drawCircle(mars.x, mars.y, 9, 100);

    glColor3f(0.89f, 0.56f, 0.43f);
    drawCircle(jupiter.x, jupiter.y, 25, 100);

    glColor3f(0.98f, 0.95f, 0.48f);
    drawCircle(saturn.x, saturn.y, 23, 100);

    glColor3f(0.07f, 0.83f, 0.58f);
    drawCircle(uranus.x, uranus.y, 16, 100);

    glColor3f(0.1f, 0.18f, 0.98f);
    drawCircle(neptune.x, neptune.y, 17, 100);

    // если комета добавлена на плоскость, рисуем и её
    if (addedComet == 1) {
        glColor3f(1.0f, 1.0f, 1.0f);
        drawCircle(comet.x, comet.y, 5, 100);
        drawPath(pathX, pathY, 3000);
    }
}

void drawCircle(float xCord, float yCord, float r, int amountSegments) {
    glBegin(GL_POLYGON);
    // рисуем многоугольник, откладывая точку на расстоянии r от центра, с частотой в amountSegments
    for (int i = 0; i < amountSegments; i++) {
        float angle = 2.0f * 3.1415926f * (float) i / (float) amountSegments;
        float dx = r * cosf(angle);
        float dy = r * sinf(angle);
        glVertex2f(xCord + dx, yCord + dy);
    }
    glEnd();
}

void drawPath(float *xArr, float *yArr, int n) {
    glBegin(GL_POINTS);
    // рисуем путь кометы точками
    for (int i = 0; i < n && xArr[i] != 0; i++) {
        glVertex2f(xArr[i], yArr[i]);
    }
    glEnd();
}