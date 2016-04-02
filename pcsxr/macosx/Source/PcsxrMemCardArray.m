//
//  PcsxrMemCardArray.m
//  Pcsxr
//
//  Created by C.W. Betts on 7/6/13.
//
//

#import "PcsxrMemCardArray.h"
#import "ConfigurationController.h"
#include "sio.h"

#define MAX_MEMCARD_BLOCKS 15
#define ISLINKMIDBLOCK(Info) (((Info)->Flags & 0xF) == 0x2)
#define ISLINKENDBLOCK(Info) (((Info)->Flags & 0xF) == 0x3)
#define ISLINKBLOCK(Info) (ISLINKENDBLOCK((Info)) || ISLINKMIDBLOCK((Info)))
#define ISDELETED(Info) (((Info)->Flags & 0xF) >= 1 && ((Info)->Flags & 0xF) <= 3)
#define ISBLOCKDELETED(Info) (((Info)->Flags & 0xF0) == 0xA0)
#define ISSTATUSDELETED(Info) (ISBLOCKDELETED(Info) && ISDELETED(Info))
#define ISLINKED(Data) ( ((Data) != 0xFFFFU) && ((Data) <= MAX_MEMCARD_BLOCKS) )
#define GETLINKFORBLOCK(Data, block) (*((Data)+(((block)*128)+0x08)))


static int GetMcdBlockCount(int mcd, u8 startblock, u8* blocks) {
	NSCAssert1((mcd == 1) || (mcd == 2), @"Mcd block %i is invalid", mcd);
	int i=0;
	u8 *data = NULL, *dataT, curblock=startblock;
	u16 linkblock;
	
	if (mcd == 1) {
		data = Mcd1Data;
	} else if (mcd == 2) {
		data = Mcd2Data;
	} else {
		return 0;
	}
	
	blocks[i++] = startblock;
	do {
		dataT = data+((curblock*128)+0x08);
		linkblock = ((u16*)dataT)[0];
		
		// TODO check if target block has link flag (2 or 3)
		linkblock = ( ISLINKED(linkblock) ? linkblock : 0xFFFFU );
		blocks[i++] = curblock = linkblock + 1;
		//printf("LINKS %x %x %x %x %x\n", blocks[0], blocks[i-2], blocks[i-1], blocks[i], blocks[i+1]);
	} while (ISLINKED(linkblock));
	return i-1;
}


static inline void CopyMemcardData(char *from, char *to, int srci, int dsti, char *str)
{
	// header
	memmove(to + (dsti + 1) * 128, from + (srci + 1) * 128, 128);
	SaveMcd(str, to, (dsti + 1) * 128, 128);
	
	// data
	memmove(to + (dsti + 1) * 1024 * 8, from + (srci+1) * 1024 * 8, 1024 * 8);
	SaveMcd(str, to, (dsti + 1) * 1024 * 8, 1024 * 8);
}

static inline char* BlankHeader()
{
	struct PSXMemHeader {
		unsigned int allocState;
		unsigned int fileSize;
		unsigned short nextBlock;
		char fileName[21];
		unsigned char garbage[96];
		unsigned char checksum;
	};
	
	static struct PSXMemHeader *toReturn = NULL;
	if (!toReturn) {
		toReturn = calloc(sizeof(struct PSXMemHeader), 1);
		
		//FIXME: Which value is right?
		toReturn->allocState = 0x000000a0;
		//toReturn->allocState = 0xa0000000;
		toReturn->nextBlock = 0xFFFF;
		unsigned char *bytePtr = (unsigned char*)toReturn;
		for (int i = 0; i < sizeof(struct PSXMemHeader) - sizeof(unsigned char); i++) {
			toReturn->checksum ^= bytePtr[i];
		}
	}
	
	return (char*)toReturn;
}

static inline void ClearMemcardData(char *to, int dsti, char *str)
{
	// header
	char *header = BlankHeader();
	memcpy(to + (dsti + 1) * 128, header, 128);
	SaveMcd(str, to, (dsti + 1) * 128, 128);
	
	// data
	memset(to + (dsti + 1) * 1024 * 8, 0, 1024 * 8);
	SaveMcd(str, to, (dsti + 1) * 1024 * 8, 1024 * 8);
}

@interface PcsxrMemCardArray ()
@property (strong) NSArray *rawArray;
@property (readonly) char* memDataPtr;
@property int cardNumber;
@end

@implementation PcsxrMemCardArray
@synthesize rawArray;
@synthesize cardNumber;

- (char*)memDataPtr
{
	if (cardNumber == 1) {
		return Mcd1Data;
	} else {
		return Mcd2Data;
	}
}

- (const char *)memCardCPath
{
	if (cardNumber == 1) {
		return Config.Mcd1;
	} else {
		return Config.Mcd2;
	}
}

