// Future site for thesis software  
// 
//
//
//
// To Do:
// 1. Generate and view 3d honeycombs 
// 2. 
//
//
// Author: Nathan Rising
//

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>
#include <fstream> // file stream
#include <sstream>

// window
GLFWwindow* win;
// shader program
GLuint shaderProgram;
// vertex shader
const GLchar *v_shader = "\
#version 330 core\n\
layout (location = 0) in vec3 position;\
layout (location = 1) in vec3 offset;\
uniform mat4 MVP;\
out vec3 fragmentColor;\
void main(){\
    gl_Position = MVP * vec4(position + offset,1.0);\
	fragmentColor = vec3(cos(offset.x*0.2),cos(offset.y*0.2),cos(offset.z*0.2));\
}\
";
// fragment shader
const GLchar *f_shader = "\
#version 330 core\n\
in vec3 fragmentColor;\
void main(){\
    gl_FragColor = vec4(fragmentColor,1.0);\
}\
";

bool put(int x, int y, int z, int m, int gen){
	switch(gen){
	case 0:
		return (abs(x) <= m) & (abs(y) <= m) & (abs(z) <= m);
	default:
		return x*x+y*y+z*z <= m*m;
	}
}

// CREATE LATTICE for center points of each polyhedron.
// These points are offsets that will be applied to position the geometry in space
// The first generation type is a cubic boundary. Lattice points that extend beyound
// the boundary are discarded.
// Create_lattice([-1,1], [-1,1], [-1,1], {{1,0,0},{0,1,0},{0,0,1}}, 1)
// Create_lattice([-x, x], [-y, y], [-z, z], [basis], [lattice generation type], [max])
std::vector<GLfloat> lat;
void create_lattice(std::vector<GLfloat> &lat, int x[2], int y[2], int z[2], float bas[], int gen, int max){
    // Cubic boundary
    for(int i = max * x[0];i <= max * x[1];++i){
        for(int j = max * y[0];j <= max * y[1];++j){
            for(int k = max * z[0];k <= max * z[1];++k){
                GLfloat xx = i * bas[0] + j * bas[3] + k * bas[6];
                GLfloat yy = i * bas[1] + j * bas[4] + k * bas[7];
                GLfloat zz = i * bas[2] + j * bas[5] + k * bas[8];
                if(put(xx,yy,zz,max,gen)){
          	    	lat.push_back(xx);
                    lat.push_back(yy);
                    lat.push_back(zz);
                }
        	}
    	}
    }
}

// Print help message explaining the commands to use the program
void help_message(void){
    std::cout<<" This is a help message\n";
    std::cout<<"  -h [value]  Choose honeycomb (1-28) default = 1\n";
    std::cout<<"  -x [-n n]   x extents default = [-1 1]\n";
    std::cout<<"  -y [-n n]   y extents default = [-1 1]\n";
    std::cout<<"  -z [-n n]   z extents default = [-1 1]\n";
    std::cout<<"  -m [value]  Max extents symmetric override\n";
    
    std::cout<<"  -ap \n";
    std::cout<<"  -\n";
    
}

//const GLfloat cube[] = {-0.5,-0.5,-0.5, 0.5,-0.5,-0.5, 0.5,0.5,-0.5,-0.5,0.5,-0.5,
//                        -0.5,-0.5, 0.5, 0.5,-0.5, 0.5, 0.5,0.5, 0.5,-0.5,0.5, 0.5};
                          
//const GLfloat bas[3][3] = {{1.0,0.0,0.0},{0.0,1.0,0.0},{0.0,0.0,1.0}};                          
                        
//const GLint mesh[] = {0,1,2, 0,2,3, 0,1,5, 0,5,4,
//                     0,3,7, 0,7,4, 1,2,6, 1,6,5,
//           	      3,2,6, 3,6,7, 4,5,6, 4,6,7};
            	      
            	      
            	      
            	      
//const GLfloat cube[] = {0.5,-0.5,0.707106781186548, 1.5,-0.5,0.707106781186548, 1.5,0.5,0.707106781186548,
//			            0.5,0.5,0.707106781186548, 1.0,0.0,1.414213562373095, 1.0,0.0,0.0};

//GLfloat bas[] ={1.0, 1.0, 0.0, -1.0, 1.0, 0.0, 0.0, 0.0, 1.414213562373095};

//const GLint mesh[] = {0,1,4,0,1,5,0,3,4,0,3,5,
//		              1,2,4,1,2,5,2,3,4,2,3,5};            	      

