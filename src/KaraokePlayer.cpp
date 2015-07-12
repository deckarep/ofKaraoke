#include "KaraokePlayer.h"

KaraokePlayer::KaraokePlayer() {
    // FFT initialization
    fftSmoothed = new float[8192];
    for (int i = 0; i < 8192; i++){
        fftSmoothed[i] = 0;
    }
    nBandsToGet = 128;
    avgSound = 0;
    
    cout << "KaraokePlayer() constructor called" << endl;
}

void KaraokePlayer::open(const char *baseFilename) {
    char *filename = (char *) malloc( strlen(baseFilename) + 5 );
    
    // Load the MP3
    sprintf(filename, "%s.%s", baseFilename, "mp3");
    mp3File.loadSound(filename);
    
    // Load the CDG
    sprintf(filename, "%s.%s", baseFilename, "cdg");
    cdgFile.open(filename);
    
    free(filename);
}

void KaraokePlayer::play() {
    mp3File.play();
    volume = mp3File.getVolume();
}

void KaraokePlayer::restart() {
    mp3File.stop();
    cdgFile.resetCDG();
    mp3File.setPosition(0);
    play();
}

void KaraokePlayer::stop(){
    mp3File.stop();
    cdgFile.resetCDG();
    cdgFile.close();
}

// This basically works like fast-forward
void KaraokePlayer::end(){
    cout << "karaokePlayer::end() called." << endl;
    mp3File.setPosition(0.80f);
}

void KaraokePlayer::pause() {
    mp3File.setPaused(!isPaused);
    isPaused = !isPaused;
}

float KaraokePlayer::getPosition(){
    return mp3File.getPosition();
}

void KaraokePlayer::volumeUp(){
    volume += 0.1;

    // clamp it to no more than 1.0
    if (volume > 1.0){
        volume = 1.0;
    }

    mp3File.setVolume(volume);
}

void KaraokePlayer::volumeDn(){
    volume -= 0.1;
    
    // clamp it to no more than 1.0
    if (volume < 0){
        volume = 0;
    }
    mp3File.setVolume(volume);

}

// The CD+G format takes advantage of the unused channels R thru W.  These unused
// six bits are used to store graphics information. Note that this is an extremely
// thin stream of information.  6 bits per byte * 16 bytes per packet * 4
// packets per sector * 75 sectors per second = 28800 bits per second, or 3.6 K per
// second.  By comparison, a typical 160 x 120 QuickTime movie uses 90K per second.
void KaraokePlayer::update() {
    // All visualizer code
    float* val = ofSoundGetSpectrum(nBandsToGet);
    avgSound = 0;
    
    // smooth fft and calc average volume
    for (int i = 0;i < nBandsToGet; i++){
        fftSmoothed[i] *= 0.96f;
        if (fftSmoothed[i] < val[i]) fftSmoothed[i] = val[i];
        avgSound += fftSmoothed[i];
    }
    
    // calculate average loudness of the music for "volume"
    avgSound /= nBandsToGet;
    // End visualizer code
    
    // Karaoke Code here
    // 24 bytes * 4 packets * 75 sectors = 7200 bytes per second
    long int bytesForPosition = mp3File.getPositionMS() * 7.2;
    while ( cdgFile.bytesRead() <= bytesForPosition )  //TODO: Fix this....it's hogging all CPU when music file ends.
        cdgFile.readNext();
}

void KaraokePlayer::draw() {
    // Referenced audio visualizer from here.
    // https://github.com/jahfer/Audio-visualizer/blob/master/src/testApp.cpp#L343
    
    ofSetColor(255, 255, 255, 255);
    ofPushMatrix();
    ofTranslate(0, 0);
    
    float yLoc = 620.0f;
    // Bass drum
    float bass = (fftSmoothed[1]*7 > 3) ? fftSmoothed[1]*7 : 3;
    ofCircle(880, yLoc, bass * 1.2f);
    // Synthesized guitar in bridge/chorus
    float guitar = (fftSmoothed[7]*75 > 3) ? fftSmoothed[7]*75 : 3;
    ofCircle(920, yLoc, guitar * 1.2f);
    // Drum symbol
    float symbol = (fftSmoothed[27]*150 > 3) ? fftSmoothed[27]*150 : 3;
    ofCircle(960, yLoc, symbol * 1.2f);
    ofPopMatrix();
}

unsigned char * KaraokePlayer::pixels() {
    return cdgFile.pixels();
}


