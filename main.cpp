#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <AL/al.h>
#include <AL/alc.h>
#include "audioMaster.h"
#include "sourceAL.h"

using namespace std;
using namespace cv;

// Valeur de sensibilité utilisée dans absdiff()
const static int SENSITIVITY_VALUE = 20;
// Taille du BLUR pour lisser l'image en sortie de absdiff()
const static int BLUR_SIZE = 10;

// Nombre de frames consécutives sans détection d'objet
int nbFramesWithoutObject = 0;
// Booléen pour activer/désactiver le dessin autour de l'objet
bool dessinMode = false;

SourceAL *source;
ALuint buffer;

int maximum;
int minimum;

// Fonction de conversion int -> string
string intToString(int number){
    std::stringstream ss;
    ss << number;
    return ss.str();
}

// Fonction de détection du mouvement
void searchForMovement(Mat thresholdImage, Mat &cameraFeed){
    bool objectDetected = false;
    Mat temp;
    int theObject[2];
    thresholdImage.copyTo(temp);

    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;
    //Trouve les contours des objets (régions à 1 dans l'image binarisée)
    findContours(temp,contours,hierarchy,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE );

    // Si on trouve des contours, des objets sont détectés
    if(contours.size()>0)
        objectDetected=true;
    else
        objectDetected = false;

    if(objectDetected){
        nbFramesWithoutObject = 0;
        // Le contours le plus grand est à la fin du vecteur contours
        vector< vector<Point> > largestContourVec;
        largestContourVec.push_back(contours.back());
        // On crée une boîte englobante autour de l'objet pour récupérer son centre
        // qui va être la position de l'objet finale
        Rect objectBoundingRectangle = boundingRect(largestContourVec.at(0));
        int xpos = objectBoundingRectangle.x+objectBoundingRectangle.width/2;
        int ypos = objectBoundingRectangle.y+objectBoundingRectangle.height/2;

        theObject[0] = xpos , theObject[1] = ypos;

        // Lancement de la source sonore
        if (source->isPlaying()) {
            source->setPosition(xpos-(cameraFeed.cols/2), ypos, 0.0f);
        } else {
            source->play(buffer);
            source->setPosition(xpos, ypos, 0.0f);
        }

    }
// arreter la source sonore si aucun mouvement depuis un certain nombre de frame ?
//    else {
//        nbFramesWithoutObject++;
//        if (nbFramesWithoutObject > 10)
//            source->stop();
//    }


    // Dessine un cercle sur l'objet en mouvement avec la position en x et y
    if (dessinMode) {
        int x = theObject[0];
        int y = theObject[1];
        circle(cameraFeed,Point(x,y),20,Scalar(0,255,0),2);
        line(cameraFeed,Point(x,y),Point(x,y-25),Scalar(0,255,0),2);
        line(cameraFeed,Point(x,y),Point(x,y+25),Scalar(0,255,0),2);
        line(cameraFeed,Point(x,y),Point(x-25,y),Scalar(0,255,0),2);
        line(cameraFeed,Point(x,y),Point(x+25,y),Scalar(0,255,0),2);
        putText(cameraFeed,"(" + intToString(x)+","+intToString(y)+")",Point(x,y),1,1,Scalar(255,0,0),2);

    }
}



