/************************************************************************

ConfigCallbacks.cpp

Copyright (C) 2007 Virus
Copyright (C) 2002 mooby

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

************************************************************************/

#pragma warning(disable:4786)


#include "FileInterface.hpp"
#include "Preferences.hpp"
#include "ConfigCallbacks.hpp"
#include "ConfigFunctions.hpp"

#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

extern Preferences prefs;

// a container for a window with a progress bar.  used for compress and decompress
class ProgWindow
{
public:
	ProgWindow(const string& text)
	{
      {
         Fl_Window* o = new Fl_Window(220, 50, text.c_str());
		w = o;
         prog = new Fl_Progress(0,0,200,20);
         prog->minimum(0);
         prog->maximum(1);
		prog->value(0);
		o->end();
	  }
      w->show();
      Fl::wait(0);
	}

   void update(const float moo)
   {
      prog->value(moo);
      w->redraw();
      Fl::wait(0);  
   }

   ~ProgWindow() {w->hide(); w->redraw(); delete w;}
private:
   Fl_Window* w;
   Fl_Progress* prog;
};

// compresses cd using the compression methods in zipCD
void compressIt(FileInterface* cd, 
                CompressedFileInterface* zipCD, 
                const string& outputFileName,
                const string& indexFileName)
{
   cd->setCacheMode(FileInterface::oldMode);
   CDTime start(0,2,0);
		// these hold the info needed for generating the .index or .table file
   vector<unsigned long> indexTable;
   vector<unsigned long> sizeTable;

		// open the output file
   std::ofstream outfile(outputFileName.c_str(), ios::binary);

		// the buffers for compression
   unsigned char* inputBuffer = 
      new unsigned char[zipCD->getCompressedFrames() * bytesPerFrame];
   unsigned char* outputBuffer = 
      new unsigned char[(zipCD->getCompressedFrames() + 1) * bytesPerFrame];

   ProgWindow* p = new ProgWindow("Compressing...");

   while (start < cd->getCDLength())
   {
      unsigned int i = 0;
			// read up to compressedFrames or until the end of the CD, whichever is smaller
      while ( (start < cd->getCDLength()) && (i < zipCD->getCompressedFrames()) )
      {
         p->update((float)start.getAbsoluteFrame() / (float)cd->getCDLength().getAbsoluteFrame());
         cd->seek(start);
         memcpy(inputBuffer + bytesPerFrame * i, cd->getBuffer(), bytesPerFrame);
         i++;
         start += CDTime(0,0,1);
      }
			// add the index information for the file location
      indexTable.push_back(outfile.tellp());

			// compress the data
      unsigned int outputBytes = (zipCD->getCompressedFrames() + 1) * bytesPerFrame;
      unsigned int inputBytes = i * bytesPerFrame;
      zipCD->compressData((char*)inputBuffer, (char*)outputBuffer, 
         inputBytes, outputBytes);

			// write the compressed data and add the compressed size to the sizeTable
      outfile.write((char*)outputBuffer, outputBytes);
      sizeTable.push_back(outputBytes);
   }

		// write the index file
   string tableToWrite = zipCD->toTable(indexTable, sizeTable);

   ofstream indexFile(indexFileName.c_str(), ios::binary);
   indexFile.write(tableToWrite.c_str(), tableToWrite.size());

		// clean up
   indexFile.close();
   outfile.close();
   delete [] inputBuffer;
   delete [] outputBuffer;
   delete cd;
   delete zipCD;
   delete p;

      moobyMessage("Done");
}

// decompressing is much easier
void decompressIt(FileInterface* cd, 
                const string& outputFileName)
{
   cd->setCacheMode(FileInterface::oldMode);
   CDTime start(0,2,0);
   std::ofstream outfile(outputFileName.c_str(), ios::binary);

   ProgWindow* p = new ProgWindow("Decompressing...");

	// write out every frame in the file
	while (start < cd->getCDLength())
	{
		p->update((float)start.getAbsoluteFrame() / (float)cd->getCDLength().getAbsoluteFrame());
		cd->seek(start);
		outfile.write((char*)cd->getBuffer(), bytesPerFrame);
		start += CDTime(0,0,1);
   }

   outfile.close();
   delete cd;
   delete p;

      moobyMessage("Done");
}

// callback for bzcompress
void bzCompress(Fl_Button*, void*)
{
   char * returned;
   if ( (returned = moobyFileChooser("Choose a file to compress in bz.index format", theUsualSuspects.c_str())) == NULL)
   {
      return;
   }
   string outputFileName(returned);
   outputFileName += ".bz";
   string indexFileName(outputFileName + string(".index"));
   FileInterface* cd = new UncompressedFileInterface(1);
   CompressedFileInterface* zipCD = new BZIndexFileInterface(1);
   cd->openFile(returned);
   compressIt(cd, zipCD, outputFileName, indexFileName);
}

