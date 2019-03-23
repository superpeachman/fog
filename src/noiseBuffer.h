#ifndef noiseBuffer_h
#define noiseBuffer_h

struct noiseBuffer {
public:
    void allocate(
                  int _width,
                  int _height,
                  int _numColorBuffers,
                  int _internalformat = GL_RGBA,
                  int oct = 6,
                  int ofs = 3,
                  float per = 0.6
                  ){
        
        octave = oct; // 5
        offset = ofs; // 2
        persistence = per; // 0.6

//        ofLog(OF_LOG_NOTICE,
//              "_width:" + ofToString(_width)
//              + ", _height:" + ofToString(_height)
//              + ", octave:" + ofToString(octave)
//              + ", offset:" + ofToString(offset)
//              + ", persistence:" + ofToString(persistence)
//              );

        ofFbo::Settings fboSettings;
        fboSettings.width  = _width;
        fboSettings.height = _height;
        fboSettings.textureTarget = GL_TEXTURE_2D;
        fboSettings.numColorbuffers = _numColorBuffers;
        fboSettings.useDepth = false;
        fboSettings.internalformat = _internalformat;// Gotta store the data as floats, they won't be clamped to 0..1
        fboSettings.wrapModeHorizontal = GL_MIRRORED_REPEAT;
        fboSettings.wrapModeVertical = GL_MIRRORED_REPEAT;
        fboSettings.minFilter = GL_LINEAR; // No interpolation, that would mess up data reads later!
        fboSettings.maxFilter = GL_LINEAR;
        fboSettings.numSamples = 8;
        
        fBuffer.allocate(fboSettings);
        
        // Clean
        clear();
        
//        generate();
    }
    
    void clear(){
        fBuffer.begin();
        ofClear(0, 255);
        fBuffer.end();
    }

    void setSeed(float _seed){
        seed = _seed;
    };
    
    void generate(){
        int noiseBaseWidth = pow(2, octave + offset);
        setSeed(ofGetCurrentTime().getAsSeconds());
//        allocate(noiseBaseWidth, noiseBaseWidth, 1, GL_RGBA32F, octave, offset, persistence);
        
        int colorNum = noiseBaseWidth * noiseBaseWidth * 4;
        float * noiseColors = new float[colorNum];
        
        for(int y = 0; y < noiseBaseWidth; y++){
            for(int x = 0; x < noiseBaseWidth; x++){
                int i = noiseBaseWidth * y + x;
                float tempColor = noise(x, y);
                //            tempColor *= tempColor;
                //                ofLog(OF_LOG_NOTICE,
                //                      ofToString(i) + ":" + ofToString(tempColor)
                //                      );
                
                noiseColors[i*4 + 0] = tempColor;
                noiseColors[i*4 + 1] = tempColor;
                noiseColors[i*4 + 2] = tempColor;
                noiseColors[i*4 + 3] = 1.0;
            }
        }
        
        fBuffer.getTexture(0).loadData(noiseColors, noiseBaseWidth, noiseBaseWidth, GL_RGBA);
        delete [] noiseColors;
    }

    ofFbo fBuffer;
    int octave;
    int offset;
    float persistence;
    float seed;
    
    float rnd(float x, float y){
        int a = 123456789;
        int b = a ^ (a << 11);
        int c = seed + x + seed * y;
        int d = c ^ (c >> 19) ^ (b ^ (b >> 8));
        double e = double(d % 16777216) / 16777216;
        e *= 10000000.0;
        return e - floor(e);
    };
    
    float srnd(float x, float y){
        float corners =  (
                          rnd(x - 1, y - 1)
                          + rnd(x + 1, y - 1)
                          + rnd(x - 1, y + 1)
                          + rnd(x + 1, y + 1)
                          ) * 0.03125;
        
        float sides   =  (
                          rnd(x - 1, y)
                          + rnd(x + 1, y)
                          + rnd(x,     y - 1)
                          + rnd(x,     y + 1)
                          ) * 0.0625;
        
        float center = rnd(x, y) * 0.625;
        return corners + sides + center;
    };
    
    float interpolate(float a, float b, float t){
        return  a * t + b * (1.0 - t);
    };
    
    float irnd(float x, float y){
        int ix = floor(x);
        int iy = floor(y);
        float fx = x - ix;
        float fy = y - iy;
        
        float a = srnd(ix,     iy);
        float b = srnd(ix + 1, iy);
        float c = srnd(ix,     iy + 1);
        float d = srnd(ix + 1, iy + 1);
        float e = interpolate(b, a, fx);
        float f = interpolate(d, c, fx);
        return interpolate(f, e, fy);
    };
    
    float noise(float x, float y){
        float t = 0;
        int o = octave + offset; // 5 + 2 = 7
        int w = pow(2, o); // 2 ^ 7 = 128
        
        for(int i = offset; i < o; i++){
            int f = pow(2, i); // 4, 8, 16, 32, 64, 128
            float p = pow(persistence, i - offset + 1);
            float b = w / f;
            t += irnd(
                      x / b,
                      y / b
                      ) * p; // 0.6, 0.36, 0.216, 0.1296, 0.07776, 0.046656
        }
        return t;
    };
    
    float snoise(int x, int y, float w, float h){
        float t, u, v;
        u = float(x) / w; // 0 to 1
        v = float(y) / h; // 0 to 1
        t = noise(x,     y    );
        
        t = noise(x,     y    ) *        u  *        v
        + noise(x,     y + w) *        u  * (1.0 - v)
        + noise(x + w, y    ) * (1.0 - u) *        v
        + noise(x + w, y + w) * (1.0 - u) * (1.0 - v);
        return t;
    };
  
//private:

};

#endif
