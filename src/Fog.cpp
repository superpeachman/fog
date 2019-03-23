//
//  Fog.cpp
//  50-ParticleFog
//
//  Created by superpeachman on 3/23/19.
//

#include "Fog.h"
//Fog::Fog(){
Fog::Fog(ofCamera& _cam){
    cam = &_cam;
    
    // shader setings
    shaderDepth.load("shaders/depth");
    shaderFog.load("shaders/fog");
    
    /*--------------------------------------------------------------
     
     
     ofGui
     
     
     --------------------------------------------------------------*/
    float guiWidth = int(ofGetWidth() / 8);
    gui.setDefaultWidth(guiWidth);
    gui.setup("Fog");
    gui.setPosition(guiWidth, 0);
//    gui.add(lightPosition.setup("Light Pos",
//                                ofVec3f(-100.0, 50.0, 50.0),
//                                ofVec3f(-500, -500, -500),
//                                ofVec3f(500, 500, 500))
//            );
    gui.add(octave.setup("Noise octave", 5, 1, 10));
    gui.add(offset.setup("Noise offset", 3, 1, 6));
    gui.add(persistence.setup("Noise persistence", 0.4, 0.1, 1.0));
    gui.add(distLength.setup("Apply Alpha Dist", 0.15, 0.1, 0.5));
    gui.add(fogHighlightX.setup("Fog Highlight X", 0.5, 0.0, 1.0));
    gui.add(fogHighlightY.setup("Fog Highlight Y", 0.5, 0.0, 1.0));
    gui.add(fogPosZ.setup("Fog Pos Z", -200.0, -500.0, -10.0));
    gui.add(fogTexS.setup("Fog Tex Speed", 1.0, 0.0, 5.0));
    gui.add(fogScale.setup("Fog Scale", 1024, 256, 2048));
    fogPosZ.addListener(this, &Fog::changeFogSetting);
    fogTexS.addListener(this, &Fog::changeFogSetting2);
    

    
    /*--------------------------------------------------------------
     
     
     noise buffer
     
     
     --------------------------------------------------------------*/
    noise.allocate(
                   pow(2, octave + offset),
                   pow(2, octave + offset), 1, GL_RGBA32F, octave, offset, persistence);
    createNoise();
    
    /*--------------------------------------------------------------
     
     
     Fog
     
     
     --------------------------------------------------------------*/
    offsetPositionX = new float[fogBufferCount];
    offsetPositionZ = new float[fogBufferCount];
    offsetPositionS = new float[fogBufferCount];
    offsetTexCoordS = new float[fogBufferCount];
    offsetTexCoordT = new float[fogBufferCount];
    
    updateFogSetting();
    
    std::cout << "Created noiseBufferCount" << endl;
    
    meshFog.setMode(OF_PRIMITIVE_TRIANGLES);
    meshFog.enableIndices();
    
    meshFog.addVertex(ofVec3f(-1.0,  1.0,  0.0));
    meshFog.addVertex(ofVec3f(1.0,  1.0,  0.0));
    meshFog.addVertex(ofVec3f(-1.0, -1.0,  0.0));
    meshFog.addVertex(ofVec3f(1.0, -1.0,  0.0));
    
    meshFog.addTexCoord(ofVec2f(0.0, 0.0));
    meshFog.addTexCoord(ofVec2f(1.0, 0.0));
    meshFog.addTexCoord(ofVec2f(0.0, 1.0));
    meshFog.addTexCoord(ofVec2f(1.0, 1.0));
    
    meshFog.addColor(ofFloatColor(1.0, 1.0, 1.0, 1.0));
    meshFog.addColor(ofFloatColor(1.0, 1.0, 1.0, 1.0));
    meshFog.addColor(ofFloatColor(1.0, 1.0, 1.0, 1.0));
    meshFog.addColor(ofFloatColor(1.0, 1.0, 1.0, 1.0));
    
    meshFog.addTriangle(0, 2, 1);
    meshFog.addTriangle(1, 2, 3);
    
    setNormals(meshFog);
    
    /*--------------------------------------------------------------
     
     
     depth
     
     
     --------------------------------------------------------------*/
    ofFbo::Settings setting;
    depthMapRes = 1024;
    setting.width = depthMapRes;
    setting.height = 1024 * (float(ofGetHeight())/ ofGetWidth());
    setting.textureTarget = GL_TEXTURE_2D;
    setting.internalformat = GL_R8;
    setting.useDepth = true;
    setting.depthStencilAsTexture = true;
    setting.useStencil = true;
    setting.minFilter = GL_LINEAR;
    setting.maxFilter = GL_LINEAR;
    setting.wrapModeHorizontal = GL_CLAMP_TO_EDGE;
    setting.wrapModeVertical = GL_CLAMP_TO_EDGE;
    //    setting.numSamples = 8;
    depthMap.allocate(setting);
}

