#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // initial setting
    ofSetFrameRate(60);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    /*--------------------------------------------------------------
     
     
     ofGui
     
     
     --------------------------------------------------------------*/
    float guiWidth = int(ofGetWidth() / 8);
    gui.setDefaultWidth(guiWidth);
    gui.setup();
    gui.setPosition(0, 0);
    gui.add(rotationFlag.setup("Rotataion", true));
    gui.add(camNear.setup("Camera Near Clip", 100.0, 1.0, 100.0));
    gui.add(camFar.setup("Camera Far Clip", 3000.0, 100.0, 3000.0));
    gui.add(boxScale.setup("Box Scale", 15.0, 1.0, 20.0));
    gui.add(boxAngle.setup("Box Angle", 45.0, 0.0, 360.0));
    gui.add(lightPosition.setup("Light Pos",
                                ofVec3f(-80.0, 50.0, -150.0),
                                ofVec3f(-500, -500, -500),
                                ofVec3f(500, 500, 500))
            );
    camNear.addListener(this, &ofApp::changeNearClip);
    camFar.addListener(this, &ofApp::changeFarClip);
    
    /*--------------------------------------------------------------
     
     
     Shader
     
     
     --------------------------------------------------------------*/
    shader.load("shaders/shader");
    
    /*--------------------------------------------------------------
     
     
     Camera
     
     
     --------------------------------------------------------------*/
    // ----------------------------- debug
    // projection
    std::cout << "Fov : " << easyCam.getFov() << endl;
    std::cout << "AspectRatio : " << easyCam.getAspectRatio() << endl;
    std::cout << "NearClip : " << easyCam.getNearClip() << endl;
    std::cout << "FarClip : " << easyCam.getFarClip() << endl;
    // view
    std::cout << "Position: " << easyCam.getPosition() << endl;
    std::cout << "LookAt : " << easyCam.getLookAtDir() << endl;
    std::cout << "Up : " << easyCam.getUpDir() << endl;

    // I dont know why but if setupPerspective comes first, Depthmap works wrong!!
//    easyCam.setupPerspective();
    easyCam.setNearClip(camNear);
    easyCam.setFarClip(camFar);
    easyCam.setFov(60);
    //    easyCam.setForceAspectRatio(1.0);

    pos = ofVec3f(0.0, 0.0, 0.0);

    /*--------------------------------------------------------------
     
     
     Fog
     
     
     --------------------------------------------------------------*/
    fog = new Fog(easyCam);
    
//    easyCam.setNearClip(camNear);
//    easyCam.setFarClip(camFar);
}

