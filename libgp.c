#include "stdafx.h"
#include <string.h>
#include <stdlib.h>

unsigned int readInt(FILE* f) {
	char buf[4] = {0, 0, 0, 0};
	unsigned int val;
	fread(buf,sizeof(int), 1, f);
	val = ((buf[3] & 0xff) << 24) | ((buf[2] & 0xff) << 16)| ((buf[1] & 0xff) << 8) | (buf[0] & 0xff);
	printf("readInt: %i\n", val);
	return val;
}

char* readString(FILE* f, int length) {
	int i;

	char* str = malloc(length);
	fread(str, length, 1, f);
	if(length > 0) printf("%s\n", str);
	return str;
}

char* readStringInteger(FILE* f)  {
	int length = readInt(f);
	return readString(f, length);
}

char* readStringIntegerPlusOne(FILE* f) {
	int lengthPlusOne = readInt(f);
	int length = lengthPlusOne - 1;
	if(lengthPlusOne > 0) {
		int r = getc(f);
		if(length != r) {
			printf("Wrong length? %d %d", length, lengthPlusOne);
		}
		return readString(f, length);
	}
	return "";

}

char readByte(FILE* f) {
	char b;
	b = getc(f);
	printf("readByte: %i\n", b);
	return b;
}

unsigned char readUnsignedByte(FILE* f) {
	unsigned char b;
	b = getc(f);
	printf("readUnsignedByte: %i\n", b);
	return b;
}


char* readStringByte(FILE* f, int expectedLength) {
	int realLength = readUnsignedByte(f);
	return readString(f, expectedLength);
}


char* readVersion(FILE* f) {
	int realLength = getc(f);
	char version[30];
	fread(version, 30, 1, f);
	printf("%s\n", version);
	return version;
}

void readMidiChannel(FILE* f) {
	int instrument = readInt(f);
	int volume = readByte(f);
	int balance = readByte(f);
	int chorus = readByte(f);
	int reverb = readByte(f);
	int phaser = readByte(f);
	int tremolo = readByte(f);
	char buf[2] = {0 , 0};
    fread(buf, 2, 1, f); // Backward compatibility with version 3.0
	printf("readMidiChannel instrument:%i volume:%i balance:%i chorus:%i reverb:%i phaser:%i tremolo:%i\n"
		, instrument, volume, balance, chorus, reverb, phaser,tremolo);
}

void readColor(FILE* f) {
	unsigned int red, green,blue,white;
	printf("readColor\n");
	red = readUnsignedByte(f);
	green = readUnsignedByte(f);
	blue = readUnsignedByte(f);
	white = readByte(f);
}

void readMarker(FILE* f) {
	char* name = readStringIntegerPlusOne(f);
	readColor(f);
}

void readTrack(FILE* f) {
	int header, isDrum, is12, isBanjo, numStrings, port, chan, chanEffect, numFrets, capo, i;
	char* name;
	printf("readTrack\n");
	header = readUnsignedByte(f);
    isDrum = ((header & 0x01) != 0);
    is12 = ((header & 0x02) != 0);
    isBanjo = ((header & 0x04) != 0);
	name  = readStringByte(f, 40);
	numStrings = readInt(f);
    for (i = 0; i < 7; i++) {
		int num = readInt(f);
		printf("Tuning: %i %i\n", i, num);
    }
	port = readInt(f);
	chan = readInt(f);
	chanEffect = readInt(f);
	numFrets = readInt(f);
    capo = readInt(f);
	readColor(f);
	printf("readTrack header:%i isDrum:%i is12:%i isBanjo:%i name:%s numStrings:%i port:%i chan:%i chanEffect:%i numFrets:%i capo:%i\n",
		header, isDrum, is12, isBanjo, name, numStrings, port, chan, chanEffect, numFrets, capo);
}

void readTonalityType(FILE* f, int numBytes) {
		printf("readTonalityType\n");
        if (numBytes == 1) {
			fgetc(f);
        } else if (numBytes == 4) {
            readInt(f);
        }
    }

void readChordDiagram(FILE* f) {
        int header;
        int i, sharp, root, chorType, nineElevenThirteen
		, bass, addednote, basefred, numBarres, showdiagr;
		char* name;

		printf("readChordDiagram\n");
        header = fgetc(f);

		sharp = fgetc(f);

		fseek(f, 3, SEEK_CUR); // Ignore 3 bytes

		root = fgetc(f);
		chorType = fgetc(f);
		nineElevenThirteen = fgetc(f);
		bass = readInt(f);

        readTonalityType(f,4);

		addednote = fgetc(f);

		name = readStringByte(f, 20);
		fseek(f, 2, SEEK_CUR);

        readTonalityType(f,1);
        readTonalityType(f,1);
        readTonalityType(f,1);

        basefred = readInt(f);

        for (i = 1; i <= 7; i++) {
			int fret = readInt(f);
        }

		numBarres = fgetc(f);

        for (i = 1; i <= 5; i++) {
			int fretBarre = fgetc(f);
        }
        for (i = 1; i <= 5; i++) {
            int barreStart = fgetc(f);
        }
        for (i = 1; i <= 5; i++) {
			int barreEnd = fgetc(f);
        }

        /*
         * SKIP THIS FIELDS Omission1, Omission3, Omission5, Omission7,
         * Omission9, Omission11, Omission13: Bytes Blank6: byte
         */
		fseek(f, 8, SEEK_CUR);

        // Fingering: List of 7 Bytes
        for (i = 1; i <= 7; i++) {
			int fingering = fgetc(f);
        }
        /*
         * ShowDiagFingering: byte
         */
		showdiagr = fgetc(f);
    }

