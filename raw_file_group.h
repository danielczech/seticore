#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector> 

#include "raw_file.h"

using namespace std;

vector<vector<string> > scanForRawFileGroups(const string& directory);

/*
  The RawFileGroup represents a set of raw files. They share a prefix on disk, like
  /my/dir/blah-blorp-qux.0000.raw
  /my/dir/blah-blorp-qux.0001.raw
  /my/dir/blah-blorp-qux.0002.raw

  Logically, this group of files acts like a single file. We usually split it up
  because the individual files get really big on disk. But each file contains a sequence
  of blocks, and the blocks look the same regardless of which file they came from.

  In particular, the RawFileGroup handles missing blocks. Each block has a pktidx
  and if a pktidx is missing, we're missing data for that block and we typically
  want to replace missing data with zeros. Missing blocks can occur between two
  consecutive raw files, so to handle missing blocks correctly, it has to happen
  at the RawFileGroup level.

  The RawFileGroup is not threadsafe and the only access pattern it supports is to
  call resetBand for the band you want to read, followed by a number of
  readTasks calls which provide functions to read sequential batches.
  Typically you can get metadata from the RawFileGroup directly but to read it
  you want a RawFileGroupReader.
*/
class RawFileGroup {
 private:
  // Index of the current file in filenames that reader is opening.
  // -1 if there is none.
  int current_file;

  // Index of the header in the current file
  int header_index;
  
  // The next pktidx that we will return from read()
  int next_pktidx;

  // Helper to open the file with the given name
  const RawFile& openFile(const string& filename);
  
  // Helper to advance the reader and header
  void openNextFile();

  const RawFile& getFile();
  const raw::Reader& getReader();
  const raw::Header& getHeader();
  
  map<string, unique_ptr<RawFile> > files;
  
 public:
  const vector<string> filenames;

  // Has the directory name stripped
  string prefix;
  
  // Defining a sub-band of the files we are reading in
  int band;
  const int num_bands;

  // Metadata that should be the same for all blocks
  int nants;
  int num_coarse_channels;
  int npol;
  int schan;
  double obsbw;    // in Mhz
  double obsfreq;  // in Mhz

  string obsid;
  string source_name;
  double tbin;
  int timesteps_per_block;

  // Metadata used for timing
  int synctime;
  int piperblk;  

  // Metadata for the very first block
  int start_pktidx;
  
  // The total number of blocks in this set of raw files.
  // This includes missing blocks. 
  int num_blocks;

  // How many bytes each read returns 
  int read_size;

  // Starts off reading band zero
  RawFileGroup(const vector<string>& filenames, int num_bands);

  ~RawFileGroup();

  void resetBand(int new_band);

  /*
    readTasks reads data from a band of the next block into buffer. Sort of.
    It's indirect - instead of directly reading the data in this thread, it
    inserts functions in tasks, and when those functions are called, they read
    the data. This lets the RawFileGroupReader split up these tasks among multiple
    threads for efficiently, without making the RawFileGroup itself do any
    parallelism.
   */
  void readTasks(char* buffer, vector<function<bool()> >* tasks);
  
  double getStartTime(int block) const;

  // The total time, in seconds, that this file group represents
  float totalTime() const;

  // The total amount of data in gigabytes that this file group represents
  // Does not count headers, only data
  float totalDataGB() const;

  // The bandwidth of a coarse channel, in Mhz
  float coarseChannelBandwidth() const;
};