// callback for bzdecompress
void bzDecompress(Fl_Button*, void*)
{
   char * returned;
   if ( (returned = moobyFileChooser("Choose a .bz file to decompress", "*.bz")) == NULL)
   {
      return;
   }
   FileInterface* cd = new BZIndexFileInterface(1);
   string theFile(returned);
   cd->openFile(theFile);
   string outputFileName(theFile);
   outputFileName.erase(theFile.rfind(".bz"));
   decompressIt(cd, outputFileName);
}

// callback for zcompress
void zCompress(Fl_Button*, void*)
{
   char * returned;
   if ( (returned = moobyFileChooser("Choose a file to compress in Z.table format", theUsualSuspects.c_str())) == NULL)
   {
      return;
   }
   string outputFileName(returned);
   outputFileName += ".Z";
   string indexFileName(outputFileName + string(".table"));
   FileInterface* cd = new UncompressedFileInterface(1);
   CompressedFileInterface* zipCD = new ZTableFileInterface(1);
   cd->openFile(returned);
   compressIt(cd, zipCD, outputFileName, indexFileName);
}

// callback for zdecompress
void zDecompress(Fl_Button*, void*)
{
   char * returned;
   if ( (returned = moobyFileChooser("Choose a .Z file to decompress", "*.Z")) == NULL)
   {
      return;
   }
   FileInterface* cd = new ZTableFileInterface(1);
   string theFile(returned);
   cd->openFile(theFile);
   string outputFileName(theFile);
   outputFileName.erase(theFile.rfind(".Z"));
   decompressIt(cd, outputFileName);
}

// callback for cddavolume
void CDDAVolume(Fl_Value_Slider* slider, void*)
{
   ostringstream ost;
   ost << slider->value();
   prefs.prefsMap[volumeString] = ost.str();
   prefs.write();
}

// callback for repeat
void repeatAllCDDA(Fl_Check_Button* button, void* val)
{
   if (button == NULL)
   {
      ((Fl_Check_Button*)val)->value(0);
   }
   else
   {
      button->value(1);
      prefs.prefsMap[repeatString] = std::string(repeatAllString);
      prefs.write();
      repeatOneCDDA(NULL, ((ConfigWindow*)val)->repeatOneButton);
      playOneCDDA(NULL, ((ConfigWindow*)val)->playOneButton);
   }
}

// callback for repeat
void repeatOneCDDA(Fl_Check_Button* button, void* val)
{
   if (button == NULL)
   {
      ((Fl_Check_Button*)val)->value(0);
   }
   else
   {
      button->value(1);
      prefs.prefsMap[repeatString] = std::string(repeatOneString);
      prefs.write();
      repeatAllCDDA(NULL, ((ConfigWindow*)val)->repeatAllButton);
      playOneCDDA(NULL, ((ConfigWindow*)val)->playOneButton);
   }
}

// callback for repeat
void playOneCDDA(Fl_Check_Button* button, void* val)
{
   if (button == NULL)
   {
      ((Fl_Check_Button*)val)->value(0);
   }
   else
   {
      button->value(1);
      prefs.prefsMap[repeatString] = std::string(playOneString);
      prefs.write();
      repeatAllCDDA(NULL, ((ConfigWindow*)val)->repeatAllButton);
      repeatOneCDDA(NULL, ((ConfigWindow*)val)->repeatOneButton);
   }
}

// callback for selecting an image for autorun
void chooseAutorunImage(Fl_Button* button, void* val)
{
   char *returned = NULL;
   bool done = false;
   while (!done)
   {
      returned = moobyFileChooser("Choose an image to run", theUsualSuspects.c_str(), prefs.prefsMap[lastrunString]);
      if (returned == NULL)
      {
         if (moobyAsk("You hit cancel or didn't pick a file.\nPick a different file?") == 0)
         {
            done = true;
         }
      }
      else
      {
         done = true;
      }
   }
   if (returned != NULL)
   {
      prefs.prefsMap[autorunString] = std::string(returned);
   }
   ((ConfigWindow*)val)->autorunBox->label(prefs.prefsMap[autorunString].c_str());
}

void clearAutorunImage(Fl_Button* button, void* val)
{
   prefs.prefsMap[autorunString] = std::string();
   ((ConfigWindow*)val)->autorunBox->label("No autorun image selected");   
}


// callback for OK
void configOK(Fl_Return_Button* button, void* val)
{
   //delete ((ConfigWindow*)val)->w;
   ((ConfigWindow*)val)->w->hide();
}

// callback for cachesize
void cacheSize(Fl_Value_Slider* slider, void*)
{
   ostringstream ost;
   int tmp=(int)(slider->value()*446);
   ost << tmp;
   prefs.prefsMap[cacheSizeString] = ost.str();
   prefs.write();
}

// callback for new caching button
void newCaching(Fl_Check_Button* button, void*)
{
   if (button->value() == 1)
      prefs.prefsMap[cachingModeString] = std::string(newCachingString);
   else
      prefs.prefsMap[cachingModeString] = std::string(oldCachingString);
   prefs.write();
}

// callback for enable/disable subchannel
void subEnable(Fl_Check_Button* button, void*)
{
   if (button->value() == 1)
      prefs.prefsMap[subEnableString] = std::string("1");
   else
      prefs.prefsMap[subEnableString] = std::string();
   prefs.write();
}