void readBeats(FILE* f) {
	int header, beatStatus, emptyBeat, restBeat
	, dotted, duration;

	printf("readBeat\n");
	header = readUnsignedByte(f);

	if ((header & 0x40) != 0) {
		beatStatus = readUnsignedByte(f);
        emptyBeat = beatStatus == 0x00;
        restBeat = beatStatus == 0x02;
    }

        // Dotted notes
        dotted = ((header & 0x01) != 0);

        // Beat duration
		duration = readByte(f);

        // N-Tuplet
        if ((header & 0x20) != 0) {
            int ntuplet = readInt(f);
        }

        // Chord diagram
        if ((header & 0x02) != 0) {
            readChordDiagram(f);
        }

        // Text
        if ((header & 0x04) != 0) {
            char* text = readStringIntegerPlusOne(f);
        }

        // Effects on the beat
        if ((header & 0x08) != 0) {
            // Harmonics on a note cause this flag to be set
            //beat.effects = readEffectsOnBeat();
            // throw new IOException("Effects on the beat not handled yet");
        }

        // Mix table change
        if ((header & 0x10) != 0) {
            //beat.mixTableChange = readMixTableChange();
        }

        // Finds out which strings are played
        //int stringsPlayed = readUnsignedByte();
        //int numberOfStrings = 0;

        //for (int i = 0; i < 7; i++) {
        //    if ((stringsPlayed & (1 << i)) != 0) {
        //        numberOfStrings++;
        //        beat.setString(i, true);
        //    }
        //}

        //// Gets the corresponding notes
        //List notes = beat.getNotes();
        //for (int i = 0; i < numberOfStrings; i++) {
        //    GPNote note = readNote();
        //    notes.add(note);
        //}



}

void readMeasureTrackPair(FILE* f) {
	int numBeats, i;
	printf("readMeasureTrackPair\n");
	numBeats = readInt(f);
	for(i = 0; i < numBeats; i++) {
		readBeats(f);
	}
}

void readMeasure(FILE* f) {
	int header, numerator, denominator
		, repeatStart, numRepetition, numAlternate, marker
		, tonalityType, tonalityValue
		, hasDoubleBar;
	
	printf("readMeasure\n");
	header = readUnsignedByte(f);

	if ((header & 0x01) != 0) {
		 numerator = readByte(f);
    }

    if ((header & 0x02) != 0) {
         denominator = readByte(f);
    }

    repeatStart = ((header & 0x04) != 0);

    if ((header & 0x08) != 0) {
		numRepetition = readByte(f);
	}

    if ((header & 0x10) != 0) {
		numAlternate = readByte(f);
    }

    if ((header & 0x20) != 0) {
		readMarker(f);
    }

	if ((header & 0x40) != 0) {
		int type = readByte(f);
		int key = readByte(f);
	}

	hasDoubleBar = ((header & 0x80) != 0);
}

int _tmain(int argc, _TCHAR* argv[])
{
	int i, j;
	int tripletFeel, lyricsTrackNumber, tempo, type, octave, numberOfMeasures, numberOfTracks, numberOfNotes;
	char* version, title, subtitle, interprete, album, authorSong, copyright, autorPiece, instruction;
	FILE* f;

	// f = fopen("C:\\Documenti\\roba\\DGuitar-0.5.8\\DGuitar-0.5.8\\files\\ritmos varios.gp4", "r");
	f = fopen("C:\\Documenti\\roba\\The Tell-tale Heart.gp4", "rb");

	version = readVersion(f);
	title = readStringIntegerPlusOne(f);
	subtitle = readStringIntegerPlusOne(f);
	interprete = readStringIntegerPlusOne(f);
	album = readStringIntegerPlusOne(f);
	authorSong = readStringIntegerPlusOne(f);
	copyright = readStringIntegerPlusOne(f);
	autorPiece = readStringIntegerPlusOne(f);
	instruction = readStringIntegerPlusOne(f);

	numberOfNotes = readByte(f);
	for (i = 0; i < numberOfNotes; i++) {
		char* note = readStringIntegerPlusOne(f);
	}

	tripletFeel = readInt(f);

	lyricsTrackNumber = readInt(f);
	for (i = 0; i < 5; i++) {
		int measureNumber = readInt(f);
 		char* lyr = readStringInteger(f);
	}

	tempo = readInt(f);
	
	type = readByte(f);
	octave = readInt(f);

	for(i = 0; i < 64; i++) {
		readMidiChannel(f);
	}

	numberOfMeasures = readInt(f);
    numberOfTracks = readInt(f);

	if(numberOfMeasures > 0) {
		readMeasure(f);
		for (i = 1; i < numberOfMeasures; i++) {
            readMeasure(f);
        }
	}

	for (i = 0; i < numberOfTracks; i++) {
		readTrack(f);
    }

    for (i = 0; i < numberOfMeasures; i++) {
        for (j = 0; j < numberOfTracks; j++) {
            readMeasureTrackPair(f);
        }
    }
	fclose (f);
	return 0;
}