Fog::~Fog() {}

//--------------------------------------------------------------
void Fog::createNoise(){
    noise.generate();
}
//--------------------------------------------------------------
void Fog::updateFogSetting(){
//    ofLog(OF_LOG_WARNING, "Update fog setting");
    
    for(int i = 0; i < fogBufferCount; i++){
        offsetPositionX[i] =  ofRandomf() * 100.0;
        offsetPositionZ[i] =  fogPosZ + i * float(abs(fogPosZ) * 2.0) / (fogBufferCount);
        offsetPositionS[i] =  (ofRandom(0.3) + fogTexS) * 0.01;
        offsetTexCoordS[i] =  offsetTexCoordT[i] = ofRandom(1.0);
    }
    fogChangeFlag = false;
}

//--------------------------------------------------------------
void Fog::update(){
    
    ofFbo::Settings setting;
    depthMapRes = 1024;
    setting.width = depthMapRes;
    setting.height = 1024 * (float(ofGetHeight())/ ofGetWidth());
    setting.textureTarget = GL_TEXTURE_2D;
    setting.internalformat = GL_R8;
    setting.useDepth = true;
    setting.depthStencilAsTexture = true;
    setting.useStencil = true;
    setting.minFilter = GL_LINEAR;
    setting.maxFilter = GL_LINEAR;
    setting.wrapModeHorizontal = GL_CLAMP_TO_EDGE;
    setting.wrapModeVertical = GL_CLAMP_TO_EDGE;
    depthMap.allocate(setting);
}
//--------------------------------------------------------------
void Fog::draw(){
    if(fogChangeFlag) updateFogSetting();

    for(int i = 0; i < fogBufferCount; i++){
        
        offsetTexCoordS[i] += offsetPositionS[i];
        offsetTexCoordT[i] += offsetPositionS[i];
        
        if(offsetTexCoordS[i] > 1.0 || offsetTexCoordT[i] > 1.0){
            offsetTexCoordS[i] = 0.0;
            offsetTexCoordT[i] = 0.0;
        }
        
        ofMatrix4x4 modelMatrix;
        modelMatrix.scale(fogScale, fogScale, 1.0);
        modelMatrix.translate(
                              offsetPositionX[i],
                              0.0,
                              offsetPositionZ[i]
                              );
        
        // inv Matrix
        ofMatrix4x4 invMatrix;
        invMatrix = modelMatrix.getInverse();
        
        // view Matrix
        ofMatrix4x4 viewMatrix;
//        viewMatrix = cam->getViewMatrix();
        viewMatrix = ofGetCurrentViewMatrix();
        
        // projection Matrix
        ofMatrix4x4 projectionMatrix;
//        projectionMatrix = easyCam.getProjectionMatrix();
        projectionMatrix = cam->getProjectionMatrix();

        // mvp Matrix
        ofMatrix4x4 mvpMatrix;
        mvpMatrix = modelMatrix * viewMatrix * projectionMatrix;
        
        shaderFog.setUniformMatrix4f("model", modelMatrix);
        shaderFog.setUniformMatrix4f("mvp", mvpMatrix);
        shaderFog.setUniform2f("offset", ofVec2f(offsetTexCoordS[i], offsetTexCoordT[i]));
        shaderFog.setUniform1f("distLength", distLength * 1.0);
        shaderFog.setUniform1f("fogHighlightX", fogHighlightX);
        shaderFog.setUniform1f("fogHighlightY", fogHighlightY);
        shaderFog.setUniformMatrix4f("inv", invMatrix);
        
//        shaderFog.setUniform3f("eyeDirection", easyCam.getPosition());
        shaderFog.setUniform3f("eyeDirection", cam->getPosition());
        shaderFog.setUniform4f("ambientColor", ofVec4f(0.1, 0.1, 0.1, 1.0));
        
        shaderFog.setUniformTexture("depthTexture", depthMap.getDepthTexture(), 0);
        shaderFog.setUniformTexture("noiseTexture", noise.fBuffer.getTexture(0), 1);
        
        meshFog.draw();
    }
}

