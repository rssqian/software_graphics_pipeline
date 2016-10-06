glMatrixMode(GL_PROJECTION);
glLoadIdentity();
glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 100.0);
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();

glColor3f(1, 1, 1);
glBegin(GL_QUADS);
glVertex3f(-1.0, -1.0, -5.0);
glVertex3f(1.0, -1.0, -2.0);
glVertex3f(1.0, 1.0, -2.0);
glVertex3f(-1.0, 1.0, -5.0);
glEnd();
