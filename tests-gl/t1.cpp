glMatrixMode(GL_PROJECTION);
glLoadIdentity();
glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();

glColor3f(1,1,1);
glBegin(GL_TRIANGLES);
glVertex2f(0.25, 0.25);
glVertex2f(0.75, 0.25);
glVertex2f(0.75, 0.75);
glEnd();

