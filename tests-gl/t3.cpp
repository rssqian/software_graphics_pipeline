glMatrixMode(GL_PROJECTION);
glLoadIdentity();
glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();

glPushMatrix();

glTranslatef(0.25, 0.5, 0.0);
glRotatef(-45, 0.0, 1.0, 0.0);
glScalef(0.25, 0.25, 1.0);

glColor3f(0, 0, 1);
glBegin(GL_QUADS);
glVertex2f(-1.0, -1.0);
glVertex2f(1.0, -1.0);
glVertex2f(1.0, 1.0);
glVertex2f(-1.0, 1.0);
glEnd();

glPopMatrix();

glColor3f(1, 0, 0);
glBegin(GL_TRIANGLES);
glVertex3f(0.5, 0.25, 0.5);
glVertex3f(0.75, 0.25, -0.5);
glVertex3f(0.75, 0.75, -0.5);
glEnd();
