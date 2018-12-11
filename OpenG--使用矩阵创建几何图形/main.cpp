//
//  main.m
//  OpenG--使用矩阵创建几何图形
//
//  Created by zhongding on 2018/12/10.
//

#include "GLTools.h"    // OpenGL toolkit
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLBatch.h"
#include "GLGeometryTransform.h"
#include "StopWatch.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

//视景体
GLFrustum viewFrustum;

//观察者
GLFrame cameraFrame;

//世界坐标系
GLFrame worldFrame;

//模型矩阵堆栈
GLMatrixStack modelMatri;
//投影矩阵堆栈
GLMatrixStack projectMatri;

//渲染管线
GLGeometryTransform transformLine;

//球
GLTriangleBatch     sphereBatch;
//环(甜甜圈)
GLTriangleBatch     torusBatch;
//圆柱
GLTriangleBatch     cylinderBatch;
//锥
GLTriangleBatch     coneBatch;
//磁盘
GLTriangleBatch     diskBatch;

//着色器管理器
GLShaderManager shaderManager;

int step = 0;

void specialKey(int key, int x, int y){
    
    if (key == GLUT_KEY_UP)
        worldFrame.RotateWorld(m3dDegToRad(-5.f), 1.0f, 0.f, 0.f);
    
    if (key == GLUT_KEY_DOWN)
        worldFrame.RotateWorld(m3dDegToRad(5.f), 1.0f, 0.f, 0.f);
    
    if (key == GLUT_KEY_LEFT)
        worldFrame.RotateWorld(m3dDegToRad(-5.f), 0.0f, 1.f, 0.f);
    
    if (key == GLUT_KEY_RIGHT)
        worldFrame.RotateWorld(m3dDegToRad(5.f), 0.0f, 1.f, 0.f);
    
    glutPostRedisplay();
}


void setup(){
    glClearColor(0.6f, 0.7f, 0.8f, 1.0f);
    shaderManager.InitializeStockShaders();
    
    cameraFrame.MoveForward(-15.f);
    
    
    /*球体
     gltMakeSphere(GLTriangleBatch& sphereBatch, GLfloat fRadius, GLint iSlices, GLint iStacks)
     sphereBatch：三角形批次类
     fRadius:球体半径
     iSlices：构成球体的三角形带
     iStacks：每个三角形带的三角形对
     */
    gltMakeSphere(sphereBatch, 3, 20, 40);
    
    /*甜甜圈
     gltMakeTorus(GLTriangleBatch& torusBatch, GLfloat majorRadius, GLfloat minorRadius, GLint numMajor, GLint numMinor)
     torusBatch:三角形批次类
     majorRadius：外圆半径
     minorRadius：内圆半径
     numMajor：三角形环的数量
     numMinor:每一个三角形环上的三角形对
     */
    gltMakeTorus(torusBatch, 3.0f, 0.75f, 50, 40);
    
    /*圆柱
     
     gltMakeCylinder(GLTriangleBatch& cylinderBatch, GLfloat baseRadius, GLfloat topRadius, GLfloat fLength, GLint numSlices, GLint numStacks)
     cylinderBatch:三角形批次类
     baseRadius：底面圆半径
     topRadius：顶部圆半径
     numSlices:构成圆柱侧面的三角形带个数
     numStacks：每条三角形带上的三角形对个数
     */
    gltMakeCylinder(cylinderBatch, 2.5f, 2.5f, 5.0f, 20, 20);
    
    /*圆锥*/
    gltMakeCylinder(coneBatch, 2.5f, 0.f, 3, 20, 20);
    
    /*
     磁盘
     gltMakeDisk(GLTriangleBatch& diskBatch, GLfloat innerRadius, GLfloat outerRadius, GLint nSlices, GLint nStacks)
     diskBatch:三角形批次类
     innerRadius:内圆半径
     outerRadius：外圆半径
     nSlices：磁盘面的三角形带个数
     nStacks：每个三角形带上的三角形对个数
     */
    gltMakeDisk(diskBatch, 2.0f, 4.5f, 20, 10);
}

