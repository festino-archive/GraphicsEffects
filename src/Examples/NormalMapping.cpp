#include<GL/glew.h>
#include<GL/freeglut.h>
#include<glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include"FreeImage.h"

#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<chrono>
#include<numeric>

namespace ex5 {

	using namespace std;

#pragma pack(1)
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texcoords;
		glm::vec3 tangent;
		glm::vec3 binormal;
	};
#pragma pack()

	const int N1 = 40;
	const int N2 = 15;

	Vertex torus_vertices[(N1 + 1) * (N2 + 1)];
	GLuint torus_indices[N1 * N2 * 4];

	const double Pi = 3.14159265358979323846;

	void MakeTorus(double r1, double r2) {

		for (int i = 0; i <= N1; i++) {
			double phi = 2 * i * Pi / N1;
			for (int j = 0; j <= N2; j++) {
				double psi = 2 * j * Pi / N2;

				// Нормаль
				double nx = cos(phi) * cos(psi);
				double ny = sin(psi);
				double nz = sin(phi) * cos(psi);

				// Касательная
				double tx = -sin(phi);
				double ty = 0;
				double tz = cos(phi);

				torus_vertices[i * (N2 + 1) + j].position = glm::vec3(r1 * cos(phi) + r2 * nx, r2 * ny, r1 * sin(phi) + r2 * nz);
				torus_vertices[i * (N2 + 1) + j].texcoords = glm::vec2(i / (float)N1, j / (float)N2);
				torus_vertices[i * (N2 + 1) + j].normal = glm::vec3(nx, ny, nz);
				torus_vertices[i * (N2 + 1) + j].tangent = glm::vec3(tx, ty, tz);
				torus_vertices[i * (N2 + 1) + j].binormal = glm::vec3(ny * tz - nz * ty, tx * nz - nx * tz, nx * ty - ny * tx);
			}
		}

		int iidx = 0;
		for (int i = 0; i < N1; i++) {
			int i2 = i + 1;
			for (int j = 0; j < N2; j++) {
				int j2 = j + 1;
				torus_indices[iidx++] = i * (N2 + 1) + j;
				torus_indices[iidx++] = i * (N2 + 1) + j2;
				torus_indices[iidx++] = i2 * (N2 + 1) + j2;
				torus_indices[iidx++] = i2 * (N2 + 1) + j;
			}
		}
	}

	GLuint vertexBuffer;
	GLuint vertexArray;
	GLuint program;
	GLuint mvpLoc;
	GLuint mvLoc;
	GLuint nmLoc;

	float xAngle = 0;
	float yAngle = 0;

	bool init()
	{
		MakeTorus(0.7, 0.2);

		// Загрузка текстуры
		const char fname[] = "normalmap.bmp";
		FREE_IMAGE_FORMAT format = FreeImage_GetFileType(fname, 0);
		FIBITMAP* image = FreeImage_Load(format, fname);
		FIBITMAP* temp = image;
		image = FreeImage_ConvertTo32Bits(image);
		FreeImage_Unload(temp);

		int w = FreeImage_GetWidth(image);
		int h = FreeImage_GetHeight(image);

		GLubyte* bits = (GLubyte*)FreeImage_GetBits(image);

		//Now generate the OpenGL texture object 
		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid*)bits);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		FreeImage_Unload(image);

		std::ifstream vsh_file("NormalVertex.glsl");
		std::ifstream fsh_file("NormalFragment.glsl");
		string vsh_src = string(istreambuf_iterator<char>(vsh_file), istreambuf_iterator<char>());
		string fsh_src = string(istreambuf_iterator<char>(fsh_file), istreambuf_iterator<char>());

		// Create Shader And Program Objects
		program = glCreateProgram();
		GLenum vertex_shader = glCreateShader(GL_VERTEX_SHADER_ARB);
		GLenum fragment_shader = glCreateShader(GL_FRAGMENT_SHADER_ARB);

		// Load Shader Sources
		const char* src = vsh_src.c_str();
		glShaderSource(vertex_shader, 1, &src, NULL);
		src = fsh_src.c_str();
		glShaderSource(fragment_shader, 1, &src, NULL);

		char log[10000];
		int log_len;
		// Compile The Shaders
		glCompileShader(vertex_shader);
		glGetShaderInfoLog(vertex_shader, sizeof(log) / sizeof(log[0]) - 1, &log_len, log);
		log[log_len] = 0;
		cout << "Vertex shader compile result: " << log << endl;
		glCompileShader(fragment_shader);
		glGetShaderInfoLog(fragment_shader, sizeof(log) / sizeof(log[0]) - 1, &log_len, log);
		log[log_len] = 0;
		cout << "Fragment shader compile result: " << log << endl;

		// Attach The Shader Objects To The Program Object
		glAttachShader(program, vertex_shader);
		glAttachShader(program, fragment_shader);

		// Link The Program Object
		glLinkProgram(program);

		glGetProgramInfoLog(program, sizeof(log) / sizeof(log[0]) - 1, &log_len, log);
		log[log_len] = 0;
		cout << "Program link result: " << log << endl;

		mvpLoc = glGetUniformLocation(program, "mvp");
		mvLoc = glGetUniformLocation(program, "mv");
		nmLoc = glGetUniformLocation(program, "nm");

		glUseProgramObjectARB(program);

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(torus_vertices), torus_vertices, GL_STATIC_DRAW);

		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);
		int posLoc = glGetAttribLocation(program, "position");
		glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glEnableVertexAttribArray(posLoc);
		int normLoc = glGetAttribLocation(program, "normal");
		glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(normLoc);
		int tcLoc = glGetAttribLocation(program, "tex");
		glVertexAttribPointer(tcLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(tcLoc);
		int tanLoc = glGetAttribLocation(program, "tangent");
		glVertexAttribPointer(tanLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(8 * sizeof(GLfloat)));
		glEnableVertexAttribArray(tanLoc);
		int binormLoc = glGetAttribLocation(program, "binormal");
		glVertexAttribPointer(binormLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(11 * sizeof(GLfloat)));
		glEnableVertexAttribArray(binormLoc);

		GLuint texLoc = glGetUniformLocation(program, "normalmap");
		glUniform1ui(texLoc, textureID);

		glClearColor(0.0, 0.0, 0.0, 0.0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_DEPTH_TEST);
	}

	vector<float> times(10);
	chrono::steady_clock::time_point prevFrame;
	bool firstFrame;

	glm::mat4x4 proj;

	void reshape(int w, int h)
	{
		proj = glm::perspectiveFovRH(45.0f, float(w), float(h), 0.1f, 3.0f);
		glViewport(0, 0, w, h); // Область рисования -- все окно
		times.clear();
		firstFrame = true;
	}

	void idle(void) {
		if (firstFrame) {
			prevFrame = chrono::steady_clock::now();
			firstFrame = false;
		}
		else {
			chrono::steady_clock::time_point currFrame = chrono::steady_clock::now();
			double delta = double(chrono::duration_cast<std::chrono::microseconds>(currFrame - prevFrame).count());
			prevFrame = currFrame;
			if (times.size() >= 10)
				times.pop_back();
			times.push_back(delta);

			double avg = accumulate(times.begin(), times.end(), 0.0) / times.size();
			xAngle += 0.0000007 * avg;
			yAngle -= 0.0000003 * avg;

		}
	}

	void display(void)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4x4 mv =
			glm::translate(glm::vec3(0.0f, 0.0f, -1.5f)) *
			glm::rotate(xAngle, glm::vec3(1.0f, 0.0f, 0.0f)) *
			glm::rotate(yAngle, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4x4 mvp = proj * mv;
		glm::mat3x3 nm = glm::transpose(glm::inverse(glm::mat3x3(mv)));

		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);
		glUniformMatrix4fv(mvLoc, 1, GL_FALSE, &mv[0][0]);
		glUniformMatrix3fv(nmLoc, 1, GL_FALSE, &nm[0][0]);

		glDrawElements(GL_QUADS, sizeof(torus_indices) / sizeof(torus_indices[0]), GL_UNSIGNED_INT, torus_indices);
		glFinish();
		glutSwapBuffers();
		glutPostRedisplay();
	}

	int start_example(int argc, char** argv)
	{
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
		glutCreateWindow("Torus with normal mapping");

		glewInit();

		init();
		glutDisplayFunc(display);
		glutReshapeFunc(reshape);
		glutIdleFunc(idle);

		glutMainLoop();

		return 0;
	}

	//
	//bool init()
	//{
	//	glClearColor(0.0, 0.0, 0.0, 0.0);
	//	glEnable(GL_DEPTH_TEST);
	//
	//	// Инициализируем вершины тора
	//	const double R1 = 20;
	//	const double R2 = 5;
	//	for (int i = 0; i <= N1; i++)
	//		for (int j = 0; j <= N2; j++)
	//		{
	//			Vertex *v = vertices + i*(N2 + 1) + j;
	//			double phi = 2 * i*Pi / N1;
	//			double psi = 2 * j*Pi / N2;
	//			double nx = cos(phi)*cos(psi);
	//			double ny = sin(psi);
	//			double nz = sin(phi)*cos(psi);
	//			double tx = -sin(phi);
	//			double ty = 0;
	//			double tz = cos(phi);
	//
	//			v->tangent[0] = (float)tx;
	//			v->tangent[1] = (float)ty;
	//			v->tangent[2] = (float)tz;
	//			v->binormal[0] = (float)(ny*tz - nz*ty);
	//			v->binormal[1] = (float)(tx*nz - nx*tz);
	//			v->binormal[2] = (float)(nx*ty - ny*tx);
	//			v->normal[0] = (float)nx;
	//			v->normal[1] = (float)ny;
	//			v->normal[2] = (float)nz;
	//			v->pos[0] = (float)(R1*cos(phi) + R2*nx);
	//			v->pos[1] = (float)(R2*ny);
	//			v->pos[2] = (float)(R1*sin(phi) + R2*nz);
	//			v->pos[3] = 1.0f;
	//			v->uv[0] = i / (float)N1;
	//			v->uv[1] = j / (float)N2;
	//		}
	//
	//	// Инициализируем индексы тора
	//	int *p = indices;
	//	for (int i = 0; i<N1; i++)
	//		for (int j = 0; j<N2; j++)
	//		{
	//			*p++ = i * (N2 + 1) + j;
	//			*p++ = (i + 1) * (N2 + 1) + j;
	//			*p++ = (i + 1) * (N2 + 1) + j + 1;
	//			*p++ = i * (N2 + 1) + j + 1;
	//		}
	//
	//	if (!GLEW_ARB_shading_language_100)
	//	{
	//		cout << "No GLSL 1.00 support" << endl;
	//		return false;
	//	}
	//
	//	string vsh_src((istreambuf_iterator<char>(fstream("BumpVertex.glsl"))), istreambuf_iterator<char>());
	//	string fsh_src((istreambuf_iterator<char>(fstream("BumpFragment.glsl"))), istreambuf_iterator<char>());
	//
	//	// Create Shader And Program Objects
	//	GLenum program = glCreateProgramObjectARB();
	//	GLenum vertex_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	//	GLenum fragment_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	//
	//	// Load Shader Sources
	//	const char *src = vsh_src.c_str();
	//	glShaderSourceARB(vertex_shader, 1, &src, NULL);
	//	src = fsh_src.c_str();
	//	glShaderSourceARB(fragment_shader, 1, &src, NULL);
	//
	//	// Compile The Shaders
	//	glCompileShaderARB(vertex_shader);
	//	PrintInfoLog("compiling vertex shader", vertex_shader);
	//	glCompileShaderARB(fragment_shader);
	//	PrintInfoLog("compiling fragment shader", fragment_shader);
	//
	//	// Attach The Shader Objects To The Program Object
	//	glAttachObjectARB(program, vertex_shader);
	//	glAttachObjectARB(program, fragment_shader);
	//
	//	// Link The Program Object
	//	glLinkProgramARB(program);
	//
	//	// Use The Program Object Instead Of Fixed Function OpenGL
	//	glUseProgramObjectARB(program);
	//
	//	glUniform4f(glGetUniformLocation(program, "LightPosition"), 0.0f, 0.0f, 5.0f, 1.0f);
	//	glUniform4f(glGetUniformLocation(program, "MaterialSpecular"), 1.0f, 1.0f, 1.0f, 1.0f);
	//	glUniform4f(glGetUniformLocation(program, "MaterialAmbient"), 0.5f, 0.5f, 0.5f, 0.5f);
	//	glUniform4f(glGetUniformLocation(program, "MaterialDiffuse"), 0.5f, 0.5f, 0.5f, 0.5f);
	//	glUniform4f(glGetUniformLocation(program, "LightDiffuse"), 1.0f, 1.0f, 1.0f, 1.0f);
	//	glUniform4f(glGetUniformLocation(program, "LightAmbient"), 0.1f, 0.1f, 0.1f, 0.1f);
	//	glUniform1f(glGetUniformLocation(program, "Shininess"), 20.0f);
	//
	//	LoadTrueColorTexture("bumpmap.bmp", program, 0, "bumpMap");
	//
	//	// Привязываем атрибуты
	//	int loc;
	//	glVertexAttribPointer(loc = glGetAttribLocation(program, "pos"), 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), vertices);
	//	glEnableVertexAttribArray(loc);
	//	glVertexAttribPointer(loc = glGetAttribLocation(program, "normal"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)vertices + 4 * sizeof(float));
	//	glEnableVertexAttribArray(loc);
	//	glVertexAttribPointer(loc = glGetAttribLocation(program, "tangent"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)vertices + 7 * sizeof(float));
	//	glEnableVertexAttribArray(loc);
	//	glVertexAttribPointer(loc = glGetAttribLocation(program, "binormal"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)vertices + 10 * sizeof(float));
	//	glEnableVertexAttribArray(loc);
	//	glVertexAttribPointer(loc = glGetAttribLocation(program, "uv"), 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)vertices + 13 * sizeof(float));
	//	glEnableVertexAttribArray(loc);
	//
	//	return true;
	//}
	//

}