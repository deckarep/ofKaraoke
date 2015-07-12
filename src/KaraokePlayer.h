//
//  KaraokePlayer.h
//  robotKaraoke
//
//  Created by Szymczykowski, Pawel on 6/16/14.
//
//

#ifndef __robotKaraoke__KaraokePlayer__
#define __robotKaraoke__KaraokePlayer__

#include <iostream>
#include "ofMain.h"
#include "CDGFile.h"

class KaraokePlayer {
    
    CDGFile cdgFile;
    ofSoundPlayer mp3File;
    bool isPaused;
    float volume;
    
    //Visualizer crap
    float* fftSmoothed;
    int nBandsToGet;
    float avgSound;
    
public:

    KaraokePlayer();
    void open(const char *filename);
    void play();
    void pause();
    void restart();
    void stop();
    void volumeUp();
    void volumeDn();
    void end();
    unsigned char *pixels();
    void update();
    void draw();
    float getPosition();
};



#endif /* defined(__robotKaraoke__KaraokePlayer__) */