int main(){


    // Initialisation de la source sonore
    AudioMaster audioMast;
    audioMast.init();
    audioMast.setListenerDate(0.f, 0.f, 0.f);

    source = new SourceAL();
    buffer = audioMast.loadSound("../tracker/son/moustik.wav");

    source->setVolume(5.0f);
    minimum = 5000;
    maximum = -20;

    // Debug mode, activer en appuyant sur la touche 'd'
    bool debugMode = false;
    // Actvier/Désactiver le tracking en appuyant sur la touche 't'
    bool trackingEnabled = true;
    // Pause de la vidéo en appuyant sur la touche 'p'
    bool pause = false;

    // Images, frame1 et frame2 à comparer
    Mat frame1,frame2;
    // Images en niveau de gris (needed for absdiff() function)
    Mat grayImage1,grayImage2;
    // Différence entre les deux frame
    Mat differenceImage;
    // Image différence binarisée (utilisée dans findContours())
    Mat thresholdImage;
    // Vidéo
    VideoCapture capture;

    int imageIndex = 1;

    // Création des fenêtres
    namedWindow("Difference Image");
    namedWindow("Threshold Image");
    namedWindow("Final Threshold Image");

    // Lecture de la vidéo en boucle
    while(1) {
        imageIndex = 1;
        // Parcours de frames deux par deux, avec un pas de 1
        while(imageIndex < 255){

            stringstream ss;
            ss << imageIndex;

            capture.open("../tracker/video/bouncingBall/image" + ss.str() + ".jpg");
            cout << ss.str() << endl;
            imageIndex++;

            if(!capture.isOpened()){
                cout<<"ERROR ACQUIRING VIDEO FEED\n";
                getchar();
                return -1;
            }

            // Lecture de la première frame
            capture.read(frame1);
            // Convertion de la première frame en niveau de gris
            cv::cvtColor(frame1,grayImage1,COLOR_BGR2GRAY);

            // Idem pour frame 2
            capture.read(frame2);
            cv::cvtColor(frame2,grayImage2,COLOR_BGR2GRAY);

            // Image différence
            cv::absdiff(grayImage1,grayImage2,differenceImage);

            // Image binarisé selon la valeur de sensibilité donnée
            cv::threshold(differenceImage,thresholdImage,SENSITIVITY_VALUE,255,THRESH_BINARY);

            // Debug mode qui affiche les différentes images du processus
            if(debugMode==true)
            {
                cv::imshow("Difference Image",differenceImage);
                cv::imshow("Threshold Image2", thresholdImage);
                cv::imshow("Threshold Image", thresholdImage);
            } else {
                cv::destroyWindow("Difference Image");
                cv::destroyWindow("Threshold Image");
                cv::destroyWindow("Final Threshold Image");
            }

            // Blur l'image binarisé pour la lisser et diminuer le bruit
            cv::blur(thresholdImage,thresholdImage,cv::Size(BLUR_SIZE,BLUR_SIZE));

            // Binarise l'image lissée (car blur renvoie une image en niveau de gris.
            cv::threshold(thresholdImage,thresholdImage,SENSITIVITY_VALUE,255,THRESH_BINARY);
            if(debugMode==true){
                // Affiche l'image finale binarisée
                imshow("Final Threshold Image",thresholdImage);
            }

            // Détection du mouvement
            if(trackingEnabled){
                searchForMovement(thresholdImage,frame1);
            }

            //show our captured frame
            imshow("Frame1",frame1);
            // Check si une touche a été préssée
            // 10ms de délais obligatoire pour raffraichir les frames
            switch(waitKey(10)){
            case 27 : // 'Echap' : Quitte le programme
                cv::destroyAllWindows();
                return 0;
            case 116: // 'T' Active/Désactive le tracking
                trackingEnabled = !trackingEnabled;
                if(trackingEnabled == false)
                    cout << "Tracking désactivé." << endl;
                else
                    cout << "Tracking activé." << endl;
                break;

            case 100: // 'D' : Active/Désactive le mode débug
                debugMode = !debugMode;
                if(debugMode == false)
                    cout << "Debug mode désactivé." << endl;
                else
                    cout << "Debug mode activé." << endl;
                break;

            case 99: // 'C' : Active/Désactive le dessin autour de l'objet
                dessinMode = !dessinMode;
                if(dessinMode == false)
                    cout << "Dessin mode désactivé." <<endl;
                else
                    cout << "Dessin mode activé." << endl;
                break;

            case 112: // 'P' Active la pause
                pause = !pause;
                if(pause == true){
                    cout << "En pause, appuyez sur 'p' pour enlever la pause." << endl;
                    while (pause == true){
                        // Bloque le programme dans la boucle ici
                        // et met en pause le son
                        if (source->isPlaying())
                            source->stop();
                        switch (waitKey()){
                        // 'P' Désactive la pause
                        case 112:
                            source->continuePlaying();
                            pause = false;
                            cout<<"Pause désactivé"<<endl;
                            break;
                        }
                    }
                }
                break;
            }
        }
        capture.release();
    }

    return 0;
}
