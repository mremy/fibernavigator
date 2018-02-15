varying vec4 myColor;

void main() {

	myColor = abs(gl_Color);
	gl_Position = ftransform();
}
