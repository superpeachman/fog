#pragma once

#define MODEL_NUM 7

#include "ofMain.h"
//#include "noiseBuffer.h"
#include "ofxGui.h"
#include "Fog.h"

class ofApp : public ofBaseApp{
    
public:
    void setup();
//    void createNoise();
    void update();
//    void updateFogSetting();
    void changeFogSetting(float &fogPosZ);
    void changeFogSetting2(float &fogTexS);
    void draw();
    void renderScene(int mode);
    void changeNearClip(float &camNear);
    void changeFarClip(float &camFar);
    bool flagCameraClipChanged = false;

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    ofEasyCam easyCam;
    ofShader shader;
    ofVboMesh vboMesh;
    ofSpherePrimitive sphere;
    ofBoxPrimitive box;

    ofVec3f pos;
    float time;
    
    /*--------------------------------------------------------------
     
     
     Fog
     
     
     --------------------------------------------------------------*/
    Fog *fog;
    
    /*--------------------------------------------------------------


     ofGui


     --------------------------------------------------------------*/
    ofxPanel gui;
    ofxToggle rotationFlag;
    ofxFloatSlider camNear;
    ofxFloatSlider camFar;
    ofxVec3Slider lightPosition;
    ofxFloatSlider boxScale;
    ofxFloatSlider boxAngle;
    
    bool debugFlag;
};