- (instancetype)initWithMemoryCardNumber:(int)carNum
{
	NSParameterAssert(carNum == 1 || carNum == 2);
	if (self = [super init]) {
		NSMutableArray *tmpMemArray = [[NSMutableArray alloc] initWithCapacity:MAX_MEMCARD_BLOCKS];
		cardNumber = carNum;
		int i = 0, x;
		unsigned char cardNums[MAX_MEMCARD_BLOCKS+1];
		BOOL populated[MAX_MEMCARD_BLOCKS] = {0};
		while (i < MAX_MEMCARD_BLOCKS) {
			if (populated[i]) {
				i += 1;
				continue;
			}
			x = 1;
			McdBlock memBlock;
			GetMcdBlockInfo(carNum, i + 1, &memBlock);
			
			// ignore Free space
			if ([PcsxrMemoryObject memFlagsFromBlockFlags:memBlock.Flags] == PCSXRMemFlagFree/* ||
				[PcsxrMemoryObject memFlagsFromBlockFlags:memBlock.Flags] == PCSXRMemFlagLink ||
				[PcsxrMemoryObject memFlagsFromBlockFlags:memBlock.Flags] == PCSXRMemFlagEndLink*/) {
				i++;
				continue;
			}
			@autoreleasepool {
				int idxCount = GetMcdBlockCount(carNum, i+1, cardNums);
				NSMutableIndexSet *cardIdx = [[NSMutableIndexSet alloc] init];
				for (int idxidx = 0; idxidx < idxCount; idxidx++) {
					[cardIdx addIndex:cardNums[idxidx] - 1];
					populated[cardNums[idxidx] - 1] = YES;
				}
				PcsxrMemoryObject *obj = [[PcsxrMemoryObject alloc] initWithMcdBlock:&memBlock blockIndexes:cardIdx];
				[tmpMemArray addObject:obj];
			}
			i += 1;
		}
		self.rawArray = [[NSArray alloc] initWithArray:tmpMemArray];
	}
	return self;
}

- (int)indexOfFreeBlocksWithSize:(int)asize
{
	int foundcount = 0, i = 0;
	
	McdBlock obj;
	// search for empty (formatted) blocks first
	while (i < MAX_MEMCARD_BLOCKS && foundcount < asize) {
		GetMcdBlockInfo(cardNumber, 1 + i++, &obj);
		//&Blocks[target_card][++i];
		if ((obj.Flags & 0xFF) == 0xA0) { // if A0 but not A1
			foundcount++;
		} else if (foundcount >= 1) { // need to find n count consecutive blocks
			foundcount = 0;
		} else {
			//i++;
		}
		//printf("formatstatus=%x\n", Info->Flags);
 	}
	
	if (foundcount == asize)
		return (i-foundcount);
	
	// no free formatted slots, try to find a deleted one
	foundcount = i = 0;
	while (i < MAX_MEMCARD_BLOCKS && foundcount < asize) {
		GetMcdBlockInfo(cardNumber, 1 + i++, &obj);
		if ((obj.Flags & 0xF0) == 0xA0) { // A2 or A6 f.e.
			foundcount++;
		} else if (foundcount >= 1) { // need to find n count consecutive blocks
			foundcount = 0;
		} else {
			//i++;
		}
		//printf("delstatus=%x\n", Info->Flags);
 	}
	
	if (foundcount == asize)
		return (i-foundcount);
	
 	return -1;
}

- (BOOL)moveBlockAtIndex:(int)idx toMemoryCard:(PcsxrMemCardArray*)otherCard
{
	if (idx == [rawArray count]) {
#ifdef DEBUG
		NSLog(@"Trying to get an object one more than the length of the raw array. Perhaps you were trying to \"move\" the free blocks. We don't want to do this.");
#endif
		return NO;
	}
	PcsxrMemoryObject *tmpObj = rawArray[idx];

	int memSize = tmpObj.blockSize;
	
	if ([otherCard availableBlocks] < memSize) {
		NSLog(@"Failing because the other card does not have enough space!");
		return NO;
	}
	
	int toCopy = [otherCard indexOfFreeBlocksWithSize:memSize];
	if (toCopy == -1) {
		NSLog(@"Not enough consecutive blocks. Compacting the other card.");
		[otherCard compactMemory];
		//Since we're accessing the mem card data directly (instead of via PcsxrMemoryObject objects) using the following calls, we don't need to reload the data.
		toCopy = [otherCard indexOfFreeBlocksWithSize:memSize];
		NSAssert(toCopy != -1, @"Compacting the card should have made space!");
	}
	
	NSIndexSet *memIdxs = tmpObj.indexes;
	int i = 0;
	for (NSInteger memIdx = memIdxs.firstIndex; memIdx != NSNotFound; memIdx = [memIdxs indexGreaterThanIndex:memIdx]) {
		CopyMemcardData([self memDataPtr], [otherCard memDataPtr], (int)memIdx, toCopy + i++, (char*)otherCard.memCardCPath);
	}
	return YES;
}

