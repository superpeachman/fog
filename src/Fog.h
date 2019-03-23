//
//  Fog.hpp
//  50-ParticleFog
//
//  Created by superpeachman on 3/23/19.
//

#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "noiseBuffer.h"

class Fog {
public:
    Fog(ofCamera& _cam);
    ~Fog();
    void createNoise();
    void updateFogSetting();
    void update();
    void draw();
    void drawDebug();
    void beginDepthMap();
    void endDepthMap();
    void beginDepthShader();
    void endDepthShader();
    void beginFogShader();
    void endFogShader();
    void setNormals(ofMesh &mesh);
    void changeFogSetting(float &fogPosZ);
    void changeFogSetting2(float &fogTexS);

    ofCamera *cam;
    /*--------------------------------------------------------------
     
     
     depth
     
     
     --------------------------------------------------------------*/
    ofMesh meshFog;
    ofFbo depthMap;
    ofShader shaderDepth, shaderFog;
    int depthMapRes;
    
private:
    /*--------------------------------------------------------------
     
     
     ofGui
     
     
     --------------------------------------------------------------*/
    bool isAxisShown = false;
    bool isGuiShown = true;
    bool isFabricGuiShown = false;
    bool isLightGuiShown = true;
    
    ofxPanel gui;
//    ofxVec3Slider lightPosition;
    ofxIntSlider octave;
    ofxIntSlider offset;
    ofxFloatSlider persistence;
    ofxFloatSlider distLength;
    
    ofxFloatSlider fogHighlightX;
    ofxFloatSlider fogHighlightY;
    ofxFloatSlider fogPosZ;
    ofxFloatSlider fogTexS;
    ofxFloatSlider fogScale;
    
    /*--------------------------------------------------------------
     
     
     noise buffer
     
     
     --------------------------------------------------------------*/
    noiseBuffer noise;
    
    /*--------------------------------------------------------------
     
     
     Fog
     
     
     --------------------------------------------------------------*/
    bool fogChangeFlag = false;
    const int fogBufferCount = 14;
    
    float * offsetPositionX;
    float * offsetPositionZ;
    float * offsetPositionS;
    float * offsetTexCoordS;
    float * offsetTexCoordT;
    
};