//--------------------------------------------------------------
void ofApp::update(){
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    if(rotationFlag) time = ofGetElapsedTimef();
    
    if(flagCameraClipChanged){
        easyCam.setNearClip(camNear);
        easyCam.setFarClip(camFar);
        
        fog->update();

        flagCameraClipChanged = false;
    }
}
//--------------------------------------------------------------
void ofApp::draw(){
//    glEnable(GL_CULL_FACE);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    /*--------------------------------------------------------------
     
     
     depth
     
     
     --------------------------------------------------------------*/
    fog->beginDepthMap();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    ofClear(255, 0, 0, 255);
    ofDisableSmoothing();
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    easyCam.begin();
    fog->beginDepthShader();
    renderScene(0);
    fog->endDepthShader();
    easyCam.end();
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    fog->endDepthMap();

    
    easyCam.begin();

    /*--------------------------------------------------------------
     
     
     render
     
     
     --------------------------------------------------------------*/
    shader.begin();
    renderScene(1);
    shader.end();

    /*--------------------------------------------------------------
     
     
     fog
     
     
     --------------------------------------------------------------*/
    fog->beginFogShader();
    renderScene(2);
    fog->endFogShader();


    easyCam.end();
    
//    glDisable(GL_CULL_FACE);
    
    if(debugFlag){
        ofPushStyle();
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        ofSetColor(255);
        /*--------------------------------------------------------------
         
         
         gui
         
         
         --------------------------------------------------------------*/
        gui.draw();
        
        /*--------------------------------------------------------------
         
         
         debug
         
         
         --------------------------------------------------------------*/
        fog->drawDebug();

        ofPopStyle();
    }
}
//--------------------------------------------------------------
void ofApp::renderScene(int mode){
    ofEnableDepthTest();
    ofPushStyle();
    
    if(mode == 0 || mode == 1){
        /*--------------------------------------------------------------
         
         
         Scene
         
         
         --------------------------------------------------------------*/

        for(int i=0; i<MODEL_NUM; i++){
            
            // model Matrix
            ofMatrix4x4 modelMatrix;
            if(i == 0) modelMatrix.translate(-100, 0, 0);
            if(i == 1) modelMatrix.translate(0, 0, 0);
            if(i == 2) modelMatrix.translate(100, 0, 0);
            if(i == 3) modelMatrix.translate(0, -100, 0);
            if(i == 4) modelMatrix.translate(0, 100, 0);
            if(i == 5) modelMatrix.translate(0, 0, -100);
            if(i == 6) modelMatrix.translate(0, 0, 100);
            
            modelMatrix.rotate(time*10, 1.0, 0.5, 0.0);

            // view Matrix
            ofMatrix4x4 viewMatrix;
            viewMatrix = ofGetCurrentViewMatrix();
            
            // projection Matrix
            ofMatrix4x4 projectionMatrix;
            projectionMatrix = easyCam.getProjectionMatrix();
            
            // mvp Matrix
            ofMatrix4x4 mvpMatrix;
            mvpMatrix = modelMatrix * viewMatrix * projectionMatrix;
            
            // inv Matrix
            ofMatrix4x4 invMatrix;
            invMatrix = modelMatrix.getInverse();

            if (mode == 0){
//                shaderDepth.setUniformMatrix4f("mvp", mvpMatrix);
                fog->shaderDepth.setUniformMatrix4f("mvp", mvpMatrix);
            } else if (mode == 1){
                shader.setUniformMatrix4f("model", modelMatrix);
                shader.setUniformMatrix4f("modelView", easyCam.getModelViewMatrix());
                shader.setUniformMatrix4f("mvp", mvpMatrix);
                shader.setUniformMatrix4f("inv", invMatrix);
                shader.setUniform3f("eyeDirection", easyCam.getPosition());
                shader.setUniform3f("lightDirection", ofVec3f(
                                                             lightPosition->x,
                                                             lightPosition->y,
                                                             lightPosition->z
                                                             ));
                shader.setUniform4f("ambientColor", ofVec4f(0.1, 0.1, 0.1, 1.0));
            }
            
            sphere.setPosition(pos);
            sphere.setRadius(50);
            
            vboMesh = sphere.getMesh();
            for(int j=0; j<vboMesh.getVertices().size(); j++){
                vboMesh.addColor(ofFloatColor(1.0, 1.0, 1.0, 1.0));
            }
            vboMesh.setMode(OF_PRIMITIVE_TRIANGLES);
            vboMesh.draw();
            
            // BOX
            if(i == MODEL_NUM - 1){
//                ofLog(OF_LOG_WARNING, "Draw Box Here");
                
                ofMatrix4x4 modelMatrix;
                modelMatrix.translate(0, 0, 0);
                
                modelMatrix.rotate(boxAngle, 0.0, 1.0, 0.0);
                modelMatrix.scale(boxScale, boxScale, boxScale);

                // view Matrix
                ofMatrix4x4 viewMatrix;
                viewMatrix = ofGetCurrentViewMatrix();
                
                // projection Matrix
                ofMatrix4x4 projectionMatrix;
                projectionMatrix = easyCam.getProjectionMatrix();
                
                // mvp Matrix
                ofMatrix4x4 mvpMatrix;
                mvpMatrix = modelMatrix * viewMatrix * projectionMatrix;
                
                // inv Matrix
                ofMatrix4x4 invMatrix;
                invMatrix = modelMatrix.getInverse();
                
                if (mode == 0){
//                    shaderDepth.setUniformMatrix4f("mvp", mvpMatrix);
                    fog->shaderDepth.setUniformMatrix4f("mvp", mvpMatrix);
                } else if (mode == 1){
                    shader.setUniformMatrix4f("model", modelMatrix);
                    shader.setUniformMatrix4f("modelView", easyCam.getModelViewMatrix());
                    shader.setUniformMatrix4f("mvp", mvpMatrix);
                    shader.setUniformMatrix4f("inv", invMatrix);
                    shader.setUniform3f("eyeDirection", easyCam.getPosition());
                    shader.setUniform3f("lightDirection", ofVec3f(
                                                                  lightPosition->x,
                                                                  lightPosition->y,
                                                                  lightPosition->z
                                                                  ));
                    shader.setUniform4f("ambientColor", ofVec4f(0.1, 0.1, 0.1, 1.0));
                }
                
                vboMesh = box.getMesh();
                for(int j=0; j<vboMesh.getVertices().size(); j++){
                    vboMesh.addColor(ofFloatColor(0.0, 0.0, 0.0, 0.0));
                }
                vboMesh.setMode(OF_PRIMITIVE_TRIANGLES);
                vboMesh.draw();
            }

        }
        
        // draw axis (x, y, z)
//        ofSetLineWidth(0.1);
//        ofSetColor(255, 0, 0);
//        ofDrawLine(-500, 0, 0, 500, 0, 0);
//        ofSetColor(0, 255, 0);
//        ofDrawLine(0, -400, 0, 0, 400, 0);
//        ofSetColor(0, 0, 255);
//        ofDrawLine(0, 0, -400, 0, 0, 400);

    }else{
        fog->draw();
    }
    ofPopStyle();
    ofDisableDepthTest();
    
}
//--------------------------------------------------------------
void ofApp::changeNearClip(float &camNear){
    flagCameraClipChanged = !flagCameraClipChanged;
}
//--------------------------------------------------------------
void ofApp::changeFarClip(float &camFar){
    flagCameraClipChanged = !flagCameraClipChanged;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
//    if(key =='n'){
//        createNoise();
//    }
    if(key =='d'){
        debugFlag = !debugFlag;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
