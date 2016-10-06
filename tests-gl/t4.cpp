glMatrixMode(GL_PROJECTION);
glLoadIdentity();
glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();

glColor3f(0, 0, 1);
glBegin(GL_QUADS);
glVertex2f(0.4, 0.2);
glVertex2f(2.0, 0.2);
glVertex2f(2.0, 0.8);
glVertex2f(0.4, 0.8);
glEnd();

glColor3f(1, 0, 0);
glBegin(GL_TRIANGLES);
glVertex3f(0.2, 0.2, -0.5);
glVertex3f(0.8, 0.5, 0.5);
glVertex3f(0.2, 0.8, -0.5);
glEnd();