const GLfloat cube[] = {-0.5,-0.5,0.707106781186548,   0.5,-0.5,0.707106781186548, -0.5,0.5,0.707106781186548,       // cuboctahedron 
			0.5,0.5,0.707106781186548,    0.0,-1.0,0.0,                0.0,1.0,0.0, 
			-1.0,0.0,0.0,                  1.0,0.0,0.0,                -0.5,-0.5,-0.707106781186548, 
			0.5,-0.5,-0.707106781186548, -0.5,0.5,-0.707106781186548,  0.5,0.5,-0.707106781186548,
			0.5,-0.5,0.707106781186548, 1.5,-0.5,0.707106781186548, 1.5,0.5,0.707106781186548,           // octahedron
			0.5,0.5,0.707106781186548, 1.0,0.0,1.414213562373095, 1.0,0.0,0.0};

GLfloat bas[] ={1.0, 1.0, 0.0,
	        -1.0, 1.0, 0.0,
	         0.0, 0.0, 1.414213562373095};


const GLint mesh[] = {0,1,4, 0,2,6, 1,3,7, 2,3,5,		// cuboctahedron
                        4,8,9, 5,10,11, 6,8,10, 7,9,11,
                        0,1,3, 0,3,2, 4,1,7, 4,7,9, 
                        5,3,7, 5,7,11, 6,0,4, 6,4,8, 
                        6,2,5, 6,5,10, 8,9,11, 8,11,10,
		        12,13,16,12,13,17,12,15,16,12,15,17,	// octahedron
		        13,14,18,13,14,17,14,15,16,14,15,17};            	                              
                        
                        
// initialize glfw and glew libraries and create window                        
void initialize_libraries(void){
    // initialize glfw
    glfwInit();
    //
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,1.0);
    glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);
    
    // Create window
    win = glfwCreateWindow(1300,600,"A Most Excellent Window",0,0);
    if(!win){glfwTerminate();std::cout<<"Could not create window\n";}
    glfwMakeContextCurrent(win); 

    // initialize GLEW
    glewExperimental = GL_TRUE;
    if(glewInit()!=GLEW_OK){std::cout<<"Could not initialize GLEW\n";}
    
    // define viewport dimensions
    glViewport(0,0,1300,600);

    // print some graphics information
    std::cout<<"Vendor: "<<glGetString(GL_VENDOR)<<'\n';
    std::cout<<"Renderer: "<<glGetString(GL_RENDERER)<<'\n';
    std::cout<<"Version: "<<glGetString(GL_VERSION)<<'\n';
    std::cout<<"Shading language: "<<glGetString(GL_SHADING_LANGUAGE_VERSION)<<'\n';
}

void compile_shader(void){
    // create and compile vertex shader
    GLuint vertexshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexshader,1,&v_shader,0);
    glCompileShader(vertexshader);
    GLint s_compile;
    GLchar infolog[300];
    glGetShaderiv(vertexshader,GL_COMPILE_STATUS,&s_compile);
    if(!s_compile)
    {
        glGetShaderInfoLog(vertexshader,300,0,infolog);
        std::cout<<"vertex shader compilation failed\n";
        std::cout<<infolog<<'\n';
    }

    // create and compile fragment shader
    GLuint fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentshader,1,&f_shader,0);
    glCompileShader(fragmentshader);
    glGetShaderiv(fragmentshader,GL_COMPILE_STATUS,&s_compile);
    if(!s_compile){
        glGetShaderInfoLog(fragmentshader,300,0,infolog);
        std::cout<<"fragment shader compilation failed\n";
        std::cout<<infolog<<"\n";
    }
    
    // Link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram,vertexshader);
    glAttachShader(shaderProgram,fragmentshader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram,GL_LINK_STATUS,&s_compile);
    if(!s_compile)
    {
        glGetProgramInfoLog(shaderProgram,300,0,infolog);
        std::cout<<"linking shaders failed\n";
        std::cout<<infolog<<'\n';
    }
    
    // Use shader program
    glUseProgram(shaderProgram);
    glDeleteShader(vertexshader);
    glDeleteShader(fragmentshader);
    
}

// vertex buffer, element array buffer, instance buffer, and vertex array object 
GLuint VBO, EBO, IBO, VAO;

// render
void render(void){
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElementsInstanced(GL_TRIANGLES, sizeof(mesh)/sizeof(mesh[0]), GL_UNSIGNED_INT,0,lat.size()/3);
    glBindVertexArray(0);        
}

// create buffer
void generate_buffer(void){
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);  // vertex buffer
    glGenBuffers(1, &EBO);  // element buffer
    glGenBuffers(1, &IBO);  // instance buffer
         
    glBindVertexArray(VAO); // bind VAO
}

