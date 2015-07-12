#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "KaraokePlayer.h"

class ofApp : public ofBaseApp{
    
    ofSoundStream soundStream;
    KaraokePlayer karaokePlayer;
    ofImage cdgScreen;
    vector <float> audioBuffer;
    string currentTrack;
    int cdgScreenX;
    int cdgScreenY;
    int cdgScreenRotation;
    
    ofxTCPServer TCP;

    public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        string getFileName(const string& s);
        void startSong(const string& s);

        void audioReceived(float *input, int bufferSize, int nChannels);
        void audioOut(float *output, int bufferSize, int nChannels);
    
        string readTCPMessage();
};
