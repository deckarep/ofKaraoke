#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetFrameRate(60.0f);
    ofEnableSmoothing();
    ofSetWindowTitle("Johnson Karaoke v7.0");
    ofSetCircleResolution(30);
    
    TCP.setup(11999);
    TCP.setMessageDelimiter("\n");

    currentTrack = "";
    
    soundStream.setup( this, 1, 1, 44100, 512, 4);
    audioBuffer.assign( 512, 0.0 );
    
    cdgScreenX = 0;
    cdgScreenY = 0;
    cdgScreenRotation = 0;
    
    ofBackground(0, 0, 0);
    
    //Turning this to false makes it not refresh the background (on by default)
    //ofSetBackgroundAuto(false);
    
    //TODO: Add cool background like particles or stars, etc
    //TODO: Add cool interface stage or reflection with stage like presence
    //TODO: Open karaoke files from a server so we can get better stuff going on
    //TODO: Show scroll banner on the bottom of the screen showing who is next
    //TODO: Do cool transitions
    //TODO: Show spectrum of music for added bonus
    //TODO: Integrate queueing system, etc
    //TODO: Remote control with mobile for admin
    //TODO: Maybe show camera video of someone singing
    //TODO: Reflection of cdgScreen (cool effect)
    //TODO: Screen the black out of the cdgScreen so the box goes away
    //TODO: show volume indicator
    //TODO: Figure out to route audio to headphones and not appletv
    //TODO: Use keywords from song track name to show random images from the web in the background
    ofSoundStreamListDevices();
    
    // Based on the output above sets it to built in output (we don't want audio going to airplay TV)
    // TODO: figure out how to not hardcode this number.
    //soundStream.setDeviceID(2);
    
    // The app used to start automatically
    //karaokePlayer.open("/Users/deckarep/Desktop/karaoke4go/cdg/Prince - Beautiful Ones, The");
    //karaokePlayer.play();
}

//--------------------------------------------------------------
void ofApp::update(){
    ofSoundUpdate();
    karaokePlayer.update();
    
    cdgScreen.setFromPixels(karaokePlayer.pixels(), CDG_WIDTH, CDG_HEIGHT, OF_IMAGE_COLOR_ALPHA);
    cdgScreen.update();
    
    mirrorCdgScreen.setFromPixels(karaokePlayer.pixels(), CDG_WIDTH, CDG_HEIGHT, OF_IMAGE_COLOR_ALPHA);
    mirrorCdgScreen.update();
    
    //This for example scrolls the image to the right
    //cdgScreenX+=1;
    cdgScreenRotation +=1;
    
    string message = readTCPMessage();
    if (message.length() > 0){
        cout << "Got a tcp message: " << message << endl;
        startSong(message);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    // Cool gradient effect (must be called in draw() in this order
    ofColor colorOne;
    ofColor colorTwo;
    //colorOne.set (10, 73, 90);
    colorOne.set(44, 141, 173);
    colorTwo.set (0, 0, 0);
    ofBackgroundGradient(colorOne, colorTwo, OF_GRADIENT_CIRCULAR);
    // End gradient effect

    // Blending MODES!!! YAY!
    //ofEnableAlphaBlending();
    //ofEnableBlendMode(OF_BLENDMODE_ADD);
    
    // This works
    mirrorCdgScreen.mirror(true,true);

    
    // This affects the cdgImage in terms of colors and alpha (255,255,255,255) shows the colors correctly
    ofSetColor(255,255,255,255);
    
    // all this commented out shit demonstrates how to do rotation, scaling of the screen and stuff
    ofPushMatrix();
        // This works, it skews the screen and stuff
        ofRotateX(-10);
        // ofTranslate(100, 100);
        ofScale(2, 2);
        // ofRotateZ(cdgScreenRotation);
        cdgScreen.draw(100 + cdgScreenX,10);
        ofSetColor(255,255,255,100);
        mirrorCdgScreen.draw(100 + cdgScreenX, 216);
        ofSetColor(255,255,255,255);
    ofPopMatrix();
    
    // Karaoke player
    karaokePlayer.draw();
    
    // Banner related
    ofDrawBitmapString(currentTrack, 20, 700);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    // Toggles fullscreen and stuff
    switch (key) {
        case 'f':
            ofToggleFullscreen();
            break;
        case 'p':
            karaokePlayer.pause();
            break;
        case 'r':
            karaokePlayer.restart();
            cout << "Restarting playback of file." << endl;
            break;
        case OF_KEY_UP:
            karaokePlayer.volumeUp();
            break;
        case OF_KEY_DOWN:
            karaokePlayer.volumeDn();
            break;
        case OF_KEY_RIGHT:
            karaokePlayer.end();
            break;
        default:
            cout << "Key pressed: " << key << endl;
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

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
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

// TODO: investigate what these functions do
void ofApp::audioReceived(float *input, int bufferSize, int nChannels) {
    for (int i=0;i<bufferSize;i++) {
        //        audioBuffer[ i ] = i > 150 ? input[ i ] : input[ i ] * 0.5;
        audioBuffer[ i ] = input[ i ];
    }
}

void ofApp::audioOut(float *output, int bufferSize, int nChannels) {
    for (int i=0;i<bufferSize;i++) {
        output[ i ] = audioBuffer[ i ];
    }
}

string ofApp::readTCPMessage(){
    string msg = "";
    //for each connected client lets get the data being sent and lets print it to the screen
    for(unsigned int i = 0; i < (unsigned int)TCP.getLastID(); i++){
        if( !TCP.isClientConnected(i) )continue;
        
        // Could be multiple clients but we only care
        // about just a single client. TODO: refactor
        cout << "in readTCPMessage" << endl;
        msg = TCP.receive(i);
        TCP.disconnectClient(i);
    }
    
    return msg;
}

string ofApp::getFileName(const string &s){
    char sep = '/';
    
    size_t i = s.rfind(sep, s.length());
    if (i != string::npos) {
        return(s.substr(i+1, s.length() - i));
    }
    
    return("");
}

void ofApp::startSong(const string& s){
    // Set the currenTrack for the banner.
    currentTrack = getFileName(s);
    
    karaokePlayer.stop();
    karaokePlayer.open(s.c_str());
    karaokePlayer.restart();
}