// load buffer
void load_buffer(void){
    // vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);
    // element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // bind EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mesh), mesh, GL_STATIC_DRAW);
    
    // instance buffer
    glBindBuffer(GL_ARRAY_BUFFER, IBO); // bind IBO
    glBufferData(GL_ARRAY_BUFFER, lat.size()*sizeof(GLfloat), lat.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,3*sizeof(GLfloat),0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1,1);
   
    glBindVertexArray(0);  // unbind VAO
    glBindBuffer(GL_ARRAY_BUFFER,0);  // unbind VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0); // unbind EBO
    glBindBuffer(GL_ARRAY_BUFFER,0); // unbind IBO	
}

// delete buffer
void delete_buffer(void){
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &IBO);
}


// Camera stuff
glm::vec3 position = {10,10,10};
glm::vec3 direction = {-10,-10,-10};
glm::vec3 up = {0,1,0};
glm::vec3 right = {1,0,0};

GLfloat deltaTime;
GLfloat speed = 3.0f;
GLfloat nowTime;
GLfloat lastTime = 0.0;

glm::mat4 model_matrix = glm::mat4(1.0);

// Rotate model
// [X] rotate x [Y] rotate y [Z] rotate z
glm::mat4 rotate_model(){
    // X rotation
    if(glfwGetKey(win,GLFW_KEY_X) == GLFW_PRESS){
        glm::mat4 xmatrix = glm::mat4(1,0,0,0,
                         	0,glm::cos(deltaTime*speed),glm::sin(-deltaTime*speed),0,
                        	0,glm::sin(deltaTime*speed),glm::cos(deltaTime*speed),0,
                        	0,0,0,1);

		model_matrix = glm::transpose(xmatrix) * model_matrix;
	}
	// y rotation
	if(glfwGetKey(win,GLFW_KEY_Y) == GLFW_PRESS){
		glm::mat4 ymatrix = glm::mat4(cos(deltaTime*speed),0,sin(deltaTime*speed),0,
                  			0,1,0,0,
                  			sin(-deltaTime*speed),0,cos(deltaTime*speed),0,
                  			0,0,0,1);
		model_matrix = glm::transpose(ymatrix) * model_matrix;
	}
	// z rotation
	if(glfwGetKey(win,GLFW_KEY_Z) == GLFW_PRESS){
    	glm::mat4 zmatrix = glm::mat4(glm::cos(deltaTime*speed),glm::sin(-deltaTime*speed),0,0,
                      	    glm::sin(deltaTime*speed),glm::cos(deltaTime*speed),0,0,
                            0,0,1,0,
                            0,0,0,1);
		model_matrix = glm::transpose(zmatrix) * model_matrix;
	}
	return model_matrix;
}
         
int main(int argc, char* argv[]){

	initialize_libraries();   // initialize libraries and opengl context
    compile_shader();         // create and compile shaders
    generate_buffer();        // generate buffers

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

    glm::mat4 projection_matrix = glm::perspective(glm::radians(45.0f), 1300.0f/600.0f, 0.1f, 25.0f);
    glm::mat4 view_matrix = glm::lookAt(position,position + direction,up); 
   
    glm::mat4 MVP = projection_matrix * view_matrix * model_matrix;
    GLuint matrix_ID = glGetUniformLocation(shaderProgram,"MVP");
    glUniformMatrix4fv(matrix_ID,1,GL_FALSE,&MVP[0][0]);
   
//------- Read from file
	
	const std::string file_name = "data.txt";	
	std::fstream in_file;
	in_file.open(file_name,std::ios::in);
	if(in_file.is_open()){
		std::string in_str;
		std::string token;
		float in_num;
		int n_v, n_b, n_i;
		std::vector<GLfloat> vert;
		//GLfloat bas
		//GLint mesh
		getline(in_file,in_str);  // #0
	//	while(in_str.compare("#1")){
			std::cout<<in_str<<'\n';
			getline(in_file,in_str);  // #V
			getline(in_file,in_str);  // 24
			std::stringstream(in_str) >> n_v;

			
	
			//getline(in_file,in_str);			
	//	}

		
	}
	in_file.close();

//------ End read from file
	
    int a[2] = {-1,1};
    create_lattice(lat,a,a,a,bas,0,3);
    load_buffer();// load buffer data
   
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
          
    while((!glfwWindowShouldClose(win)) & (glfwGetKey(win,GLFW_KEY_SPACE) != GLFW_PRESS)){
		glfwPollEvents();
		
	 	nowTime = glfwGetTime();
		deltaTime = float(nowTime - lastTime);
		lastTime = nowTime;

		model_matrix = rotate_model();
		glm::mat4 MVP = projection_matrix * view_matrix * model_matrix;
		glUniformMatrix4fv(matrix_ID,1,GL_FALSE,&MVP[0][0]);
    
        glClearColor(0.0,0.0,0.1,1.0); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
        render(); // render data     
            
        glfwSwapBuffers(win);
    }    
    
    delete_buffer(); // delete buffers
    

	glfwTerminate();
	return 0;
}


 