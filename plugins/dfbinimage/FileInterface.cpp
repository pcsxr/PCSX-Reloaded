/************************************************************************

FileInterface.cpp

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
#include "TrackParser.hpp"
#include "Utils.hpp"
#include "Preferences.hpp"

#include <sstream>

#include <stdio.h>
#include <bzlib.h>
#include <zlib.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/cdrom.h>


extern Preferences prefs;

FileInterface::FileInterface(const unsigned long requestedFrames, 
      const unsigned long requiredFrames)
   : bufferFrames(0), fileBuffer(NULL), pregapLength(CDTime(0,0,0)), 
     pregapTime(CDTime(99,59,74)), cacheMode(oldMode)
{
   cache.setMaxSize(atoi(prefs.prefsMap[cacheSizeString].c_str()));
   if (requiredFrames != 0)
   {
      bufferFrames = (requestedFrames < requiredFrames) ? requiredFrames : requestedFrames;
      fileBuffer = new unsigned char[bufferFrames * bytesPerFrame];
   }
      // set the holdout size
   if (prefs.prefsMap[cachingModeString] == newCachingString)
      cacheMode = newMode;
   else if (prefs.prefsMap[cachingModeString] == oldCachingString)
      cacheMode = oldMode;
}


// given a file name, return the extension of the file and
// return the correct FileInterface for that file type
FileInterface* FileInterfaceFactory(const std::string& filename,
                                    string& extension)
{
   // for every file type that's supported, try to match the extension
   FileInterface* image;
   struct stat statBuf;

   if (extensionMatches(filename, ".bz.index"))
   {
      extension = filename.substr(filename.size() - string(".bz.index").size());
      extension = filename.substr(filename.rfind(".", filename.rfind(extension) - 1));
      image = new BZIndexFileInterface(1);
      image->openFile(std::string(filename).erase(filename.rfind(".index")));
      
   }
   else if (extensionMatches(filename, ".bz"))
   {
      extension = filename.substr(filename.size() - string(".bz").size());
      extension = filename.substr(filename.rfind(".", filename.rfind(extension) - 1));
      image = new BZIndexFileInterface(1);
      image->openFile(filename);
   }
   else if (extensionMatches(filename, ".Z.table"))
   {
      extension = filename.substr(filename.size() - string(".Z.table").size());
      extension = filename.substr(filename.rfind(".", filename.rfind(extension) - 1));
      image = new ZTableFileInterface(1);
      image->openFile(std::string(filename).erase(filename.rfind(".table")));
   }
   else if (extensionMatches(filename, ".Z"))
   {
      extension = filename.substr(filename.size() - string(".Z").size());
      extension = filename.substr(filename.rfind(".", filename.rfind(extension) - 1));
      image = new ZTableFileInterface(1);
      image->openFile(filename);
   }
   else if (extensionMatches(filename, ".ccd"))
   {
      moobyMessage("Please open the image and not the ccd file.");
      image = new UncompressedFileInterface(1);
      extension = filename.substr(filename.size() - string(".ccd").size());
      image->openFile(filename.substr(filename.size() - string(".ccd").size()) 
         + string(".img"));
   }

		// the CUE interface will take the name of the file
		// in the cue sheet and open it as an uncompressed file
   else if (extensionMatches(filename, ".cue"))
   {
      moobyMessage("Please open the image and not the cue sheet.");
      extension = filename.substr(filename.size() - string(".cue").size());
      CueParser cp(filename);
      cp.parse();
      image = new UncompressedFileInterface(1);

         // try to figure out the directory of the image.
         // if none is in the cue sheet,
         // just use the directory of filename.
      std::string::size_type pos = cp.getCDName().rfind('/');
      if (pos == std::string::npos)
      {
         pos = cp.getCDName().rfind('\\');
      }
      if (pos == std::string::npos)
      {
         pos = filename.rfind('/');
         if (pos == std::string::npos)
            pos = filename.rfind('\\');
         image->openFile(std::string(filename).erase(pos + 1) + cp.getCDName());
      }
      else
      {
         image->openFile(cp.getCDName());
      }
   }

   else if(stat(filename.c_str(), &statBuf)>=0 && S_ISBLK(statBuf.st_mode)) {
      extension = "";
      image = new DeviceFileInterface(1);
      image->openFile(std::string(filename));
   }

		// all other file types that aren't directly supported, 
		// try to open them with UncompressedFileInterface
   else
   {
      if (extensionMatches(filename, ".iso"))
      {
         moobyMessage("This plugin does not support ISO-9660 images. "
            "If this is a binary image, rename it with a \".bin\" extension.");
      }

      extension = filename.substr(filename.find_last_of('.'));
      image = new UncompressedFileInterface(1);
      image->openFile(filename);
   }

   if (image)
      return image;
   else
   {
      Exception e(string("Error opening file: ") + filename);
      THROW(e);
   }
}

FileInterface& FileInterface::setPregap(const CDTime& gapLength,
                                        const CDTime& gapTime)
{ 
   if (pregapLength == CDTime(0,0,0))
   {
      pregapLength = gapLength; 
      pregapTime = gapTime; 
      CDLength += gapLength; 
   }
   return *this; 
}

// opens the file and calculates the length of the cd
void FileInterface::openFile(const std::string& str)
      throw(Exception)
{
   file.open(str.c_str(), std::ios::binary);
   if (!file) 
   {
      Exception e(std::string("Cannot open file: ") + str);
      THROW(e);
   }
   fileName = str;
   CDLength= CDTime(file.seekg(0, std::ios::end).tellg(), CDTime::abByte) + CDTime(0,2,0);
   file.clear();
   bufferPos.setMSF(MSFTime(255,255,255));
}

// seekUnbuffered for the .index/.table files.
void CompressedFileInterface::seekUnbuffered(const CDTime& cdt)
throw(std::exception, Exception)
{
		// calculate the index in the lookupTable where the data for CDTime cdt
		// should be located
   unsigned long requestedFrame = cdt.getAbsoluteFrame() - 150;
   unsigned long cdtime = requestedFrame / compressedFrames;
   if ((cdtime + 1) >= lookupTable.size())
   {
      Exception e("Seek past end of compressed index");
      THROW(e);
   }
   unsigned long seekStart = lookupTable[cdtime];
   unsigned long seekEnd = lookupTable[cdtime + 1];

		// read and decompress that data
   file.clear();
   file.seekg(seekStart, ios::beg);
   file.read((char*)compressedDataBuffer, seekEnd - seekStart);
   unsigned int destLen = compressedFrames * bytesPerFrame;

   decompressData ((char*)fileBuffer,
                   (char*)compressedDataBuffer,
                   seekEnd - seekStart,
                   destLen);

		// set the buffer pointer - make sure to check if
		// the requested frame doesn't line up with how the file is compressed
   bufferPointer = fileBuffer + (requestedFrame % compressedFrames) * bytesPerFrame;

		// set the buffer start and end times
   bufferPos = CDTime(cdtime * compressedFrames + 150, CDTime::abFrame);
   bufferEnd = CDTime(cdtime * compressedFrames + compressedFrames + 150, CDTime::abFrame);
}

void ZTableFileInterface::openFile(const std::string& str)
   throw(Exception)
{
   // open the z file
   FileInterface::openFile(str);

   // open the table file and read it 
   string indexFileName = str + string(".table");
   ifstream indexFile(indexFileName.c_str(), ios::binary);
   if (!indexFile)
   {
      Exception e(string("Cannot open file: ") + indexFileName);
      THROW(e);
   }

		// a Z.table file is binary where each element represents
		// one compressed frame.  
		//    4 bytes: the offset of the frame in the .Z file
		//    2 bytes: the length of the compressed frame
		// for every frame in the file.  internally, this plugin only
		// uses the offsets, but it needs the last element of the lookupTable
		// to be the entire CD length.  
		// The data is stored little endian, so flipBits will swap the
		// bits if it's not a little endian system
   unsigned long offset;
   short length;

   indexFile.read((char*)&offset, 4);
   indexFile.read((char*)&length, 2);

   while (indexFile)
   {
      flipBits(offset);
      flipBits(length);
      lookupTable.push_back(offset);
      indexFile.read((char*)&offset, 4);
      indexFile.read((char*)&length, 2);
   }

   lookupTable.push_back(offset + length);

		// seek to the end of the CD and use the bufferEnd time as the
		// CDLength
   seekUnbuffered(CDTime((lookupTable.size() - 2) * compressedFrames, CDTime::abFrame) + 
      CDTime(0,2,0));
   CDLength = bufferEnd;
}

// compresses the data in uncompressedData
void ZTableFileInterface::compressData(char* uncompressedData,
                             char* compressedData,
                             unsigned int inputLen,
                             unsigned int& outputLen)
   throw(Exception)
{
   int rc;
   if ( ( rc = compress( (unsigned char*)compressedData,
             (unsigned long*)&outputLen,
             (unsigned char*)uncompressedData,
             inputLen) ) != 0)
   {
      Exception e("ZDecompress error");
      THROW(e);
   }
}

// decompresses the data in compressedData
void ZTableFileInterface::decompressData(char* uncompressedData,
                                          char* compressedData,
                                          unsigned int inputLen,
                                          unsigned int& outputLen) 
   throw(Exception)
{
   int rc;
   if ( (rc = uncompress ( (unsigned char*)uncompressedData,
                           (unsigned long*)&outputLen,
                           (unsigned char*)compressedData,
                           inputLen) ) != 0)
   {
      Exception e("ZDecompress error");
      THROW(e);
   }
}

// table is a table of file positions, each element representing
// the position for that compressed frame.
// sizes is a table of the length of the compressed frame.
// this returns the data for the .Z.table file 
string ZTableFileInterface::toTable(const vector<unsigned long>& table,
                                    const vector<unsigned long>& sizes)
{
   string toReturn;
   vector<unsigned long>::size_type s;
   for (s = 0; s < table.size(); s++)
   {
      unsigned long aNumber= table[s];
      flipBits(aNumber);
      toReturn += string((char*)&aNumber, 4);
      short length = sizes[s];
      flipBits(length);
      toReturn += string((char*)&length, 2);
   }
   return toReturn;
}

// opens a BZ file
void BZIndexFileInterface::openFile(const std::string& str)
   throw(Exception)
{
   // open the bz file
   FileInterface::openFile(str);

   // open the index file and read it into the table
   string indexFileName = str + string(".index");

	ifstream indexFile(indexFileName.c_str(), ios::binary);
		if (!indexFile)
		{
			Exception e(string("Cannot open file: ") + indexFileName);
			THROW(e);
		}

   unsigned long offset;
	
	// the .BZ.table file is arranged so that one entry represents
	// 10 compressed frames.  that's because bzip only works well if
	// you have a large amount of data to compress.  each element in the 
	// table is a 4 byte unsigned integer representing the offset in
	// the .BZ file of that set of 10 compressed frames.  If there are
	// 'n' frames in the uncompressed image, there are 'n/10 + 1' entries
	// in the BZ.index file, where the last entry is the size of the
	// compressed file.

   indexFile.read((char*)&offset, 4);
   
   while (indexFile)
   {
      // if it's big endian, the bits need to be reversed
      flipBits(offset);

      lookupTable.push_back(offset);
      indexFile.read((char*)&offset, 4);
   }

		// to get the CD length, seek to the end of file and decompress the last frame
   seekUnbuffered(CDTime((lookupTable.size() - 2) * compressedFrames, CDTime::abFrame) + 
      CDTime(0,2,0));
   CDLength = bufferEnd;
}

// compresses uncompressedData
void BZIndexFileInterface::compressData(char* uncompressedData,
                             char* compressedData,
                             unsigned int inputLen,
                             unsigned int& outputLen)
   throw(Exception)
{
   int rc;
   if ( (rc = BZ2_bzBuffToBuffCompress( compressedData,
                                      &outputLen,
                                      uncompressedData,
                                      inputLen,
                                      1,
                                      0,
                                      30 ) ) != BZ_OK)
   {
      Exception e("BZCompress error");
      THROW(e);
   }
}

// decompresses compressedData
void BZIndexFileInterface::decompressData(char* uncompressedData,
                                          char* compressedData,
                                          unsigned int inputLen,
                                          unsigned int& outputLen) 
   throw(Exception)
{
   int rc;
   if ( (rc = BZ2_bzBuffToBuffDecompress ( uncompressedData,
                                           &outputLen,
                                           compressedData,
                                           inputLen,
                                           0,
                                           0) ) != BZ_OK)
   {
      Exception e("BZDecompress error");
      THROW(e);
   }
}

// writes the .BZ.index file
string BZIndexFileInterface::toTable(const vector<unsigned long>& table,
                                     const vector<unsigned long>& sizes)
{
   string toReturn;
   vector<unsigned long>::size_type s;
   unsigned long aNumber;
   for (s = 0; s < table.size(); s++)
   {
      aNumber= table[s];
      flipBits(aNumber);
      toReturn += string((char*)&aNumber, 4);
   }
   aNumber = table[table.size() - 1];
   aNumber += sizes[sizes.size() - 1];
   flipBits(aNumber);
   toReturn += string((char*)&aNumber, 4);

   return toReturn;
}


// reads data into the cache for UncompressedFileInterface
void UncompressedFileInterface::seekUnbuffered(const CDTime& cdt)
   throw(std::exception, Exception)
{
   file.clear();
   CDTime seekTime(cdt - CDTime(0,2,0));
   file.seekg(seekTime.getAbsoluteByte(), ios::beg);
   file.read((char*)fileBuffer, bufferFrames * bytesPerFrame);
   bufferPointer = fileBuffer;
   bufferPos = cdt;
   bufferEnd = cdt + CDTime(bufferFrames, CDTime::abFrame);
}


// opens a device node.
void DeviceFileInterface::openFile(const std::string& str)
      throw(Exception)
{
   if(theFd >= 0)
      close(theFd);
   theFd = open(str.c_str(), O_RDONLY);
   if(theFd < 0) {
      Exception e(std::string("Cannot open device: ") + str);
      THROW(e);
   }
   fileName = str;
   struct cdrom_tocentry tocEntry;
   memset(&tocEntry, 0, sizeof(tocEntry));
   tocEntry.cdte_track = CDROM_LEADOUT;
   tocEntry.cdte_format = CDROM_LBA;
   if(ioctl(theFd, CDROMREADTOCENTRY, &tocEntry) < 0) {
      Exception e(std::string("Cannot read TOC: ") + str);
      THROW(e);
   }
   CDLength= CDTime(tocEntry.cdte_addr.lba, CDTime::abFrame) + CDTime(0,2,0);
   bufferPos.setMSF(MSFTime(255,255,255));
}

// reads data into the cache for DeviceFileInterface
void DeviceFileInterface::seekUnbuffered(const CDTime& cdt)
   throw(std::exception, Exception)
{
   CDTime seekTime(cdt);
   char *buf = (char*)fileBuffer;
   int cnt;
   for(cnt = 0; cnt < bufferFrames; cnt++) {
     memcpy(buf, seekTime.getMSFbuf(), 3);
     if(ioctl(theFd, CDROMREADRAW, buf)<0)
       break;
     buf += bytesPerFrame;
     seekTime += CDTime(0,0,1);
   }
   bufferPointer = fileBuffer;
   bufferPos = cdt;
   bufferEnd = cdt + CDTime(cnt, CDTime::abFrame);
}

DeviceFileInterface::~DeviceFileInterface()
{
   if(theFd >= 0)
      close(theFd);
}