//--------------------------------------------------------------
void Fog::drawDebug(){
    gui.draw();
    
    float guiWidth = int(ofGetWidth() / 8);
    ofDrawBitmapStringHighlight("noise", guiWidth * 2 + 5, 0);
    noise.fBuffer.getTexture(0).draw(
                                     guiWidth * 2 + 5,
                                     10,
                                     noise.fBuffer.getWidth() * 0.2,
                                     noise.fBuffer.getHeight() * 0.2
                                     );
    ofDrawBitmapStringHighlight("depth",
                                guiWidth * 2
                                + 5
                                + noise.fBuffer.getWidth() * 0.2
                                + 5,
                                0
                                );
    depthMap.getDepthTexture().draw(
                                    guiWidth * 2
                                    + 5
                                    + noise.fBuffer.getWidth() * 0.2
                                    + 5,
                                    10,
                                    depthMap.getWidth()*0.2,
                                    depthMap.getHeight()*0.2
                                    );
}
//--------------------------------------------------------------
void Fog::beginDepthMap(){
    depthMap.begin();
}
//--------------------------------------------------------------
void Fog::endDepthMap(){
    depthMap.end();
}
//--------------------------------------------------------------
void Fog::beginDepthShader(){
    shaderDepth.begin();
}
//--------------------------------------------------------------
void Fog::endDepthShader(){
    shaderDepth.end();
}
//--------------------------------------------------------------
void Fog::beginFogShader(){
    shaderFog.begin();
}
//--------------------------------------------------------------
void Fog::endFogShader(){
    shaderFog.end();
}
//--------------------------------------------------------------
void Fog::changeFogSetting(float &fogPosZ){
    fogChangeFlag = true;
}
//--------------------------------------------------------------
void Fog::changeFogSetting2(float &fogTexS){
    fogChangeFlag = true;
}
//--------------------------------------------------------------
void Fog::setNormals(ofMesh &mesh ){
    
    //The number of the vertices
    int nV = mesh.getNumVertices();
    
    //The number of the triangles
    int nT = mesh.getNumIndices() / 3;
    
    //    vector<ofPoint> norm( nV );            //Array for the normals
    vector<ofVec3f> norm( nV );            //Array for the normals
    
    //Scan all the triangles. For each triangle add its
    //normal to norm's vectors of triangle's vertices
    for (int t=0; t<nT; t++) {
        
        //Get indices of the triangle t
        int i1 = mesh.getIndex( 3 * t );
        int i2 = mesh.getIndex( 3 * t + 1 );
        int i3 = mesh.getIndex( 3 * t + 2 );
        
        //Get vertices of the triangle
        const ofPoint &v1 = mesh.getVertex( i1 );
        const ofPoint &v2 = mesh.getVertex( i2 );
        const ofPoint &v3 = mesh.getVertex( i3 );
        
        //Compute the triangle's normal
        ofPoint dir = ( (v2 - v1).cross( v3 - v1 ) ).normalize();
        
        //Accumulate it to norm array for i1, i2, i3
        norm[ i1 ] += dir;
        norm[ i2 ] += dir;
        norm[ i3 ] += dir;
    }
    
    //Set the normals to mesh
    mesh.clearNormals();
    
    //Normalize the normal's length
    for (int i=0; i<nV; i++) {
        //        norm[i].normalize();
        mesh.addNormal(norm[i].normalize());
    }
    
    //    mesh.addNormals(norm);
    //    meshFog.addNormals(norm);
    //    mesh.addNormals(norm);
}
