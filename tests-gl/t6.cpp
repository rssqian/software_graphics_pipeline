glMatrixMode(GL_PROJECTION);
glLoadIdentity();
glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();

glTranslatef(0.1667, 0.5, 0.0);
glBegin(GL_TRIANGLES);
glColor3f(1, 0, 0);
for( int i = 0; i < 3; i++ )
{
  glVertex2f(0 + 0.33*i, 0.25);
  glVertex2f(-0.1667 + 0.33*i, -0.25);
  glVertex2f(0.1667 + 0.33*i, -0.25);
  if( i == 0 ) glColor3f(0, 1, 0);
  else if( i == 1) glColor3f(0, 0, 1);
}
glEnd();
