#include <iostream>
#include <fstream>
#include <list>
using namespace std;

int main() {
   // Create a text string, which is used to output the text file
   string myText;

   // Read from the text file
   ifstream MyReadFile("PNMLFiles\\SimpleLoadBal\\PT\\simple_lbs-20.pnml");
   cout << "Yo I'm here1337";

   list<string> placeList = {};
   list<string> transitionList = {};
   list<string> arcList = {};
   int noPlaces = 0;
   int noTransitions = 0;
   int noArcs = 0;

   // Use a while loop together with the getline() function to read the file line by line
    while (getline (MyReadFile, myText)) {
      if(myText.rfind("<place ", 0) == 0) {
         noPlaces++;
         placeList.push_back(myText);
      }
      if(myText.rfind("<transition ", 0) == 0) {
         noTransitions++;
         transitionList.push_back(myText);
      }
      if(myText.rfind("<arc ", 0) == 0) {
         noArcs++;
         arcList.push_back(myText);
      }
   }

   cout << endl;
   cout << "Number of places: " << noPlaces << endl;
   cout << "Number of places in placeList: " << placeList.size() << endl;
   cout << "Number of transitions: " << noTransitions << endl;
   cout << "Number of transitions in transitionList: " << transitionList.size() << endl;
   cout << "Number of arcs: " << noArcs << endl;
   cout << "Number of arcs in arcList: " << arcList.size() << endl;

   // Close the file
   MyReadFile.close();
}