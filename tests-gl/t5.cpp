static GLfloat trans[6][3] = {
    {0.0, 0.0, 1.0},
    {0.0, 0.0, -1.0},
    {1.0, 0.0, 0.0},
    {-1.0, 0.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, -1.0, 0.0}
};
static GLfloat rots[6][4] = {
    {0.0, 0.0, 1.0, 0.0},
    {180.0, 0.0, 1.0, 0.0},
    {90.0, 0.0, 1.0, 0.0},
    {-90.0, 0.0, 1.0, 0.0},
    {-90.0, 1.0, 0.0, 0.0},
    {90.0, 1.0, 0.0, 0.0}
};
static GLfloat cols[6][3] = {
    {1, 0, 0},
    {0, 1, 1},
    {0, 1, 0},
    {1, 0, 1},
    {0, 0, 1},
    {1, 1, 0}
};

glMatrixMode(GL_PROJECTION);
glLoadIdentity();
glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 100.0);
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();

glTranslatef(0.0, 0.0, -5.0);
glRotatef(-60, -1.0, 1.0, -1.0);

for (int i = 0; i < 6; ++i) {
    GLfloat* tran = trans[i];
    GLfloat* rot = rots[i];
    GLfloat* col = cols[i];

    glPushMatrix();

    glTranslatef(tran[0], tran[1], tran[2]);
    glRotatef(rot[0], rot[1], rot[2], rot[3]);

    glColor3f(col[0], col[1], col[2]);
    glBegin(GL_QUADS);
    glVertex2f(-1.0, -1.0);
    glVertex2f(1.0, -1.0);
    glVertex2f(1.0, 1.0);
    glVertex2f(-1.0, 1.0);
    glEnd();

    glPopMatrix();
}