- (int)freeBlocks
{
	int memSize = MAX_MEMCARD_BLOCKS;
	for (PcsxrMemoryObject *memObj in rawArray) {
		memSize -= memObj.blockSize;
	}
	return memSize;
}

- (int)availableBlocks
{
	int memSize = MAX_MEMCARD_BLOCKS;
	for (PcsxrMemoryObject *memObj in rawArray) {
		if (memObj.flag != PCSXRMemFlagDeleted) {
			memSize -= memObj.blockSize;
		}
	}
	return memSize;
}

- (NSArray*)memoryArray
{
	int freeSize = [self freeBlocks];
	
	if (freeSize) {
		McdBlock theBlock;
		//Create a blank "block" that will be used to show the amount of free blocks
		theBlock.Flags = 0xA0;
		theBlock.IconCount = 0;
		PcsxrMemoryObject *freeObj = [[PcsxrMemoryObject alloc] initWithMcdBlock:&theBlock startingIndex:MAX_MEMCARD_BLOCKS - 1 - freeSize size:freeSize];
		return [rawArray arrayByAddingObject:freeObj];
	} else
		return rawArray;
}

- (NSURL*)memCardURL
{
	if (cardNumber == 1) {
		return [[NSUserDefaults standardUserDefaults] URLForKey:@"Mcd1"];
	} else {
		return [[NSUserDefaults standardUserDefaults] URLForKey:@"Mcd2"];
	}
}

- (int)memorySizeAtIndex:(int)idx
{
	if (idx == [rawArray count]) {
#ifdef DEBUG
		NSLog(@"Trying to get an object one more than the length of the raw array. Perhaps you were trying to \"count\" the free blocks?");
#endif
		return [self freeBlocks];
	}

	return [rawArray[idx] blockSize];
}

- (void)compactMemory
{
	int i = 0, x = 1;
	while (i < MAX_MEMCARD_BLOCKS && x < MAX_MEMCARD_BLOCKS) {
		x = i;
		McdBlock baseBlock;
		GetMcdBlockInfo(cardNumber, i+1, &baseBlock);
		PCSXRMemFlag theFlags = [PcsxrMemoryObject memFlagsFromBlockFlags:baseBlock.Flags];
		
		if (theFlags == PCSXRMemFlagDeleted || theFlags == PCSXRMemFlagFree) {
			PCSXRMemFlag up1Flags = theFlags;
			while ((up1Flags == PCSXRMemFlagDeleted || up1Flags == PCSXRMemFlagFree) && x < MAX_MEMCARD_BLOCKS) {
				x++;
				McdBlock up1Block;
				GetMcdBlockInfo(cardNumber, x+1, &up1Block);
				up1Flags = [PcsxrMemoryObject memFlagsFromBlockFlags:up1Block.Flags];
			}
			if (x >= MAX_MEMCARD_BLOCKS) {
				break;
			}
			
			CopyMemcardData(self.memDataPtr, self.memDataPtr, x, i, (char*)[[self.memCardURL path] fileSystemRepresentation]);
			ClearMemcardData(self.memDataPtr, x, (char*)self.memCardCPath);
		}
		i++;
	}
	
	while (i < MAX_MEMCARD_BLOCKS) {
		ClearMemcardData(self.memDataPtr, i, (char*)self.memCardCPath);
		i++;
	}
	
	LoadMcd(cardNumber, (char*)self.memCardCPath);
}

- (void)deleteMemoryBlocksAtIndex:(int)slotnum
{
	char *data, *ptr, *filename;
	if (cardNumber == 1) {
		filename = Config.Mcd1;
		data = Mcd1Data;
	} else {
		filename = Config.Mcd2;
		data = Mcd2Data;
	}
	
	if (slotnum == [rawArray count]) {
#ifdef DEBUG
		NSLog(@"Trying to get an object one more than the length of the raw array. Perhaps you were trying to \"delete\" the free blocks?");
#endif
		return;
	}
	
	PcsxrMemoryObject *theObj = rawArray[slotnum];
	
	McdBlock flagBlock;
	
	for (NSInteger i = theObj.indexes.firstIndex; i != NSNotFound; i = [theObj.indexes indexGreaterThanIndex:i])
	{
		char xor = 0;
		GetMcdBlockInfo(cardNumber, (int)i, &flagBlock);
		ptr = data + i * 128;
		
		if ((flagBlock.Flags & 0xF0) == 0xA0) {
			if ((flagBlock.Flags & 0xF) >= 1 &&
				(flagBlock.Flags & 0xF) <= 3) { // deleted
				*ptr = 0x50 | (flagBlock.Flags & 0xF);
			} else return;
		} else if ((flagBlock.Flags & 0xF0) == 0x50) { // used
			*ptr = 0xA0 | (flagBlock.Flags & 0xF);
		} else { continue; }
		
		for (unsigned char j = 0; j < 127; j++) xor ^= *ptr++;
		*ptr = xor;
		
		SaveMcd(filename, data, (int)(i * 128), 128);
	}
}

@end