void changeSize(int w, int h){
    
    glViewport(0, 0, w, h);
    
    /*设置投影矩阵*/
    viewFrustum.SetPerspective(35.f, (float)w/(float)h, 1, 500.f);
    
    //获取投影矩阵
    projectMatri.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    //加入一个单元矩阵
    modelMatri.LoadIdentity();
    
    /*把模型矩阵、投影矩阵加入渲染管道*/
    transformLine.SetMatrixStacks(modelMatri, projectMatri);
}

//颜色值，绿色、黑色
GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };

void DrawWireFramedBatch(GLTriangleBatch* pBatch){

    
    shaderManager.UseStockShader(GLT_SHADER_FLAT,transformLine.GetModelViewProjectionMatrix(),vGreen);
    
//    shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT,transformLine.GetModelViewMatrix(),transformLine.GetProjectionMatrix(),vGreen);
    
    pBatch->Draw();
    
    //3.开启线的抗锯齿处理
    glEnable(GL_LINE_SMOOTH);
    
     //开启颜色混合
    glEnable(GL_BLEND);
    
//    glEnable(GL_CULL_FACE);
    
    
   //指定混合因子
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //开线偏移（应对ZFighting问题）
    glEnable(GL_POLYGON_OFFSET_LINE);
    
    //glPolygonMode：填充模式
    //GL_FRONT:正面处理
    //GL_LINE：以线的方式填充
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    //线的宽度
    glLineWidth(2.5f);
   
    //画线
    shaderManager.UseStockShader(GLT_SHADER_FLAT,transformLine.GetModelViewProjectionMatrix(),vBlack);
    pBatch->Draw();
    
    //恢复设置
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glLineWidth(1.0f);
    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);
//    glDisable(GL_CULL_FACE);

}

void KeyPressFunc(unsigned char key, int x, int y){

    if (key == 32) {
        step++;
        
        if(step >=5) step = 0;
    }
    

    glutPostRedisplay();
}

void renderScene(void){
    
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    glEnable(GL_DEPTH_TEST);
    
    //压栈，把栈顶的矩阵复制一份
    modelMatri.PushMatrix();
    
    M3DMatrix44f mCamera;
    //获取观察者的坐标矩阵
    cameraFrame.GetCameraMatrix(mCamera);
    
    //把观察者的坐标矩阵与模型视图的栈顶矩阵叉乘，并把结果替换成栈顶矩阵
    modelMatri.MultMatrix(mCamera);
    
    M3DMatrix44f mWorld;
    //获取世界坐标系矩阵
    worldFrame.GetMatrix(mWorld);
    //把世界坐标系矩阵与投影矩阵堆栈的栈顶矩阵叉乘，并把结果替换成栈顶矩阵
    modelMatri.MultMatrix(mWorld);

    
    switch (step) {
        case 0:
            DrawWireFramedBatch(&sphereBatch);
            break;
        case 1:
            DrawWireFramedBatch(&torusBatch);
            break;
            
        case 2:
              DrawWireFramedBatch(&cylinderBatch);
            break;
        case 3:
            DrawWireFramedBatch(&coneBatch);
            break;
            
        case 4:
            DrawWireFramedBatch(&diskBatch);
            break;
        default:
            break;
    }
    
    //把模型视图矩阵的栈顶矩阵移除
    modelMatri.PopMatrix();
    
    glutSwapBuffers();
}


int main(int argc, char * argv[]) {
    
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Sphere");
    glutReshapeFunc(changeSize);
    glutKeyboardFunc(KeyPressFunc);
    glutSpecialFunc(specialKey);
    glutDisplayFunc(renderScene);
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    
    
    setup();
    glutMainLoop();
    return 0;
}
